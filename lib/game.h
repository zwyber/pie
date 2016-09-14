//
// Created by paul on 9/7/16.
//

#ifndef PIE_GITHUB_FRAMEWORK_H

#include "framework.h"

#endif

#ifndef TUTORIALS_GAME_H
#define TUTORIALS_GAME_H

const int SCENE_QUIT = -1;
const int SCENE_MENU = 0;
const int SCENE_ABOUT = 1;

const int SCENE_GENESIS = 10;
const int SCENE_TUTORIAL = 11;
const int SCENE_INGAME = 12;
const int SCENE_PAUSE = 13;
const int SCENE_DIED = 14;

void maingame();

int show_menu(Window* window, TextureShader * menuMultiTex, std::vector<glm::mat3> menuElementTMat);
int show_about(Window* window);
int show_ingame(Window* window);

std::vector<glm::mat3> loadMenuResources(TextureShader * myMultiTex);

void addRandomObjects(Universe* universe, unsigned seed, int objectAmount);
bool CollidesWithAny(Object* obj, Universe* uni);


void showMenuDebug();
#include "game.cpp"
#endif //TUTORIALS_GAME_H


