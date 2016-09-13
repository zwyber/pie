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
std::vector<glm::mat3> loadMenuResources(TextureShader* myMultiTex){
    //// Input for menu properties
    const GLfloat imageSize = 1024;
    const GLfloat menuMeasHeight = 1024;
    const GLfloat menuMeasWidth = menuMeasHeight;
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
    GLfloat objectOriginCoords[]{ // objectOriginCoords uses the alignment we wish on screen (origin is top left corner)
            225, 123,
            363, 437,
            369, 615,
            366, 787
    };
    GLfloat  objectSizes[]{ // Width and hight of the objects
            585, 164,
            298, 117,
            300, 107,
            298, 97
    };

    //// Make the multi texture and get transformation matrices (so we can solve resizing later)
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
    const int numBoxes = sizeof(objectOriginCoords)/(sizeof(GLfloat)*2);
    if(numBoxes != coords/8){
        std::cerr << "[Warning]: The vertical coordinates do not match the amount of coordinates in UVs. horCoords: "<< numBoxes << " images, UVs: " << coords/(double)8 << " images." << std::endl;
    }
    std::vector<glm::mat3> outputTMatrices;
    for (int ii = 0; ii< numBoxes; ii++){
        objectOriginCoords[2*ii] *= 2/menuMeasWidth;
        objectOriginCoords[2*ii+1] *= 2/menuMeasHeight;
        objectOriginCoords[2*ii] -= 1;
        objectOriginCoords[2*ii+1] -= 1;
        objectOriginCoords[2*ii+1] *= -1;
        objectSizes[2*ii] /= menuMeasWidth;
        objectSizes[2*ii+1] /= menuMeasHeight;
        outputTMatrices.push_back({
                objectSizes[2*ii], 0, objectOriginCoords[2*ii]+objectSizes[2*ii],
                0, objectSizes[2*ii + 1], objectOriginCoords[2*ii + 1],
                0, 0, 1
        });
    }
    return outputTMatrices;

}
int show_menu(Window* window, TextureShader * menuMultiTex, std::vector<glm::mat3> menuElementTMat){
    int exitFlag = SCENE_MENU;
    //get set resources;
    glClearColor(0.2, 0.2, 0.3, 1.0);
    int highlightedButton = -1;
    vec2d cursorPos;
    //GLdouble R = 0.1;
    while(exitFlag == SCENE_MENU){
        //draw the menu;
        glClear(GL_COLOR_BUFFER_BIT);
        highlightedButton = -1;
        cursorPos = window->cursorPosition();
        for(int ii = 0; ii < menuElementTMat.size(); ii++) {
            menuMultiTex->transformationMatrix = menuElementTMat[ii];
            if(cursorPos[0] > menuElementTMat[ii][0][2] - menuElementTMat[ii][0][0] && cursorPos[0] <menuElementTMat[ii][0][2] + menuElementTMat[ii][0][0]){
                if(cursorPos[1] > menuElementTMat[ii][1][2] - menuElementTMat[ii][1][1] && cursorPos[1] <menuElementTMat[ii][1][2] + menuElementTMat[ii][1][1]){
                    highlightedButton = ii;
                }
            }
            menuMultiTex->draw(ii);
        };
        //window->drawFilledCircle(cursorPos,R,40,{0,1,0,1});
        glfwSwapBuffers(window->GLFWpointer);
        glfwPollEvents();
        if(glfwGetMouseButton(window->GLFWpointer, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
            std::cout << highlightedButton << std::endl;
            //// BUTTON MEANING
            switch(highlightedButton){
                case -1: break; //no highlight
                case 0: break;  //menu name highlight
                case 1: exitFlag = SCENE_TUTORIAL; break;
                case 2: exitFlag = SCENE_ABOUT; break;
                case 3: exitFlag = -1; break;
            }
        }
        if(glfwGetKey(window->GLFWpointer, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(window->GLFWpointer) != 0){
            exitFlag = -1;
        }
    }
    return exitFlag;

}


void showMenuDebug(){

    Window thisGame = Window();

    GLuint menuTex = loadDDS("menuDummy.DDS");
    TextureShader menuMultiTex(menuTex);

    std::vector<glm::mat3> tMats = loadMenuResources(&menuMultiTex);

    std::cout << "exit code " << show_menu(&thisGame, &menuMultiTex,tMats) << endl;
}