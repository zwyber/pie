//
// Created by paul on 8/1/16.
//

#include "objects.h"


std::vector<Object> Universe::get_objects() {
    return objects;
}

void Universe::add_object (Object &obj, Universe &me) {
    // Add a copy of the object to the vector of objects in the universe.
    objects.push_back(obj);

    // Get a reference to this copy
    Object* new_obj = &objects[objects.size()-1];

    // Give the object a unique ID
    new_obj->set_id(me);
}

void Universe::remove_object(int obj_index) {
    objects.erase(objects.begin() + obj_index);
}

void Universe::debug_display_world () {
    // Iterate over all objects
    for(int ii = 0; ii < objects.size(); ii++) {
        std::array<double, 2> pos = objects[ii].get_position();
        std::array<double, 2> vel = objects[ii].get_velocity();

        std::cout << "Object " << ii << " at position (" << pos[0] << ", " << pos[1] << ") and velocity (";
        std::cout << vel[0] << ", " << vel[1] << ")." << std::endl;
    }
}

int Universe::give_new_object_id() {
    new_object_id++;
    return new_object_id - 1;
}

Object* Universe::get_object_by_index(int index) {
    return &objects[index];
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

void Object::set_id(Universe universe) {
    id = universe.give_new_object_id();
}

int Object::get_id() {
    return id;
}
