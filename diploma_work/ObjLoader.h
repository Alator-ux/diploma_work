#pragma once

#include "GL\glew.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <map>
#include "Material.h"

struct ObjVertex {
    glm::vec3 position;
    glm::vec2 texcoord;
    glm::vec3 normal;
};
// Нужна для конструкции класса модели
struct ModelConstructInfo {
    std::string name;
    Material material;
    std::vector<ObjVertex> vertices;
    std::vector<size_t> lengths;
    unsigned char render_mode;
    bool smooth = false;
    ModelConstructInfo();
    ModelConstructInfo(const ModelConstructInfo& other);
    ModelConstructInfo(ModelConstructInfo&& other);
    void swap(ModelConstructInfo& other);
    ModelConstructInfo& operator=(const ModelConstructInfo& other);
    ModelConstructInfo& operator=(ModelConstructInfo&& other);
};
void loadMaterial(const std::string& mtl_path, const std::string mtl_fname, 
    const std::string& material, Material& dest);

std::vector<ModelConstructInfo> loadOBJ(const std::string& path, const std::string& fname);

void fill_normals(const std::string& path, const std::string& fname);