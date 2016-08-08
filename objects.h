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
class Physics;

// Definition of Object class
class Object {

private:
    // Position array of two doubles [m]
    vec2d position = {{ 0 }};
    // Velocity array of two doubles [m/s]
    vec2d velocity = {{ 0 }};
    // Mass of object [kg]
    double mass = 1;
    // Radius of object [m]
    double radius = 1;
    // Colour vector of this object {Red, Green, Blue, alpha}. All values are between 1 and 0
    std::array<double, 4> colour = {{(double)rand()/RAND_MAX,(double)rand()/RAND_MAX,(double)rand()/RAND_MAX,1.0}};
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
    void set_position(vec2d new_pos);
    vec2d get_position ();

    // Get and setters for the velocity
    void set_velocity(double new_vx, double new_vy);
    void set_velocity(vec2d new_v);
    vec2d get_velocity ();

    // Get and setters for the mass, cannot be negative
    void set_mass(double m);
    double get_mass();

    // Get and setters for the radius, cannot be negative
    void set_radius(double r);
    double get_radius();

    // Getter and setter for colour, colours values needs to be between 0 and 1
    void set_colour(std::array<double, 4> Colour);
    std::array<double, 4> get_colour();

    // Calculate new position and velocity function
    std::array<vec2d, 2> calc_new_pos_vel (std::vector<Object> &objects, double &time_step, Physics &physics);

};


class Physics {
public:

    double distance_between(Object* A, Object* B);

    vec2d acceleration (Object* X, Object* Y);

};



// Definition of Universe class
class Universe {

private:
    // To keep track of what the next available object_id is
    id_type new_object_id = 0;
    double _Height;
    double _Width;

public:
    // Constructor function, currently not really used
    Universe();
    Universe(double Width, double Height);

    Physics physics;

    const double &height;
    const double &width;

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
    std::vector<Object> get_all_objects ();

    // Give out a new unique id for objects
    id_type give_new_object_id();

    // Function to update the mapping of id to object.
    // MUST BE CALLED EVERY TIME THE UNIVERSE OBJECTS LIST CHANGES!
    void update_id_to_object_map();

    // Map for getting an object given the id
    std::map<id_type, Object*> id_to_object_map;

    // Check collisions between objects
    bool check_collision (Object* A, Object* B);
    // Resolve object collision between two objects, i.e. change their velocities
    void resolve_collision (Object* A, Object* B);

    // Physics engine
    void physics_runtime_iteration ();

    double timestep = 0.01;

};


#include "objects.cpp"

#endif //PIE_GITHUB_OBJECTS_H
