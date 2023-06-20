#include "ObjLoader.h"
#include <unordered_set>
void ModelConstructInfo::swap(ModelConstructInfo& other) {
    std::swap(this->lengths, other.lengths);
    std::swap(this->material, other.material);
    std::swap(this->name, other.name);
    std::swap(this->render_mode, other.render_mode);
    std::swap(this->smooth, other.smooth);
    std::swap(this->vertices, other.vertices);
}
ModelConstructInfo::ModelConstructInfo() : lengths(), material(), name(), render_mode(), vertices() {}
ModelConstructInfo::ModelConstructInfo(const ModelConstructInfo& other) {
    this->lengths = other.lengths;
    this->name = other.name;
    this->render_mode = other.render_mode;
    this->material = other.material;
    this->smooth = other.smooth;
    this->vertices = other.vertices;
}
ModelConstructInfo::ModelConstructInfo(ModelConstructInfo&& other) {
    this->swap(other);
}
ModelConstructInfo& ModelConstructInfo::operator=(const ModelConstructInfo& other) {
    this->lengths = other.lengths;
    this->name = other.name;
    this->render_mode = other.render_mode;
    this->material = other.material;
    this->smooth = other.smooth;
    this->vertices = other.vertices;
    return *this;
}
ModelConstructInfo& ModelConstructInfo::operator=(ModelConstructInfo&& other) {
    this->swap(other);
    return *this;
}
void loadMaterial(const std::string& mtl_path, const std::string mtl_fname,
    const std::string& material, Material& dest) {
    std::stringstream ss;
    std::string line = "";
    std::string prefix = "";
    glm::vec3 temp_vec3;
    GLfloat temp_float;
    std::ifstream mtl_file(mtl_path + "/" + mtl_fname);
    if (!mtl_file.is_open())
    {
        throw "ERROR::OBJLOADER::Could not open mtl file.";
    }
    bool processing = false;
    while (std::getline(mtl_file, line))
    {
        ss.clear();
        if (line.size() == 0) {
            prefix.clear();
        }
        else {
            ss.str(line);
            ss >> prefix;
        }
        if (prefix == "#") {

        }
        else if (prefix == "newmtl") {
            if (processing) { // ≈сли наткнулись на другой материал во врем€ обработки, то прошлый "закончилс€"
                break;
            }
            std::string mat_name;
            ss >> mat_name;
            if (mat_name != material) {
                continue;
            }
            processing = true; // Ќашли нужный материал и начали его считывать
        }
        else if (!processing) {
            continue;
        }
        else if (prefix == "Ka") {
            ss >> temp_vec3.r >> temp_vec3.g >> temp_vec3.b;
            dest.ambient = temp_vec3;
        }
        else if (prefix == "Kd") {
            ss >> temp_vec3.r >> temp_vec3.g >> temp_vec3.b;
            dest.diffuse = temp_vec3;
        }
        else if (prefix == "Ks") {
            ss >> temp_vec3.r >> temp_vec3.g >> temp_vec3.b;
            dest.specular = temp_vec3;
        }
        else if (prefix == "Ke") {
            ss >> temp_vec3.r >> temp_vec3.g >> temp_vec3.b;
            dest.emission = temp_vec3;
        }
        else if (prefix == "d") {
            ss >> temp_float;
            dest.opaque = temp_float;
        }
        else if (prefix == "Tr") {
            ss >> temp_float;
            dest.opaque = 1.f - temp_float;
        }
        else if (prefix == "Ns") {
            ss >> temp_float;
            dest.shininess = temp_float;
        }
        else if (prefix == "Ni") {
            ss >> temp_float;
            dest.refr_index = temp_float;
        }
        else if (prefix == "map_Kd") {
            std::string map_path;
            ss >> map_path;
            map_path = mtl_path + "/" + map_path;
            dest.map_Kd = ObjTexture(map_path.c_str(), 'n');
        }
    }
    mtl_file.close();
}

void build_vertices(std::vector<ObjVertex>& vertices, std::vector<glm::fvec3>& vertex_positions,
    std::vector<glm::fvec2>& vertex_texcoords, std::vector<glm::fvec3>& vertex_normals,
    std::vector<GLint>& vertex_position_indicies, std::vector<GLint>& vertex_texcoord_indicies,
    std::vector<GLint>& vertex_normal_indicies) {
    vertices.resize(vertex_position_indicies.size(), ObjVertex());
    for (size_t i = 0; i < vertices.size(); ++i)
    {
        GLint pos_ind = vertex_position_indicies[i] - 1;
        if (pos_ind < 0) {
            pos_ind = vertex_positions.size() + pos_ind + 1;
        }
        vertices[i].position = vertex_positions[pos_ind];
        if (vertex_texcoord_indicies.size() != 0)
        {
            vertices[i].texcoord = vertex_texcoords[vertex_texcoord_indicies[i] - 1];
        }
        if (vertex_normal_indicies.size() != 0) {
            GLint normal_ind = vertex_normal_indicies[i] - 1;
            if (normal_ind < 0) {
                normal_ind = vertex_normals.size() + normal_ind + 1;
            }
            vertices[i].normal = vertex_normals[normal_ind];
        }
    }

    /*vertex_positions = std::vector<glm::fvec3>();
    vertex_texcoords = std::vector<glm::fvec2>();
    vertex_normals = std::vector<glm::fvec3>();*/
    vertex_position_indicies = std::vector<GLint>();
    vertex_texcoord_indicies = std::vector<GLint>();
    vertex_normal_indicies = std::vector<GLint>();
}

std::vector<ModelConstructInfo> loadOBJ(const std::string& path, const std::string& fname)
{
    //Vertex portions
    std::vector<glm::fvec3> vertex_positions;
    std::vector<glm::fvec2> vertex_texcoords;
    std::vector<glm::fvec3> vertex_normals;

    //Face vectors
    std::vector<GLint> vertex_position_indicies;
    std::vector<GLint> vertex_texcoord_indicies;
    std::vector<GLint> vertex_normal_indicies;

    std::vector<ModelConstructInfo> res;
    ModelConstructInfo cur;
    std::string cur_name = "n1";
    std::unordered_set<std::string> names;
    std::stringstream ss;
    std::ifstream obj_file(path + "/" + fname);
    std::string mtl_fname;
    std::string line = "";
    std::string prefix = "";
    glm::vec3 temp_vec3;
    glm::vec2 temp_vec2;
    GLint temp_glint = 0;
    bool desc_finished = false;

    if (!obj_file.is_open())
    {
        throw "ERROR::OBJLOADER::Could not open file.";
    }

    while (std::getline(obj_file, line))
    {
        //Get the prefix of the line
        ss.clear();
        if (line.size() == 0) {
            prefix.clear();
        }
        else {
            ss.str(line);
            ss >> prefix;
        }
        if (desc_finished && prefix != "f") {
            build_vertices(cur.vertices, vertex_positions, vertex_texcoords, vertex_normals, vertex_position_indicies,
                vertex_texcoord_indicies, vertex_normal_indicies);
            //DEBUG
            std::cout << "Nr of vertices: " << cur.vertices.size() << "\n";
            if (names.find(cur_name) != names.end()) {
                cur_name += '1';
            }
            names.insert(cur_name);
            cur.name = cur_name;
            auto old_mat = cur.material;
            res.push_back(std::move(cur));
            cur = ModelConstructInfo();
            cur.material = old_mat;
            desc_finished = false;
            //std::cout << "OBJ file loaded!" << "\n";
            //return res;
        }
        if (prefix == "#")
        {
            std::string comment;
            ss >> comment;

        }
        else if (prefix == "mtllib") {
            ss >> mtl_fname;
        }
        else if (prefix == "g") {
            ss >> cur_name;
        }
        else if (prefix == "o")
        {
            ss >> cur_name;
        }
        else if (prefix == "s")
        {
            std::string temp_str;
            ss >> temp_str;
            if (temp_str == "1" || temp_str == "on") {
                cur.smooth = true;
            }
        }
        else if (prefix == "usemtl")
        {
            std::string mat_name;
            ss >> mat_name;
            cur.material = Material();
            loadMaterial(path, mtl_fname, mat_name, cur.material);
            // позорно проиграл плюсам и сдалс€. TODO: разобратьс€ и исправить
            //Material mat(loadMaterial(path, mtl_fname, mat_name));
            //cur.material = Material(mat);
        }
        else if (prefix == "v") //Vertex position
        {
            ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
            vertex_positions.push_back(temp_vec3);
        }
        else if (prefix == "vt")
        {
            ss >> temp_vec2.x >> temp_vec2.y;
            vertex_texcoords.push_back(glm::vec2(temp_vec2.x, 1 - temp_vec2.y));
        }
        else if (prefix == "vn")
        {
            ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
            vertex_normals.push_back(temp_vec3);
        }
        else if (prefix == "f")
        {
            desc_finished = true; // ѕосле всех "f" либо конец файла, либо следующа€ модель
            int counter = 0;
            size_t ps = 0; // poly size
            while (ss >> temp_glint)
            {
                //Pushing indices into correct arrays
                if (counter == 0)
                    vertex_position_indicies.push_back(temp_glint);
                else if (counter == 1)
                    vertex_texcoord_indicies.push_back(temp_glint);
                else if (counter == 2)
                    vertex_normal_indicies.push_back(temp_glint);

                //Handling characters
                if (ss.peek() == '/')
                {
                    ++counter;
                    ss.ignore(1, '/');
                    if (ss.peek() == '/')
                    {
                        ++counter;
                        ss.ignore(1, '/');
                    }
                }
                else if (ss.peek() == ' ' || ss.eof())
                {

                    ps++; // ≈сли модель содержит описание вида f v v 
                    counter++; // ≈сли модель содержит описание вида f v/vt/vn v/vt/vn v/vt/vn
                    ss.ignore(1, ' ');
                }

                //Reset the counter
                if (counter > 2) {
                    counter = 0;
                }
            }
            cur.lengths.push_back(ps);
            if (ps == 3) {
                cur.render_mode = GL_TRIANGLES;
            }
            else if (ps == 4) {
                cur.render_mode = GL_QUADS;
            }
            else if (ps > 4) {
                cur.render_mode = GL_POLYGON;
            }
        }
        else
        {

        }
    }
    if (desc_finished) {
        build_vertices(cur.vertices, vertex_positions, vertex_texcoords, vertex_normals, vertex_position_indicies,
            vertex_texcoord_indicies, vertex_normal_indicies);
        //DEBUG
        std::cout << "Nr of vertices: " << cur.vertices.size() << "\n";
        if (names.find(cur_name) != names.end()) {
            cur_name += '1';
        }
        names.insert(cur_name);
        cur.name = cur_name;
        res.push_back(cur);
    }

    //Loaded success
    std::cout << "OBJ file loaded!" << "\n";
    return res;
}

void fill_normals(const std::string& path, const std::string& fname) {
    std::vector<glm::fvec3> vertex_positions;

    std::stringstream ss;
    std::string full_name = path + "/" + fname;
    std::string old_name = path + "/" + fname + ".old";
    const char* full_cname = full_name.c_str();
    const char* old_cname = old_name.c_str();
    if (std::rename(full_cname, old_cname)) {
        throw "ERROR::OBJLOADER::Could not rename file.";
    }
    std::ifstream old_file(old_name);
    std::ofstream new_file(full_name);

    std::string line = "";
    std::string prefix = "";
    GLint temp_glint = 0;
    glm::vec3 temp_vec3;
    if (!old_file.is_open())
    {
        throw "ERROR::OBJLOADER::Could not open file.";
    }
    bool processing_v = false;
    while (std::getline(old_file, line))
    {
        ss.clear();
        if (line.size() == 0) {
            prefix.clear();
        }
        else {
            ss.str(line);
            ss >> prefix;
        }
        if (prefix == "v") //Vertex position
        {
            ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
            vertex_positions.push_back(temp_vec3);
            processing_v = true;
        }
        else if (processing_v)
        {
            auto v1 = vertex_positions[1] - vertex_positions[0];
            auto v2 = vertex_positions[vertex_positions.size() - 1] - vertex_positions[0];
            auto norm = glm::normalize(glm::cross(v1, v2));
            new_file << "vn  " << norm.x << " " << norm.y << " " << norm.z << '\n';
            vertex_positions.clear();
            processing_v = false;
        }
        if (prefix == "f") {
            new_file << "f  ";
            while (ss >> temp_glint)
            {
                if (ss.peek() == ' ')
                {
                    ss.ignore(1, ' ');
                }
                new_file << temp_glint << "////" << "-1 ";
            }
            new_file << "\n";
            continue;
        }
        new_file << line << '\n';
    }
    old_file.close();
    std::remove(old_cname);
}
