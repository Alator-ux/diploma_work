#pragma once

struct PhotonMapSettings {
    enum FilterType { none = 0, cone, gaussian };
    FilterType ftype = none;
    size_t np_size = 2000;
    // Cone filter coef k
    const float cf_k = 1.1f; // Must be >= 1
    // Gaussian filter coef alpha
    const float gf_alpha = 1.818f;
    // Gaussian filter coef beta
    const float gf_beta = 1.953f;
    float disc_compression = 0.f;
};

struct PhotonMappingSettings {
    // disable photon mapping for direct illumination
    bool dpmdi = false;
    int max_rt_depth = 5;
    float ca_mult = 1.f;
    float gl_mult = 1.f;
};

struct PMSceneSettings {
    int active_preset;
};

struct PhotonCollectorSettings {
    bool update = false;
    size_t gsize = 0;
    size_t csize = 0;
};

struct PMDrawerSettings {
    struct Changed {
        bool resolution = false;
        bool layers = false;
        bool render = false;
        bool layerShader = false;
        bool bloomMergerShader = false;
        bool grainShader = false;
    };
    struct LayerShader {
        bool hdr = true;
        float ca_mult = 1.f;
        float gl_mult = 1.f;
        float exposure = 1.f;
        float brightness = 1.f;
        float gamma = 2.2f;
        std::vector<int> active;
    };
    struct BloomMergeShader {
        float bloom_intensity;
        float sigma;
        bool calc_sigma;
        int kernel_size;
    };
    struct GrainShader {
        bool on = false;
        float amount;
        float size;
    };
    Changed changed;
    LayerShader layerShader;
    BloomMergeShader bloomMergeShader;
    GrainShader grainShader;
    size_t width, height, xstart;
};

struct WindowSettings {
    struct Changed {
        bool resolution;
    };
    Changed changed;
    const PMDrawerSettings* output;
    size_t width = 600;
    size_t height = 600;
};


class PMSettingsUpdater {
    PhotonMappingSettings* main_settings;
    PhotonMapSettings* gmap_settings;
    PhotonMapSettings* cmap_settings;
    PhotonCollectorSettings* pc_settings;
    PMDrawerSettings* d_settings;
    PMSceneSettings* sc_settings;
public:
    WindowSettings window_settings;
    void link_main(PhotonMappingSettings* ms) {
        main_settings = ms;
    }
    void link_gmap(PhotonMapSettings* gms) {
        gmap_settings = gms;
    }
    void link_cmap(PhotonMapSettings* cms) {
        cmap_settings = cms;
    }
    void link_pc(PhotonCollectorSettings* pc) {
        pc_settings = pc;
    }
    void link_drawer(PMDrawerSettings* drawer) {
        d_settings = drawer;
        window_settings.output = drawer;
    }
    void link_scene(PMSceneSettings* sc) {
        sc_settings = sc;
    }
    void update_preset(int value) {
        sc_settings->active_preset = value;
    }
    void update_rt_depth(int value) {
        main_settings->max_rt_depth = value;
    }
    /* ----------Main Settings---------- */
    /// <summary>
    /// Update disabling photon mapping for direct illumination parameter
    /// </summary>
    /// <param name="value"> - if true — disable. if false - enable</param>
    /// <returns></returns>
    void update_dpmdi(bool value) {
        main_settings->dpmdi = value;
    }
    void update_max_rt(size_t depth) {
        main_settings->max_rt_depth = depth;
    }

    /* ----------Photon Maps Settings---------- */
    /// <summary>
    /// Update the number of photons to be emitted
    /// </summary>
    /// <param name="phc"> - photons count</param>
    void update_gphc(size_t gphc) {
        pc_settings->gsize = gphc;
        pc_settings->update = true;
    }
    void update_cphc(size_t cphc) {
        pc_settings->csize = cphc;
        pc_settings->update = true;
    }
    void update_gnp_count(size_t count) {
        gmap_settings->np_size = count;
    }
    void update_cnp_count(size_t count) {
        cmap_settings->np_size = count;
    }
    void update_disc_compression(float coef) {
        gmap_settings->disc_compression = coef;
        cmap_settings->disc_compression = coef;
    }
    void update_gfilter(int filter) {
        if (filter < 0 || filter > 2) {
            return;
        }
        gmap_settings->ftype = (PhotonMapSettings::FilterType)filter;
    }
    void update_cfilter(int filter) {
        if (filter < 0 || filter > 2) {
            return;
        }
        cmap_settings->ftype = (PhotonMapSettings::FilterType)filter;
    }
    /* ----------PhotonMapping Settings---------- */
    void update_rcaustic_multiplier(float value) {
        main_settings->ca_mult = value;
    }
    void update_rglobal_multiplier(float value) {
        main_settings->gl_mult = value;
    }
    /* ----------Drawer => LayerShader Settings---------- */
    void update_resolution(size_t width, size_t height) {
        d_settings->width = width;
        d_settings->height = height;
        d_settings->changed.resolution = true;
        window_settings.changed.resolution = true;
    }
    void update_exposure(float exposure) {
        d_settings->layerShader.exposure = exposure;
        d_settings->changed.layerShader = true;
    }
    void update_brightness(float brightness) {
        d_settings->layerShader.brightness = brightness;
        d_settings->changed.layerShader = true;
    }
    void update_dcaustic_multiplier(float value) {
        d_settings->layerShader.ca_mult = value;
        d_settings->changed.layerShader = true;
    }
    void update_dglobal_multiplier(float value) {
        d_settings->layerShader.gl_mult = value;
        d_settings->changed.layerShader = true;
    }
    void update_active_layers(size_t ind, bool value) {
        d_settings->layerShader.active[ind] = value;
        d_settings->changed.layerShader = true;
    }
    void update_hdr(bool value) {
        d_settings->layerShader.hdr = value;
        d_settings->changed.layerShader = true;
    }
    void update_layers_status(bool value) {
        d_settings->changed.layers = value;
    }
    void update_render_status(bool value) {
        d_settings->changed.render = value;
    }
    /* ----------Drawer => BloomShader Settings---------- */
    void update_bloom_intensity(float value) {
        d_settings->bloomMergeShader.bloom_intensity = value;
        d_settings->changed.bloomMergerShader = true;
    }
    void update_bloom_sigma(float value) {
        d_settings->bloomMergeShader.sigma = value;
        d_settings->changed.bloomMergerShader = true;
    }
    void update_bloom_kernel_size(int value) {
        d_settings->bloomMergeShader.kernel_size = value;
        d_settings->changed.bloomMergerShader = true;
    }
    void update_bloom_calc_sigma(bool value) {
        d_settings->bloomMergeShader.calc_sigma = value;
        d_settings->changed.bloomMergerShader = true;
    }
    /* ----------Drawer => Grain Settings---------- */
    void update_grain_amount(float value) {
        d_settings->grainShader.amount = value;
        d_settings->changed.grainShader = true;
    }
    void update_grain_size(float value) {
        d_settings->grainShader.size = value;
        d_settings->changed.grainShader = true;
    }
    void update_grain_on(bool value) {
        d_settings->grainShader.on = value;
    }
    ~PMSettingsUpdater() {
        main_settings = nullptr;
        gmap_settings = nullptr;
        cmap_settings = nullptr;
        pc_settings = nullptr;
        d_settings = nullptr;
        sc_settings = nullptr;
    }
};