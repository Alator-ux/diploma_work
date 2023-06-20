#pragma once
#include "GL/glew.h"
#include <vector>
#include "Primitives.h"
#include <functional>
#include "ThreeDInterface.h"
/*
Immortal temptation
Takes over my mind, condemned
Falling weak on my knees
Summon the strength of mayhem

I am the storm that is approaching
Provoking black clouds in isolation
I am reclaimer of my name
Born in flames, I have been blessed
My family crest is a demon of death
Forsakened, I am awakened
A phoenix's ash in dark divine
Descending misery
Destiny chasing time

Inherit the nightmare, surrounded by fate
Can't run away
Keep walking the line, between the light
Led astray

Through vacant halls I won't surrender
The truth revealed in eyes of ember
We fight through fire and ice forever
Two souls once lost, and now they remember

I am the storm that is approaching
Provoking black clouds in isolation
I am reclaimer of my name
Born in flames, I have been blessed
My family crest is a demon of death
Forsakened, I am awakened
A phoenix's ash in dark divine
Descending misery
Destiny chasing time

Disappear into the night
Lost shadows left behind
Obsession's pulling me
Fading, I've come to take what's mine

Lurking in the shadows under veil of night
Constellations of blood pirouette
Dancing through the graves of those who stand at my feet
Dreams of the black throne I keep on repeat
A derelict of dark, summoned from the ashes
The puppet master congregates all the masses
Pulling strings, twisting minds as blades hit
You want this power? Then come try and take it

Beyond the tree, fire burns
Secret love, bloodline yearns
Dark minds embrace, crimson joy
Does your dim heart heal or destroy?

Bury the light deep within
Cast aside, there's no coming home
We're burning chaos in the wind
Drifting in the ocean all alone
*/


/// <summary>
/// ...и тут товарищ майор, вы не поверите,
/// выясняется, что грань фигуры - это фэйс
/// </summary>
class Face : public primitives::Polygon{
public:
    Face() {
        //drawing_type = GL_TRIANGLES;
        drawing_type = GL_LINE_STRIP;
    }
    Face(glm::vec3 coords, glm::vec3 color) {
        this->points.push_back(coords);
        this->colors.push_back(color);
        drawing_type = GL_LINE_STRIP;
        //this->drawing_type = GL_TRIANGLES;
    }
    void push_point(glm::vec3 coords) {
        points.push_back(coords);
    }
    void push_point(glm::vec3 coords, glm::vec3 color) {
        points.push_back(coords);
        colors.push_back(color);
    }
    bool primitive_is_finished() {
        if (points.size() < 3) {
            return false;
        }
        drawing_type = GL_LINE_LOOP;
        //drawing_type = GL_TRIANGLE_FAN;
        return true;

    }
    glm::vec3 center() const {
        float x = 0, y = 0, z = 0;
        auto res = glm::vec3(0.0f);
        for (auto point : points) {
            res += point;
        }
        size_t size = points.size();
        res /= size;
        return res;
    }
    glm::vec3 center() {
        float x=0, y=0, z=0;
        auto res = glm::vec3(0.0f);
        for (auto point : points) {
            res += point;
        }
        size_t size = points.size();
        res /= size;
        return res;
    }
};

class SkeletalFace : public Face {
public:
    SkeletalFace() {
        drawing_type = GL_LINE_STRIP;
    }
    bool primitive_is_finished() {
        if (points.size() > 1) {
            return false;
        }
        drawing_type = GL_LINE_LOOP;
        return true;
    }
};

/// <summary>
/// Ну, типа, фигура, да
/// </summary>
class Figure : public HighLevelInterface {
public:
    Figure() {
        objects = std::vector<primitives::Primitive>();
        type = ThreeDTypes::figure;
    }

    glm::vec3 center() {
        float x = 0, y = 0, z = 0;
        auto res = glm::vec3(0.0f);
        for (auto prim : objects) {
            Face* face = reinterpret_cast<Face*>(&prim);
            res += face->center();
        }
        size_t size = objects.size();
        res /= size;
        return res;
    }

    /// <summary>
    /// Трансформаторная будка
    /// ту-ту-ту
    /// </summary>
    void transform(std::function<glm::vec3(glm::vec3)> transofrmator) {
        for (auto i = 0; i < objects.size(); i++) {
            for (auto j = 0; j < objects[i].points.size(); j++) {
                objects[i].points[j] = transofrmator(objects[i].points[j]);
            }
        }
    }
    
    void transform(const glm::mat4x4& transform_matrix) {
        this->transform([transform_matrix](glm::vec3 p)->glm::vec3 {
            return transform_matrix * glm::vec4(p.x, p.y, p.z, 1);
            });
    }
};


class Tetrahedron : public Figure {};

class Octahedron : public Figure {};

class Hexahedron : public Figure {};

class Icosahedron : public Figure {};

class Dodecahedron : public Figure {};

class FuncFigure : public Figure {};

class CubeFigure : public Figure {};

enum FigureType {
    Tetrahed, Octahed,
    Hexahed, Icosahed,
    Dodecahed, Cube
};
