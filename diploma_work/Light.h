#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <string>

struct LightSource {
    glm::vec3 intensity = glm::vec3(1.f);
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    LightSource(glm::vec3 position = glm::vec3(0.f), glm::vec3 ambient = glm::vec3(1.0),
        glm::vec3 diffuse = glm::vec3(1.0), glm::vec3 specular = glm::vec3(1.0)) {
        this->position = position;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
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
};

struct PointLight : public LightSource {
    glm::vec3 attenuation;
    PointLight(glm::vec3 position = glm::vec3(0.0), glm::vec3 ambient = glm::vec3(1.0),
        glm::vec3 diffuse = glm::vec3(1.0), glm::vec3 specular = glm::vec3(1.0),
        glm::vec3 attenuation = glm::vec3(1.0, 0.22, 0.20)) {
        this->position = position;
        this->attenuation = attenuation;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
    }
    void set_atten_zero() {
        attenuation = glm::vec3(1.0, 0.0, 0.0);
    }
    std::string get_pos_name(const std::string& pref = "", const std::string& suf = "") const {
        return pref + "pos" + suf;
    }
    std::string get_atten_name(const std::string& pref = "", const std::string& suf = "") const {
        return pref + "atten" + suf;
    }
};

struct DirectionLight : public LightSource {
    glm::vec3 direction;
    DirectionLight(glm::vec3 direction = glm::vec3(0.0), glm::vec3 ambient = glm::vec3(1.0),
        glm::vec3 diffuse = glm::vec3(1.0), glm::vec3 specular = glm::vec3(1.0)) {
        this->direction = direction;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
    }
    std::string get_dir_name(const std::string& pref = "", const std::string& suf = "") const {
        return pref + "direction" + suf;
    }
};

struct FlashLight : public LightSource {
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 attenuation;
    float cutOff;
    FlashLight(glm::vec3 position = glm::vec3(0.0), glm::vec3 direction = glm::vec3(1.0),
        float cuttOff = 12.5f, glm::vec3 ambient = glm::vec3(1.0),
        glm::vec3 diffuse = glm::vec3(1.0), glm::vec3 specular = glm::vec3(1.0),
        glm::vec3 attenuation = glm::vec3(1.0, 0.22, 0.20)) {
        this->position = position;
        this->direction = direction;
        this->cutOff = cutOff;
        this->attenuation = attenuation;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
    }
    void set_atten_zero() {
        attenuation = glm::vec3(1.0, 0.0, 0.0);
    }
    std::string get_pos_name(const std::string& pref = "", const std::string& suf = "") const {
        return pref + "pos" + suf;
    }
    std::string get_dir_name(const std::string& pref = "", const std::string& suf = "") const {
        return pref + "direction" + suf;
    }
    std::string get_cutOff_name(const std::string& pref = "", const std::string& suf = "") const {
        return pref + "cutOff" + suf;
    }
    std::string get_atten_name(const std::string& pref = "", const std::string& suf = "") const {
        return pref + "atten" + suf;
    }
};