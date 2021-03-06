#include "ui_menu.h"

#include "cfg.h"

#include "sdl1_menu.h"
#include "video_base.h"
#include "driver_base.h"

#include "variables.h"
#include "core_manager.h"

namespace gui {

using std::placeholders::_1;

int ui_menu::select_core_menu(const std::vector<const libretro::core_info *> &core_list) {
    sdl1_menu menu(driver, true);

    menu.set_title("[SELECT CORE TO USE]");

    std::vector<menu_item> items;
    for (const auto *core: core_list) {
        menu_item mi = { menu_static, core->name + ' ' + core->version };
        items.emplace_back(mi);
    }
    menu.set_items(items);
    uint32_t w, h;
    std::tie(w, h) = g_cfg.get_resolution();
    menu.set_rect(50, 50, w - 100, h - 100);
    if (!menu.enter_menu_loop()) return -1;
    return menu.get_selected();
}

void ui_menu::in_game_menu() {
    sdl1_menu menu(driver, true);

    menu.set_title("[IN-GAME MENU]");

    auto *vari = driver->get_variables();
    const auto &vars = vari->get_variables();
    if (vars.empty()) {
        std::vector<menu_item> items = {
            {menu_static, "Global Settings", "", 0, {}, std::bind(&ui_menu::global_settings_menu, this, _1)},
            {menu_static, "Reset", "", 0, {}, [this](const menu_item&) { driver->reset(); return true; } },
            {menu_static, "Exit", "", 0, {}, [this](const menu_item&) { driver->shutdown(); return true; } },
        };
        menu.set_items(items);
    } else {
        std::vector<menu_item> items = {
            {menu_static, "Global Settings", "", 0, {}, std::bind(&ui_menu::global_settings_menu, this, _1)},
            {menu_static, "Core Settings", "", 0, {}, std::bind(&ui_menu::core_settings_menu, this, _1)},
            {menu_static, "Reset", "", 0, {}, [this](const menu_item&) { driver->reset(); return true; } },
            {menu_static, "Exit", "", 0, {}, [this](const menu_item&) { driver->shutdown(); return true; } },
        };
        menu.set_items(items);
    }
    uint32_t w, h;
    std::tie(w, h) = g_cfg.get_resolution();
    menu.set_rect(50, 50, w - 100, h - 100);
    menu.enter_menu_loop();
}

enum :size_t {
    check_secs_count = 4
};
const uint32_t check_secs[check_secs_count] = {0, 30, 60, 300};
bool ui_menu::global_settings_menu(const menu_item&) {
    sdl1_menu menu(driver, false);

    menu.set_title("[GLOBAL SETTINGS]");

    size_t check_sec_idx;
    const uint32_t *n = std::lower_bound(check_secs, check_secs + check_secs_count, g_cfg.get_save_check());
    check_sec_idx = n - check_secs;
    if (check_sec_idx >= check_secs_count) {
        check_sec_idx = 0;
    }
    std::vector<menu_item> items = {
        { menu_values, "SRAM Save Interval", "", check_sec_idx,
            {"off", "30", "60", "300"},
            [](const menu_item &item)->bool {
                if (item.selected < check_secs_count)
                    g_cfg.set_save_check(check_secs[item.selected]);
                return false;
            }
        },
    };
    menu.set_items(items);
    uint32_t w, h;
    std::tie(w, h) = g_cfg.get_resolution();
    auto border = w / 16;
    menu.set_rect(border, border, w - border * 2, h - border * 2);
    menu.set_item_width(w - border * 2 - 90);
    menu.enter_menu_loop();
    g_cfg.save();
    return false;
}

bool ui_menu::core_settings_menu(const menu_item&) {
    auto *vari = driver->get_variables();
    const auto &vars = vari->get_variables();
    if (vars.empty()) return false;
    sdl1_menu menu(driver, false);

    menu.set_title("[CORE SETTINGS]");

    std::vector<menu_item> items;
    for (auto &var: vars) {
        menu_item item = { menu_values, var.label, var.info, var.curr_index };
        for (auto &opt: var.options)
            item.values.push_back(opt.first);
        item.callback = [this, &vari, &var](const menu_item &item)->bool {
            vari->set_variable(var.name, item.selected);
            return false;
        };
        items.emplace_back(item);
    }
    menu.set_items(items);
    uint32_t w, h;
    std::tie(w, h) = g_cfg.get_resolution();
    auto border = w / 16;
    menu.set_rect(border, border, w - border * 2, h - border * 2);
    menu.set_item_width(w - border * 2 - 90);
    menu.enter_menu_loop();
    driver->save_variables_to_cfg();
    return false;
}

}
