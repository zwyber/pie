//
// Created by paul on 8/1/16.
//

#ifndef PIE_GITHUB_FRAMEWORK_H
#include "simulation.h"
#endif

/*
 * Universe class functions
 *
 * The Universe is a class which keeps track of all objects in the game.
 * Also it will contain the physics engine etc.
 *
 * It has functions for adding objects, removing objects, getting objects, etc.
 *
 */

// Constructors

/*
 * Note on how the constructors are used: We implemented a code sequence to get "read-only" parameters.
 * The idea is that you can access properties like they are public (read them), but cannot change their
 * value, unless you are a member function of the class. See the example at http://www.cplusplus.com/forum/articles/36872/
 * or copied here below:
 *
 * class A
 * {
 *     int _x; // Private variable, it can modify in this class directly
 * public:
 *     A() : x(_x) // Bind reference variable x to _x
 *     {
 *     }
 *
 *     void setX(int x)
 *     {
 *         this->_x = x;
 *     }
 *
 *     const int &x; // Constant variable
 * };
 *
 * // Usage:
 *
 * int main()
 * {
 *     A a;
 *
 *     a.setX(50);
 *
 *     cout << a.x << endl; // Correct
 *     a.x = 12; // Error !
 *
 *     return 0;
 * }
 */
Universe::Universe() : height(_height), width(_width), score(_score) {
    this->_width = 640;
    this->_height = 480;

    this->begin_time = std::chrono::steady_clock::now();
}

Universe::Universe(double width, double height) : height(_height), width(_width), score(_score) {
    this->_width = width;
    this->_height = height;

    this->begin_time = std::chrono::steady_clock::now();
}

/*
 * add_object(Object &obj)
 *
 * Add a copy of obj to the objects list of the current universe. Returns the id of the object
 * in the universe.
 */
void Universe::add_object (Object* obj) {
    // Add a copy of the object to the vector of objects in the universe.
    objects.push_back(obj);
}

Object* Universe::add_object () {
    Object* obj = new Object;

    objects.push_back(obj);

    return obj;
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
    Object* X = this->objects[obj_index];

    // Remove the object from the vector of objects
    objects.erase(objects.begin() + obj_index);

    // Remove it from heap memory
    delete X;
}

void Universe::remove_object(Object* obj)  {
    // Find it in the list of stored objects
    for (int ii = 0; ii < objects.size(); ++ii) {
        if (objects[ii] == obj) {
            this->remove_object_by_index(ii);
            return;
        }
    }
}

Universe::~Universe() {
    // Delete all objects stored in the universe
    int n = this->objects.size();

    // Remove the first item n times
    for (int ii = 0; ii < n; ++ii) {
        this->remove_object_by_index(0);
    }

}

// Get object functions

/*
 * get_object_by_index()
 *
 * Returns the object pointer in this universe by index number. Usage of this function
 * is not recommended, use get_object_by_id() instead.
 */
Object* Universe::get_object_by_index(int index) {
    return objects[index];
}

/*
 * get_all_objects()
 *
 * Returns the vector of all objects in the universe.
 */
std::vector<Object*> Universe::get_all_objects() {
    return objects;
}

void Universe::resize(double width, double height) {
    if(width > 0){
        _width = width;
    }else{
        std::cerr << "[WARN] Tried to set resize universe width to invalid value " << width << ". Resize was not executed for width." << std::endl;
    }
    if(height > 0){
        _height = height;
    }else{
        std::cerr << "[WARN] Tried to set resize universe height to invalid value " << height << ". Resize was not executed for height." << std::endl;
    }
    // Optional add some code here that keeps objects within universe
}


// Object

void Universe::physics_runtime_iteration () {
    // Temporary result storage
    std::map<Object*,std::array<vec2d, 2>> new_pos_vel_universe;

    // Iterate over all objects
    for (int ii = 0; ii < objects.size(); ++ii) {
        std::array<vec2d, 2> new_pos_vel = objects[ii]->calc_new_pos_vel(objects, this->physics);
        new_pos_vel_universe[objects[ii]] = new_pos_vel;
    }

    // Now update the positions and velocities of all objects
    for (int ii = 0; ii < objects.size(); ++ii) {
        objects[ii]->set_position(new_pos_vel_universe[objects[ii]][0]);
        objects[ii]->set_velocity(new_pos_vel_universe[objects[ii]][1]);
    }

    // Check for collisions
    for (int ii = 0; ii < objects.size(); ++ii) {
        for (int jj = ii + 1; jj < objects.size(); ++jj) {
            // Do not check yourself
            // Check for a collision
            if ( physics.check_collision(objects[ii], objects[jj]) ) {
                // If that is the case, go fix it!
                //std::cout << "Resolving collision...\n";
                physics.resolve_collision(objects[ii], objects[jj]);
                objects[ii]->on_collide(objects[jj], this->physics);
                objects[jj]->on_collide(objects[ii], this->physics);
            }
        }

        // Check if we are near a wall
        vec2d pos = objects[ii]->position;
        double r = objects[ii]->radius;

        // If it exceeds the world
        if (    pos[0] - r < -this->_width/2  || pos[0] + r > this->_width/2 || \
                pos[1] - r < -this->_height/2 || pos[1] + r > this->_height/2 ) {
            // Make a mirror object at the
        }

        //// Durp wall collision is rather ugly now, put in subroutine

        // Colliding in the west wall
        if ( pos[0] - r < -this->_width/2 ) {
            // Do the wall collision
            physics.wall_collision(objects[ii], this->_width, this->_height, 4);
        }

        // Colliding into the east wall
        if ( pos[0] + r > this->_width/2 ) {
            // Do the wall collision
            physics.wall_collision(objects[ii], this->_width, this->_height, 2);
        }

        // Collide into the north wall
        if ( pos[1] + r > this->_height/2 ) {
            // Do the wall collision
            physics.wall_collision(objects[ii], this->_width, this->_height, 1);
        }

        // Collide into the south wall
        if ( pos[1] - r < -this->_height/2 ) {
            // Do the wall collision
            physics.wall_collision(objects[ii], this->_width, this->_height, 3);
        }
    }

}

void Universe::simulate_one_time_unit(double fps) {

    // Call physics_runtime_iteration as many times as required to advance it one time "unit",
    // which is defined as 1/FPS

    int iterations = double(1.0/fps) / this->physics.timestep;
    for ( int ii = 0; ii < iterations; ++ii ) {
        this->physics_runtime_iteration();
    }

    this->_score++;
}

