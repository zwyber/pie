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

    Window window = Window();

    GLuint menuTex = loadDDS("MenuTextures.DDS");

    // Init shader
    TextureShader menuMultiTex = TextureShader(menuTex);
    //// I'd suggest building a circle shader for universes to use.
    CircleShader allDebrisShader;

    // Load menu resources
    std::vector<glm::mat3> tMats = loadMenuResources(&menuMultiTex);

    initScreenRatio = window.windowSize()[0]/(double)window.windowSize()[0];
    do {

        // I don't like switch :(
        // Don't worry I think the if statements in a while loop makes more sense in our case ;)
        if (scene == SCENE_MENU) {
            // Create a universe and bind it to the window
            Universe *universe = new Universe();
            window.bindUniverse(universe);

            // Setup a few objects in the universe
            addRandomObjects(window.boundUniverse, std::rand()/(double)RAND_MAX, 50);

            // Show the menu
            scene = show_menu(&window, &menuMultiTex, tMats, &allDebrisShader);

            // Clear all heap variables
            window.bindUniverse(NULL);
            delete universe;

        }

        if (scene == SCENE_ABOUT) {
            scene = SCENE_MENU;

        }

        if (scene == SCENE_GENESIS) {
            // Generate a standard universe
            Universe *universe = new Universe();
            window.bindUniverse(universe);

            addRandomObjects(window.boundUniverse, std::rand()/(double)RAND_MAX, 5);

            Player* player = new Player();

            window.boundUniverse->add_object(player);

            scene = SCENE_INGAME;
        }

        if (scene == SCENE_INGAME) {
            scene = show_ingame(&window, &allDebrisShader);

            // do not forget to delete universe (although this is better to do after SCENE_DIED)
            delete window.boundUniverse;
        }

    }
    while(scene != SCENE_QUIT);
    glfwTerminate();
}


int show_ingame (Window* window, CircleShader* circleShader) {
    int exitFlag = SCENE_INGAME;

    while(exitFlag == SCENE_INGAME){
        // Do a physics step and draw the universe
        glClear(GL_COLOR_BUFFER_BIT);
        window->drawObjectList(circleShader);
        window->boundUniverse->physics_runtime_iteration();

        // Handle user input


        // Do frame pacing
        window->pace_frame();

        //// Paul you forgot the Swap buffer command in your haste ;) that was all
        glfwSwapBuffers(window->GLFWpointer);
        glfwPollEvents();

        if(glfwWindowShouldClose(window->GLFWpointer) != 0){
            exitFlag = SCENE_QUIT;
        }
        if(glfwGetKey(window->GLFWpointer, GLFW_KEY_ESCAPE) == GLFW_PRESS ){
            //exitFlag = SCENE_PAUSE;
            exitFlag = SCENE_MENU; // Use this until pause scene is available;
        }
    }

    return exitFlag;
}


std::vector<glm::mat3> loadMenuResources(TextureShader* myMultiTex){
    //// Input for menu properties
    const GLfloat imageSize = 1024;         // input image needs to be 2^n in width and height
    const GLfloat menuMeasHeight = 900;
    const GLfloat menuMeasWidth = 1200;   //The scales used for measuring the objects Origins and sizes
    GLfloat UVs[] = {       // UV uses the actual file supplied
            1, 160,       //Menu title
            1, 160+131,
            1+1020, 160+131,
            1+1020, 160,
            554, 5,     //Button 1
            554, 5+52,
            554+183, 5+52,
            554+183, 5,
            741, 1,   //Button 2
            741, 1+71,
            741+156, 1+71,
            741+156, 1,
            900, 1,     //Button 3
            900, 1+48,
            900+123, 1+48,
            900+123, 1
    };
    GLfloat objectOriginCoords[]{ // objectOriginCoords uses the alignment we wish on screen (origin is top left corner)
            68, 183,
            484, 402,
            496, 520,
            511, 655
    };
    GLfloat  objectSizes[]{ // Width and hight of the objects
            1053, 136,
            183, 52,
            156, 71,
            123, 48
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

int show_menu(Window* window, TextureShader * menuMultiTex, std::vector<glm::mat3> menuElementTMat, CircleShader * circleShader){
    int exitFlag = SCENE_MENU;

    //get set resources;
    glClearColor(0.2, 0.2, 0.3, 1.0);
    int highlightedButton = -1;
    vec2d cursorPos;
    bool cursorMode = false;
    GLFWcursor* arrowCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    GLFWcursor* handCursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    while(exitFlag == SCENE_MENU){
        // Do a physics step and draw the universe
        glClear(GL_COLOR_BUFFER_BIT);
        window->drawObjectList(circleShader);
        window->boundUniverse->physics_runtime_iteration();
        double newWidthScale = initScreenRatio/(window->windowSize()[0]/(double)window->windowSize()[1]);

        //draw the menu;
        highlightedButton = -1;

        cursorPos = window->cursorPosition();
        for(int ii = 0; ii < menuElementTMat.size(); ii++) {
            menuMultiTex->transformationMatrix = menuElementTMat[ii];
            menuMultiTex->tMatrixScale({newWidthScale,1});
            if(cursorPos[0] > menuElementTMat[ii][0][2] - menuElementTMat[ii][0][0] && cursorPos[0] <menuElementTMat[ii][0][2] + menuElementTMat[ii][0][0]){
                if(cursorPos[1] > menuElementTMat[ii][1][2] - menuElementTMat[ii][1][1] && cursorPos[1] <menuElementTMat[ii][1][2] + menuElementTMat[ii][1][1]){
                    highlightedButton = ii;
                    if(ii >0) {
                        menuMultiTex->transformationMatrix[0][0] *= 1.2;
                        menuMultiTex->transformationMatrix[1][1] *= 1.2;
                    }
                }
            }
            menuMultiTex->draw(ii);
        };

        glfwSwapBuffers(window->GLFWpointer);
        glfwPollEvents();
        if(glfwGetMouseButton(window->GLFWpointer, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
            //// BUTTON MEANING
            switch(highlightedButton){
                case -1: //no highlight
                case 0:  break;  //menu name highlight
                case 1: exitFlag = SCENE_ABOUT; break;
                case 2: exitFlag = SCENE_GENESIS; break;
                case 3: exitFlag = SCENE_QUIT; break;
            }
        }
        if(cursorMode && highlightedButton<=0){
            cursorMode = false;
            glfwSetCursor(window->GLFWpointer,arrowCursor);
        }
        if(!cursorMode && highlightedButton >0){
            cursorMode = true;
            glfwSetCursor(window->GLFWpointer,handCursor);
        }
        if(glfwGetKey(window->GLFWpointer, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(window->GLFWpointer) != 0){
            exitFlag = SCENE_QUIT;
        }

        // Do frame pacing
        window->pace_frame();
    }
    // Set the cursor back to normal after the button is pressed
    glfwSetCursor(window->GLFWpointer,arrowCursor);
    return exitFlag;

}

void showMenuDebug(){
    // easy generate a window
    Window thisGame = Window();

    // load a texture from file and create a texture shader for it
    GLuint menuTex = loadDDS("MenuTextures.DDS");
    TextureShader menuMultiTex(menuTex);

    // convert the texture shader to a proper hardcoded multi-textureshader and get matrices of the positions.
    std::vector<glm::mat3> tMats = loadMenuResources(&menuMultiTex);

    // Store initial size so resizing can be applied with the right reference of initiation.
    vec2d initSize = thisGame.windowSize();

    // test sho
    std::cout << "show_menu exit code (next scene): " << show_menu(&thisGame, &menuMultiTex,tMats) << endl;
}

void timerDebug(){
    Window thisGame = Window();

    GLuint textShader = LoadShaders("shaders/text.glvs", "shaders/text.glfs");

    FontTexHandler myText("frabk.ttf",32, textShader, thisGame.windowSize());
    bool exit = false;
    int thisTime;
    std::string text;
    glfwSetTime(0.0);
    while(!exit){
        glClear(GL_COLOR_BUFFER_BIT);

        thisTime = 10*glfwGetTime();
        thisTime *= 10;
        text = std::to_string(thisTime);
        myText.renderText(text,240,210,1.0,{1.0,0.0,0.0});

        glfwSwapBuffers(thisGame.GLFWpointer);
        glfwPollEvents();
        if(glfwWindowShouldClose(thisGame.GLFWpointer) != 0 || glfwGetKey(thisGame.GLFWpointer, GLFW_KEY_ESCAPE) == GLFW_PRESS){
            exit = true;
        }
    }
}

void addRandomObjects(Universe* universe, unsigned seed, int objectAmount) {
    if(!seed){
        srand(time(NULL));
    }else{
        srand(seed);
    }
    std::array<double,2> radiusLim = {0.2, 1};
    std::array<double,2> massLim = {1, 3};
    std::array<double,2> velocityLim = {-3, 3};
    for(int ii = 0; ii < objectAmount; ii++){
        Object* A = new Object;
        A->set_mass((std::rand()/(double)RAND_MAX)*(massLim[1]-massLim[0])+massLim[0]);
        A->set_velocity((std::rand()/(double)RAND_MAX)*(velocityLim[1]-velocityLim[0])+velocityLim[0],(std::rand()/(double)RAND_MAX)*(velocityLim[1]-velocityLim[0])+velocityLim[0]);
        A->set_radius((std::rand()/(double)RAND_MAX)*(radiusLim[1]-radiusLim[0])+radiusLim[0]);
        A->set_bouncyness(0.9);
        std::array<double,2> xLim = {-universe->width/2+A->radius, universe->width/2-A->radius};
        std::array<double,2> yLim = {-universe->height/2+A->radius, universe->height/2-A->radius};
        do{
            A->set_position((std::rand()/(double)RAND_MAX)*(xLim[1]-xLim[0])+xLim[0], (std::rand()/(double)RAND_MAX)*(yLim[1]-yLim[0])+yLim[0]);
        }while(CollidesWithAny(A, universe));
        universe->add_object(A);
    }
}

bool CollidesWithAny(Object* obj, Universe* uni) {
    for(int ii = 0; ii < uni->objects.size(); ii++){
        if(uni->objects[ii] != obj){
            if(uni->physics.distance_between(uni->objects[ii],obj) < (uni->objects[ii]->radius + obj->radius) ){
                return true;
            }
        }
    }

    return false;
}