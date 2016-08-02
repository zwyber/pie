//
// Created by paul on 7/15/16.
//

#include "framework.h"
#include "testing.h"

int main() {

    // Make the universe
    Universe universe;

    // Open a new scope when creating objects, see below why
    int A_id;
    int B_id;
    {
        // Generate a few objects
        Object A;
        Object B;

        // Give them new positions
        A.set_position(1, 1);
        B.set_position(-1, 2);

        // Add the objects to the universe
        A_id = universe.add_object(A, universe);
        B_id = universe.add_object(B, universe);

        // This is pointless, as a copy is stored in the universe! By keeping
        // the objects in a scope you can prevent problems like this.
        A.set_position(2, 2);
    }

    // You now need pointers to find the objects back! We get a pointer back by giving the index number
    Object* A = universe.get_object_by_index(0);
    Object* B = universe.get_object_by_index(1);

    // Or you can do this by id!
    A = universe.get_object_by_id(0);
    B = universe.get_object_by_id(1);

    debug_display_world(universe);

    debug_display_id_to_object_mapping(universe);






    // So to update the position you have to do this
    A->set_position(2, 2);

    // Display the world to the commandline
    //debug_display_world(universe);

    std::cout << "Removing A" << std::endl;
    universe.remove_object_by_index(0);

    // Now try to modify the position of A
    A->set_position(3, 3);

    // And the position of B
    B->set_position(-2, -2);

    // But this just updated the position of A! Be very careful when deleting objects!
    // This can be solved by implementing a get_object_by_id() thing
    //debug_display_world(universe);

    std::cout << "Removing B" << std::endl;

    universe.remove_object_by_index(0);
    //debug_display_world(universe);



}