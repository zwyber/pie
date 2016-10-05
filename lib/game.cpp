// Created by paul on 9/7/16.
//

#include "game.h"

// Player pointer
Player* boundPlayer = NULL;
std::vector<int> keyHandler;
bool Joystick;

void maingame(int startScene) {
    // Initialise the scene switcher
    int scene = startScene;

    // Initialise the window
    double windowWidth = 1000;
    double windowHeight = 700;

    double pixRatio = 25;

    int universeWidth = windowWidth/pixRatio;
    int universeHeight = windowHeight/pixRatio;

    bool shownTutorial = false;

    Window window = Window(pixRatio*universeWidth,pixRatio*universeHeight,vis::NO_RESIZE);
    window.pixRatio = pixRatio;
    GLuint menuTex = loadDDS("Textures/MenuTextures.DDS");
    GLuint tutorialDDS = loadDDS("Textures/Tutorial2.DDS");
    GLuint space = loadDDS("Textures/SpaceCrop.DDS");

    // Init shader
    TextureShader menuMultiTex = TextureShader(menuTex);
    TextureShader tutorialTex = TextureShader(tutorialDDS);
    TextureShader spaceTex = TextureShader(space);

    vec2d tutorialSize = {640, 480};
    CircleShader allDebrisShader;

    TextShader scoreText = TextShader("Fonts/Courier New Bold.ttf");
    TextShader aboutText = TextShader("Fonts/verdana.ttf");

    // Load menu resources
    std::vector<glm::mat3> tMats = loadMenuResources(&menuMultiTex);

    initScreenRatio = window.windowSize()[0]/(double)window.windowSize()[0];
    do {

        // I don't like switch :(
        // Don't worry I think the if statements in a while loop makes more sense in our case ;)
        if (scene == SCENE_MENU) {
            // Create a universe and bind it to the window
            Universe *universe = new Universe(universeWidth, universeHeight);
            window.bindUniverse(universe);

            // Setup a few objects in the universe
            addRandomObjects(window.boundUniverse, std::rand(), 40);

            // Show the menu
            scene = show_menu(&window, &menuMultiTex, tMats, &allDebrisShader,&spaceTex);

            // Clear all heap variables
            window.bindUniverse(NULL);
            delete universe;
        }

        if (scene == SCENE_ABOUT) {
            scene = show_about(&window, &aboutText);
            continue;
        }

        if (scene == SCENE_GENESIS) {
            // Generate a standard universe
            Universe *universe = new Universe(universeWidth, universeHeight);
            window.bindUniverse(universe);

            generateStandardUniverse(&window);
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


            glfwSetKeyCallback(window.GLFWpointer,escape_key_callback);
            glfwSwapBuffers(window.GLFWpointer);
            do {
                keyHandler ={};
                glfwPollEvents();

                if(glfwWindowShouldClose(window.GLFWpointer) != 0){
                    break;
                }
                if(keyHandler.size() ){
                    glfwSetKeyCallback(window.GLFWpointer,NULL);
                    break;
                }

                // Wait 100ms
                usleep(1E5);
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
    glfwTerminate();
}

int show_ingame (Window* window, CircleShader* circleShader, TextShader* textShader, TextureShader* background) {
    int exitFlag = SCENE_INGAME;

    // Initialise a few variables
    std::chrono::steady_clock::time_point now_time;
    std::chrono::steady_clock::duration time_elapsed;
    std::chrono::seconds seconds_passed;
    int seconds_count = 0;
    int NEW_OBJECT_DELAY = 3;
    int MORE_OBJECTS_DELAY = 2;
    bool addedAlready = false;

    // Reinitialize the universe time clock, so the start time
    window->boundUniverse->begin_time = std::chrono::steady_clock::now();

    std::stringstream scoreText;
    textShader->colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    glfwSetKeyCallback(window->GLFWpointer,escape_key_callback);
    while(exitFlag == SCENE_INGAME){
        // Do a physics step and draw the universe
        keyHandler = {};
        glClear(GL_COLOR_BUFFER_BIT);
        if(background!=NULL){
            background->draw();
        }
        window->boundUniverse->simulate_one_time_unit(window->fps);
        window->drawObjectList(circleShader);
        if(textShader!=NULL) {
            scoreText.str(std::string());
            scoreText << "Score: " << window->boundUniverse->score;
            textShader->draw(scoreText.str() ,{0, -0.92},DRAWTEXT::ALIGN_CENTER, window->windowSize() , 0.02);
        }
        // Determine if we want to add another piece of debris
        now_time = std::chrono::steady_clock::now();
        time_elapsed = now_time - window->boundUniverse->begin_time;
        seconds_passed = std::chrono::duration_cast<std::chrono::seconds>(time_elapsed);
        seconds_count = seconds_passed.count();

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

        glfwSwapBuffers(window->GLFWpointer);
        glfwPollEvents();

        if(glfwWindowShouldClose(window->GLFWpointer) != 0){
            exitFlag = SCENE_QUIT;
        }
        if(keyHandler.size() ){
            //exitFlag = SCENE_PAUSE;
            exitFlag = SCENE_MENU; // Use this until pause scene is available;
        }
    }

    glfwSetKeyCallback(window->GLFWpointer,NULL);
    return exitFlag;
}

int show_about (Window* window, TextShader* newText) {
    int exitFlag = SCENE_ABOUT;

    glClear(GL_COLOR_BUFFER_BIT);
    //// Paul please don't generate Textshaders multiple times, although more efficient they still require some recourses
    newText->colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    std::stringstream aboutText;
    aboutText << "Space Debris Evaders is a game developed for the course";
    newText->draw(aboutText.str(), {0, 0.8}, DRAWTEXT::ALIGN_CENTER, window->windowSize(), 0.02);
    aboutText.str(std::string()); // To clear the stringstream

    aboutText << "'Programming in Engineering' at the University of Twente";
    newText->draw(aboutText.str(), {0, 0.7}, DRAWTEXT::ALIGN_CENTER, window->windowSize(), 0.02);
    aboutText.str(std::string());

    aboutText << "The goal is to learn C++ by developing a game.";
    newText->draw(aboutText.str(), {0, 0.6}, DRAWTEXT::ALIGN_CENTER, window->windowSize(), 0.02);
    aboutText.str(std::string());

    aboutText << "Authors:";
    newText->draw(aboutText.str(), {0, 0.4}, DRAWTEXT::ALIGN_CENTER, window->windowSize(), 0.02);
    aboutText.str(std::string());

    aboutText << "Arash Edresi";
    newText->draw(aboutText.str(), {0, 0.3}, DRAWTEXT::ALIGN_CENTER, window->windowSize(), 0.02);
    aboutText.str(std::string());

    aboutText << "Yvan Klaver";
    newText->draw(aboutText.str(), {0, 0.2}, DRAWTEXT::ALIGN_CENTER, window->windowSize(), 0.02);
    aboutText.str(std::string());

    aboutText << "Paul van Swinderen";
    newText->draw(aboutText.str(), {0, 0.1}, DRAWTEXT::ALIGN_CENTER, window->windowSize(), 0.02);
    aboutText.str(std::string());


    aboutText << "Press ESC to return to the menu";
    newText->draw(aboutText.str(), {0, -0.4}, DRAWTEXT::ALIGN_CENTER, window->windowSize(), 0.02);
    aboutText.str(std::string());


    glfwSwapBuffers(window->GLFWpointer);
    glfwSetKeyCallback(window->GLFWpointer,escape_key_callback);
    do {
        keyHandler ={};
        glfwPollEvents();

        if(glfwWindowShouldClose(window->GLFWpointer) != 0){
            exitFlag = SCENE_QUIT;

            glfwSetKeyCallback(window->GLFWpointer,NULL);
            return exitFlag;
        }
        if(keyHandler.size() ){
            exitFlag = SCENE_MENU;

            glfwSetKeyCallback(window->GLFWpointer,NULL);
            return exitFlag;
        }

        // Wait 100ms
        usleep(1E5);
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
    int highlightedButton = -1;
    vec2d cursorPos;
    bool cursorMode = false;
    GLFWcursor* arrowCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    GLFWcursor* handCursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    while(exitFlag == SCENE_MENU){
        // Do a physics step and draw the universe
        glClear(GL_COLOR_BUFFER_BIT);
        if(background!=NULL){
            background->draw();
        }
        window->drawObjectList(circleShader);
        window->boundUniverse->simulate_one_time_unit(window->fps);
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
        if(glfwWindowShouldClose(window->GLFWpointer) != 0){
            exitFlag = SCENE_QUIT;
        }

        // Do frame pacing
        window->pace_frame();
    }
    // Set the cursor back to normal after the button is pressed
    glfwSetCursor(window->GLFWpointer,arrowCursor);
    return exitFlag;

}


void escape_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        keyHandler.push_back(key);
    }
}

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


void generateStandardUniverse (Window* window) {

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
    player->set_bouncyness(0.5);
    window->boundUniverse->add_object(player);

    boundPlayer = player;
}


void addRandomObject(Universe* universe, unsigned seed) {
    if(!seed){
        srand(time(NULL));
    }else{
        srand(seed);
    }

    std::array<double,2> radiusLim = {0.5, 1};
    std::array<double,2> massLim = {0.5, 3};
    std::array<double,2> velocityLim = {-8, 8};
    std::array<double,2> bouncyLim = {0.5, 0.9};
    double AVOID_RADIUS = 5;

    Object* A = new Object;
    A->set_mass((std::rand()/(double)RAND_MAX)*(massLim[1]-massLim[0])+massLim[0]);
    A->set_velocity((std::rand()/(double)RAND_MAX)*(velocityLim[1]-velocityLim[0])+velocityLim[0],(std::rand()/(double)RAND_MAX)*(velocityLim[1]-velocityLim[0])+velocityLim[0]);
    A->set_radius((std::rand()/(double)RAND_MAX)*(radiusLim[1]-radiusLim[0])+radiusLim[0]);
    A->set_bouncyness((std::rand()/(double)RAND_MAX)*(bouncyLim[1]-bouncyLim[0])+bouncyLim[0]);


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
    universe->add_object(A);

}

void addRandomObjects(Universe* universe, unsigned seed, int objectAmount) {
    for(int ii = 0; ii < objectAmount; ii++){
        addRandomObject(universe, seed+ii*100);
    }
}

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