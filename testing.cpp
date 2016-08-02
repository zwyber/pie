//
// Created by paul on 8/2/16.
//

#include "testing.h"

void debug_display_world (Universe &universe) {
    // Iterate over all objects
    for(int ii = 0; ii < universe.objects.size(); ++ii) {
        std::array<double, 2> pos = universe.objects[ii].get_position();
        std::array<double, 2> vel = universe.objects[ii].get_velocity();
        int id = universe.objects[ii].get_id();

        std::cout << "Object ID " << id << " at position (" << pos[0] << ", " << pos[1] << ") and velocity (";
        std::cout << vel[0] << ", " << vel[1] << ")." << std::endl;
    }
}

void debug_display_id_to_object_mapping (Universe &universe) {
    // Iterate over all items in the mapping
    Object* objref;
    for (int ii = 0; ii < universe.id_to_object_map.size(); ++ii) {
        objref = universe.id_to_object_map[ii];
        std::cout << "Mapping id " << ii << " is mapped to object id " << objref->get_id() << std::endl;
    }
}