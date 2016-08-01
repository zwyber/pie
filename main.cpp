//
// Created by paul on 7/15/16.
//

#include "objects.h"

int main() {


    // Generate a few objects
    Object A;
    Object B;

    // Give them new positions
    A.set_position(1, 1);
    B.set_position(-1, 2);

    // Make the universe
    Universe universe;
    THIS_UNIVERSE = &universe;
    universe.add_object(A, universe);
    universe.add_object(B, universe);

    // This is pointless, as a copy is stored in the universe!
    A.set_position(2, 2);

    // You now need pointers to find the objects back! We get a pointer back by giving the index number
    Object* A_iref = universe.get_object_by_index(0);
    Object* B_iref = universe.get_object_by_index(1);

    // So to update the position you have to do this
    A_iref->set_position(2, 2);

    // Display the world to the commandline
    universe.debug_display_world();

    std::cout << "Removing A" << std::endl;
    universe.remove_object(0);

    // Now try to modify the position of A
    A_iref->set_position(3, 3);

    // And the position of B
    B_iref->set_position(-2, -2);

    // But this just updated the position of A! Be very careful when deleting objects!
    // This can be solved by implementing a get_object_by_id() thing, TODO
    universe.debug_display_world();




    std::cout << "Removing B" << std::endl;

    universe.remove_object(0);
    universe.debug_display_world();



}