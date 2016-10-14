//
// Created by paul on 9/7/16.
//

#ifndef PIE_GITHUB_FRAMEWORK_H

#include "framework.h"

#endif

#ifndef TUTORIALS_GAME_H
#define TUTORIALS_GAME_H

// Scene switcher constants
const int SCENE_QUIT = -1;
const int SCENE_MENU = 0;
const int SCENE_ABOUT = 1;

const int SCENE_GENESIS = 10;
const int SCENE_TUTORIAL = 11;
const int SCENE_INGAME = 12;
const int SCENE_PAUSE = 13;
const int SCENE_DIED = 14;

// Menu measured screen ratio
double initScreenRatio = 1200/900;

// Main game loop
void maingame(int startScene = SCENE_MENU);

// Scene functions
int show_menu(Window* window, TextureShader * menuMultiTex, std::vector<glm::mat3> menuElementTMat, CircleShader * circleShader = NULL,TextureShader* background=NULL);
int show_about(Window* window, TextShader* newText);
int show_tutorial(Window* window, CircleShader* circleShader,TextureShader* tutorialTex, vec2d tutorialSize, TextureShader* background=NULL);
int show_ingame(Window* window, CircleShader* circleShader = NULL, TextShader* textShader =NULL, TextureShader* background =NULL);

// Load menu resources
std::vector<glm::mat3> loadMenuResources(TextureShader * myMultiTex);

// Key callback functions
void tutorial_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void escape_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// Macros for adding objects to the universe
void addRandomObject(Universe* universe, unsigned seed = 0);
void addRandomObjects(Universe* universe, unsigned seed, int objectAmount);
void generateStandardUniverse(Window* window);

// Check for a collision with objects
bool collidesWithAny(Object* obj, Universe* uni);

#include "game.cpp"
#endif //TUTORIALS_GAME_H


