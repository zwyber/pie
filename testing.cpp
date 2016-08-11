// Created by paul on 8/2/16.
//

#include "testing.h"

void debug_display_world (Universe &universe) {
    // Iterate over all objects
    for(int ii = 0; ii < universe.objects.size(); ++ii) {
        vec2d pos = universe.objects[ii]->get_position();
        vec2d vel = universe.objects[ii]->get_velocity();

        std::cout << "Object " << universe.objects[ii] << ", index " << ii << ", at position (" << pos[0] << ", " << pos[1] << ") and velocity (";
        std::cout << vel[0] << ", " << vel[1] << ")." << std::endl;
    }
}

void DrawBox(double Width, double Height){
    glBegin(GL_LINE_LOOP);
    glColor4d(1.0,0,0,1.0);
    glVertex2d(Width, Height);
    glVertex2d(-Width, Height);
    glVertex2d(-Width, -Height);
    glVertex2d(Width, -Height);
    glEnd();
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

    A->calc_new_pos_vel(universe.objects, universe.timestep, universe.physics);

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
    GLFWwindow* window;
    int width = 1600;
    int height = 900;

    int universeWidth = 700;
    int universeHeight = 700;
    visuals::pixRatio = 50;

    Object* A = new Object;
    Object* B = new Object;

    // Set them apart, and on a collision course
    A->set_position(-2, -2);
    A->set_velocity(2,0);
    A->set_mass(0.5);
    A->bouncyness = 1;

    B->set_position(2, 2);
    B->set_velocity(0, 1);
    B->set_mass(0.5);
    B->set_radius(1);

    // Generate a universe
    Universe universe(universeWidth/visuals::pixRatio, universeHeight/visuals::pixRatio);

    // Add them to the universe
    universe.add_object(A);
    universe.add_object(B);

    vec2d posA;
    vec2d posB;

    // initNewWindow already makes window currentContext so no need to call it again later on.
    window = initNewWindow(width, height);
    // Set the buffer clear color to:
    glClearColor(0.2, 0.2, 0.3, 1.0);
    do{
        // Clear the buffers to set values (in our case only colour buffer needs to be cleared)
        glClear(GL_COLOR_BUFFER_BIT);
        drawGrid(visuals::pixRatio);
        drawObjectList(universe.objects);
        DrawBox((double)universeWidth/width, (double)universeHeight/height);
        universe.physics_runtime_iteration();
        posA = A->get_position();
        posB = B->get_position();
        // std::cout << "Position of A: " << posA[0] << "," << posA[1] << " Position of B: " << posB[0] << "," << posB[1] << ";" << std::endl;

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    // Close OpenGL window and terminate GLFW
    glfwTerminate();
}