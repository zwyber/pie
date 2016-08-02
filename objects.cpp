//
// Created by paul on 8/1/16.
//

#include "objects.h"

/*
 * Universe class functions
 *
 * The Universe is a class which keeps track of all objects in the game.
 * Also it will contain the physics engine etc.
 *
 * It has functions for adding objects, removing objects, getting objects, etc.
 *
 */

// Constructor, currently not used
Universe::Universe() {

}

/*
 * add_object(Object &obj)
 *
 * Add a copy of obj to the objects list of the current universe. Returns the id of the object
 * in the universe.
 */
id_type Universe::add_object (Object &obj) {
    // Add a copy of the object to the vector of objects in the universe.
    objects.push_back(obj);

    // Get a reference to this copy
    Object* new_obj = &objects[objects.size()-1];

    // Give the object a unique ID
    id_type new_id = this->give_new_object_id();
    new_obj->set_id(new_id);

    // Update the id to object mapping
    this->update_id_to_object_map();

    // Return the new object's id
    return new_obj->get_id();
}

// Remove object functions

/*
 * remove_object_by_index(int obj_index)
 *
 * The fastest way to remove an object from the universe. Removes the object
 * which is at index obj_index in the vector of objects. Shifts all other
 * objects to make the list continuous again. (uses std::vector.erase() to do so)
 *
 * Calls update_id_to_object_map() at the end
 */
void Universe::remove_object_by_index(int obj_index) {
    // Remove the object from the vector of objects
    objects.erase(objects.begin() + obj_index);

    // Update the id mapping
    this->update_id_to_object_map();
}

/*
 * remove_object_by_id(id_type id)
 *
 * This function removes the object given the id. First has to find the object,
 * then notes the index and calles remove_object_by_index() to do the dirty work.
 */
void Universe::remove_object_by_id(id_type id) {
    // Loop through all objects, until we have found the correct object
    int delete_index = 0;
    bool found_it = false;

    for (int ii = 0; ii < objects.size(); ++ii) {

        if ( objects[ii].get_id() == id ) {
            // We found the correct one! Store the index
            found_it = true;
            delete_index = ii;
        }
    }

    // Remove it if it was found
    if (found_it)
        remove_object_by_index(delete_index);
    else{
        // Whoops, couldn't find it!
        std::cerr << "[WARN] Cannot delete object with id " << id << " because couldn't find it!" << std::endl;
    }
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

void Object::set_id(id_type &new_id) {
    if ( this->id_set == false ) {
        this->id = new_id;
        this->id_set = true;
    }
    else{
        // Attempted to re-set the id!
        std::cerr << "[WARN] Object::set_id() called to set the id to " << new_id << ", but it is already set to " << this->id << std::endl;
    }

}

id_type Object::get_id() {
    return id;
}
