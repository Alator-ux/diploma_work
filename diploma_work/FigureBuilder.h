#pragma once
#include "Figure.h"
#include "STLLoader.h"
#include <vector>
class FigureBuilder {
    float bound = 100;
public:
    Figure buildFigure(FigureType kind, glm::vec3 color) {
        switch (kind) {
        case Tetrahed:
            return buildTetrahedron(color);
        case Hexahed:
            return buildHexahedron(color);
        case Octahed:
            return buildOctahedron(color);
        case Icosahed:
            return buildIcosahedron(color);
        case Dodecahed:
            return buildDodecahedron(color);
        case Cube:
            return buildCube(color);
        }
    }
    Dodecahedron buildDodecahedron(glm::vec3 color) {
        auto r = STL::load_from_file("./models/ddc.stl");
        Dodecahedron* res = reinterpret_cast<Dodecahedron*>(&r);
        return *res;
    }
    CubeFigure buildCube(glm::vec3 color) {
        GLfloat len = 2.f;
        glm::vec3 flt(-len, len, len), brb(len, -len, -len);
        
        auto cube = CubeFigure();
        Face front = Face(flt, glm::vec3(1.f, 0.f, 0.f));
        front.push_point({ -len, -len, len }, glm::vec3(1.f, 1.f, 1.f));
        front.push_point({ len, -len, len }, glm::vec3(0.f, 0.f, 1.f));
        front.push_point({ len, len, len }, glm::vec3(0.f,1.f,0.f));
        front.set_uv_vec({
            glm::vec2(0,0), glm::vec2(0,1),glm::vec2(1,1), glm::vec2(1,0)
            });

        Face left = Face(flt, glm::vec3(1.f, 0.f, 0.f));
        left.push_point({ -len, len, -len }, glm::vec3(1.f, 1.f, 0.f));
        left.push_point({ -len, -len, -len }, glm::vec3(1.f, 0.f, 1.f));
        left.push_point({ -len, -len, len }, glm::vec3(1.f, 1.f, 1.f));
        left.set_uv_vec({
            glm::vec2(1,1), glm::vec2(0,1),glm::vec2(0,0), glm::vec2(1,0)
            });

        Face top = Face(flt, glm::vec3(1.f, 0.f, 0.f));
        top.push_point({ len, len, len }, glm::vec3(0.f, 1.f, 0.f));
        top.push_point({ len, len, -len }, glm::vec3(0.f, 1.f, 1.f));
        top.push_point({ -len, len, -len }, glm::vec3(1.f, 1.f, 0.f));
        top.set_uv_vec({
            glm::vec2(0,0), glm::vec2(0,1),glm::vec2(1,1), glm::vec2(1,0)
            });

        Face right = Face(brb, glm::vec3(0.25f, 0.75f, 0.f));
        right.push_point({ len, len, -len }, glm::vec3(0.f, 1.f, 1.f));
        right.push_point({ len, len, len }, glm::vec3(0.f, 1.f, 0.f));
        right.push_point({ len, -len, len }, glm::vec3(0.f, 0.f, 1.f));
        right.set_uv_vec({
            glm::vec2(1,0), glm::vec2(1,1),glm::vec2(0,1), glm::vec2(0,0)
            });

        Face bot = Face(brb, glm::vec3(0.25f, 0.75f, 0.f));
        bot.push_point({ len, -len, len }, glm::vec3(0.f, 0.f, 1.f));
        bot.push_point({ -len, -len, len }, glm::vec3(1.f, 1.f, 1.f));
        bot.push_point({ -len, -len, -len }, glm::vec3(1.f, 0.f, 1.f));
        bot.set_uv_vec({
            glm::vec2(1,0), glm::vec2(1,1),glm::vec2(0,1), glm::vec2(0,0)
            });

        Face back = Face(brb, glm::vec3(0.25f, 0.75f, 0.f));
        back.push_point({ -len, -len, -len }, glm::vec3(1.f, 0.f, 1.f));
        back.push_point({ -len, len, -len }, glm::vec3(1.f, 1.f, 0.f));
        back.push_point({ len, len, -len }, glm::vec3(0.f, 1.f, 1.f));
        back.set_uv_vec({
            glm::vec2(0,0), glm::vec2(1,0),glm::vec2(1,1), glm::vec2(0,1)
            });

        cube.objects.push_back(front);
        cube.objects.push_back(left);
        cube.objects.push_back(right);
        cube.objects.push_back(back);
        cube.objects.push_back(top);
        cube.objects.push_back(bot);
        return cube;
    }
    Tetrahedron buildTetrahedron(glm::vec3 color) {
        auto tetr = Tetrahedron();
        glm::vec3 a = glm::vec3(0, 0, 0);
        glm::vec3 c = glm::vec3(bound / 2, 0, -bound / 2);
        glm::vec3 f = glm::vec3(bound / 2, bound / 2, 0);
        glm::vec3 h = glm::vec3(0, bound / 2, -bound / 2);
        a /= bound;
        c /= bound;
        f /= bound;
        h /= bound;

        auto ac = glm::vec3(1.f, 0.f, 0.f);
        auto fc = glm::vec3(0.f, 1, 0.f);
        auto cc = glm::vec3(0.f, 0.f, 1);
        auto hc = glm::vec3(1.f, 1.f, 1.f);

        Face face1 = Face(a, ac);
        face1.push_point(f, fc);
        face1.push_point(c, cc);
        face1.set_uv_vec({
            glm::vec2(0, 0),
            glm::vec2(1, 0),
            glm::vec2(0, 1) });
        face1.primitive_is_finished();

        Face face2 = Face(f, fc);
        face2.push_point(c, cc);
        face2.push_point(h, hc);
        face2.set_uv_vec({
            glm::vec2(0, 0),
            glm::vec2(1, 0),
            glm::vec2(0, 1) });
        face2.primitive_is_finished();

        Face face3 = Face(c, cc);
        face3.push_point(h, hc);
        face3.push_point(a, ac);
        face3.set_uv_vec({
            glm::vec2(0, 0),
            glm::vec2(1, 0),
            glm::vec2(0, 1) });
        face3.primitive_is_finished();

        Face face4 = Face(f, fc);
        face4.push_point(h, hc);
        face4.push_point(a, ac);
        face4.set_uv_vec({
            glm::vec2(0, 0),
            glm::vec2(1, 0),
            glm::vec2(0, 1) });
        face4.primitive_is_finished();

        tetr.objects.push_back(face1);
        tetr.objects.push_back(face2);
        tetr.objects.push_back(face3);
        tetr.objects.push_back(face4);
        return tetr;

    }

    Hexahedron buildHexahedron(glm::vec3 color) {
        auto hexa = Hexahedron();
        glm::vec3 a = glm::vec3(0, 0, 0);
        glm::vec3 b = glm::vec3(bound / 2, 0, 0);
        glm::vec3 c = glm::vec3(bound / 2, 0, -bound / 2);
        glm::vec3 d = glm::vec3(0, 0, -bound / 2);
        glm::vec3 e = glm::vec3(0, bound / 2, 0);
        glm::vec3 f = glm::vec3(bound / 2, bound / 2, 0);
        glm::vec3 g = glm::vec3(bound / 2, bound / 2, -bound / 2);
        glm::vec3 h = glm::vec3(0, bound / 2, -bound / 2);
        a /= bound;
        b /= bound;
        c /= bound;
        d /= bound;
        e /= bound;
        f /= bound;
        g /= bound;
        h /= bound;

        Face face1 = Face(a, glm::vec3(1.f, 0.f, 0.f));
        face1.push_point(b);
        face1.push_point(c);
        face1.push_point(d);
        face1.primitive_is_finished();

        Face face2 = Face(b, glm::vec3(0.f, 1.f, 0.f));
        face2.push_point(c);
        face2.push_point(g);
        face2.push_point(f);
        face2.primitive_is_finished();

        Face face3 = Face(f, glm::vec3(0.f, 0.f, 1.f));
        face3.push_point(g);
        face3.push_point(h);
        face3.push_point(e);
        face3.primitive_is_finished();

        Face face4 = Face(h, glm::vec3(1.f, 1.f, 0.f));
        face4.push_point(e);
        face4.push_point(a);
        face4.push_point(d);
        face4.primitive_is_finished();

        Face face5 = Face(a, glm::vec3(1.f, 0.f, 1.f));
        face5.push_point(b);
        face5.push_point(f);
        face5.push_point(e);
        face5.primitive_is_finished();


        Face face6 = Face(d, glm::vec3(0.f, 1.f, 1.f));
        face6.push_point(c);
        face6.push_point(g);
        face6.push_point(h);
        face6.primitive_is_finished();

        hexa.objects.push_back(face1);
        hexa.objects.push_back(face2);
        hexa.objects.push_back(face3);
        hexa.objects.push_back(face4);
        hexa.objects.push_back(face5);
        hexa.objects.push_back(face6);

        return hexa;
    }

    Octahedron buildOctahedron(glm::vec3 color) {
        auto res = Octahedron();
        auto core = buildHexahedron(color);
        Face* face = reinterpret_cast<Face*>(&core.objects[0]);
        auto a = face->center();
        face = reinterpret_cast<Face*>(&core.objects[1]);
        auto b = face->center();
        face = reinterpret_cast<Face*>(&core.objects[2]);
        auto c = face->center();
        face = reinterpret_cast<Face*>(&core.objects[3]);
        auto d = face->center();
        face = reinterpret_cast<Face*>(&core.objects[4]);
        auto e = face->center();
        face = reinterpret_cast<Face*>(&core.objects[5]);
        auto f = face->center();

        Face face1 = Face(a, glm::vec3(1.f, 0.f, 0.f));
        face1.push_point(f);
        face1.push_point(b);
        face1.primitive_is_finished();

        Face face2 = Face(b, glm::vec3(0.f, 1.f, 0.f));
        face2.push_point(c);
        face2.push_point(f);
        face2.primitive_is_finished();

        Face face3 = Face(c, glm::vec3(0.f, 0.f, 1.f));
        face3.push_point(d);
        face3.push_point(f);
        face3.primitive_is_finished();

        Face face4 = Face(d, glm::vec3(1.f, 1.f, 0.f));
        face4.push_point(a);
        face4.push_point(f);
        face4.primitive_is_finished();

        Face face5 = Face(a, glm::vec3(1.f, 0.f, 1.f));
        face5.push_point(e);
        face5.push_point(b);
        face5.primitive_is_finished();

        Face face6 = Face(b, glm::vec3(0.f, 1.f, 1.f));
        face6.push_point(e);
        face6.push_point(c);
        face6.primitive_is_finished();

        Face face7 = Face(c, glm::vec3(1.f, 1.f, 1.f));
        face7.push_point(e);
        face7.push_point(d);
        face7.primitive_is_finished();

        Face face8 = Face(d, glm::vec3(0.5f, 0.5f, 0.5f));
        face8.push_point(e);
        face8.push_point(a);
        face8.primitive_is_finished();

        res.objects.push_back(face1);
        res.objects.push_back(face2);
        res.objects.push_back(face3);
        res.objects.push_back(face4);
        res.objects.push_back(face5);
        res.objects.push_back(face6);
        res.objects.push_back(face7);
        res.objects.push_back(face8);
        return res;
    }

    Icosahedron buildIcosahedron(glm::vec3 color) {
        auto r = STL::load_from_file("./models/icosahedron.stl");
        Icosahedron* res = reinterpret_cast<Icosahedron*>(&r);
        return *res;

    }
};