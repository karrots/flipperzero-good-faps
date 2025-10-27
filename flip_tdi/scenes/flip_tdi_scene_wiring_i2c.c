#include "../flip_tdi_app_i.h"

void flip_tdi_scene_wiring_i2c_on_enter(void* context) {
    furi_assert(context);

    FlipTDIApp* app = context;
    widget_add_string_multiline_element(
        app->widget,
        0,
        0,
        AlignLeft,
        AlignTop,
        FontSecondary,
        "I2C wiring:\n"
        "SCL -> ADBUS0 (EXT PA7)\n"
        "SDA Out -> ADBUS1 (EXT PA6)\n"
        "SDA In  -> ADBUS2 (EXT PA4)\n"
        "Pull-ups required on SDA/SCL.\n"
        "Mode: FT232H I2C over MPSSE");
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipTDIViewWidget);
}

bool flip_tdi_scene_wiring_i2c_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void flip_tdi_scene_wiring_i2c_on_exit(void* context) {
    furi_assert(context);

    FlipTDIApp* app = context;
    widget_reset(app->widget);
}
