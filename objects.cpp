//
// Created by paul on 8/1/16.
//

#include "objects.h"

Universe::Universe() {

}

// This stuff is broken real bad, so I just made the objects public.
/*std::vector<Object>* Universe::get_objects() {
    return &objects;
}*/

id_type Universe::add_object (Object &obj, Universe &me) {
    // Add a copy of the object to the vector of objects in the universe.
    objects.push_back(obj);

    // Get a reference to this copy
    Object* new_obj = &objects[objects.size()-1];

    // Give the object a unique ID
    new_obj->set_id(me);

    // Update the id to object mapping
    this->update_id_to_object_map();

    // Return the new object's id
    return new_obj->get_id();
}

void Universe::remove_object_by_index(int obj_index) {
    objects.erase(objects.begin() + obj_index);
}


id_type Universe::give_new_object_id() {
    new_object_id++;
    return new_object_id - 1;
}



Object* Universe::get_object_by_index(int index) {
    return &objects[index];
}

Object* Universe::get_object_by_id(int id) {
    return id_to_object_map[id];
}




void Universe::update_id_to_object_map() {
    // Clear the current mapping
    id_to_object_map = {};

    // Loop through all objects stored in the universe
    for (int ii = 0; ii < objects.size(); ii++) {
        // Create a mapping from the object id to the object pointer
        id_to_object_map[ objects[ii].get_id() ] = &objects[ii];
    }
}













void Object::set_position(double new_x, double new_y) {
    // IMPLEMENT INPUT SANITATION CHECKS HERE?
    position[0] = new_x;
    position[1] = new_y;
}

std::array<double, 2> Object::get_position () {
    return position;
}

void Object::set_velocity(double new_vx, double new_vy) {
    // IMPLEMENT INPUT SANITATION CHECKS HERE?
    velocity[0] = new_vx;
    velocity[1] = new_vy;
}

std::array<double, 2> Object::get_velocity () {
    return velocity;
}

void Object::set_id(Universe &universe) {
    id = universe.give_new_object_id();
}

id_type Object::get_id() {
    return id;
}
