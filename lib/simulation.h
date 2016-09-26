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
    vec2d _position = {{ 0 }};

    // Velocity array of two doubles [m/s]
    vec2d _velocity = {{ 0 }};

    // Mass of object [kg]
    double _mass = 1;

    // Radius of object [m]
    double _radius = 1;

    // Coefficient of resitution factor between 0 and (including) 1
    double _bouncyness = 1;

    // Colour vector of this object {Red, Green, Blue, alpha}. All values are between 1 and 0
    std::array<double, 4> colour = {{(double)std::rand()/RAND_MAX,(double)std::rand()/RAND_MAX,(double)std::rand()/RAND_MAX,1.0}};


public:
    // Constructor
    Object();

    // Definition of parameters
    const double &bouncyness;
    const double &mass;
    const double &radius;
    const vec2d &position;
    const vec2d &velocity;

    // Get and setters for the position
    void set_position(double new_x, double new_y);
    void set_position(vec2d new_pos);

    void set_bouncyness(double bouncyness);

    // Get and setters for the velocity
    void set_velocity(double new_vx, double new_vy);
    void set_velocity(vec2d new_v);

    // Get and setters for the mass, cannot be negative
    void set_mass(double m);

    // Get and setters for the radius, cannot be negative
    void set_radius(double r);

    // Getter and setter for colour, colours values needs to be between 0 and 1
    void set_colour(std::array<double, 4> Colour);
    std::array<double, 4> get_colour();
    glm::vec4 get_colour_glm();

    // Calculate new position and velocity function
    virtual std::array<vec2d, 2> calc_new_pos_vel (std::vector<Object*> &objects, Physics &physics);

    // Lose energy on command :D
    void lose_energy (double factor, Physics &physics);

    virtual void on_collide (Object* target, Physics &physics);

};

class Physics {
public:

    double G = 2;
    double timestep = double(1.0/60)/6;
    double distance_between(Object* A, Object* B);

    vec2d acceleration (Object* X, Object* Y);
    vec2d net_acceleration (std::vector<Object*> &objects, Object* me);

    std::array<vec2d, 2> de_solver (vec2d &acceleration, Object* me);

    // Resolve object collision between two objects, i.e. change their velocities
    void resolve_collision (Object* A, Object* B);

    // Check collisions between objects
    bool check_collision (Object* A, Object* B);

    void wall_collision(Object* X, double width, double height, int wall);

    void lose_energy (Object* me, double factor);
    void lose_collision_energy(Object* A, Object* B, double factor = 0.02);

};

// Definition of Universe class
class Universe {

private:
    // To keep track of what the next available object_id is
    double _height;
    double _width;

    long unsigned _score = 0;


public:
    // Constructor function, currently not really used
    Universe();
    Universe(double width, double weight);

    // Destructor
    ~Universe();

    Physics physics;

    const double &height;
    const double &width;
    const long unsigned &score;

    // A vector of objects. This contains the objects in the world!
    std::vector<Object*> objects = {};

    // Add a new object, return the new id of the object
    void add_object (Object* obj);
    Object* add_object ();

    // Remove objects, either by id or by index
    void remove_object_by_index(int obj_index);
    void remove_object (Object* obj);

    // Get objects, either by id or by index
    Object* get_object_by_index(int index);
    std::vector<Object*> get_all_objects ();

    // Resize the universe
    void resize(double width, double height);

    // To keep track of the time the universe is alive
    std::chrono::steady_clock::time_point begin_time;

    // Physics engine
    void physics_runtime_iteration ();
    void simulate_one_time_unit (double fps);

};

class Player : public Object {

public:
    bool joystick = glfwJoystickPresent(GLFW_JOYSTICK_1);
    double thruster_force = 20;
    bool i_collided = false;

    // Calculate new position and velocity function
    std::array<vec2d, 2> calc_new_pos_vel (std::vector<Object*> &objects, Physics &physics);

    void on_collide (Object* target, Physics &physics);

};

#include "objects.cpp"
#include "physics.cpp"
#include "universe.cpp"

#endif //PIE_GITHUB_OBJECTS_H
