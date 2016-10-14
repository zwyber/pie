//
// Created by paul on 8/1/16.
//

#ifndef PIE_GITHUB_FRAMEWORK_H

#include "framework.h"

#endif

#ifndef PIE_GITHUB_OBJECTS_H
#define PIE_GITHUB_OBJECTS_H

// Class prototypes
class Universe;
class Object;
class Physics;

// Prototype of Object
class Object {

private:
    // Position array of two doubles [m]
    vec2d _position = {{ 0 }};

    // Velocity array of two doubles [m/s]
    vec2d _velocity = {{ 0 }};

    // Mass of object [kg]
    double _mass = 1;

    // Radius of object [m]
    double _radius = 1;

    // Coefficient of resitution factor between 0 and (including) 1
    double _bounciness = 1;

    // Colour vector of this object {red, green, blue, alpha}. All values are between 1 and 0
    std::array<double, 4> _colour = {{(double)std::rand()/RAND_MAX,(double)std::rand()/RAND_MAX,(double)std::rand()/RAND_MAX,1.0}};


public:
    // Constructor
    Object();

    // Read-only references to object properties
    const double &bounciness;
    const double &mass;
    const double &radius;
    const vec2d &position;
    const vec2d &velocity;
    const std::array<double, 4> &colour;

    // Setters for object properties
    void set_position(double new_x, double new_y);
    void set_position(vec2d new_pos);

    void set_bounciness(double bounciness);

    void set_velocity(double new_vx, double new_vy);
    void set_velocity(vec2d new_v);

    void set_mass(double m);

    void set_radius(double r);

    void set_colour(std::array<double, 4> Colour);
    glm::vec4 get_colour_glm();

    // Calculate new position and velocity function, called by Universe::physics_runtime_iteration
    virtual std::array<vec2d, 2> calc_new_pos_vel (std::vector<Object*> &objects, Physics &physics);

    // Collision function
    virtual void on_collide (Object* target, Physics &physics);

};

class Physics {

public:
    // Attractive constant between objects. In units of [m^3 kg^-1 s^-2]. Set to something that makes a fun game.
    double G = 2;

    // Default timestep of the system. Default value of 6 iterations per frame at 60 FPS
    double timestep = double(1.0/60)/6;

    // Calculate distance between object A and B
    double distance_between(Object* A, Object* B);

    // Attractive acceleration calculation functions
    vec2d acceleration (Object* X, Object* Y);
    vec2d net_acceleration (std::vector<Object*> &objects, Object* me);

    // A simple DE solver for calculating the new position and velocity based on acceleration
    std::array<vec2d, 2> de_solver (vec2d &acceleration, Object* me);

    // Resolve object collision between two objects, i.e. change their velocities
    void resolve_collision (Object* A, Object* B);

    // Check collisions between objects
    bool check_collision (Object* A, Object* B);

    // Resolve a collision with a wall
    void wall_collision(Object* X, double width, double height, int wall);

};

// Definition of Universe class
class Universe {

private:
    // Dimensions of the Universe
    double _height;
    double _width;

    // Score of the game, as defined as physics time steps survived
    long unsigned _score = 0;


public:
    // Constructor functions
    Universe();
    Universe(double width, double height);

    // Destructor
    ~Universe();

    // Implement a local copy of physics. Useful for access with member functions
    Physics physics;

    // Read-only references to universe properties
    const double &height;
    const double &width;
    const long unsigned &score;

    // A vector of objects. This contains the objects in the world
    std::vector<Object*> objects = {};

    // Adding new objects
    void add_object (Object* obj);
    Object* add_object ();

    // Remove objects, either by pointer or index
    void remove_object_by_index(int obj_index);
    void remove_object (Object* obj);

    // Resize the universe
    void resize(double width, double height);

    // To keep track of the time the universe is alive
    std::chrono::steady_clock::time_point begin_time;

    // Functions for the physics engine
    void physics_runtime_iteration ();
    void simulate_one_time_unit (double fps);

};

class Player : public Object {

public:
    // Defalt joystick
    bool joystick = glfwJoystickPresent(GLFW_JOYSTICK_1);

    // Thruster force exerted when player moves
    double thruster_force = 20;

    // To keep track if the player collided into an object
    bool i_collided = false;

    // Calculate new position and velocity function, which overrides the default
    std::array<vec2d, 2> calc_new_pos_vel (std::vector<Object*> &objects, Physics &physics);

    // Override collision function
    void on_collide (Object* target, Physics &physics);
};

// Include prototype implementations
#include "objects.cpp"
#include "physics.cpp"
#include "universe.cpp"

#endif //PIE_GITHUB_OBJECTS_H
