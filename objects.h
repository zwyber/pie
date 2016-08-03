//
// Created by paul on 8/1/16.
//

#ifndef PIE_GITHUB_FRAMEWORK_H

#include "framework.h"

#endif

#ifndef PIE_GITHUB_OBJECTS_H
#define PIE_GITHUB_OBJECTS_H

// Initial class definitions
class Universe;
class Object;

// Definition of Object class
class Object {

private:
    // Position array of two doubles [m]
    vec2d position = {{ 0 }};
    // Velocity array of two doubles [m/s]
    vec2d velocity = {{ 0 }};
    // Mass of object [kg]
    double mass = 0;
    // Radius of object [m]
    double radius = 1;

    // Identification number definitions
    id_type id = 0;
    bool id_set = false; // Toggle for allowing a new id to be generated or not.

public:
    // Return the id of the object, return 0 if undefined
    id_type get_id();
    // Set to the id given. Advised to use Universe::give_new_object_id()
    void set_id(id_type &new_id);

    // Get and setters for the position
    void set_position(double new_x, double new_y);
    vec2d get_position ();

    // Get and setters for the velocity
    void set_velocity(double new_vx, double new_vy);
    void set_velocity(vec2d new_v);
    vec2d get_velocity ();

    // Get and setters for the mass, cannot be negative
    void set_mass(double m);
    double get_mass();

    // Get and setters for the radius, cannot be negative

};




// Definition of Universe class
class Universe {

private:
    // To keep track of what the next available object_id is
    id_type new_object_id = 0;

public:
    // Constructor function, currently not really used
    Universe();

    // A vector of objects. This contains the objects in the world!
    std::vector<Object> objects = {};

    // Add a new object, return the new id of the object
    id_type add_object (Object &obj);

    // Remove objects, either by id or by index
    void remove_object_by_index(int obj_index);
    bool remove_object_by_id(id_type id);

    // Get objects, either by id or by index
    Object* get_object_by_index(int index);
    Object* get_object_by_id(id_type id);

    // Give out a new unique id for objects
    id_type give_new_object_id();

    // Function to update the mapping of id to object.
    // MUST BE CALLED EVERY TIME THE UNIVERSE OBJECTS LIST CHANGES!
    void update_id_to_object_map();

    // Map for getting an object given the id
    std::map<id_type, Object*> id_to_object_map;

    // Resolve object collision between two objects
    void resolve_collision (Object* A, Object* B);
};



#include "objects.cpp"

#endif //PIE_GITHUB_OBJECTS_H
