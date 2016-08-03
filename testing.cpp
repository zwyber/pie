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