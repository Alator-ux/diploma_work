#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "GLM/glm.hpp"
#include <string>
#include <vector>
#include "CImg.h"
#include <stdint.h>
#include "stb_image.h"
struct Texture {
    // Здесь вы наблюдаете позор. Я просто не знаю почему, когда я делаю new Texture до какого-то момента id сохраняется,
    // а после становится неиницализованным. Может быть дело, в копировании.
    bool initialized = false;
    GLuint id;
    Texture(bool init = false) {
        if (init) {
            glGenTextures(1, &id);
        }
    }
    Texture(const Texture& other) {
        this->id = other.id;
        this->initialized = other.initialized;
    }
    void create_rgba16f_buffer(GLuint width, GLuint height) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        initialized = true;
    }
    void create_rgb_buffer(GLuint width, GLuint height, unsigned char* data = NULL) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        initialized = true;
    }
    void create_rgba_buffer(GLuint width, GLuint height) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        initialized = true;
    }
    void create_wrap_clamp_buffer(GLuint width, GLuint height) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        initialized = true;
    }
    void create_wrap_clamp_rgb16f_buffer(GLuint width, GLuint height) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        initialized = true;
    }
    void create_point_buffer(GLuint width, GLuint height) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        initialized = true;
    }

    void create_noise_texture(GLuint width, GLuint height, const std::vector<glm::vec3>& noise) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGB, GL_FLOAT, &noise[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        initialized = true;
    }
    void bind(const GLint texture_unit = 0)
    {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture(GL_TEXTURE_2D, id);
    }
    GLuint getId() {
        return id;
    }
    void del() {
        glDeleteTextures(1, &id);
        initialized = false;
    }
    static void unbind()
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    };
};



class CImgTexture {
public:
    cimg_library::CImg<unsigned char> image;
    CImgTexture(GLuint width, GLuint height) {
        this->image = cimg_library::CImg<unsigned char>(width, height, 1, 3, 255);
    }
    CImgTexture(GLuint width, GLuint height, unsigned char color) {
        this->image = cimg_library::CImg<unsigned char>(width, height, 1, 3, color);
    }
    CImgTexture(GLuint width, GLuint height, const char* filename) {
        int img_width, img_height, bpp;
        unsigned char* rgb_image = stbi_load(filename, &img_width, &img_height, &bpp, 3);
        this->image = cimg_library::CImg<unsigned char>(rgb_image, width, height, 1, 3);
        stbi_image_free(rgb_image);
    }
    CImgTexture(const char* filename) {
        int img_width, img_height, bpp;
        unsigned char* rgb_image = stbi_load(filename, &img_width, &img_height, &bpp, 3);
        this->image = cimg_library::CImg<unsigned char>(rgb_image, img_width, img_height, 1, 3);
        stbi_image_free(rgb_image);
    }
    void clear() {
        this->image = cimg_library::CImg<unsigned char>(image.width(), image.height(), 1, 3, 255);
    }
    void set_rgb(glm::vec3 coord, const glm::vec3& color) {
        unsigned char rgb[3];
        for (size_t i = 0; i < 3; i++) {
            image(coord.x, coord.y, 0, i) = color[i];
            rgb[i] = color[i];
        }
    }
    void set_rgb(int x, int y, const glm::vec3& color) {
        unsigned char uch_color[3];
        for (size_t i = 0; i < 3; i++) {
            uch_color[i] = color[i];
        }
        image.draw_point(x, y, uch_color);
    }
    glm::vec3 get_uv(glm::vec2 uv) {
        return get_rgb(uv.x * (get_width() - 1), uv.y * (get_height() - 1));
    }
    void draw_st_line(glm::vec3 fc, glm::vec3 sc, glm::vec3 color) {
        unsigned char uch_color[3];
        for (size_t i = 0; i < 3; i++) {
            uch_color[i] = color[i];
        }
        image.draw_line(fc.x, fc.y, sc.x, sc.y, uch_color);
    }
    void draw_st_line(glm::vec2 fc, glm::vec2 sc, glm::vec3 color) {
        unsigned char uch_color[3];
        for (size_t i = 0; i < 3; i++) {
            uch_color[i] = color[i];
        }
        image.draw_line(fc.x, fc.y, sc.x, sc.y, uch_color);
    }
    glm::vec3 get_rgb(int x, int y) {
        glm::vec3 res;
        unsigned char uch_color[3];
        for (size_t i = 0; i < 3; i++) {
            res[i] = *(image.data() + x * 3 + y * image.width() * 3 + i);
        }
        return res;
    }
    glm::vec3 get_rgb(glm::vec3 coord) {
        return get_rgb(coord.x, coord.y);
    }
    glm::vec3 get_rgb(glm::vec2 coord) {
        return get_rgb(coord.x, coord.y);
    }
    int get_height() {
        return image.height();
    }
    int get_width() {
        return image.width();
    }
};

class ObjTexture : public Texture {
    static void fill_with_img(GLuint& id, CImgTexture& tex, const char flipTexture) {
        tex.image.RGBtosRGB();
        if (flipTexture == 'y') {
            tex.image.mirror('y');
        }
        else if (flipTexture == 'x') {
            tex.image.mirror('x');
        }
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex.get_width(), tex.get_height(),
            0, GL_RGB, GL_UNSIGNED_BYTE, (void*)tex.image.data());
    }
public:
    ObjTexture() {

    }
    ObjTexture(const ObjTexture& other) {
        this->id = other.id;
    }
    static ObjTexture get_raw_sample(const char* filename, const char flipTexture) {
        auto tex = ObjTexture();
        CImgTexture img(filename);
        fill_with_img(tex.id, img, flipTexture);
        glBindTexture(GL_TEXTURE_2D, 0);
        return tex;
    }
    ObjTexture(const char* filename, const char flipTexture) {
        CImgTexture tex(filename);
        fill_with_img(this->id, tex, flipTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
        initialized = true;
    }
};