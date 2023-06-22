#define STB_IMAGE_IMPLEMENTATION
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "OpenGLWrappers.h"
#include "imgui.h"
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include "MainWindow.h"
#include "Camera.h"
#include "Texture.h"
#include "ObjModel.h"
#include "Tools.h"
#include "PMModel.h"
#include "PhotonMapping.h"
#include "Photon.h"
const GLuint W_WIDTH = 600;
const GLuint W_HEIGHT = 600;
PMDrawer drawer(600, 600);
PMSettingsUpdater pmsu;
PhotonMapping pm;
void Init(OpenGLManager*);
void Release();

int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(W_WIDTH, W_HEIGHT, "Diploma window", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    const char* glsl_version = "#version 410";
    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    glewInit();
    srand(static_cast <unsigned> (time(0)));
    auto manager = OpenGLManager::get_instance();
    Init(manager);
    drawer.opengl_init();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImGui::StyleColorsDark();

    // pmsu.update_xstart(300);

    std::unique_ptr<Window> main_window(new MainWindow(&pmsu, &pm, &drawer));

    bool show_demo_window = false;
    std::string vbo_name = "";
    pmsu.window_settings.width = pmsu.window_settings.output->width + 500;
    pmsu.window_settings.height = pmsu.window_settings.output->height;
    glfwSetWindowSize(window, pmsu.window_settings.width, pmsu.window_settings.height);
    while (!glfwWindowShouldClose(window))
    {
        if (pmsu.window_settings.changed.resolution) {
            //glfwSetWindowSize(window, 
            //    pmsu.window_settings.output->width + 500, pmsu.window_settings.output->height);
            glfwSetWindowSize(window,
                pmsu.window_settings.output->width + 500, pmsu.window_settings.output->height);
        }
        glfwPollEvents();
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        main_window.get()->draw();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwMakeContextCurrent(window);
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawer.display();
        double time = glfwGetTime();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void Release() {
    OpenGLManager::get_instance()->release();
}

void load_preset(const std::string path, const std::string fname, std::vector<PMPreset>& presets) {
    auto map = loadOBJ(path, fname);
    glm::vec3 lspos(0.f);
    std::vector<PMModel> scene;
    PMPreset preset;
    for (auto& kv : map) {
        LightSource* ls = nullptr;
        if (kv.name == "light") {
            std::for_each(kv.vertices.begin(), kv.vertices.end(), [&lspos](const ObjVertex& vert) {lspos += vert.position;});
            lspos /= kv.vertices.size();
            ls = new LightSource(lspos);
        }
        PMModel m(kv, ls);
        preset.objects.push_back(m);
    }
    presets.push_back(preset);
}

void Init(OpenGLManager* manager) {
    Random<unsigned>::set_seed();

    std::vector<PMPreset> presets;
    load_preset("./models/cornell_box_original", "CornellBox-Original.obj", presets);
    presets.back().pos = glm::vec3(-0.00999999046, 0.795000017, 2.35000001);
    presets.back().dir = glm::vec3(0.f, 0.f, -1.f);
    //presets.back().global_map_path = "photon_maps"

    load_preset("./models/cornell_box_sphere", "CornellBox-Sphere.obj", presets);
    presets.back().pos = glm::vec3(-0.00999999046, 0.795000017, 2.35000001);
    presets.back().dir = glm::vec3(0.f, 0.f, -1.f);

    //load_preset("./models/wine_glass", "glasses.obj", presets);
    load_preset("./models/diamond", "diamond.obj", presets);
    presets.back().pos = glm::vec3(2.f, 1.f, 4.f);
    presets.back().dir = glm::vec3(-0.3f, -0.21f, -0.87f);

    load_preset("./models/flagon", "flagon.obj", presets);
    presets.back().pos = glm::vec3(2.f, 2.f, 4.f);
    presets.back().dir = glm::normalize(glm::vec3(-0.4f, -0.31f, -0.87f));

    load_preset("./models/wine_glass", "shot_glass.obj", presets);
    presets.back().pos = glm::vec3(0.25f, 0.5f, 2.f);
    presets.back().dir = glm::normalize(glm::vec3(-0.4f, -0.31f, -0.87f));

    load_preset("./models/ring", "metal_ring.obj", presets);
    presets.back().pos = glm::vec3(0.f, 1.f, 1.f);
    presets.back().dir = glm::normalize(-presets.back().pos);

    load_preset("./models/cornell_box_water", "CornellBox-Water.obj", presets);
    presets.back().pos = glm::vec3(-0.00999999046, 0.795000017, 1.99000001);
    presets.back().dir = glm::vec3(0.f, 0.f, -1.f);

    //auto map = loadOBJ("./models/cornell_box_original", "CornellBox-Original.obj");
    //auto map = loadOBJ("./models/cornell_box_sphere", "CornellBox-Sphere.obj");
    //auto map = loadOBJ("./models/cornell_box_water", "CornellBox-Water.obj");
    //auto map = loadOBJ("./models/cornell_box_high_water", "cornellbox-water2.obj");
    //auto map = loadOBJ("./models/wine_glass", "WineGlasses.obj");
    //auto map = loadOBJ("./models/ring", "metal_ring.obj");
    //auto map = loadOBJ("./models/wine_glass", "glasses.obj");

    pm.init(&drawer, std::move(presets), pmsu);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 1);
    manager->checkOpenGLerror();
}