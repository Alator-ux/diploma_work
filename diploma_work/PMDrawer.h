#pragma once
#include "Texture.h"
#include "Tools.h"
#include "CImg.h"
#include <GLM/exponential.hpp>
#include "PMSettingsUpdater.h"
#include "OpenGLWrappers.h"
class PMDrawer
{
public:
    /// <summary>
    /// di - direct illumination;
    /// sp - specular;
    /// tr - transmission;
    /// em - emission
    /// gi - global illumination;
    /// ca - caustic;
    /// </summary>
    enum Layer { di = 0, sp, tr, em, gi, ca };
private:
    enum ShaderT {layer = 0, fxaa = 1, ssaa = 2, final = 3 };
    struct Canvas {
        unsigned int quadVAO;
        void init();
        void draw();
    };
    class FrameBuffer {
    protected:
        std::string fb100x100, cb100x100;
        std::string fb300x300, cb300x300;
        std::string fb600x600, cb600x600;
        std::string fb_cur, cb_cur;
        OpenGLManager* manager;
        void gen_fb(const std::string& fbname, const std::string& cbname, size_t width, size_t height);
    public:
        FrameBuffer();
        FrameBuffer(const FrameBuffer& other);
        FrameBuffer(const std::string& pref);
        void set_resolution(size_t width, size_t height);
        void bind_fb();
        void unbind_fb();
        const std::string& get_fb();
        const std::string& get_cb();
    };
    class TwoOutFrameBuffer : public FrameBuffer {
        std::string cb100x100_other, cb300x300_other, cb600x600_other;
        std::string cb_cur_other;
        void gen_fb(const std::string& fbname, const std::string& cbname, const std::string& cbname_other,
            size_t width, size_t height);
    public:
        TwoOutFrameBuffer();
        TwoOutFrameBuffer(const TwoOutFrameBuffer& other);
        TwoOutFrameBuffer(const std::string& pref);
        void set_resolution(size_t width, size_t height);
        const std::string& get_ocb();
    };
    class BloomShader {
        Shader shader;
        PMDrawer* dr;
        OpenGLManager* manager;
    public:
        bool horiz;
        BloomShader();
        void init(PMDrawer* dr);
        void draw(size_t count, FrameBuffer& first, FrameBuffer& second,
            TwoOutFrameBuffer& orig);
        void update();
    };
    class BloomMergerShader {
        Shader shader;
        PMDrawer* dr;
        OpenGLManager* manager;
    public:
        BloomMergerShader();
        void init(PMDrawer* dr);
        void draw(bool horiz, FrameBuffer& first, FrameBuffer& second, FrameBuffer& main);
        void update();
    };
    class GrayGrainShader {
        Shader shader;
        PMDrawer* dr;
        OpenGLManager* manager;
    public:
        GrayGrainShader();
        void init(PMDrawer* dr);
        void draw(FrameBuffer& in_buffer, FrameBuffer& out_buffer);
        void update();
    };
    const size_t layers_number;
    std::vector<Matrix<glm::vec3>> layers;
    PMDrawerSettings settings;
    std::vector<Texture> textures;
    std::vector<Shader> shaders;
    OpenGLManager* manager;
    Canvas canvas;
    FrameBuffer buffer1, buffer2, buffer3;
    TwoOutFrameBuffer tobuffer1;
    BloomShader bloom_shader;
    BloomMergerShader bloom_merge_shader;
    GrayGrainShader gray_grain_shader;
    void update_textures();
    void updateLayerShader();
    void updateFXAAShader();
    void updateSSAAShader();
    void update_resolution(size_t width, size_t height);
    void update_framebuffer(size_t width, size_t height);
    void update();
    void hdr(glm::vec3& dest);
    void fill_texture(Texture& tex, glm::vec3* data);
public:
    void opengl_init();
    PMDrawer(size_t width, size_t height);
    void set_rgb(int i, int j, const glm::vec3& color, Layer layer, int depth);
    void display();
    void clear();
    void set_settings_updater(PMSettingsUpdater& pmsu);
    void check_resolution();
    int get_width();
    int get_height();
};