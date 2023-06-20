#pragma once

#include "GLM/vec3.hpp"
#include "GLM/glm.hpp"

glm::mat4x4 build_shift_matrix(glm::vec3 delta) {
    
    return glm::mat4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        delta.x, delta.y, delta.z, 1
    );
}

glm::mat4x4 build_scale_matrix(glm::vec3 vec) {
    return glm::mat4x4(
        vec.x, 0, 0, 0,
        0, vec.y, 0, 0,
        0, 0, vec.z, 0,
        0, 0, 0,     1
    );
}

glm::mat4x4 build_rotation_matrix(Axis axis, float angle) {
    float radian_angle = toRadians(angle);
    auto angle_cos = cos(radian_angle);
    auto angle_sin = sin(radian_angle);
    switch (axis) {
    case Axis::ox:
        return glm::mat4x4(
            1, 0, 0, 0,
            0, angle_cos, -angle_sin, 0,
            0, angle_sin, angle_cos, 0,
            0, 0, 0, 1
        );
        break;
    case Axis::oy:
        return glm::mat4x4(
            angle_cos, 0, angle_sin, 0,
            0, 1, 0, 0,
            -angle_sin, 0, angle_cos, 0,
            0, 0, 0, 1
        );
        break;
    case Axis::oz:
        return glm::mat4x4(
            angle_cos, -angle_sin, 0, 0,
            angle_sin, angle_cos, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        );
    }
    return glm::mat4x4();
    
}

glm::mat4x4 build_reflection_matrix(Axis axis) {
    switch (axis)
    {
    case Axis::ox:
        return glm::mat4x4(
            1, 0, 0, 0,
            0, -1, 0, 0,
            0, 0, -1, 0,
            0, 0, 0, 1);
    case Axis::oy:
        return glm::mat4x4(
            -1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, -1, 0,
            0, 0, 0, 1);
    case Axis::oz:
        return glm::mat4x4(
            -1, 0, 0, 0,
            0, -1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1);
    }
    return glm::mat4x4();

}