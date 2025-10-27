#pragma once

typedef enum {
    //SubmenuIndex
    SubmenuIndexModeDefault = 10,
    SubmenuIndexModeI2c,
    SubmenuIndexWiringUart,
    SubmenuIndexWiringSpi,
    SubmenuIndexWiringGpio,
    SubmenuIndexWiringI2c,
    SubmenuIndexAbout,

    //FlipTDICustomEvent
    FlipTDICustomEventStartId = 100,
    FlipTDICustomEventMainMore,

} FlipTDICustomEvent;
