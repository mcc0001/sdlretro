#pragma once

#include <utility>
#include <vector>
#include <memory>
#include <cstdint>

namespace libretro {
struct core_info;
}

namespace drivers {
class driver_base;
}

namespace gui {

struct menu_item;

class ui_menu {
public:
    inline explicit ui_menu(std::shared_ptr<drivers::driver_base> driver): driver(std::move(driver)) {}
    virtual ~ui_menu() = default;

    /* select core from menu, return -1 if `cancel` is pressed */
    int select_core_menu(const std::vector<const libretro::core_info*> &core_list);

    /* enter ingame menu */
    void in_game_menu();

    bool global_settings_menu(const menu_item&);
    bool core_settings_menu(const menu_item&);

private:
    std::shared_ptr<drivers::driver_base> driver;
};

}
