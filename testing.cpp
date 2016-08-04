//
// Created by paul on 8/2/16.
//

#include "testing.h"

void debug_display_world (Universe &universe) {
    // Iterate over all objects
    for(int ii = 0; ii < universe.objects.size(); ++ii) {
        vec2d pos = universe.objects[ii].get_position();
        vec2d vel = universe.objects[ii].get_velocity();
        int id = universe.objects[ii].get_id();

        std::cout << "Object ID " << id << ", index " << ii << ", at position (" << pos[0] << ", " << pos[1] << ") and velocity (";
        std::cout << vel[0] << ", " << vel[1] << ")." << std::endl;
    }
}

void debug_display_id_to_object_mapping (Universe &universe) {
    // Iterate over all items in the mapping
    Object* objref;
    std::map<id_type, Object*>::iterator it = universe.id_to_object_map.begin();
    for (; it != universe.id_to_object_map.end(); ++it) {
        objref = it->second;
        std::cout << "Mapping id " << it->first << " is mapped to object id " << objref->get_id() << std::endl;
    }
}




void test_00 () {

    // Show the user we are running test 00
    std::cout << "Running test_00..." << std::endl << std::endl;

    // Make the universe
    Universe universe;

    // Open a new scope when creating objects, see below why
    id_type A_id;
    id_type B_id;
    {
        // Generate a few objects
        Object A;
        Object B;

        // Give them new positions
        A.set_position(1, 1);
        B.set_position(-1, 2);

        // Add the objects to the universe
        A_id = universe.add_object(A);
        B_id = universe.add_object(B);

        // This is pointless, as a copy is stored in the universe! By keeping
        // the objects in a scope you can prevent problems like this.
        A.set_position(2, 2);
    }

    // Show a few things about the world we just created
    debug_display_world(universe);
    debug_display_id_to_object_mapping(universe);

    std::cout << "\nUpdating the positions\n";

    // You now need pointers to find the objects back! We get a pointer back by giving the index number
    Object* A = universe.get_object_by_index(0);
    Object* B = universe.get_object_by_index(1);

    // Or you can do this by id!
    A = universe.get_object_by_id(A_id);
    B = universe.get_object_by_id(B_id);

    // So to update the position you have to do this
    A->set_position(2, 2);
    A->set_velocity(3, 1);

    B->set_position(-2, -2);
    B->set_velocity(-1, -2);

    // Display the world to the commandline
    debug_display_world(universe);

    // Now let's try to remove an object
    std::cout << "\n Removing A" << std::endl;
    universe.remove_object_by_id(A_id);

    debug_display_world(universe);
    debug_display_id_to_object_mapping(universe);

    // Now create a new object
    std::cout << "\n Adding another object C " << std::endl;
    id_type C_id;
    {
        Object C;
        C.set_position(0.5, 0.5);
        C_id = universe.add_object(C);
    }

    // And show the mapping and world
    debug_display_id_to_object_mapping(universe);
    debug_display_world(universe);

    std::cout << "\n Removing B" << std::endl;

    universe.remove_object_by_id(B_id);
    debug_display_world(universe);
}

void test_01() {
    // Test collision detection
    std::cout.setf(std::ios::boolalpha); // make the stream interpret booleans as true/false instead of 1/0
    std::cout << "Running test_01..." << std::endl << std::endl;

    // Generate a few objects
    Object A;
    Object B;
    // Set positions and velocities
    A.set_position(50, 20);
    B.set_position(49.2, 19.2);

    A.set_velocity(-2,-1);
    B.set_velocity(1,1);

    // Generate a universe
    Universe universe;
    // Make some id_tag duplicates
    id_type A_id;
    id_type B_id;
    // Add the objects to the universe
    A_id = universe.add_object(A);
    B_id = universe.add_object(B);
    // Get the pointers of the representative objects in the universe
    Object* pA = universe.get_object_by_id(A_id);
    Object* pB = universe.get_object_by_id(B_id);


    /////// COLLISION DETECTION
    // Print initial conditions
    std::cout << "Collision detection scenario 1:" << std::endl;
    debug_display_world(universe);
    // Collision of objects moving toward each other and overlapping
    std::cout << "Collision between object A and B is: " << universe.check_collision(pA, pB) <<"\n" << std::endl;

    pA->set_velocity(2,-1);
    pB->set_velocity(1,1);

    std::cout << "Collision detection scenario 2:" << std::endl;
    debug_display_world(universe);
    // Collision of objects moving just away from each other and overlapping
    std::cout << "Collision between object A and B is: " << universe.check_collision(pA, pB) <<"\n" << std::endl;

    pA->set_velocity(-2,-1);
    pB->set_velocity(1,1);

    pA->set_position(50,20);
    pB->set_position(48,18);

    std::cout << "Collision detection scenario 3:" << std::endl;
    debug_display_world(universe);
    // Collision of objects moving toward each other and not overlapping
    std::cout << "Collision between object A and B is: " << universe.check_collision(pA, pB) <<"\n" << std::endl;

    ////// COLLISION RESOLVING
    std::cout << "Collision resolving scenario:" << std::endl;
    pA->set_velocity(-2,-1);
    pB->set_velocity(1,1);

    pA->set_position(50,20);
    pB->set_position(49.2,19.2);
    debug_display_world(universe);

    if(universe.check_collision(pA,pB)){
        std::cout << "Collision between object A and B is: " << universe.check_collision(pA, pB) <<". Resolving collision..." << std::endl;
        universe.resolve_collision(pA,pB);
        std::cout << "Scenario after collision: " << std::endl;
        debug_display_world(universe);

    }
}