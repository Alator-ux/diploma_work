#pragma once
#include "GL/glew.h"
#include "GLM/vec3.hpp"

struct Photon {
    glm::vec3 pos;
    glm::vec3 power;
    glm::vec3 inc_dir;
    Photon() {}
    Photon(const Photon& other) {
        this->pos = other.pos;
        this->power = other.power;
        this->inc_dir = other.inc_dir;
    }
    Photon(const glm::vec3& pos, const glm::vec3& power, const glm::vec3& inc_dir) {
        this->pos = pos;
        this->power = power;
        this->inc_dir = inc_dir;
    }
};
enum class PathType {
    dif_refl = 0, spec_refl, absorption, refr, none
};