#include "PhotonMap.h"
#include <GLM/ext/scalar_constants.hpp>
#include "PhotonMapping.h"

/* ========== PhotonMapp class begin ========== */

/* ==========! PhotonMapp substruct !========== */
/* ========== Node struct begin ========== */
PhotonMap::Node::Node(Node* left, Node* right) {
    this->left = left;
    this->right = right;
}
PhotonMap::Node::Node(const Photon& value, short plane, Node* left, Node* right) {
    this->value = value;
    this->plane = plane;
    this->left = left;
    this->right = right;
}
PhotonMap::Node::~Node() {
    delete left;
    delete right;
    left = nullptr;
    right = nullptr;
}
/* ========== Node struct end ========== */

/* ==========! PhotonMapp substruct !========== */
/* ========== NPNode struct begin ========== */
PhotonMap::NPNode::NPNode(const Node* photon, float sq_dist) : node(photon), sq_dist(sq_dist) {}
/* ========== NPNode struct end ========== */

/* ==========! PhotonMapp substruct !========== */
/* ========== NPNodeCopmarator struct begin ========== */
bool PhotonMap::NPNodeCopmarator::operator() (const NPNode& f, const NPNode& s) const {
    return f.sq_dist < s.sq_dist;
}
/* ========== NPNodeCopmarator struct end ========== */

/* ==========! PhotonMapp substruct !========== */
/* ========== NPContainerQ class begin ========== */
PhotonMap::NPContainerQ::NPContainerQ(size_t capacity) : _capacity(capacity) {}
void PhotonMap::NPContainerQ::fpush(const NPNode& elem) {
    priority_queue::push(elem);
}
void PhotonMap::NPContainerQ::push(const NPNode& elem) {
     // если рассматриваемый больше максимального, то он нам не нужен
    if (priority_queue::comp(priority_queue::top(), elem)) {
        return;
    }
    priority_queue::push(elem); // иначе добавляем
    if (this->size() > _capacity) { // и проверяем, не привысили ли вместимость
        priority_queue::pop();
    }
}
void PhotonMap::NPContainerQ::pop() {
    if (priority_queue::size() == 0) {
        throw std::exception("Trying pop from zero size container");
    }
    priority_queue::pop();
}
const Photon& PhotonMap::NPContainerQ::operator[](size_t index) {
    return priority_queue::c[index].node->value;
}
float PhotonMap::NPContainerQ::max_dist() const { return priority_queue::top().sq_dist; }
float PhotonMap::NPContainerQ::min_dist() const { return 0; }
size_t PhotonMap::NPContainerQ::capacity() const { return _capacity; }
/* ========== NPContainerQ class end ========== */

/* ==========! PhotonMapp substruct !========== */
/* ========== NPNRNodeComparator struct begin ========== */
bool PhotonMap::NPNRNodeComparator::operator() (const NPNRNode& f, const NPNRNode& s) const {
    return f.priority > s.priority;
}
/* ========== NPNRNodeComparator struct end ========== */

/* ==========! PhotonMapp substruct !========== */
/* ========== NearestPhotons struct begin ========== */
PhotonMap::NearestPhotons::NearestPhotons(const glm::vec3& pos, const glm::vec3& normal, 
    size_t capacity) : pos(pos), normal(normal), container(NPContainerQ(capacity))
{}
/* ========== NearestPhotons struct end ========== */

/* ==========! PhotonMapp subclass !========== */
/* ========== Filter abstract class begin ========== */
PhotonMap::Filter::Filter(float norm_coef) : norm_coef(norm_coef) {}
float PhotonMap::Filter::call(const glm::vec3& ppos, const glm::vec3& loc, float r) {
    return 1.f;
}
/* ========== Filter abstract class end ========== */

/* ==========! PhotonMapp subclass !========== */
/* ========== ConeFilter class begin ========== */
PhotonMap::ConeFilter::ConeFilter(float k) : k(k), Filter(1.f - 2 / (3 * k)) {}
float PhotonMap::ConeFilter::call(const glm::vec3& ppos, const glm::vec3& loc, float r) {
    float dist = glm::distance(ppos, loc);
    return 1.f - dist / (k * r);
}
/* ========== ConeFilter class end ========== */

/* ==========! PhotonMapp subclass !========== */
/* ========== GaussianFilter class begin ========== */
PhotonMap::GaussianFilter::GaussianFilter(float alpha, float beta) : alpha(alpha), beta(beta), Filter(1.f) {}
float PhotonMap::GaussianFilter::call(const glm::vec3& ppos, const glm::vec3& loc, float r) {
    float dist = glm::distance(ppos, loc);
    return alpha * (1.f - (1.f - std::pow(M_E, -beta * dist * dist / (2 * r))) /
        (1.f - std::pow(M_E, -beta)));
}
/* ========== GaussianFilter class end ========== */

short PhotonMap::find_largest_plane(const glm::vec3& bigp, const glm::vec3& smallp) {
    size_t largest_plane = 0;
    {
        glm::vec3 dims(
            std::abs(bigp.x - smallp.x),
            std::abs(bigp.y - smallp.y),
            std::abs(bigp.z - smallp.z)
        );
        float max = dims[0];
        for (size_t i = 1; i < 3; i++) {
            if (dims[i] > max) {
                max = dims[i];
                largest_plane = i;
            }
        }
    }
    return largest_plane;
}
void PhotonMap::update_cube(const glm::vec3& p, glm::vec3& bigp, glm::vec3& smallp) {
    for (size_t point_i = 0; point_i < 3; point_i++) {
        if (p[point_i] > bigp[point_i]) {
            bigp[point_i] = p[point_i];
        }
        if (p[point_i] < smallp[point_i]) {
            smallp[point_i] = p[point_i];
        }
    }
}
/*oid PhotonMap::locate(NearestPhotons* const np) const {
    std::stack<Node*> node_recur;
    node_recur.push(root);
    while (!node_recur.empty()) {
        const Node* p = node_recur.top();
        node_recur.pop();
        if (p == nullptr) {
            continue;
        }
        float dist1 = np->pos[p->plane] - p->value.pos[p->plane];
        if (dist1 > 0.f) { // if dist1 is positive search right plane
            if (dist1 * dist1 < np->container.max_dist()) {
                node_recur.push(p->left);
            }
            node_recur.push(p->right);
        }
        else { // dist1 is negative search left first
            if (dist1 * dist1 < np->container.max_dist()) {
                node_recur.push(p->right);
            }
            node_recur.push(p->left);
        }
        // compute squared distance between current photon and np->pos
        auto dists1 = p->value.pos - np->pos;
        float sq_dist = dists1.x * dists1.x + dists1.y * dists1.y + dists1.z * dists1.z;
        np->container.push(NPNode(p, sq_dist)); // алгоритм плох, т.к. в рекурсивном варианте сначала опускаемся максимально вниз и начинаем заполнять с ближайших. или нет?
    }
}*/
void PhotonMap::locate(NearestPhotons* np) const {
    std::priority_queue<NPNRNode, std::vector<NPNRNode>, NPNRNodeComparator> nodeq;
    nodeq.push(NPNRNode(root, 0));
    int double_depth = 2 * depth;
    while (!nodeq.empty()) {
        NPNRNode npnrnode = nodeq.top();
        nodeq.pop();
        const Node* p = npnrnode.node;
        if (p == nullptr) {
            continue;
        }
        float dist1;
        dist1 = np->pos[p->plane] - p->value.pos[p->plane];
        if (dist1 > 0.f) { // if dist1 is positive search right plane
            nodeq.push(NPNRNode(p->right, npnrnode.priority + 1));
            if (dist1 * dist1 < np->container.max_dist()) {
                nodeq.push(NPNRNode(p->left, double_depth - npnrnode.priority));
            }
        }
        else { // dist1 is negative search left first
            nodeq.push(NPNRNode(p->left, npnrnode.priority + 1));
            if (dist1 * dist1 < np->container.max_dist()) {
                nodeq.push(NPNRNode(p->right, double_depth - npnrnode.priority));
            }
        }
        /*std::cout << "x = " << p->value.pos.x << " y = " << p->value.pos.y << " z = " << p->value.pos.z
            << std::endl;*/
        // dpn = dp * (1 + f * |cos(nx, x -> p)|) = dp + f * dp * |cos(nx, x->p)|
        float dpn = glm::distance(p->value.pos, np->pos) * (1.f + settings.disc_compression *
            glm::abs(glm::dot(np->normal, glm::normalize(p->value.pos - np->pos))));
        np->container.push(NPNode(p, dpn));
    }
}
void PhotonMap::locate(NearestPhotons* const np, const Node* photon) const {
    if (photon == nullptr) {
        return;
    }
    const Node* p = photon;
    float dist1;
    dist1 = np->pos[p->plane] - p->value.pos[p->plane];
    if (dist1 > 0.f) { // if dist1 is positive search right plane
        locate(np, p->right);
        if (dist1 * dist1 < np->container.max_dist()) {
            locate(np, p->left);
        }
    }
    else { // dist1 is negative search left first
        locate(np, p->left);
        if (dist1 * dist1 < np->container.max_dist()) {
            locate(np, p->right);
        }
    }
    // compute squared distance between current photon and np->pos
    //auto dists1 = p->value.pos - np->pos;
    //float z = dists1.x * dists1.x + dists1.y * dists1.y + dists1.z * dists1.z;
    // dpn = dp * (1 + f * |cos(nx, x -> p)|) = dp + f * dp * |cos(nx, x->p)|
    float dpn = glm::distance(p->value.pos, np->pos) * (1.f + settings.disc_compression *
        glm::abs(glm::dot(np->normal, glm::normalize(p->value.pos - np->pos))));
   // std::cout << "Count: " << count << std::endl;
   // std::cout << "Size: ";
    /*std::cout << "x = " << p->value.pos.x << " y = " << p->value.pos.y << " z = " << p->value.pos.z
        << std::endl;*/
    np->container.push(NPNode(p, dpn));
}
bool PhotonMap::locate_q(NearestPhotons* np) const {
    Node* susNode = new Node(Photon(np->pos, glm::vec3(0.f), glm::vec3(0.f)), 0);
    np->container.fpush({ susNode, max_distance * max_distance });
    locate(np);
    if (np->container.size() < np->container.capacity()) {
        np->container.pop();
    }
    delete susNode;
    return np->container.size() == settings.np_size;
}
bool PhotonMap::locate_r(NearestPhotons* np) const {
    Node* susNode = new Node(Photon(np->pos, glm::vec3(0.f), glm::vec3(0.f)), 0);
    np->container.fpush({ susNode, max_distance * max_distance });
    locate(np, root);
    /*if (np->container.size() < np->container.capacity()) {
        np->container.pop();
    }*/
    if (np->container.top().sq_dist == max_distance * max_distance) {
        return false;
    }
    delete susNode;
    return np->container.size() == settings.np_size;
}
Timer timer;
void PhotonMap::total_locate_time() {
    timer.print_total();
}
bool PhotonMap::radiance_estimate(const glm::vec3& inc_dir, const glm::vec3& iloc, const glm::vec3& norm, glm::vec3& out_rad) {
    out_rad.x = out_rad.y = out_rad.z = 0;
    if (size == 0 || settings.np_size == 0) {
        return false;
    }
    NearestPhotons np(iloc, norm, settings.np_size);
    if (!locate_r(&np)) {
        return false;
    }

    float r, filter_r;
    filter_r = r = glm::distance(iloc, (np.container[0].pos));
    if (settings.ftype == PhotonMapSettings::FilterType::gaussian) {
        filter_r *= filter_r;
    }
    for (size_t i = 0; i < np.container.size(); i++) {
        auto& p = np.container[i];
        float cosNL = glm::dot(-p.inc_dir, norm);
        if (cosNL > 0.f) {
            out_rad += p.power * cosNL * (this->filters[settings.ftype])->call(p.pos, iloc, filter_r);
        }
    }

    //float temp = (1.f / (M_PI * r * r * this->filters[settings.ftype]->norm_coef));
    float temp = (1.f / (M_PI * (r * r) * this->filters[settings.ftype]->norm_coef));
    out_rad *= temp;
    return true;
}
PhotonMap::PhotonMap(Type type) {
    this->settings.ftype = PhotonMapSettings::FilterType::none;
    this->type = type;
    this->size = 0;
    this->root = nullptr;
}
void PhotonMap::fill_balanced(const std::vector<Photon>& photons) {
    size = photons.size();
    
    filters = std::vector<Filter*>(3);
    filters[PhotonMapSettings::FilterType::none] = new Filter(1.f);
    filters[PhotonMapSettings::FilterType::cone] = new ConeFilter(settings.cf_k);
    filters[PhotonMapSettings::FilterType::gaussian] = new GaussianFilter(settings.gf_alpha, settings.gf_beta);
    //heap = new glm::vec3*[size];

    if (photons.size() == 0) {
        std::cout << "There is nothing to insert" << std::endl;
        root = nullptr;
        return;
    }
    std::cout << "Filling balanced KD-tree (photon map) started" << std::endl;
    depth = std::ceil(std::log2((double)size + 1.0));
    /*
    * нахождение "куба", захватывающего все точки и заполнение вектор указателей для работы с ними
    * + перевод вектор точек в вектор указателей на точки для дальнейшего удобства
    */
    std::vector<const Photon*> photons_pointers(photons.size());
    glm::vec3 bigp(photons[0].pos), smallp(photons[0].pos);
    for (size_t i = 0; i < photons.size(); i++) {
        photons_pointers[i] = &photons[i];
        update_cube(photons[i].pos, bigp, smallp);
    }

    /*
    * нахождение измерения с наибольшей длиной
    */
    auto largest_plane = find_largest_plane(bigp, smallp);

    size_t count = 0;
    // from, to, plane, node
    std::stack<std::tuple<size_t, size_t, short, Node**>> ftpn_recur;
    ftpn_recur.push({ 0, photons_pointers.size(), largest_plane, &root });
    while (!ftpn_recur.empty()) {
        short plane;
        size_t from, to;
        Node** node;
        std::tie(from, to, plane, node) = ftpn_recur.top();
        ftpn_recur.pop();
        if (to - from == 0) {
            node = nullptr;
            continue;
        }
        count++;
        
        if (count % ((size / 10)) == 0) {
            std::cout << "\tPhotons inserted: " << count << std::endl;
        }
        *node = new Node();
        (*node)->plane = plane;
        if (to - from == 1) {
            (*node)->value = *photons_pointers[from];
        }

        // Сортировка для нахождения среднего. Сортируем указатели.
        std::sort(std::next(photons_pointers.begin(), from), std::next(photons_pointers.begin(), to),
            [&plane](const Photon* p1, const Photon* p2) { return p1->pos[plane] < p2->pos[plane]; });
        size_t mid = (to - from) / 2 + from;
        const Photon* medium = photons_pointers[mid];

        std::vector<const Photon*> s1, s2;
        glm::vec3 left_bigp(std::numeric_limits<float>::lowest()), left_smallp(std::numeric_limits<float>::max());
        glm::vec3 right_bigp(left_bigp), right_smallp(left_smallp);
        std::for_each(std::next(photons_pointers.begin(), from), std::next(photons_pointers.begin(), mid),
            [&left_bigp, &left_smallp, this](const Photon* p) {
                update_cube(p->pos, left_bigp, left_smallp);
            });
        std::for_each(std::next(photons_pointers.begin(), mid + 1), std::next(photons_pointers.begin(), to),
            [&right_bigp, &right_smallp, this](const Photon* p) {
                update_cube(p->pos, right_bigp, right_smallp);
            });

        size_t left_plane = find_largest_plane(left_bigp, left_smallp);
        size_t right_plane = find_largest_plane(right_bigp, right_smallp);

        (*node)->value = *medium;
        ftpn_recur.push({ from, mid, left_plane, &(*node)->left }); // left
        ftpn_recur.push({ mid + 1, to, right_plane, &(*node)->right }); // right
    }
    std::cout << "\tPhotons inserted: " << count << std::endl;
    std::cout << "Filling balanced KD-tree (photon map) ended" << std::endl;
}
PhotonMap::~PhotonMap() {
    clear();
}
void PhotonMap::clear() {
    delete root;
    root = nullptr;
    for (Filter* filter : filters) {
        delete filter;
    }
}
void PhotonMap::set_settings_updater(PMSettingsUpdater& pmsu) {
    if (type == def) {
        pmsu.link_gmap(&settings);
    }
    else {
        pmsu.link_cmap(&settings);
    }
}
/* ========== PhotonMapp class end ========== */
