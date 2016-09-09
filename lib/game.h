//
// Created by paul on 9/7/16.
//

#ifndef PIE_GITHUB_FRAMEWORK_H

#include "framework.h"

#endif

#ifndef TUTORIALS_GAME_H
#define TUTORIALS_GAME_H

const int SCENE_MENU = 0;
const int SCENE_ABOUT = 1;

const int SCENE_GENESIS = 10;
const int SCENE_TUTORIAL = 11;
const int SCENE_INGAME = 12;
const int SCENE_PAUSE = 13;
const int SCENE_DIED = 14;


void maingame();

int show_menu(Window* window);
int show_about(Window* window);


#include "game.cpp"
#endif //TUTORIALS_GAME_H


