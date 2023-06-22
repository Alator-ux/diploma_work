#include "PMModel.h"

// ========== Ray section start ==========
Ray::Ray(const glm::vec3& origin, const glm::vec3& dir) {
     this->origin = origin;
     this->dir = glm::normalize(dir);
}
Ray Ray::reflect_spherical(const glm::vec3& from, const glm::vec3& normal) const {
    /*float e1 = Random<float>::random();
    float e2 = Random<float>::random();
    float theta = std::pow(std::cos(std::sqrt(e1)), -1.f);
    float phi = M_PI * e2;
    glm::vec3 new_dir;
    float r = glm::length(normal);
    float sin_theta = sin(theta);
    new_dir.x = r * sin_theta * cos(phi);
    new_dir.y = r * sin_theta * sin(phi);
    new_dir.z = r * cos(theta);
    Ray res(from, new_dir);*/
    /*float u = Random<float>::random(-1.f, 1.f);
    float theta = Random<float>::random(0.f, 2.f * M_PI);
    float uc = std::sqrt(1.f - u * u);
    glm::vec3 new_dir(
        uc * std::cos(theta),
        uc * std::sin(theta),
        u
    );*/
    glm::vec3 new_dir;
    do {
        float x1, x2;
        float sqrx1, sqrx2;
        do {
            x1 = Random<float>::random(-1.f, 1.f);
            x2 = Random<float>::random(-1.f, 1.f);
            sqrx1 = x1 * x1;
            sqrx2 = x2 * x2;
        } while (sqrx1 + sqrx2 >= 1);
        float fx1x2 = std::sqrt(1.f - sqrx1 - sqrx2);
        new_dir.x = 2.f * x1 * fx1x2;
        new_dir.y = 2.f * x2 * fx1x2;
        new_dir.z = 1.f - 2.f * (sqrx1 + sqrx2);
    } while (glm::dot(new_dir, normal) < 0);
    Ray res;
    res.dir = glm::normalize(new_dir);
    res.origin = from + 0.0001f * res.dir;
    return res;
}
Ray Ray::reflect(const glm::vec3& from, const glm::vec3& normal, float dnd) const {
    glm::vec3 refl_dir = dir - 2.f * normal * dnd;
    Ray res;
    res.dir = glm::normalize(refl_dir);
    res.origin = from + 0.01f * normal;
    return res;
}
Ray Ray::reflect(const glm::vec3& from, const glm::vec3& normal) const {
    auto dnd = glm::dot(dir, normal);
    glm::vec3 refl_dir = dir - 2.f * normal * dnd;
    Ray res;
    res.dir = glm::normalize(refl_dir);
    res.origin = from + 0.01f * normal;
    return res;
}
bool Ray::refract(const glm::vec3& from, const glm::vec3& normal, float refr1, float refr2, 
    float c1, Ray& out) const {
    float eta = refr1 / refr2;
    c1 = -c1;
    float w = eta * c1;
    float c2m = (w - eta) * (w + eta);
    if (c2m < -1.f) {
        return false;
    }
    out.dir = eta * dir + (w - sqrt(1.f + c2m)) * normal;
    out.origin = from + 0.01f * -normal;
    return true;
}
bool Ray::refract(const glm::vec3& from, const glm::vec3& normal, float refr1, float refr2, Ray& out) const {
    float eta = refr1 / refr2;
    float c1 = -glm::dot(dir, normal);
    float w = eta * c1;
    float c2m = (w - eta) * (w + eta);
    if (c2m < -1.f) {
        return false;
    }
    out.dir = eta * dir + (w - sqrt(1.f + c2m)) * normal;
    out.origin = from + 0.01f * -normal;
    return true;
}
// ========== Ray section end ==========

// ========== PMModel section start ==========
size_t PMModel::id_gen = 1;
float PMModel::eps = 0.0001f;
PMModel::PMModel(const PMModel& other) : name(other.name), ls(other.ls) {
    this->id = other.id;
    this->mci = other.mci;
    this->bcache = other.bcache;
    this->ls = other.ls;
}
PMModel::PMModel(const ModelConstructInfo& mci, 
    LightSource* ls) : name(mci.name), ls(ls) {
    this->mci = mci;
    this->id = id_gen;
    this->bcache = std::unordered_map<glm::vec3, BarycentricCache, Vec3Hash>();
    id_gen++;
}
float triarea(float a, float b, float c)
{
    float s = (a + b + c) / 2.0;
    return sqrt(s * (s - a) * (s - b) * (s - c));
}
float calculateTriangleArea(const glm::vec2& v0, const glm::vec2& v1, const glm::vec2& v2)
{
    return 0.5f * glm::abs((v1.x - v0.x) * (v2.y - v0.y) - (v2.x - v0.x) * (v1.y - v0.y));
}
glm::vec3 PMModel::barycentric_coords(const glm::vec2& st, size_t ii0, size_t ii1, size_t ii2) const {
    auto& st0 = mci.vertices[ii0].texcoord;
    auto& st1 = mci.vertices[ii1].texcoord;
    auto& st2 = mci.vertices[ii2].texcoord;
    float areaABC = calculateTriangleArea(st0, st1, st2);

    float areaPBC = calculateTriangleArea(st, st1, st2);
    float areaPCA = calculateTriangleArea(st0, st, st2);
    float areaPAB = calculateTriangleArea(st0, st1, st);

    // Calculate the barycentric coordinates using the areas of the sub-triangles
    float barycentricA = areaPBC / areaABC;
    float barycentricB = areaPCA / areaABC;
    float barycentricC = areaPAB / areaABC;

    return glm::vec3(barycentricA, barycentricB, barycentricC);
}
bool PMModel::traingle_intersection(const Ray& ray, bool in_object, const glm::vec3& v0,
    const glm::vec3& v1, const glm::vec3& v2, float& out, glm::vec3& uvw) const {
    out = 0.f;
    // compute the plane's normal
    glm::vec3 v0v1 = v1 - v0;
    glm::vec3 v0v2 = v2 - v0;
    // no need to normalize
    glm::vec3 N = glm::cross(v0v1, v0v2); // N
    float denom = glm::dot(N, N);

    // Step 1: finding P

    // check if the ray and plane are parallel.
    float NdotRayDirection = glm::dot(N, ray.dir);
    if (fabs(NdotRayDirection) < eps) // almost 0
        return false; // they are parallel so they don't intersect! 

    // compute t (equation 3)
    out = (glm::dot(N, v0) - glm::dot(N,ray.origin)) / NdotRayDirection;
    // check if the triangle is behind the ray
    if (out < 0) return false; // the triangle is behind

    // compute the intersection point using equation 1
    glm::vec3 P = ray.origin + out * ray.dir;

    // Step 2: inside-outside test
    glm::vec3 C; // vector perpendicular to triangle's plane

    // edge 0
    glm::vec3 edge0 = v1 - v0;
    glm::vec3 vp0 = P - v0;
    C = glm::cross(edge0, vp0);
    float w = glm::dot(N, C);
    if (w < 0) return false; // P is on the right side

    // edge 2
    glm::vec3 edge2 = v0 - v2;
    glm::vec3 vp2 = P - v2;
    C = glm::cross(edge2, vp2);
    float v = glm::dot(N, C);
    if (v < 0) return false; // P is on the right side;
    w /= denom;
    v /= denom;
    float u = 1.f - v - w;
    if (u <= 0) {
        return false;
    }
    uvw.x = u;
    uvw.y = v;
    uvw.z = w;
    return true; // this ray hits the triangle
}
bool PMModel::intersection(const Ray& ray, bool in_object, float& intersection, 
    size_t& ii0, size_t& ii1, size_t& ii2, glm::vec3& normal) const {
    intersection = 0.f;
    ii0 = ii1 = ii2 = 0;
    glm::vec3 uvw;
    size_t l = 0;
    size_t i = 0;
    float temp;
    bool succ;
    while (l < mci.lengths.size()) {
        temp = 0.f;
        glm::vec3 tuvw;
        if (mci.lengths[l] == 3) {
            succ = traingle_intersection(ray, in_object, mci.vertices[i].position,
                mci.vertices[i + 1].position, mci.vertices[i + 2].position, temp, tuvw);
            if (succ && (intersection == 0 || temp < intersection)) {
                intersection = temp;
                uvw = tuvw;
                ii0 = i;
                ii1 = i + 1;
                ii2 = i + 2;
            }
            i += 3;
        }
        else if (mci.lengths[l] == 4) {
            succ = traingle_intersection(ray, in_object, mci.vertices[i].position,
                mci.vertices[i + 1].position, mci.vertices[i + 3].position, temp, tuvw);
            if (succ && (intersection == 0 || temp < intersection)) {
                intersection = temp;
                uvw = tuvw;
                ii0 = i;
                ii1 = i + 1;
                ii2 = i + 3;
            }
            else {
                temp = 0.f;
                succ = traingle_intersection(ray, in_object, mci.vertices[i + 1].position,
                    mci.vertices[i + 2].position, mci.vertices[i + 3].position, temp, tuvw);
                if (succ && (intersection == 0 || temp < intersection)) {
                    intersection = temp;
                    uvw = tuvw;
                    ii0 = i + 1;
                    ii1 = i + 2;
                    ii2 = i + 3;
                }
            }
            i += 4;
        }
        else {
            throw std::exception("Not implemented");
        }
        l++;
    }
    if (intersection == 0.f) {
        return false;
    }
    auto& n0 = mci.vertices[ii0].normal;
    auto& n1 = mci.vertices[ii1].normal;
    auto& n2 = mci.vertices[ii2].normal;
    normal = glm::normalize(n0 * uvw.x + n1 * uvw.y + n2 * uvw.z);
    return true;
}
glm::vec3 PMModel::get_normal(size_t i) const {
    return mci.vertices[i].normal;
}
void PMModel::get_normal(size_t ii0, size_t ii1, size_t ii2, glm::vec3& point, glm::vec3& normal) {
    if (mci.smooth) {
        auto uvw = barycentric_coords(point, ii0, ii1, ii2);
        auto np  = mci.vertices[ii0].position * uvw.x +
            mci.vertices[ii1].position * uvw.y + mci.vertices[ii2].position * uvw.z;
        normal = interpolate_uvw(mci.vertices[ii0].position, mci.vertices[ii1].position,
            mci.vertices[ii2].position, uvw);
        return;
    }
    normal = mci.vertices[ii0].normal;
}
bool PMModel::st_in_triangle(const glm::vec2& st, size_t ii0, size_t ii1, size_t ii2, glm::vec3& uvw) const {
    uvw = barycentric_coords(st, ii0, ii1, ii2);
    bool succ = uvw.x >= 0.0f && uvw.y >= 0.0f && uvw.z >= 0.0f;
    uvw = glm::normalize(uvw);
    return succ;
}
bool PMModel::interpolate_by_st(const glm::vec2& st, glm::vec3& position, glm::vec3& normal) const {
    glm::vec3 uvw;
    size_t l = 0;
    size_t i = 0;
    bool succ;
    while (l < mci.lengths.size()) {
        if (mci.lengths[l] == 3) {
            succ = st_in_triangle(st, i, i + 1, i + 2, uvw);
            if (succ) {
                position = interpolate_uvw(mci.vertices[i].position, mci.vertices[i + 1].position,
                    mci.vertices[i + 2].position, uvw);
                normal   = interpolate_uvw(mci.vertices[i].normal, mci.vertices[i + 1].normal,
                    mci.vertices[i + 2].normal, uvw);
                return true;
            }
            i += 3;
        }
        else if (mci.lengths[l] == 4) {
            succ = st_in_triangle(st, i, i + 1, i + 3, uvw);
            if (succ) {
                position = interpolate_uvw(mci.vertices[i].position, mci.vertices[i + 1].position,
                    mci.vertices[i + 3].position, uvw);
                normal = interpolate_uvw(mci.vertices[i].normal, mci.vertices[i + 1].normal,
                    mci.vertices[i + 3].normal, uvw);
                return true;
            }
            else {
                succ = st_in_triangle(st, i + 1, i + 2, i + 3, uvw);
                if (succ) {
                    position = interpolate_uvw(mci.vertices[i + 1].position, mci.vertices[i + 2].position,
                        mci.vertices[i + 3].position, uvw);
                    normal = interpolate_uvw(mci.vertices[i + 1].normal, mci.vertices[i + 2].normal,
                        mci.vertices[i + 3].normal, uvw);
                    return true;
                }
            }
            i += 4;
        }
        else {
            throw std::exception("Not implemented");
        }
        l++;
    }
    return false;
}
const Material* PMModel::get_material() const {
    return &mci.material;
}
bool PMModel::equal(const PMModel& other) const {
    return this->id == other.id;
}
bool PMModel::equal(size_t other_id) const {
    return this->id == other_id;
}
void PMModel::set_light_intensity(const glm::vec3& value) {
    if (ls != nullptr) {
        ls->intensity = value;
    }
}
size_t PMModel::get_id() const {
    return id;
}
const LightSource* PMModel::get_ls() const {
    return ls;
}
glm::vec3* PMModel::get_wn() const
{
    glm::vec3 right_upper(mci.vertices[0].position), left_lower(mci.vertices[0].position), normal(0.f);
    for (auto& v : mci.vertices) {
        for (size_t point_i = 0; point_i < 3; point_i++) {
            if (v.position[point_i] > right_upper[point_i]) {
                right_upper[point_i] = v.position[point_i];
            }
            if (v.position[point_i] < left_lower[point_i]) {
                left_lower[point_i] = v.position[point_i];
            }
        }
        normal += v.normal;
    }
    normal /= mci.vertices.size();
    glm::vec3 res[3];
    res[0] = left_lower;
    res[1] = right_upper;
    res[2] = glm::normalize(normal);
    return res;
}
std::pair<glm::vec3, glm::vec3> PMModel::get_radiation_info() const {
    std::pair<glm::vec3, glm::vec3> res;
    size_t r;
    glm::vec3 inds;
    if (mci.render_mode == GL_TRIANGLES) {
        r = Random<size_t>::random(0, (mci.vertices.size() - 3) / 3);
        r *= 3;
        inds = glm::vec3(r, r + 1, r + 2);
    }
    else if (mci.render_mode == GL_QUADS) {
        r = Random<size_t>::random(0, (mci.vertices.size() - 4) / 4);
        r *= 4;
        if (Random<float>::random() < 0.5f) {
            inds = glm::vec3(r, r + 1, r + 3);
        }
        else {
            inds = glm::vec3(r + 1, r + 2, r + 3);
        }
    }
    glm::vec3 uvw;
    uvw.x = Random<float>::random(0.f, 1.f);
    uvw.y = Random<float>::random(uvw.x, 1.f);
    uvw.z = 1.f - uvw.x - uvw.y;
    
    res.first = uvw.x * mci.vertices[inds.x].position + uvw.y * mci.vertices[inds.y].position +
        uvw.z * mci.vertices[inds.z].position;
    res.second = uvw.x * mci.vertices[inds.x].normal + uvw.y * mci.vertices[inds.y].normal +
        uvw.z * mci.vertices[inds.z].normal;
    return res;
}

PMCamera::PMCamera(glm::vec3 position, glm::vec3 up, glm::vec3 front, 
    float yaw, float pitch) {
    this->position = position;
    this->world_up = up;
    this->yaw = yaw;
    this->pitch = pitch;
    constexpr float fov_ = glm::radians(60.f);
    this->scale = glm::tan(fov_ / 2.0f);
    updateCameraVectors();
}
void PMCamera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);

    right = glm::normalize(glm::cross(front, world_up));
    up = glm::normalize(glm::cross(right, front));
}
void PMCamera::set_hw(float height, float width) {
    this->height = height;
    this->width = width;
    this->aspect = height / width;
}
void PMCamera::set_position(const glm::vec3& pos) {
    position = pos;
}
glm::vec3 PMCamera::get_position() {
    return position;
}
glm::vec3 PMCamera::get_normal() {
    return front;
}
void PMCamera::look_to(const glm::vec3& dir) {
    front = glm::normalize(dir);
    updateCameraVectors();
}
void PMCamera::look_at(const glm::vec3& pos) {
    front = glm::normalize(pos - this->position);
    updateCameraVectors();
}
glm::vec3 PMCamera::get_ray(float i, float j) {
    glm::mat4 camera_to_world_ = glm::inverse(glm::lookAtRH(position, position + front, up));
    glm::vec3 ray_direction = front;
    glm::vec2 pixel_ndc((i + 0.5f) / width, (j + 0.5f) / height);
    ray_direction.x = (2.0f * pixel_ndc.x - 1.0f) * scale;
    ray_direction.y = (1.0f - 2.0f * pixel_ndc.y) * scale * aspect;
    ray_direction = camera_to_world_ * glm::vec4(ray_direction, 0.0f);

    return glm::normalize(ray_direction);
}

PMScene::PMScene() {
    std::vector<PMModel> objects;
    glm::vec3 left_lower, right_upper;
}
PMScene::PMScene(std::vector<PMPreset>&& presets) :camera() {
    this->presets = presets;
    this->settings.active_preset = 0;
    this->old_preset = 0;
}
void PMScene::update_camera() {
    if (check_presets()) {
        camera.set_position(presets[settings.active_preset].pos);
        camera.look_to(presets[settings.active_preset].dir);
    }
}
bool PMScene::check_presets() {
    if (old_preset == settings.active_preset) {
        return false;
    }
    old_preset = settings.active_preset;
    return true;
}
std::vector<std::string> PMScene::get_names() const {
    std::vector<std::string> res;
    for (auto& m : presets[settings.active_preset].objects) {
        res.push_back(m.name);
    }
    return res;
}
const std::string& PMScene::global_map_path() const {
    return presets[settings.active_preset].global_map_path;
}
const std::string& PMScene::caustic_map_path() const {
    return presets[settings.active_preset].caustic_map_path;
}
std::vector<PMModel>& PMScene::objects() {
    return presets[settings.active_preset].objects;
}
