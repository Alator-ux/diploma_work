#pragma once
#include "GL\glew.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Texture.h"
struct Material {
    std::vector<ObjTexture> textures;
    ObjTexture map_Kd;
    glm::vec3 ambient = glm::vec3(0.2f);
    glm::vec3 diffuse = glm::vec3(1.f);
    glm::vec3 specular = glm::vec3(0.f);
    glm::vec3 emission = glm::vec3(0.0);
    // Optical density, also known as index of refraction
    GLfloat refr_index = 1.f;
    GLfloat opaque = 1.f;
    GLfloat shininess = 1.f;
    Material(){}
    Material(const Material& other) {
        this->textures = other.textures;
        this->map_Kd = other.map_Kd;
        this->ambient = other.ambient;
        this->diffuse = other.diffuse;
        this->specular = other.specular;
        this->emission = other.emission;
        this->opaque = other.opaque;
        this->shininess = other.shininess;
    }
    Material(std::vector<ObjTexture> textures, glm::vec3 ambient = glm::vec3(0.2),
        glm::vec3 diffuse = glm::vec3(1.0), glm::vec3 specular = glm::vec3(1.0),
        glm::vec3 emission = glm::vec3(0.0), GLfloat shininess = 16.0f,
        GLfloat roughless = 0.3) {
        this->textures = textures;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
        this->emission = emission;
        this->shininess = shininess;
    }
    // TODO очищать что-то еще?
    ~Material() {
        //delete map_Kd;
    }
    std::string get_texture_name(const std::string& pref = "", const std::string& suf = "") const {
        return pref + "text" + suf;
    }
    std::string get_ambient_name(const std::string& pref = "", const std::string& suf = "") const {
        return pref + "ambient" + suf;
    }
    std::string get_dif_name(const std::string& pref = "", const std::string& suf = "") const {
        return pref + "diffuse" + suf;
    }
    std::string get_spec_name(const std::string& pref = "", const std::string& suf = "") const {
        return pref + "specular" + suf;
    }
    std::string get_emission_name(const std::string& pref = "", const std::string& suf = "") const {
        return pref + "emission" + suf;
    }
    std::string get_shininess_name(const std::string& pref = "", const std::string& suf = "") const {
        return pref + "shininess" + suf;
    }
};