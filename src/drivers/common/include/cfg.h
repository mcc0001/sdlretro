#pragma once

#include <cstdint>
#include <string>

namespace drivers {

enum :uint32_t {
#ifdef GCW_ZERO
    DEFAULT_WIDTH = 320,
    DEFAULT_HEIGHT = 240,
    DEFAULT_SCALE = 1,
#else
    DEFAULT_WIDTH = 640,
    DEFAULT_HEIGHT = 480,
    DEFAULT_SCALE = 2,
#endif
};

class cfg {
public:
    virtual ~cfg() = default;
    inline void set_filename(const std::string &name) { filename = name; }
    void load();
    void save();

    inline std::pair<uint32_t, uint32_t> get_resolution() { return std::make_pair(res_w, res_h); }
    inline bool get_mono_audio() { return mono_audio; }
    inline uint32_t get_scale() { return scale; }
    inline uint32_t get_save_check() { return save_check; }

protected:
    std::string filename;

    uint32_t res_w = DEFAULT_WIDTH, res_h = DEFAULT_HEIGHT;
    bool mono_audio = false;
    uint32_t scale = DEFAULT_SCALE;
    uint32_t save_check = 0;
};

extern cfg g_cfg;

}
