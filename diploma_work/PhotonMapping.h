#pragma once
#include "Light.h"
#include "Tools.h"
#include "PMModel.h"
#include <stack>
#define _USE_MATH_DEFINES
#include <math.h>
#include "PhotonMap.h"
#include "Photon.h"
#include "PMTools.h"
#include "PMDrawer.h"

class PhotonMapping {
private:
    PhotonCollector photon_collector;
    MediumManager medium_manager;
    PhotonMap global_map;
    PhotonMap caustic_map;
    PMScene scene;
    PathOperator path_operator;
    PhotonMappingSettings settings;
    PMDrawer* drawer;
    void emit(const PMModel& ls);
    bool refract(float cosNL, const PMModel* ipmm);
    bool find_intersection(const Ray& ray, bool reverse_normal, 
        PMModel*& imodel, glm::vec3& normal, glm::vec3& inter_p);
    /// <summary>
    /// Возвращает значение, будет ли фотон отражен диффузно, зеркально или вовсе рассеян
    /// </summary>
    /// <param name="om">Origin material - Материал поверхности, изначальной поверхности</param>
    /// <param name="ipmm">Incident PMModel - Материал поверхности, на которую попал фотон</param>
    /// <param name="ipp">Light photon power — параметр испущенного из источника света фотона в RGB </param>
    /// <returns></returns>
    PathType destiny(float cosNL, const PMModel* ipmm, const glm::vec3& lphoton);
    /// <summary>
    /// Функция трассировки луча для составления фотонных карт
    /// </summary>
    /// <param name="ray">- ray...</param>
    /// <param name="pp">- photon power</param>
    void trace(const Ray& ray, bool in_object, const glm::vec3& pp);
    /// <summary>
    /// Функция трассировки луча для непосредственного рендеринга
    /// </summary>
    /// <param name="ray">- ray...</param>
    glm::vec3 render_trace(const Ray& ray, bool in_object, int depth, int i, int j);
    float BRDF(glm::vec3 direction, glm::vec3 location, glm::vec3 normal, const Material* mat);
    float GGX_GFunction(float cosNX, float sqRoughness); // Потерянный свет
    float GGX_DFunction(float cosNX, float sqRoughness);
    /// <summary>
    /// Returns amount of reflected light
    /// </summary>
    /// <param name="cosNL"> - angle between the light direction and the normal</param>
    /// <param name="n1"> - index of refreaction "from" medium</param>
    /// <param name="n2"> - index of refreaction "to" medium</param>
    /// <returns></returns>
    float FresnelSchlick(float cosNL, float n1, float n2);
    glm::vec3 FresnelSchlick(float cosNL, glm::vec3 F0);
    glm::vec3 CookTorrance_GGX(float NdotL, float NdotV, float NdotH, glm::vec3 F, float roughness);
public:
    PhotonMapping();
    void init(PMDrawer* drawer, std::vector<PMPreset>&& presets, PMSettingsUpdater& pmsu);
    void build_map();
    void render();
};