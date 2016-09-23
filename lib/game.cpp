// Created by paul on 9/7/16.
//

#include "game.h"

// Player pointer
Player* boundPlayer = NULL;
std::vector<int> keyHandler;

void maingame(int startScene) {
    // Initialise the scene switcher
    int scene = startScene;

    // Initialise the window
    int universeWidth = 720;
    int universeHeight = 480;

    double pixRatio = 50;

    Window window = Window();

    GLuint menuTex = loadDDS("MenuTextures.DDS");
    GLuint tutorialDDS = loadDDS("Tutorial.DDS");

    // Init shader
    TextureShader menuMultiTex = TextureShader(menuTex);
    TextureShader tutorialTex = TextureShader(tutorialDDS);

    vec2d tutorialSize = {640, 480};
    //// I'd suggest building a circle shader for universes to use.
    CircleShader allDebrisShader;

    TextShader scoreText = TextShader("Courier New Bold.ttf");
    //TextShader aboutText = TextShader("ARIALNB.ttf"); UT house font if capitalized
    TextShader aboutText = TextShader("verdana.ttf");

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
            scene = show_about(&window,&aboutText);
            continue;
        }

        if (scene == SCENE_GENESIS) {
            // Generate a standard universe
            Universe *universe = new Universe();
            window.bindUniverse(universe);

            generateStandardUniverse(&window);

            scene = SCENE_TUTORIAL;
        }

        if (scene == SCENE_TUTORIAL) {
            /*
            glClear(GL_COLOR_BUFFER_BIT);
            window.drawObjectList(&allDebrisShader);
            glfwSwapBuffers(window.GLFWpointer);
            glfwPollEvents();
            */

            // Wait for 5 seconds, or wait until mouse is clicked, whatever
            //usleep(5E6);

            scene = show_tutorial(&window,&allDebrisShader,&tutorialTex,tutorialSize);

        }

        if (scene == SCENE_INGAME) {
            scene = show_ingame(&window, &allDebrisShader,&scoreText);

            // do not forget to delete universe (although this is better to do after SCENE_DIED)
            delete window.boundUniverse;
        }

        if (scene == SCENE_DIED) {
            // Display the score!
            std::cout << window.boundUniverse->score << std::endl;

            usleep(2E6);

            scene = SCENE_MENU;
        }

    }
    while(scene != SCENE_QUIT);
    glfwTerminate();
}


int show_ingame (Window* window, CircleShader* circleShader, TextShader* textShader) {
    int exitFlag = SCENE_INGAME;

    // Initialise a few variables
    std::chrono::steady_clock::time_point now_time;
    std::chrono::steady_clock::duration time_elapsed;
    std::chrono::seconds seconds_passed;
    int seconds_count = 0;
    int NEW_OBJECT_DELAY = 7;
    int MORE_OBJECTS_DELAY = 2;
    bool addedAlready = false;

    std::stringstream scoreText;
    textShader->colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    while(exitFlag == SCENE_INGAME){
        // Do a physics step and draw the universe
        glClear(GL_COLOR_BUFFER_BIT);
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

int show_about (Window* window, TextShader* newText) {
    int exitFlag = SCENE_ABOUT;

    glClear(GL_COLOR_BUFFER_BIT);
    //// Paul please don't generate Textshaders multiple times although more efficient they still require some recourses
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

    do {
        glfwPollEvents();

        if(glfwWindowShouldClose(window->GLFWpointer) != 0){
            exitFlag = SCENE_QUIT;
            return exitFlag;
        }
        if(glfwGetKey(window->GLFWpointer, GLFW_KEY_ESCAPE) == GLFW_PRESS ){
            exitFlag = SCENE_MENU;
            return exitFlag;
        }

        // Wait 100ms
        usleep(1E5);
    }
    while (true);


}
void tutorial_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(action = GLFW_PRESS){
        keyHandler.push_back(key);
    }
}

int show_tutorial(Window* window, CircleShader* circleShader,TextureShader* tutorialTex, vec2d tutorialSize){
    int exitFlag = SCENE_TUTORIAL;
    vec2d winSize = {640, 480};
    keyHandler = {};
    glfwSetKeyCallback(window->GLFWpointer,tutorial_key_callback);
    glfwSetTime(0);
    while(exitFlag == SCENE_TUTORIAL){
        glClear(GL_COLOR_BUFFER_BIT);
        winSize = window->windowSize();
        tutorialTex->tMatrixReset();
        tutorialTex->tMatrixScale({tutorialSize[0]/winSize[0], tutorialSize[1]/winSize[1]});
        window->drawObjectList(circleShader);
        tutorialTex->draw();
        glfwSwapBuffers(window->GLFWpointer);
        glfwPollEvents();
        if((keyHandler.size() && glfwGetTime() > 1)){
            exitFlag = SCENE_INGAME;
        }
        if(glfwWindowShouldClose(window->GLFWpointer)){
            exitFlag = SCENE_QUIT;
        }
    }
    glfwSetKeyCallback(window->GLFWpointer, NULL);
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
    window->boundUniverse->add_object(player);

    boundPlayer = player;
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

    GLuint textShader = LoadShaders("shaders/textOld.glvs", "shaders/text.glfs");

    FontTexHandler myText("frabk.ttf",32, textShader, thisGame.windowSize());
    TextShader newText("verdana.ttf");
    newText.colour = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    bool exit = false;
    int thisTime;
    std::stringstream text;
    glfwSetTime(0.0);
    while(!exit){
        glClear(GL_COLOR_BUFFER_BIT);
        thisTime = 10*glfwGetTime();
        thisTime *= 10;
        text << "Score: " <<thisTime;
        //myText.renderText(text.str(),240,210,1.0,{1.0,0.0,0.0});
        newText.draw(text.str(),{0.95, 0.9},DRAWTEXT::ALIGN_RIGHT,thisGame.windowSize(),0.03);
        glfwSwapBuffers(thisGame.GLFWpointer);
        glfwPollEvents();
        text.str(std::string());
        if(glfwWindowShouldClose(thisGame.GLFWpointer) != 0 || glfwGetKey(thisGame.GLFWpointer, GLFW_KEY_ESCAPE) == GLFW_PRESS){
            exit = true;
        }
    }
}

void addRandomObject(Universe* universe, unsigned seed) {
    if(!seed){
        srand(time(NULL));
    }else{
        srand(seed);
    }

    std::array<double,2> radiusLim = {0.5, 0.8};
    std::array<double,2> massLim = {0.3, 3};
    std::array<double,2> velocityLim = {-8, 8};

    Object* A = new Object;
    A->set_mass((std::rand()/(double)RAND_MAX)*(massLim[1]-massLim[0])+massLim[0]);
    A->set_velocity((std::rand()/(double)RAND_MAX)*(velocityLim[1]-velocityLim[0])+velocityLim[0],(std::rand()/(double)RAND_MAX)*(velocityLim[1]-velocityLim[0])+velocityLim[0]);
    A->set_radius((std::rand()/(double)RAND_MAX)*(radiusLim[1]-radiusLim[0])+radiusLim[0]);

    A->set_bouncyness(0.6);

    std::array<double,2> xLim = {-universe->width/2+A->radius, universe->width/2-A->radius};
    std::array<double,2> yLim = {-universe->height/2+A->radius, universe->height/2-A->radius};
    do{
        A->set_position((std::rand()/(double)RAND_MAX)*(xLim[1]-xLim[0])+xLim[0], (std::rand()/(double)RAND_MAX)*(yLim[1]-yLim[0])+yLim[0]);
    }

    while(CollidesWithAny(A, universe));
    universe->add_object(A);

}

void addRandomObjects(Universe* universe, unsigned seed, int objectAmount) {
    for(int ii = 0; ii < objectAmount; ii++){
        addRandomObject(universe, seed+ii);
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