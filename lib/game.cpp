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

    Window window = Window();

    do {

        // I don't like switch :(
        // Don't worry I think the if statements in a while loop makes more sense in our case ;)
        if (scene == SCENE_MENU) {

            GLuint menuTex = loadDDS("menuDummy.DDS");

            // Init shader to heap memory
            TextureShader *menuMultiTex = new TextureShader(menuTex);
            // Load menu resources
            std::vector<glm::mat3> tMats = loadMenuResources(menuMultiTex);

            // Create a universe and bind it to the window
            Universe *universe = new Universe();
            window.bindUniverse(universe);

            // Show the menu
            scene = show_menu(&window, menuMultiTex, tMats);

            // Clear all heap variables
            delete menuMultiTex;
            delete universe;

        }

        if (scene == SCENE_ABOUT) {
            scene = SCENE_MENU;

        }

        if (scene == SCENE_GENESIS) {
            scene = SCENE_MENU;

        }

    }
    while(scene != SCENE_QUIT);

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
            360, 430,
            360, 630,
            360, 830
    };
    GLfloat  objectSizes[]{ // Width and hight of the objects
            585, 164,
            300, 100,
            300, 100,
            300, 100
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

    // Setup a few objects in the universe
    addRandomObjects(window->boundUniverse, std::rand()/(double)RAND_MAX, 50);

    //get set resources;
    glClearColor(0.2, 0.2, 0.3, 1.0);
    int highlightedButton = -1;
    vec2d cursorPos;
    //GLdouble R = 0.1;
    while(exitFlag == SCENE_MENU){
        // Do a physics step and draw the universe
        glClear(GL_COLOR_BUFFER_BIT);
        window->drawObjectList();
        window->boundUniverse->physics_runtime_iteration();

        //draw the menu;
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
                case 1: exitFlag = SCENE_GENESIS; break;
                case 2: exitFlag = SCENE_ABOUT; break;
                case 3: exitFlag = SCENE_QUIT; break;
            }
        }
        if(glfwGetKey(window->GLFWpointer, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(window->GLFWpointer) != 0){
            exitFlag = SCENE_QUIT;
        }

        // Do frame pacing
        window->pace_frame();
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
        A->bouncyness = 0.9;
        std::array<double,2> xLim = {-universe->width/2+A->get_radius(), universe->width/2-A->get_radius()};
        std::array<double,2> yLim = {-universe->height/2+A->get_radius(), universe->height/2-A->get_radius()};
        do{
            A->set_position((std::rand()/(double)RAND_MAX)*(xLim[1]-xLim[0])+xLim[0], (std::rand()/(double)RAND_MAX)*(yLim[1]-yLim[0])+yLim[0]);
        }while(CollidesWithAny(A, universe));
        universe->add_object(A);
    }
}

bool CollidesWithAny(Object* obj, Universe* uni) {
    for(int ii = 0; ii < uni->objects.size(); ii++){
        if(uni->objects[ii] != obj){
            if(uni->physics.check_collision(uni->objects[ii],obj)){
                return true;
            }
        }
    }
    return false;
}