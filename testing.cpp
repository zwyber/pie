// Created by paul on 8/2/16.
//

#include "testing.h"
#include <time.h>

void debug_display_world (Universe &universe) {
    // Iterate over all objects
    for(int ii = 0; ii < universe.objects.size(); ++ii) {
        vec2d pos = universe.objects[ii]->get_position();
        vec2d vel = universe.objects[ii]->get_velocity();

        std::cout << "Object " << universe.objects[ii] << ", index " << ii << ", at position (" << pos[0] << ", " << pos[1] << ") and velocity (";
        std::cout << vel[0] << ", " << vel[1] << ")." << std::endl;
    }
}

bool CollidesWithAny(Object* obj, Universe &uni){
    for(int ii = 0; ii < uni.objects.size(); ii++){
        if(uni.objects[ii] != obj){
            if(uni.physics.check_collision(uni.objects[ii],obj)){
                return true;
            }
        }
    }
    return false;
}

void addRandomObjects(Universe &universe, unsigned seed, int objectAmount){
    if(!seed){
        srand(time(NULL));
    }else{
        srand(seed);
    }
    std::array<double,2> radiusLim = {0.2, 1};
    std::array<double,2> massLim = {0.1, 1};
    std::array<double,2> velocityLim = {-1, 1};
    for(int ii = 0; ii < objectAmount; ii++){
        Object* A = new Object;
        A->set_mass((std::rand()/(double)RAND_MAX)*(massLim[1]-massLim[0])+massLim[0]);
        A->set_velocity((std::rand()/(double)RAND_MAX)*(velocityLim[1]-velocityLim[0])+velocityLim[0],(std::rand()/(double)RAND_MAX)*(velocityLim[1]-velocityLim[0])+velocityLim[0]);
        A->set_radius((std::rand()/(double)RAND_MAX)*(radiusLim[1]-radiusLim[0])+radiusLim[0]);
        //A->bouncyness = std::rand()/(double)RAND_MAX;
        std::array<double,2> xLim = {-universe.width/2+A->get_radius(), universe.width/2-A->get_radius()};
        std::array<double,2> yLim = {-universe.height/2+A->get_radius(), universe.height/2-A->get_radius()};
        do{
            A->set_position((std::rand()/(double)RAND_MAX)*(xLim[1]-xLim[0])+xLim[0], (std::rand()/(double)RAND_MAX)*(yLim[1]-yLim[0])+yLim[0]);
        }while(CollidesWithAny(A, universe));
        universe.add_object(A);
    }
}

void test_00 () {

    // Show the user we are running test 00
    std::cout << "Running test_00..." << std::endl << std::endl;

    // Make the universe
    Universe universe;

    // Open a new scope when creating objects, see below why
    Object* A;
    Object* B;
    {
        // Generate a few objects
        A = new Object;
        B = new Object;

        // Give them new positions
        A->set_position(1, 1);
        B->set_position(-1, 2);

        // Add the objects to the universe
        universe.add_object(A);
        universe.add_object(B);
    }

    // Show a few things about the world we just created
    debug_display_world(universe);

    std::cout << "\nUpdating the positions\n";

    // So to update the position you have to do this
    A->set_position(2, 2);
    A->set_velocity(3, 1);

    B->set_position(-2, -2);
    B->set_velocity(-1, -2);

    // Display the world to the commandline
    debug_display_world(universe);

    // Now let's try to remove an object
    std::cout << "\n Removing A" << std::endl;


    debug_display_world(universe);

    // Now create a new object
    std::cout << "\n Adding another object C " << std::endl;
    Object* C;
    {
        C = new Object;
        C->set_position(0.5, 0.5);
        universe.add_object(C);
    }

    // And show the mapping and world
    debug_display_world(universe);

    std::cout << "\n Removing B" << std::endl;

    universe.remove_object(B);
    debug_display_world(universe);
}

void test_01() {
    // Test collision detection
    std::cout.setf(std::ios::boolalpha); // make the stream interpret booleans as true/false instead of 1/0
    std::cout << "Running test_01..." << std::endl << std::endl;

    // Generate a few objects
    Object* A = new Object;
    Object* B = new Object;
    // Set positions and velocities
    A->set_position(50, 20);
    B->set_position(49.2, 19.2);

    A->set_velocity(-2,-1);
    B->set_velocity(1,1);

    // Generate a universe
    Universe universe(720.0, 480.0);
    // check if size print worked.
    std::cout << "Universe size: " << universe.width << "x" << universe.height << std::endl;

    // Add the objects to the universe
    universe.add_object(A);
    universe.add_object(B);

    //// COLLISION DETECTION
    // Print initial conditions
    std::cout << "Collision detection scenario 1:" << std::endl;
    debug_display_world(universe);
    // Collision of objects moving toward each other and overlapping
    std::cout << "Collision between object A and B is: " << universe.physics.check_collision(A, B) <<"\n" << std::endl;

    A->set_velocity(2,2);
    B->set_velocity(1,1);

    std::cout << "Collision detection scenario 2:" << std::endl;
    debug_display_world(universe);
    // Collision of objects moving just away from each other and overlapping
    std::cout << "Collision between object A and B is: " << universe.physics.check_collision(A, B) <<"\n" << std::endl;

    A->set_velocity(-2,-1);
    B->set_velocity(1,1);

    A->set_position(50,20);
    B->set_position(48,18);

    std::cout << "Collision detection scenario 3:" << std::endl;
    debug_display_world(universe);
    // Collision of objects moving toward each other and not overlapping
    std::cout << "Collision between object A and B is: " << universe.physics.check_collision(A, B) <<"\n" << std::endl;

    ////// COLLISION RESOLVING
    std::cout << "Collision resolving scenario:" << std::endl;
    A->set_velocity(-2,-1);
    B->set_velocity(1,1);

    A->set_position(50,20);
    B->set_position(49.2,19.2);
    debug_display_world(universe);

    if(universe.physics.check_collision(A,B)){
        std::cout << "Collision between object A and B is: " << true <<". Resolving collision..." << std::endl;
        universe.physics.resolve_collision(A,B);
        std::cout << "Scenario after collision: " << std::endl;
        debug_display_world(universe);

    }
}

void test_02() {

    // Create a universe
    Universe universe;

    // Add a bunch of objects
    Object* A = new Object;
    A->set_velocity(0, 1);
    for ( int ii = 0; ii < 10; ++ii) {
        A->set_position(ii, ii);
        universe.add_object(A);
    }

    std::cout << "Initial conditions: \n";
    debug_display_world(universe);

    // Iterate the engine a few times
    for (int ii = 0; ii < 5; ++ii) {
        universe.physics_runtime_iteration();

        std::cout << "\nIteration " << ii << ":\n";
        debug_display_world(universe);
    }

}

void test_03 () {

    // Create a universe
    Universe universe;

    // Create two objects
    Object* A = new Object;
    Object* B = new Object;

    // Set them apart, and on a collision course
    A->set_position(2, 0);
    A->set_velocity(-1, 0);
    A->set_radius(1);

    B->set_position(-2, 0);
    B->set_velocity(1, 0.1);
    B->set_radius(2);

    // Add them to the universe
    universe.add_object(A);
    universe.add_object(B);

    // Open a file
    std::ofstream outputfile;
    outputfile.open("test_03.csv");

    // Start to iterate 10000 cycles
    for (int ii = 0; ii < 10000; ++ii) {
        // Output the positions of the two objects to a file
        vec2d posA = A->get_position();
        vec2d posB = B->get_position();
        outputfile << posA[0] << "," << posA[1] << "," << posB[0] << "," << posB[1] << ";" << std::endl;

        // Iterate the physics
        universe.physics_runtime_iteration();
    }

}

void test_004 () {
    Universe universe;


    Object* A = new Object;
    Object* B = new Object;

    universe.add_object(A);
    universe.add_object(B);

    A->set_position(5, 2);
    B->set_position(3, 1);
    A->set_mass(-1.5);
    vec2d acceleration = universe.physics.acceleration(A,B);

    A->calc_new_pos_vel(universe.objects, universe.physics);

    std::cout << "Acceleration of ObjectA With Respect to ObjectB =" << acceleration[0] << "," << acceleration[1] << std::endl;

    debug_display_world(universe);

}

void test_05 () {
    // write a test function to see if physics engine is working.
    Universe universe;


    // Add them to the universe
    Object* A = new Object;
    Object* B = new Object;

    // Set them apart, and on a collision course
    A->set_position(2, 0);
    A->set_velocity(0, 0);


    B->set_position(-2, 0);
    B->set_velocity(0, 0);


    universe.add_object(A);
    universe.add_object(B);

    // Open a file
    std::ofstream outputfile;
    outputfile.open("test_03.csv");

    // Start to iterate 10000 cycles
    for (int ii = 0; ii < 10000; ++ii) {
        // Output the positions of the two objects to a file
        vec2d posA = A->get_position();
        vec2d posB = B->get_position();
        outputfile << posA[0] << "," << posA[1] << "," << posB[0] << "," << posB[1] << ";" << std::endl;

        // Iterate the physics
        universe.physics_runtime_iteration();
    }
}

void test_06() {
    ////// Press Z to zoom
    // Window dimensions [px]
    int width = 720;
    int height = 480;
    // universe dimensions [px]
    int universeWidth = 640;
    int universeHeight = 400;

    int AmountOfObjects = 4;

    // window already makes GLFWpointer currentContext so no need to call it again later on.

    // set ration of [px/m]
    double pixRatio = 25;

    Object* A = new Object;
    Object* B = new Object;

    // Set them apart, and on a collision course
    //A->set_position(-2, -2);
    //A->set_velocity(2,0);
    //A->set_mass(0.5);
    //A->bouncyness = 1;

    //B->set_position(2, 2);
    //B->set_velocity(0, 1);
    //B->set_mass(0.5);
    //B->set_radius(1);

    // Generate a universe
    Universe universe(universeWidth/pixRatio, universeHeight/pixRatio);
    Window window = Window(&universe, pixRatio);
    //// Look in the addRandomObjects function to find the parameter range of objects
    addRandomObjects(universe,0,AmountOfObjects);

    // Add them to the universe
    //universe.add_object(A);
    //universe.add_object(B);
    //vec2d posA;
    //vec2d posB;
    // Set the buffer clear color to:
    glClearColor(0.2, 0.2, 0.3, 1.0);
    bool HOLDZ = false;
    do{
        // Clear the buffers to set values (in our case only colour buffer needs to be cleared)
        glClear(GL_COLOR_BUFFER_BIT);
        // Draw a grid with m/div
        window.drawGrid(window.pixRatio);
        // Draw the universe's objects on top of that
        window.drawObjectList(universe.objects);
        // Draw A box on the boundaries of the universe
        window.drawBox(universe.width, universe.height);
        // Do a physics iteration
        universe.physics_runtime_iteration();
        //posA = A->get_position();
        //posB = B->get_position();
        // std::cout << "Position of A: " << posA[0] << "," << posA[1] << " Position of B: " << posB[0] << "," << posB[1] << ";" << std::endl;

        // Swap buffers
        glfwSwapBuffers(window.GLFWpointer);
        glfwPollEvents();

        // This makes you zoom between 25 and 50 pixels per m
        if(glfwGetKey(window.GLFWpointer, GLFW_KEY_Z) == GLFW_PRESS){
            if(!HOLDZ){
                HOLDZ = true;
                window.changeResizeFlag(vis::ZOOM_UNIVERSE);
            }
        }else{
            HOLDZ = false;
        }

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window.GLFWpointer, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window.GLFWpointer) == 0 );

    // Close OpenGL window and terminate GLFW
    glfwTerminate();
}
double potentialEnergy(Object* X, Object* Y, Universe* uni){
    double r = uni->physics.distance_between(Y , X );
    return -uni->physics.G *X->get_mass()*Y->get_mass()/r;
}
void test_07() {
    //// WRITES SYSTEM ENERGY TO FILE

    int width = 600;
    int height = 400;
    Object* A = new Player;
    Object* B = new Object;
    Object* C = new Object;
    Object* D = new Object;

    Window window = Window(width,height,NULL,30,vis::AUTO_SIZE_UNIVERSE);

    // Set them apart, and on a collision course
    A->set_position(2, 0);
    A->set_velocity(0,0);
    A->set_radius(0.5);
    A->set_mass(5);
    A->bouncyness = 1;

    B->set_position(2, 2);
    B->set_velocity(0, -1);
    B->set_mass(2);
    //B->bouncyness = 0.7;

    C->set_position(-2, -2);
    C->set_mass(2);
    //C->bouncyness = 0.7;

    D->set_position(-4, -4);
    D->set_mass(2);
    //D->bouncyness = 0.7;



    // Generate a universe
    Universe universe(width/window.pixRatio, height/window.pixRatio);
    window.bindUniverse(&universe);
    // Add them to the universe
    universe.add_object(A);
    universe.add_object(B);
    universe.add_object(C);
    universe.add_object(D);

    vec2d posA;
    vec2d posB;


    std::ofstream outputfile;
    outputfile.open("test_07.csv");

    Universe* uni = &universe;
    do{

        for(int ii = 0; ii < universe.objects.size(); ii++){
            double vel2 = len_squared(universe.objects[ii]->get_velocity());
            outputfile << 0.5*universe.objects[ii]->get_mass()*vel2 << ",";

            double ener = 0;
            for (int qq = 0; qq < universe.objects.size(); ++qq ) {
                // Make sure you are not calculating yourself
                if (qq == ii) {
                    // This is myself, skip this loop iteration
                    continue;
                }
                // Here add up the contribution to the acceleration
                ener += potentialEnergy(universe.objects[ii], universe.objects[qq],uni);
            }
            //// Made the potential energy per object only store half, as for each potential between 2 objects 2 objects store it but there is only one potential.
            if(ii == universe.objects.size()-1){
                outputfile << ener/2 << ";" << std::endl;
            }else{
                outputfile << ener/2 << ",";

            }
        }
        // Clear the buffers to set values (in our case only colour buffer needs to be cleared)
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw the universe's objects on top of that
        window.drawObjectList(universe.objects);

        // Do a physics iteration
        universe.simulate_one_time_unit(window.fps);

        // Swap buffers
        glfwSwapBuffers(window.GLFWpointer);
        glfwPollEvents();

        window.pace_frame();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window.GLFWpointer, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window.GLFWpointer) == 0 );

    outputfile.close();
    // Close OpenGL window and terminate GLFW
    glfwTerminate();

}

void test_08() {
    /// Tries to draw in different way
    int universeWidth = 720;
    int universeHeight = 480;

    int AmountOfObjects = 3;

    double pixRatio = 50;

    Universe universe(universeWidth/pixRatio, universeHeight/pixRatio);
    Window window = Window(&universe, pixRatio);

    glClearColor(0.2, 0.2, 0.3, 1.0);

    addRandomObjects(universe,1,AmountOfObjects);


    freetype::font_data myFont;
    myFont.init("frabk.ttf", 32);

    std::string text = "Lorem ipsum dolor sit amet, consectetuer adipiscing elit.";
    //char text[128];

    std::cout << text << "does this work?" << std::endl;

    do{
        // Clear the buffers to set values (in our case only colour buffer needs to be cleared)
        glClear(GL_COLOR_BUFFER_BIT);

        freetype::print(myFont,2,2,text.c_str());


        // Draw the universe's objects on top of that
        window.drawObjectList(universe.objects);

        // Do a physics iteration
        universe.physics_runtime_iteration();

        // Swap buffers
        glfwSwapBuffers(window.GLFWpointer);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window.GLFWpointer, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window.GLFWpointer) == 0 );

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

}