#include "../flip_tdi_app_i.h"

void flip_tdi_scene_menu_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    FlipTDIApp* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void flip_tdi_scene_menu_on_enter(void* context) {
    furi_assert(context);

    FlipTDIApp* app = context;
    Submenu* submenu = app->submenu;
    submenu_add_item(
        submenu, "Mode: Default", SubmenuIndexModeDefault, flip_tdi_scene_menu_submenu_callback, app);
    submenu_add_item(
        submenu, "Mode: I2C", SubmenuIndexModeI2c, flip_tdi_scene_menu_submenu_callback, app);
    submenu_add_item(
        submenu, "WiringUart", SubmenuIndexWiringUart, flip_tdi_scene_menu_submenu_callback, app);
    submenu_add_item(
        submenu, "WiringSpi", SubmenuIndexWiringSpi, flip_tdi_scene_menu_submenu_callback, app);
    submenu_add_item(
        submenu, "WiringGpio", SubmenuIndexWiringGpio, flip_tdi_scene_menu_submenu_callback, app);
    submenu_add_item(
        submenu, "WiringI2C", SubmenuIndexWiringI2c, flip_tdi_scene_menu_submenu_callback, app);
    submenu_add_item(
        submenu, "About", SubmenuIndexAbout, flip_tdi_scene_menu_submenu_callback, app);

    uint32_t state = scene_manager_get_scene_state(app->scene_manager, FlipTDISceneMenu);
    if(state == 0) {
        state = SubmenuIndexModeDefault;
    }
    submenu_set_selected_item(submenu, state);

    view_dispatcher_switch_to_view(app->view_dispatcher, FlipTDIViewSubmenu);
}

bool flip_tdi_scene_menu_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);

    FlipTDIApp* app = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexAbout) {
            scene_manager_next_scene(app->scene_manager, FlipTDISceneAbout);
            consumed = true;
        } else if(event.event == SubmenuIndexModeDefault) {
            flip_tdi_set_i2c_mode(app, false);
            consumed = true;
        } else if(event.event == SubmenuIndexModeI2c) {
            flip_tdi_set_i2c_mode(app, true);
            scene_manager_next_scene(app->scene_manager, FlipTDISceneWiringI2c);
            consumed = true;
        } else if(event.event == SubmenuIndexWiringUart) {
            scene_manager_next_scene(app->scene_manager, FlipTDISceneWiringUart);
            consumed = true;
        } else if(event.event == SubmenuIndexWiringSpi) {
            scene_manager_next_scene(app->scene_manager, FlipTDISceneWiringSpi);
            consumed = true;
        } else if(event.event == SubmenuIndexWiringGpio) {
            scene_manager_next_scene(app->scene_manager, FlipTDISceneWiringGpio);
            consumed = true;
        } else if(event.event == SubmenuIndexWiringI2c) {
            scene_manager_next_scene(app->scene_manager, FlipTDISceneWiringI2c);
            consumed = true;
        }
        scene_manager_set_scene_state(app->scene_manager, FlipTDIViewSubmenu, event.event);
    }

    return consumed;
}

void flip_tdi_scene_menu_on_exit(void* context) {
    furi_assert(context);

    FlipTDIApp* app = context;
    submenu_reset(app->submenu);
}
