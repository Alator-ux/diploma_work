#include "PhotonMapping.h"
/* ========== PhotonMapping class begin ========== */
PhotonMapping::PhotonMapping() :
    global_map(PhotonMap::Type::def), caustic_map(PhotonMap::Type::caustic) {}
void PhotonMapping::init(PMDrawer* drawer, std::vector<PMPreset>&& presets, PMSettingsUpdater& pmsu) {
    this->scene = std::move(PMScene(std::move(presets)));
    this->drawer = drawer;
    this->medium_manager.compute_critical_angles(this->scene);
    this->global_map.set_settings_updater(pmsu);
    this->caustic_map.set_settings_updater(pmsu);
    this->drawer->set_settings_updater(pmsu);
    pmsu.link_pc(&photon_collector.settings);
    pmsu.link_main(&settings);
    pmsu.link_scene(&scene.settings);
}
void PhotonMapping::emit(const PMModel& ls) {
    size_t ne = 0;// Number of emitted photons
    Ray ray;
    glm::vec3 normal;
    while (photon_collector.unfilled()) {
        medium_manager.clear();
        path_operator.clear();
        auto p = ls.get_radiation_info();
        p.first = ls.get_ls()->position;
        ray.origin = p.first + p.second * 0.0001f;

        float prob;
        float dot;
        do{
            ray.dir.x = Random<float>::random(-1.f, 1.f);
            ray.dir.y = Random<float>::random(-1.f, 1.f);
            ray.dir.z = Random<float>::random(-1.f, 1.f);
            ray.dir = glm::normalize(ray.dir);
            prob = Random<float>::random(0.f, 1.f);
            dot = glm::dot(ray.dir, p.second);
        } while (prob > dot);
        auto pp = ls.get_ls()->intensity / (float)photon_collector.settings.gsize; // photon power
        trace(ray, false, pp);
        ne++;
        if (ne % (photon_collector.settings.gsize / 10) == 0) {
            std::cout << "\tPhotons emited: " << ne << std::endl;
            photon_collector.pring_logs();
        }
    }
}
bool PhotonMapping::refract(float cosNL, const PMModel* ipmm) {
    const Material* im = ipmm->get_material();
    /*
    * Поверхность, на которую попал фотон, должна иметь не единичный показатель преломления, иначе считаем,
    * что фотон будет просто отражен. Если же показатели преломления равны, то это означает,
    * что преломления так же не должно быть. Например, фотон попал из воды в воду.
    */
    if (im->opaque == 1.f) {
        return false;
    }

    auto cn = medium_manager.get_cur_new(ipmm);
    if (!medium_manager.can_refract(cn, cosNL)) {
        return false;
    }
    float refr_probability;
    refr_probability = 1.f - FresnelSchlick(cosNL, cn.first, cn.second);
    //refr_probability = 1.f - im->opaque;
    /* ЗАКОНСЕРВИРОВАННО ДО ТЕСТА И ПОНИМАНИЯ
    float divn2n1 = im->refr_index / mat_ind.first->refr_index;
    if (ca_table.find(divn2n1) != ca_table.end()) {
        float critical_angle = ca_table[divn2n1];
        float angle = std::acos(cosNL);
        // Результат деления может быть > 1, в таком случае вероятность преломления должна быть равна 0.
        // Чем меньше угол падения, тем больше вероятность отражения
        refr_probability = 1.f - angle / critical_angle;
    }
    else {
        // Количество преломленного света описывается этим выражением.
        refr_probability = 1.f - FresnelSchlick(cosNL, mat_ind.first->refr_index, im->refr_index);
    }*/

    float e = Random<float>::random(0.f, 1.f);
    if (e > refr_probability) { // если выпала участь преломиться
        return false;
    }
    return true;
}
PathType PhotonMapping::destiny(float cosNL, const PMModel* ipmm, const glm::vec3& lphoton) {
    if (refract(cosNL, ipmm)) {
        return PathType::refr;
    }
    float e;
    const Material* im = ipmm->get_material();

    auto max_lp = std::max(std::max(lphoton.r, lphoton.g), lphoton.b);
    e = Random<float>::random(0.f, 2.f); // upper bound = 2.f because max|d + s| = 2

    auto lp_d = im->diffuse * lphoton;
    auto max_lp_d = std::max(std::max(lp_d.r, lp_d.g), lp_d.b);
    auto pd = max_lp_d / max_lp;
    if (e <= pd) {
        return PathType::dif_refl;
    }

    auto lp_s = im->specular * lphoton;
    auto max_lp_s = std::max(std::max(lp_s.r, lp_s.g), lp_s.b);
    auto ps = max_lp_s / max_lp;
    if (e <= pd + ps) {
        return PathType::spec_refl;
    }

    return PathType::absorption;
}
bool PhotonMapping::find_intersection(const Ray& ray, bool reverse_normal,
    PMModel*& imodel, glm::vec3& normal, glm::vec3& inter_p) {
    float inter = 0.f;
    imodel = nullptr;
    size_t ii0, ii1, ii2;
    for (PMModel& model : scene.objects()) {
        float temp_inter;
        size_t tii0, tii1, tii2;
        glm::vec3 tnormal;
        bool succ = model.intersection(ray, false, temp_inter, 
            tii0, tii1, tii2, tnormal); // TODO in_object скорее всего дропнуть
        if (succ && (inter == 0.f || temp_inter < inter)) {
            inter = temp_inter;
            ii0 = tii0;
            ii1 = tii1;
            ii2 = tii2;
            imodel = &model;
            normal = tnormal;
        }
    }
    if (imodel == nullptr) {
        return false;
    }
    inter_p = ray.origin + ray.dir * inter;
    if (reverse_normal && glm::dot(ray.dir, normal) > 0) { // reverse_normal &&
        normal *= -1.f;
    }
    return true;
}
void PhotonMapping::trace(const Ray& ray, bool in_object, const glm::vec3& pp) {
    glm::vec3 normal, inter_p;
    // Incident model
    PMModel* imodel;
    if (!find_intersection(ray, in_object, imodel, normal, inter_p)) {
        return;
    }
    Ray new_ray;
    float cosNL = glm::dot(-ray.dir, normal);
    PathType dest = destiny(cosNL, imodel, pp);
    switch (dest) {
    case PathType::refr:
    {
        float refr1, refr2;
        auto cn = medium_manager.get_cur_new(imodel);
        bool succ = ray.refract(inter_p, normal, cn.first, cn.second, new_ray);
        if (!succ) {
            throw std::exception("ЧЗХ?");
        }
        medium_manager.inform(succ, imodel);
        in_object = !in_object;
        break;
    }
    case PathType::dif_refl:
        photon_collector.push(Photon(inter_p, pp, ray.dir), path_operator);
        new_ray = ray.reflect_spherical(inter_p, normal);
        break;
    case PathType::spec_refl:
        new_ray = ray.reflect(inter_p, normal);
        break;
    case PathType::absorption:
        if (imodel->get_material()->specular != glm::vec3(1.f)) {
            photon_collector.push(Photon(inter_p, pp, ray.dir), path_operator);
        }
        return;
    default:
        break;
    }
    path_operator.inform(dest);
    trace(new_ray, in_object, pp);
}
float PhotonMapping::FresnelSchlick(float cosNL, float n1, float n2) {
    float f0 = std::pow((n1 - n2) / (n1 + n2), 2);
    return f0 + (1.f - f0) * pow(1.f - cosNL, 5.f);
}
void PhotonMapping::build_map() {
    photon_collector.check_updates();
    std::cout << "Photon emission started" << std::endl;
    size_t count = 0;
    for (size_t i = 0; i < scene.objects().size(); i++) {
        if (scene.objects()[i].get_ls() != nullptr) {
            std::cout << "Light source " << count << std::endl;
            emit(scene.objects()[i]);
            count++;
        }
    }
    std::cout << "Photon emission ended" << std::endl;
    std::cout << "Global map:" << std::endl;
    global_map.clear();
    global_map.fill_balanced(photon_collector.global);
    std::cout << "Caustic map:" << std::endl;
    caustic_map.clear();
    caustic_map.fill_balanced(photon_collector.caustic);
    photon_collector.clear();
    return ;
}
std::string last = "";
glm::vec3 PhotonMapping::render_trace(const Ray& ray, bool in_object, int depth, int i, int j) {
    glm::vec3 res(0.f);
    if (depth > settings.max_rt_depth) {
        return res;
    }
    glm::vec3 normal, inter_p;
    PMModel* imodel;
    if (!find_intersection(ray, in_object, imodel, normal, inter_p)) {
        return res;
    }
    if (depth == 0 && imodel->name != "floor") {
       //return res;
    }
    if (depth >= 1 && imodel->name == "leftSphere" && last == "leftSphere") {
        //return res;
        auto a = 1;
    }
    last = imodel->name;
    const Material* mat = imodel->get_material();
    if (depth == 0 && !in_object) {
        auto a = 0;
    }
    //drawer->set_rgb(i, j, mat->diffuse, PMDrawer::di, depth);
    //return mat->diffuse;
    // std::cout << "SCuucccc" << std::endl;
  // drawer->set_rgb(i, j, mat->diffuse, PMDrawer::di, depth);
  // return mat->emission;
    if (mat->diffuse != glm::vec3(0.f)) {
        glm::vec3 re(0.f);
        int lcount = 0;
        glm::vec3 tnormal, tinter_p;
        PMModel* timodel;
        Ray tray;
        for (auto& model : scene.objects()) {
            const LightSource* ls = model.get_ls();
            if (ls == nullptr) {
                continue;
            }
            tray.origin = ls->position;
            tray.dir = glm::normalize(inter_p - ls->position); // point <- ls
            tray.origin += tray.dir * 0.001f;
            if (find_intersection(tray, in_object, timodel, tnormal, tinter_p) && vec3_equal(inter_p, tinter_p)) {
                lcount++;
                re += glm::max(glm::dot(normal, -tray.dir), 0.f);
            }
        }
        re /= lcount == 0 ? 1 : lcount;
        auto di_op = mat->diffuse * mat->opaque;
        drawer->set_rgb(i, j, re * di_op, PMDrawer::di, depth);
        if (settings.dpmdi) {
            if (lcount == 0) {
                global_map.radiance_estimate(ray.dir, inter_p, normal, re);
                re *= settings.gl_mult;
                drawer->set_rgb(i, j, re * di_op, PMDrawer::gi, depth);
            }
        }
        else {
            res += re * di_op;
            global_map.radiance_estimate(ray.dir, inter_p, normal, re);
            re *= settings.gl_mult;
            drawer->set_rgb(i, j, re * di_op, PMDrawer::gi, depth);
        }
        res += re * di_op;
    }
    if (!in_object && mat->specular != glm::vec3(0.f)) {
        Ray nray = ray.reflect(inter_p, normal);
        auto t = render_trace(nray, in_object, depth+1, i, j);
        //glm::vec3 halfway = glm::normalize(nray.dir - ray.dir); // nray.dir + (-ray.dir)
        //float coef = glm::pow(glm::max(glm::dot(normal, halfway), 0.f), mat->shininess);
        float coef = glm::pow(glm::max(glm::dot(nray.dir, -ray.dir), 0.0f), mat->shininess);
        auto tres = t * mat->specular * mat->opaque * coef;
        res += tres;
        drawer->set_rgb(i, j, tres, PMDrawer::sp, depth);
    }
    if(mat->opaque != 1.f) {
        auto cn = medium_manager.get_cur_new(imodel);
        Ray nray;
        bool succ = ray.refract(inter_p, normal,
                cn.first, cn.second, nray);
        if (succ) {
            medium_manager.increase_depth();
            medium_manager.inform(true, imodel);
            auto t = render_trace(nray, !in_object, depth + 1, i, j);
            medium_manager.reduce_depth();
            //res = glm::mix(t, res, mat->opaque); // t * (1 - opaque) + res * opaque
            auto tres = t * (1.f - mat->opaque);
            res += tres;
            drawer->set_rgb(i, j, tres, PMDrawer::tr, depth);
        }
    }
    res += mat->emission;
    drawer->set_rgb(i, j, mat->emission, PMDrawer::em, depth);
    glm::vec3 caustic(0.f);
    if (caustic_map.radiance_estimate(ray.dir, inter_p, normal, caustic)) {
        auto tres = caustic * mat->opaque * settings.ca_mult;
        res += tres;
        drawer->set_rgb(i, j, tres, PMDrawer::ca, depth);
    }
    return res;
}
void PhotonMapping::render() {
    drawer->check_resolution();
    drawer->clear();
    std::cout << "Rendering has started" << std::endl;
    float width = drawer->get_width();
    float height = drawer->get_height();
    scene.camera.set_hw(height, width);
    scene.check_preset();
    //scene.camera = glm::vec3(-0.82277, 1.6047, 1.4969);
    //scene.camera = glm::vec3(-0.38551, -0.5847, 0.68857);

    /* Вине глассе
    scene.camera.set_position(glm::vec3(2.f, 1.f, 4.f));
    scene.camera.look_to(glm::vec3(-0.3f, -0.21f, -0.87f));*/


    /* Ring )))*/
    //scene.camera.set_position(glm::vec3(0.f, 1.f, 1.f));
    //scene.camera.look_at(glm::vec3(0.f));
    
    
    /*Sphere*/
    //scene.camera.set_position(glm::vec3(-0.00999999046, 0.795000017, 2.49000001));
    //scene.camera.look_to(glm::vec3(0.f, 0.f, -1.f));

    //scene.camera.set_position(glm::vec3(2.5f, 0.75f, 1.25f));
    //scene.camera.look_at(glm::vec3(0.5f, 0.f, 1.0f));

    /*water
    scene.camera.set_position(glm::vec3(-0.00999999046, 0.795000017, 1.99000001));
    scene.camera.look_to(glm::vec3(0.f, 0.f, -1.f));*/

    glm::vec3 origin = scene.camera.get_position();
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            medium_manager.clear();
            glm::vec3 dir = scene.camera.get_ray(i, j);
            Ray ray(origin, dir);
            render_trace(ray, false, 0, i, j);
        }
        if (j % ((size_t)height / 50) == 0) {
            std::cout << "\tPixels filled: " << (j + 1) * width << " of " << width * height << std::endl;   
     }
    }
    global_map.total_locate_time();
    std::cout << "Rendering has ended" << std::endl;
}
float PhotonMapping::BRDF(glm::vec3 direction, glm::vec3 location, glm::vec3 normal, const Material* mat) {
    glm::vec3 halfVector = glm::normalize(direction + glm::normalize(location));
    float NdotH = glm::max(glm::dot(normal, halfVector), 0.0f);
    float NdotL = glm::max(glm::dot(normal, direction), 0.0f);
    float HdotL = glm::max(glm::dot(halfVector, direction), 0.0f);
    float shininess = mat->shininess;
    float specular = glm::pow(NdotH, shininess);
    float diffuse = NdotL;
    return (diffuse / glm::pi<float>() + specular) * HdotL;
}
// Пока сомнительно
float PhotonMapping::GGX_GFunction(float cosNX, float sqRoughness) // Потерянный свет
{
    float alpha = sqRoughness * sqRoughness;
    float sqCosNX = cosNX * cosNX;
    float sqTan = 1 / sqCosNX - 1; // 1+ tan^2 = 1 / cos^2
    float res = 2 / (1 + sqrt(1 + alpha * sqTan));
    return res;
}
float PhotonMapping::GGX_DFunction(float cosNX, float sqRoughness)
{
    float alpha = sqRoughness * sqRoughness;
    float sqCosNX = cosNX * cosNX;
    float fourthCosNX = sqCosNX * sqCosNX;
    float sqTan = 1 / sqCosNX - 1; // 1+ tan^2 = 1 / cos^2
    float part = (alpha + sqTan);
    float res = alpha / (M_PI * fourthCosNX * part * part);
    return res;
}
glm::vec3 PhotonMapping::FresnelSchlick(float cosNL, glm::vec3 F0)
{
    return F0 + (glm::vec3(1.f) - F0) * pow(1.f - cosNL, 5.f);
}
glm::vec3 PhotonMapping::CookTorrance_GGX(float NdotL, float NdotV, float NdotH, glm::vec3 F, float roughness)
{
    if (NdotL <= 0.0 || NdotV <= 0.0) return glm::vec3(0.0);

    float sqRoughness = roughness * roughness;
    float GCoeff = GGX_GFunction(NdotL, sqRoughness) * GGX_GFunction(NdotV, sqRoughness);
    float DCoeff = GGX_DFunction(NdotH, sqRoughness);

    return glm::max(GCoeff * DCoeff * F * 0.25f / NdotV, 0.f);
}