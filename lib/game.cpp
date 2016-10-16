// Created by paul on 9/7/16.
//

#include "game.h"

// Player pointer
Player* boundPlayer = NULL;

/*
 * A keyhandler is basically a list of pressed keys. With that you can easily check
 * if a key was pressed, and empty the list afterwards. It is updated with a callback
 * function, which is executed after a glfwPollEvents().
 */
std::vector<int> keyHandler;

// Joystick support is implemented, this variable controls holds if it is enabled or not.
bool Joystick;

void maingame(int startScene) {
    // Initialise the scene switcher
    int scene = startScene;

    bool shownTutorial = false;

    // Initialise the window
    double windowWidth = 1000;
    double windowHeight = 700;

    double pixRatio = 25;

    int universeWidth = windowWidth/pixRatio;
    int universeHeight = windowHeight/pixRatio;

    Window window = Window(pixRatio*universeWidth,pixRatio*universeHeight,vis::NO_RESIZE);
    window.pixRatio = pixRatio;

    // load all textures with a texture loader provided by openGL_tutorials
    GLuint menuTex = loadDDS("Textures/MenuTextures.DDS");
    GLuint tutorialDDS = loadDDS("Textures/Tutorial2.DDS");
    GLuint space = loadDDS("Textures/SpaceCrop.DDS");

    // Creaqte Texture shaders containing the Textures previously loaded
    TextureShader menuMultiTex = TextureShader(menuTex);
    TextureShader tutorialTex = TextureShader(tutorialDDS);
    TextureShader spaceTex = TextureShader(space);

    // Store the size of the tutorial image for drawing and initilizing
    vec2d tutorialSize = {640, 480};

    // Create a circle shader to draw Objects
    CircleShader allDebrisShader;

    // Create textshaders for Score and the about scene
    TextShader scoreText = TextShader("Fonts/Courier New Bold.ttf");
    TextShader aboutText = TextShader("Fonts/verdana.ttf");

    // Load menu resources and store the transformation matrices necessary to display
    std::vector<glm::mat3> tMats = loadMenuResources(&menuMultiTex);

    do {

        if (scene == SCENE_MENU) {
            // Create a universe and bind it to the window
            Universe *universe = new Universe(universeWidth, universeHeight);
            window.bindUniverse(universe);

            // Setup a few objects in the universe
            addRandomObjects(window.boundUniverse, std::rand(), 40);

            // Check joystick presence while entering scene (Callback not yet implemented in 3.1.2 and check conflicts with keyboard check)
            Joystick = glfwJoystickPresent(GLFW_JOYSTICK_1);

            // Show the menu
            scene = show_menu(&window, &menuMultiTex, tMats, &allDebrisShader,&spaceTex);

            // Clear all heap variables
            window.bindUniverse(NULL);
            delete universe;
        }

        // Show the about scene
        if (scene == SCENE_ABOUT) {
            scene = show_about(&window, &aboutText);
            continue;
        }

        if (scene == SCENE_GENESIS) {
            // Generate a standard universe
            Universe *universe = new Universe(universeWidth, universeHeight);
            window.bindUniverse(universe);

            generateStandardUniverse(&window);

            // Check if joystick is present for this scene
            Joystick = glfwJoystickPresent(GLFW_JOYSTICK_1);

            // Only show the tutorial if it hasn't been done already
            if (!shownTutorial) {
                scene = SCENE_TUTORIAL;
            }
            else {
                scene = SCENE_INGAME;
            }
        }

        if (scene == SCENE_TUTORIAL) {
            shownTutorial = true;
            scene = show_tutorial(&window,&allDebrisShader,&tutorialTex,tutorialSize,&spaceTex);

        }

        if (scene == SCENE_INGAME) {
            scene = show_ingame(&window, &allDebrisShader,&scoreText,&spaceTex);
        }

        if (scene == SCENE_DIED) {
            // Draw a gray background

            // Display the score
            std::stringstream diedText;
            diedText << "You hit something! Press the ESC key to return to the menu";
            aboutText.draw(diedText.str(), {0,0}, DRAWTEXT::ALIGN_CENTER, window.windowSize(), 0.02);

            int joyCount;
            const unsigned char* joyButtons;
            bool hitButton=false;
            glfwSetKeyCallback(window.GLFWpointer,escape_key_callback);
            glfwSwapBuffers(window.GLFWpointer);
            do {
                keyHandler ={};
                glfwPollEvents();

                if(glfwWindowShouldClose(window.GLFWpointer) != 0){
                    break;
                }
                if(Joystick){
                    joyButtons = glfwGetJoystickButtons(GLFW_JOYSTICK_1,&joyCount);
                    for(int ii =0; ii<joyCount; ii++){
                        if(joyButtons[ii]==GLFW_PRESS){
                            glfwSetKeyCallback(window.GLFWpointer,NULL);
                            hitButton = true;
                            break;
                        }
                    }
                    if(hitButton)break;
                }
                if(keyHandler.size() ){
                    glfwSetKeyCallback(window.GLFWpointer,NULL);
                    break;
                }

                // Wait 100ms
                window.pace_frame();
            }
            while(true);

            // Clear up
            boundPlayer = NULL;
            delete window.boundUniverse;

            // Proceed to menu
            scene = SCENE_MENU;
        }

    }
    while(scene != SCENE_QUIT);
    // Make sure universe is removed when exiting this loop
    if (window.boundUniverse!=NULL){
        delete window.boundUniverse;
    }
    glfwTerminate();
}
int show_ingame (Window* window, CircleShader* circleShader, TextShader* textShader, TextureShader* background) {
    int exitFlag = SCENE_INGAME;

    // Initialise a few variables to store time data etc.
    std::chrono::steady_clock::time_point now_time;
    std::chrono::steady_clock::duration time_elapsed;
    int seconds_count = 0;
    const int NEW_OBJECT_DELAY = 3;
    const int MORE_OBJECTS_DELAY = 2;
    bool addedAlready = false;

    // Reinitialize the universe time clock, so the start time
    window->boundUniverse->begin_time = std::chrono::steady_clock::now();

    // create a textstream for the shader, and give the textShader a white colour
    std::stringstream scoreText;
    textShader->colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    // make a Escape callback to return to menu
    glfwSetKeyCallback(window->GLFWpointer,escape_key_callback);
    while(exitFlag == SCENE_INGAME){
        // Clear the screenBuffer
        glClear(GL_COLOR_BUFFER_BIT);
        // if a background shader is provided, draw that background
        if(background!=NULL){
            background->draw();
        }
        // Do a physics step and draw the universe
        window->boundUniverse->simulate_one_time_unit(window->fps);
        window->drawObjectList(circleShader);
        // If a textShader is provided draw the score to the screen
        if(textShader!=NULL) {
            scoreText.str(std::string());
            scoreText << "Score: " << window->boundUniverse->score;
            textShader->draw(scoreText.str() ,{0, -0.92},DRAWTEXT::ALIGN_CENTER, window->windowSize() , 0.02);
        }

        // Determine if we want to add another piece of debris (once every MORE_OBJECT_DELAY starting at NEW_OBJECTS_DELAY)
        now_time = std::chrono::steady_clock::now();
        time_elapsed = now_time - window->boundUniverse->begin_time;
        seconds_count = std::chrono::duration_cast<std::chrono::seconds>(time_elapsed).count();

        if (seconds_count % MORE_OBJECTS_DELAY == 0 && !addedAlready && seconds_count > NEW_OBJECT_DELAY ) {
            addRandomObject(window->boundUniverse);

            addedAlready = true;
        }

        if (seconds_count % MORE_OBJECTS_DELAY != 0) {
            addedAlready = false;
        }

        // Check if we should end the game
        if (boundPlayer->i_collided) {
            exitFlag = SCENE_DIED;
            return exitFlag;
        }

        // Do frame pacing
        window->pace_frame();

        // Put buffer on screen and find all pressed keys.
        glfwSwapBuffers(window->GLFWpointer);
        keyHandler = {};
        glfwPollEvents();

        // If window should terminate CLOSE IT
        if(glfwWindowShouldClose(window->GLFWpointer) != 0){
            exitFlag = SCENE_QUIT;
        }
        // Return
        if(keyHandler.size() ){
            //exitFlag = SCENE_PAUSE;
            exitFlag = SCENE_MENU; // Use this until pause scene is available;
        }
    }

    // Reset the Callback function
    glfwSetKeyCallback(window->GLFWpointer,NULL);
    return exitFlag;
}

int show_about (Window* window, TextShader* newText) {
    int exitFlag = SCENE_ABOUT;

    glClear(GL_COLOR_BUFFER_BIT);

    // Create white letters
    newText->colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    // Draw line by line the text to this buffer
    newText->draw("Space Debris Evaders is a game developed for the course", {0, 0.8}, DRAWTEXT::ALIGN_CENTER, window->windowSize(), 0.02);
    newText->draw("'Programming in Engineering' at the University of Twente", {0, 0.7}, DRAWTEXT::ALIGN_CENTER, window->windowSize(), 0.02);
    newText->draw("The goal is to learn C++ by developing a game.", {0, 0.6}, DRAWTEXT::ALIGN_CENTER, window->windowSize(), 0.02);
    newText->draw("Authors:", {0, 0.4}, DRAWTEXT::ALIGN_CENTER, window->windowSize(), 0.02);
    newText->draw("Arash Edrisi", {0, 0.3}, DRAWTEXT::ALIGN_CENTER, window->windowSize(), 0.02);
    newText->draw("Yvan Klaver", {0, 0.2}, DRAWTEXT::ALIGN_CENTER, window->windowSize(), 0.02);
    newText->draw("Paul van Swinderen", {0, 0.1}, DRAWTEXT::ALIGN_CENTER, window->windowSize(), 0.02);
    newText->draw("Press ESC to return to the menu", {0, -0.4}, DRAWTEXT::ALIGN_CENTER, window->windowSize(), 0.02);

    // put the buffer on screen and set a escape key callback
    glfwSwapBuffers(window->GLFWpointer);
    glfwSetKeyCallback(window->GLFWpointer,escape_key_callback);
    do {
        // keep reseting the keyhandler and poll events to store to the key handler
        keyHandler ={};
        glfwWaitEvents();
        // If program needs to be terminated, quit
        if(glfwWindowShouldClose(window->GLFWpointer) != 0){
            exitFlag = SCENE_QUIT;

            glfwSetKeyCallback(window->GLFWpointer,NULL);
            return exitFlag;
        }
        // If the escape key was pressed return to menu
        if(keyHandler.size() ){
            exitFlag = SCENE_MENU;

            glfwSetKeyCallback(window->GLFWpointer,NULL);
            return exitFlag;
        }
    }
    while (true);


}

int show_tutorial(Window* window, CircleShader* circleShader,TextureShader* tutorialTex, vec2d tutorialSize, TextureShader* background){
    // Default
    int exitFlag = SCENE_TUTORIAL;

    vec2d winSize = {640, 480};

    // Setup a keyHandler, which is a list of buttons pressed
    keyHandler = {};
    glfwSetKeyCallback(window->GLFWpointer, tutorial_key_callback);
    glfwSetTime(0);

    // Setup joystick support
    bool joyOut = false;
    int joyCount = 0;

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT);
    if(background!=NULL){
        background->draw();
    }

    // Scale the tutorial image to window size
    winSize = window->windowSize();
    tutorialTex->tMatrixReset();
    tutorialTex->tMatrixScale({tutorialSize[0]*window->pixRatio/(winSize[0]*25), tutorialSize[1]*window->pixRatio/(winSize[1]*25)});

    // Draw the universe objects
    window->drawObjectList(circleShader);

    // Draw the tutorial image
    tutorialTex->draw();

    // Display the display buffer to the user
    glfwSwapBuffers(window->GLFWpointer);
    const float* axisStates;
    // While not moving to another scene
    while(exitFlag == SCENE_TUTORIAL){

        // Poll keyboard events
        glfwPollEvents();
        if(Joystick){
            axisStates = glfwGetJoystickAxes(GLFW_JOYSTICK_1,&joyCount);
            for ( int ii = 0; ii < joyCount; ii++ ) {
                if  ( axisStates[ii] > 0.2 || axisStates[ii] < -0.2 ) {
                    joyOut = true;
                    break;
                }
            }
        }
        if ( (keyHandler.size()||joyOut) && glfwGetTime() > 1){
            exitFlag = SCENE_INGAME;
        }
        if ( glfwWindowShouldClose(window->GLFWpointer) ) {
            exitFlag = SCENE_QUIT;
        }
    }

    // Reset the key callback function
    glfwSetKeyCallback(window->GLFWpointer, NULL);

    // Move on to the next screen
    return exitFlag;
}

int show_menu(Window* window, TextureShader * menuMultiTex, std::vector<glm::mat3> menuElementTMat, CircleShader * circleShader, TextureShader* background){
    int exitFlag = SCENE_MENU;

    //get set resources;
    glClearColor(0.2, 0.2, 0.3, 1.0);
    // Set button that needs to be highlighted and button that the mouse hovers over to none
    int highlightedButton = -1;
    int mousedButton = -1;

    // place to store the cursor position
    vec2d cursorPos;
    // set the cursor mode to arrow and create cursors to change to
    bool cursorMode = false;
    GLFWcursor* arrowCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    GLFWcursor* handCursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);

    // A parameter to check if a selecting button is be pressed in the iteration
    bool buttonHit = false;

    // parameters for the keyhandler (empty keyhandler from last scene and add a callback function for all buttons)
    keyHandler = {};
    glfwSetKeyCallback(window->GLFWpointer, tutorial_key_callback);

    // parameters to store data for the Joystick check
    const float* axisStates;
    const unsigned char* buttonStates;
    glfwSetTime(0);
    int joyCount;
    double lastTime = 0;
    while(exitFlag == SCENE_MENU){
        // reset buttonhit
        buttonHit = false;
        // Do a physics step and draw the universe
        glClear(GL_COLOR_BUFFER_BIT);
        if(background!=NULL){
            background->draw();
        }
        window->drawObjectList(circleShader);
        window->boundUniverse->simulate_one_time_unit(window->fps);
        double newWidthScale = initScreenRatio/(window->windowSize()[0]/window->windowSize()[1]);

        // If a joystick was present while entering the menu check the buttons and the first axis
        // change highlighted button and button hit accordingly

        if(Joystick){
            axisStates = glfwGetJoystickAxes(GLFW_JOYSTICK_1,&joyCount);
            if(glfwGetTime() -lastTime >0.1 && joyCount>1) {
                if (axisStates[1] > 0.9) {
                    if (++highlightedButton > 3) {
                        highlightedButton = 1;
                    };
                    lastTime = glfwGetTime();
                } else if (axisStates[1] < -0.9) {
                    if (--highlightedButton <= 0) {
                        highlightedButton = 3;
                    };
                    lastTime = glfwGetTime();
                }
            }
            buttonStates =glfwGetJoystickButtons(GLFW_JOYSTICK_1,&joyCount);
            for(int ii = 0; ii <joyCount; ii++){
                if(buttonStates[ii] == GLFW_PRESS){
                    buttonHit = true;
                    break;
                }
            }
        }
        // Check what buttons were pressed in the last poll, change highlighted button or buttonhit accordingly
        for(std::vector<int>::iterator ii = keyHandler.begin(); ii != keyHandler.end(); ii++ ){
            switch(*ii){
                case GLFW_KEY_UP :
                    if(--highlightedButton <=0){
                        highlightedButton = 3;
                    };
                    break;
                case GLFW_KEY_DOWN :
                    if(++highlightedButton >3){
                        highlightedButton = 1;
                    };
                    break;
                case GLFW_KEY_ESCAPE :
                    if(highlightedButton == 3){
                        exitFlag = SCENE_QUIT;
                    }else{
                        highlightedButton = 3;
                    }
                    break;
                case GLFW_KEY_ENTER :
                case GLFW_KEY_SPACE :
                case GLFW_KEY_KP_ENTER : buttonHit = true; break;
            }
        }

        // Check if mouse hovers over button and draw the menu elements (also scale if they are highlighted )
        cursorPos = window->cursorPosition();
        mousedButton = -1;
        for(int ii = 0; ii < menuElementTMat.size(); ii++) {
            // Set position and apply appropriate rescale if window proportions have been altered
            menuMultiTex->transformationMatrix = menuElementTMat[ii];
            menuMultiTex->tMatrixScale({newWidthScale,1});
            // check if cursor is positioned ontop of the menu element and set it to highlighted
            if(cursorPos[0] > menuElementTMat[ii][0][2] - menuElementTMat[ii][0][0] && cursorPos[0] <menuElementTMat[ii][0][2] + menuElementTMat[ii][0][0]){
                if(cursorPos[1] > menuElementTMat[ii][1][2] - menuElementTMat[ii][1][1] && cursorPos[1] <menuElementTMat[ii][1][2] + menuElementTMat[ii][1][1]){
                    mousedButton = ii;
                    if(mousedButton > 0){
                        highlightedButton = mousedButton;
                    }
                }
            }
            // upscale the highlighted button
            if(highlightedButton > 0 && ii == highlightedButton) {
                menuMultiTex->transformationMatrix[0][0] *= 1.2;
                menuMultiTex->transformationMatrix[1][1] *= 1.2;
            }
            // draw the menu element
            menuMultiTex->draw(ii);
        };
        // swap screen buffers and poll events (reset keyHandler)
        glfwSwapBuffers(window->GLFWpointer);
        keyHandler = {};
        glfwPollEvents();
        // Check if mouse or button is selecting a button and switch to the corresponding scene
        if(glfwGetMouseButton(window->GLFWpointer, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
            //// BUTTON MEANING
            switch(mousedButton){
                case -1: //no highlight
                case 0:  break;  //menu name highlight
                case 1: exitFlag = SCENE_ABOUT; break;
                case 2: exitFlag = SCENE_GENESIS; break;
                case 3: exitFlag = SCENE_QUIT; break;
            }
        }else if(buttonHit){
            //// BUTTON MEANING
            switch(highlightedButton){
                case -1: //no highlight
                case 0:  break;  //menu name highlight
                case 1: exitFlag = SCENE_ABOUT; break;
                case 2: exitFlag = SCENE_GENESIS; break;
                case 3: exitFlag = SCENE_QUIT; break;
            }
        }
        // switch cursor modes when hovering over a button
        if(cursorMode && mousedButton<=0){
            cursorMode = false;
            glfwSetCursor(window->GLFWpointer,arrowCursor);
        }
        if(!cursorMode && mousedButton >0){
            cursorMode = true;
            glfwSetCursor(window->GLFWpointer,handCursor);
        }

        // If the program is asked to be terminated it, CLOSE IT.
        if(glfwWindowShouldClose(window->GLFWpointer) != 0){
            exitFlag = SCENE_QUIT;
        }

        // Do frame pacing
        window->pace_frame();
    }
    // Set the cursor back to normal after the button is pressed
    glfwSetCursor(window->GLFWpointer,arrowCursor);
    // Reset callback function
    glfwSetKeyCallback(window->GLFWpointer, NULL);
    return exitFlag;

}

// Callback function to filter for Escape key and push to keyhandler
void escape_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        keyHandler.push_back(key);
    }
}
// Callback function to push all pressed keys to the keyhandler
void tutorial_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(action == GLFW_PRESS){
        keyHandler.push_back(key);
    }
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
    // Create a standard set of UVs for each part of the texture
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
    // change the vertices and coordinates of the textureShader
    myMultiTex->setNewUVCoordinates(coords, &UVs[0], &vertices[0]);
    // Handle the output interpretation
    const int numBoxes = sizeof(objectOriginCoords)/(sizeof(GLfloat)*2);
    if(numBoxes != coords/8){
        std::cerr << "[Warning]: The vertical coordinates do not match the amount of coordinates in UVs. horCoords: "<< numBoxes << " images, UVs: " << coords/(double)8 << " images." << std::endl;
    }
    // Normalize the object sizes and positions and create a matrix out of it.
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

// Generate a standard universe with hardcoded properties
void generateStandardUniverse (Window* window) {
    // Create 3 objects and set the position, velocity, radius and colour of them.
    Object* A = window->boundUniverse->add_object();
    Object* B = window->boundUniverse->add_object();
    Object* C = window->boundUniverse->add_object();

    A->set_position(5, 5);
    B->set_position(-5, 3);
    C->set_position(0, 7);

    A->set_velocity(2, 3);
    B->set_velocity(-2, 0);
    C->set_velocity(-4, -4);

    A->set_radius(0.6);
    B->set_radius(1.2);
    C->set_radius(0.4);

    A->set_colour({0.3, 0.5, 0.1, 1.});
    B->set_colour({0.6, 0.2, 0.8, 1.});
    C->set_colour({0.2, 0.2, 0.9, 1.});

    // Add a player
    Player* player = new Player();
    player->set_colour({1.0, 0., 0., 1.});
    player->set_bounciness(0.5);
    window->boundUniverse->add_object(player);

    // Bind the player for global access (for instance addRandomObject)
    boundPlayer = player;
}


void addRandomObject(Universe* universe, unsigned seed) {
    // Give the seed to the random function. Make one based on time if no seed is presented
    if(!seed){
        srand(time(NULL)*1234);
    }else{
        srand(seed);
    }

    // define limits for the object properties and a minimum distance from the player
    std::array<double,2> radiusLim = {0.5, 1};
    std::array<double,2> massLim = {0.5, 10};
    std::array<double,2> velocityLim = {-8, 8};
    std::array<double,2> bouncyLim = {0.5, 0.9};
    double AVOID_RADIUS = 5;

    // create a object and set randomized parameters scaled to the limits
    Object* A = new Object;
    A->set_mass((std::rand()/(double)RAND_MAX)*(massLim[1]-massLim[0])+massLim[0]);
    A->set_velocity((std::rand()/(double)RAND_MAX)*(velocityLim[1]-velocityLim[0])+velocityLim[0],(std::rand()/(double)RAND_MAX)*(velocityLim[1]-velocityLim[0])+velocityLim[0]);
    A->set_radius((std::rand()/(double)RAND_MAX)*(radiusLim[1]-radiusLim[0])+radiusLim[0]);
    A->set_bounciness((std::rand()/(double)RAND_MAX)*(bouncyLim[1]-bouncyLim[0])+bouncyLim[0]);
    //A->set_colour({std::rand()/(double)RAND_MAX,std::rand()/(double)RAND_MAX,std::rand()/(double)RAND_MAX,1.0});

    //Set a limit for position and keep resetting position untill it does not collide with any objects and is a minimum distance from BoundPlayer
    std::array<double,2> xLim = {-universe->width/2+A->radius, universe->width/2-A->radius};
    std::array<double,2> yLim = {-universe->height/2+A->radius, universe->height/2-A->radius};
    do{
        // Get a new position which is not close to the player, if it exists
        if ( boundPlayer != NULL ) {
            do {
                A->set_position((std::rand()/(double)RAND_MAX)*(xLim[1]-xLim[0])+xLim[0], (std::rand()/(double)RAND_MAX)*(yLim[1]-yLim[0])+yLim[0]);
            }
            while (universe->physics.distance_between(A, boundPlayer) < AVOID_RADIUS);
        }
        else {
            // Otherwise just prevent a collision
            A->set_position((std::rand()/(double)RAND_MAX)*(xLim[1]-xLim[0])+xLim[0], (std::rand()/(double)RAND_MAX)*(yLim[1]-yLim[0])+yLim[0]);
        }

    }
    while(collidesWithAny(A, universe));
    // add the object
    universe->add_object(A);

}
// add multiple objects at once
void addRandomObjects(Universe* universe, unsigned seed, int objectAmount) {
    for(int ii = 0; ii < objectAmount; ii++){
        addRandomObject(universe, seed+ii*1234);
    }
}
// check if object overlaps with any other object in universe
bool collidesWithAny(Object* obj, Universe* uni) {
    for(int ii = 0; ii < uni->objects.size(); ii++){
        if(uni->objects[ii] != obj){
            if(uni->physics.distance_between(uni->objects[ii],obj) < (uni->objects[ii]->radius + obj->radius) ){
                return true;
            }
        }
    }

    return false;
}