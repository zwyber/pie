//
// Created by paul on 9/7/16.
//

#include "game.h"

void maingame() {
    // Initialise the scene switcher
    int scene = SCENE_MENU;

    // Initialise the window
    int universeWidth = 720;
    int universeHeight = 480;

    double pixRatio = 50;

    Universe universe(universeWidth/pixRatio, universeHeight/pixRatio);
    Window window = Window(&universe, pixRatio);

    // I don't like switch :(

    if (scene == SCENE_MENU) {



    }

    if (scene == SCENE_ABOUT) {



    }

    if (scene == SCENE_GENESIS) {



    }



}