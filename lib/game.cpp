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

    GLuint menuTex = loadDDS("menuDummy.DDS");
    TextureShader menuMultiTex(menuTex);

    Window thisGame();

    loadMenuResources(&menuMultiTex);
    // I don't like switch :(
    // Don't worry I think the if statements in a while loop makes more sense in our case ;)
    if (scene == SCENE_MENU) {



    }

    if (scene == SCENE_ABOUT) {



    }

    if (scene == SCENE_GENESIS) {


    }

}
void loadMenuResources(TextureShader* myMultiTex){

    const GLfloat imageSize = 1024;
    const GLfloat menuMeasHeight = 1024;
    GLfloat UVs[] = {       // UV uses the actual file supplied
            0, 0,       //Menu title
            0, 258,
            923, 258,
            923, 0,
            0, 256,     //Button 1
            0, 434,
            468, 434,
            468, 256,
            480, 264,   //Button 2
            480, 434,
            953, 434,
            953, 264,
            0, 438,     //Button 3
            0, 590,
            468, 590,
            468, 438
    };
    GLfloat verticalCoords[]{ // verticalCoords uses the alignment we wish on screen
            122, 258,
            439, 548,
            616, 722,
            787, 880
    };
    // Handle the texture input interpretation
    int coords = sizeof(UVs)/sizeof(GLfloat);
    if(coords%8) std::cerr << "[Warning]: There is a incorrect amount of coordinates supplied in the UVs of the loadMenu" << std::endl;
    GLfloat vertices[coords];
    for(int ii = 0; ii<coords; ii++){
        UVs[ii] /= imageSize;
        switch (ii%8) {
            case 0: vertices[ii] = -1; break;
            case 1: vertices[ii] = 1; break;
            case 2: vertices[ii] = -1; break;
            case 3: vertices[ii] = -1; break;
            case 4: vertices[ii] = 1; break;
            case 5: vertices[ii] = -1; break;
            case 6: vertices[ii] = 1; break;
            case 7: vertices[ii] = 1; break;
        }
    }
    myMultiTex->setNewUVCoordinates(coords, &UVs[0], &vertices[0]);
    // Handle the output interpretation
    const int numBoxes = sizeof(verticalCoords)/(sizeof(GLfloat)*2);
    if(numBoxes != coords/8){
        std::cerr << "[Warning]: The vertical coordinates do not match the amount of coordinates in UVs. horCoords: "<< numBoxes << " images, UVs: " << coords/(double)8 << " images." << std::endl;
    }
    std::array<glm::mat4,numBoxes> outputTMatrices;
    for (int ii = 0; ii< numBoxes*2; ii++){
        verticalCoords[ii] *= 2/menuMeasHeight;
        verticalCoords[ii] -= 1;

    }

}
int show_menu(Window* window, TextureShader * menuMultiTex){
    int exitFlag = SCENE_MENU;
    //get set resources;
    glClearColor(0.2, 0.2, 0.3, 1.0);


    while(exitFlag == SCENE_MENU){
        //draw the menu;
        glClear(GL_COLOR_BUFFER_BIT);
        menuMultiTex->tMatrixReset();

        menuMultiTex->draw(1);

        glfwSwapBuffers(window->GLFWpointer);
        glfwPollEvents();
        if(glfwGetKey(window->GLFWpointer, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(window->GLFWpointer) != 0){
            exitFlag = -1;
        }
    }
    return exitFlag;

}