#pragma once
#include "GL\glew.h"
#include "Texture.h"
#include "ObjMesh.h"
#include "ObjLoader.h"
#include <map>

class Model {
private:
    ObjTexture* overrideTextureDiffuse;
    ObjTexture* overrideTextureSpecular;
    OpenGLManager* manager;
    std::vector<Mesh> meshes;
    bool hasTexture = false;
    unsigned char render_mode;
public:
    Material material;
    glm::mat4 mm = glm::mat4(1.f); // Model Matrix
    Model() {

    }
    Model(const Model& other) {
        this->overrideTextureDiffuse = other.overrideTextureDiffuse;
        this->overrideTextureSpecular = other.overrideTextureSpecular;
        this->manager = other.manager;
        this->material = other.material;
        std::copy(other.meshes.begin(), other.meshes.end(), this->meshes.begin());
        this->hasTexture = other.hasTexture;
        this->render_mode = other.render_mode;

    }
    Model(ModelConstructInfo& mci) {
        this->manager = OpenGLManager::get_instance();
        this->material = mci.material;
        this->meshes.push_back(Mesh(mci.vertices.data(), mci.vertices.size(), NULL, 0, mci.lengths));
        this->render_mode = mci.render_mode;
    }
    /*Model(const char* objFile) {
        std::vector<ObjVertex> mesh = loadOBJ(objFile);
        meshes.push_back(Mesh(mesh.data(), mesh.size(), NULL, 0));
    }
    Model(
        const char* objFile,
        Material mat
    ) {
        manager = OpenGLManager::get_instance();
        material = mat;
        manager->checkOpenGLerror();
        hasTexture = true;
        std::vector<ObjVertex> mesh = loadOBJ(objFile);
        meshes.push_back(Mesh(mesh.data(), mesh.size(), NULL, 0));
    }
    Model(
        const char* objFile,
        ObjTexture tex
    ) {
        manager = OpenGLManager::get_instance();
        material = Material(tex);
        manager->checkOpenGLerror();
        hasTexture = true;
        std::vector<ObjVertex> mesh = loadOBJ(objFile);
        meshes.push_back(Mesh(mesh.data(), mesh.size(), NULL, 0));
    }*/

    void render(size_t count = 1, unsigned char mode = GL_NONE)
    {
        manager->checkOpenGLerror();
        //Draw
        for (auto& i : this->meshes) {
            if (material.map_Kd.initialized) {
                material.map_Kd.bind(0);
            }
            GLint texture_unit = 1;
            for (ObjTexture& tex : material.textures) {
                tex.bind(texture_unit);
                texture_unit++;
            }
            manager->checkOpenGLerror();
            i.render(count, mode == GL_NONE ? render_mode : mode);
            Texture::unbind();
        }
    }
};

/*std::map<std::string, Model*> loadObjModel(const std::string& path, const std::string& fname) {
    std::map<std::string, Model*> res;
    auto infos = loadOBJ(path, fname);
    for (auto& info : infos) {
        auto model = new Model(*it);
        res[it->first] = model;
    }
    return res;
}*/