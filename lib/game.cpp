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

    Window thisGame();
    // I don't like switch :(
    // Don't worry I think the if statements in a while loop makes more sense in our case ;)
    if (scene == SCENE_MENU) {



    }

    if (scene == SCENE_ABOUT) {



    }

    if (scene == SCENE_GENESIS) {


    }


}
void loadResources(){
    loadMenuButtons();
}
void loadMenuButtons(){
    TextureShader
}
int show_menu(Window* window){
    int exitFlag = SCENE_MENU;
    //get set resources;
    while(exitFlag == SCENE_MENU){
        //draw the menu;
        glClear(GL_COLOR_BUFFER_BIT);
        if(glfwGetKey(window->GLFWpointer, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(window->GLFWpointer) != 0){
            exitFlag = -1;
        }

        glfwPollEvents();
    }
    return exitFlag;

}