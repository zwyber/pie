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