#pragma once
#include "OpenGLWrappers.h"
#include "ThreeDInterface.h"
#include "Projection.h"
#include "Camera.h"
#include "Figure.h"
class Drawer {
    //GLuint vPos;
    //Shader* shader;
    OpenGLManager* manager;
    /*template <typename T>
    void prepare_for_drawing(std::vector<T>& elements, const std::string& buffer_name,
        Camera& camera) {
        if (elements.size() == 0) {
            return;
        }
        shader->use_program();
        glEnableVertexAttribArray(vPos);

        glBindBuffer(GL_ARRAY_BUFFER, manager->get_buffer_id(buffer_name));
        glVertexAttribPointer(vPos, 3, GL_FLOAT, GL_FALSE, 0, 0);
        manager->checkOpenGLerror();
    }
    void end_drawing() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray(vPos);
        shader->disable_program();
        manager->checkOpenGLerror();
    }*/
public:
    Drawer() {
    }
    Drawer(GLuint w_width, GLuint w_height) {
        //this->vPos = this->shader->get_attrib_location(vPosName);
        this->manager = OpenGLManager::get_instance();
    }
   /* void draw(std::vector<primitives::Primitive>& primitives, const std::string& buffer_name,
        Camera& camera) {
        prepare_for_drawing(primitives, buffer_name, camera);
        GLint from = 0;
        GLint count = 0;
        for (primitives::Primitive& pr : primitives) {
            glLineWidth(pr.width);
            count = pr.get_points_count();
            shader->uniform4f("color", pr.colors[0]);
            glDrawArrays(pr.drawing_type, from, count);
            from += count;
        }
        end_drawing();
    }*/
    void draw_tAc(Shader* shader, HighLevelInterface& hghlvl) {
        shader->use_program();
        auto vPos = shader->get_attrib_location("vPos");
        glEnableVertexAttribArray(vPos);
        glBindBuffer(GL_ARRAY_BUFFER, manager->get_buffer_id("Cube Pos"));
        glVertexAttribPointer(vPos, 3, GL_FLOAT, GL_FALSE, 0, 0);

        auto vColor = shader->get_attrib_location("vColor");
        glEnableVertexAttribArray(vColor);
        glBindBuffer(GL_ARRAY_BUFFER, manager->get_buffer_id("Cube Colors"));
        glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

        auto vTex = shader->get_attrib_location("vTex");
        glEnableVertexAttribArray(vTex);
        glBindBuffer(GL_ARRAY_BUFFER, manager->get_buffer_id("Cube Tex"));
        glVertexAttribPointer(vTex, 2, GL_FLOAT, GL_FALSE, 0, 0);

        manager->get_texture("brick").bind();

        GLint from = 0;
        GLint count = 0;
        for (primitives::Primitive& pr : hghlvl.objects) {
            glLineWidth(pr.width);
            count = pr.get_points_count();
            glDrawArrays(GL_QUADS, from, count);
            from += count;
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray(vPos);
        glDisableVertexAttribArray(vColor);
        glDisableVertexAttribArray(vTex);
        shader->disable_program();
        manager->checkOpenGLerror();
    }
    void draw_tAt(Shader* shader, HighLevelInterface& hghlvl) {
        shader->use_program();
        auto vPos = shader->get_attrib_location("vPos");
        glEnableVertexAttribArray(vPos);
        glBindBuffer(GL_ARRAY_BUFFER, manager->get_buffer_id("Cube Pos"));
        glVertexAttribPointer(vPos, 3, GL_FLOAT, GL_FALSE, 0, 0);

        auto vTex = shader->get_attrib_location("vTex");
        glEnableVertexAttribArray(vTex);
        glBindBuffer(GL_ARRAY_BUFFER, manager->get_buffer_id("Cube Tex"));
        glVertexAttribPointer(vTex, 2, GL_FLOAT, GL_FALSE, 0, 0);

        manager->get_texture("brick").bind();
        manager->get_texture("idk").bind(1);

        GLint from = 0;
        GLint count = 0;
        for (primitives::Primitive& pr : hghlvl.objects) {
            glLineWidth(pr.width);
            count = pr.get_points_count();
            glDrawArrays(GL_QUADS, from, count);
            from += count;
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray(vPos);
        glDisableVertexAttribArray(vTex);
        shader->disable_program();
        manager->checkOpenGLerror();
    }
    void draw_tetr(Shader* shader, HighLevelInterface& hghlvl) {
        shader->use_program();
        auto vPos = shader->get_attrib_location("vPos");
        glEnableVertexAttribArray(vPos);
        glBindBuffer(GL_ARRAY_BUFFER, manager->get_buffer_id("Tetr Pos"));
        glVertexAttribPointer(vPos, 3, GL_FLOAT, GL_FALSE, 0, 0);

        auto vColor = shader->get_attrib_location("vColor");
        glEnableVertexAttribArray(vColor);
        glBindBuffer(GL_ARRAY_BUFFER, manager->get_buffer_id("Tetr Colors"));
        glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);

        GLint from = 0;
        GLint count = 0;
        for (primitives::Primitive& pr : hghlvl.objects) {
            glLineWidth(pr.width);
            count = pr.get_points_count();
            glDrawArrays(GL_TRIANGLES, from, count);
            from += count;
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray(vPos);
        glDisableVertexAttribArray(vColor);
        shader->disable_program();
        manager->checkOpenGLerror();
    }
    void draw_ellipse(Shader* shader, primitives::Primitive& prim) {
        shader->use_program();
        auto vPos = shader->get_attrib_location("vPos");
        glEnableVertexAttribArray(vPos);
        glBindBuffer(GL_ARRAY_BUFFER, manager->get_buffer_id("ellipse vPos"));
        glVertexAttribPointer(vPos, 3, GL_FLOAT, GL_FALSE, 0, 0);

        /*auto vColor = shader->get_attrib_location("vColor");
        glEnableVertexAttribArray(vColor);
        glBindBuffer(GL_ARRAY_BUFFER, manager->get_buffer_id("ellipse vColors"));
        glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
        manager->checkOpenGLerror();*/

        GLint from = 0;
        GLint count = 0;
        count = prim.get_points_count();
        glDrawArrays(GL_TRIANGLE_FAN, from, count);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray(vPos);
        //glDisableVertexAttribArray(vColor);
        shader->disable_program();
        manager->checkOpenGLerror();
    }
    void set_vbo(const std::string& buffer_name, const std::vector<primitives::Primitive>& data) {
        if (data.size() == 0) {
            return;
        }
        std::vector<glm::vec3> ndata;
        GLuint size = 0;
        for (auto pr : data) {
            ndata.insert(ndata.end(), pr.points.begin(), pr.points.end());
            size += GLuint(sizeof(GLfloat) * pr.points.size() * 3);
        }
        manager->init_vbo(buffer_name, &ndata[0], size, GL_STATIC_DRAW);
    }
    void set_vbo(const std::string& pos_name, const std::string& color_name,
        const std::vector<primitives::Primitive>& data) {
        if (data.size() == 0) {
            return;
        }
        std::vector<glm::vec3> ndata;
        std::vector<glm::vec3> ncolor;
        GLuint size = 0;
        for (auto pr : data) {
            ndata.insert(ndata.end(), pr.points.begin(), pr.points.end());
            ncolor.insert(ncolor.end(), pr.colors.begin(), pr.colors.end());
            size += GLuint(sizeof(GLfloat) * pr.points.size() * 3);
        }
        manager->init_vbo(pos_name, &ndata[0], size, GL_STATIC_DRAW);
        manager->init_vbo(color_name, &ncolor[0], size, GL_STATIC_DRAW);
    }
    void set_vbo(const std::string& pos_name, const std::string& color_name, 
        const std::string& tex_name, const std::vector<Figure>& data) {
        if (data.size() == 0) {
            return;
        }
        std::vector<glm::vec3> ndata;
        std::vector<glm::vec3> ncolors;
        std::vector<glm::vec2> ntex;
        GLuint size = 0;
        GLuint size_2 = 0;
        for (auto& fig : data) {
            for (auto& pr : fig.objects) {
                ndata.insert(ndata.end(), pr.points.begin(), pr.points.end());
                ncolors.insert(ncolors.end(), pr.colors.begin(), pr.colors.end());
                ntex.insert(ntex.end(), pr.uv_vec.begin(), pr.uv_vec.end());
                size += GLuint(sizeof(GLfloat) * pr.points.size() * 3);
                size_2 += GLuint(sizeof(GLfloat) * pr.points.size() * 2);
            }
        }
        manager->init_vbo(pos_name, &ndata[0], size, GL_STATIC_DRAW);
        manager->init_vbo(color_name, &ncolors[0], size, GL_STATIC_DRAW);
        manager->init_vbo(tex_name, &ntex[0], size_2, GL_STATIC_DRAW);
    }
    void set_vbo(const std::string& pos_name, const std::string& color_name,
        const std::vector<Figure>& data) {
        if (data.size() == 0) {
            return;
        }
        std::vector<glm::vec3> ndata;
        std::vector<glm::vec3> ncolors;
        GLuint size = 0;
        for (auto& fig : data) {
            for (auto& pr : fig.objects) {
                ndata.insert(ndata.end(), pr.points.begin(), pr.points.end());
                ncolors.insert(ncolors.end(), pr.colors.begin(), pr.colors.end());
                size += GLuint(sizeof(GLfloat) * pr.points.size() * 3);
            }
        }
        manager->init_vbo(pos_name, &ndata[0], size, GL_STATIC_DRAW);
        manager->init_vbo(color_name, &ncolors[0], size, GL_STATIC_DRAW);
    }
};