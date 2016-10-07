//
// Created by paul on 8/1/16.
//

#ifndef PIE_GITHUB_FRAMEWORK_H
#include "simulation.h"
#endif

// Set the bindings from private _key to public reference key
Object::Object () : mass(_mass), bounciness(_bounciness), radius(_radius), position(_position), velocity(_velocity), colour(_colour) {

}

/*
 * set_bounciness()
 *
 * Set the private bounciness property to a value between 0 and including 1
 */
void Object::set_bounciness(double bounciness) {
    if ( bounciness > 0 && bounciness <= 1 ) {
        this->_bounciness = bounciness;
    }
    else {
        // Invalid value given
        std::cerr << "[WARN] Invalid bounciness entered, must be between 0 and including 1" << std::endl;
    }

}

/*
 * set_position()
 *
 * Sets the position of the object to the new two double input parameters. Either with
 * two doubles as position, or as a vec2d.
 *
 * No checks are performed to allow for some freedom with other functions.
 */
void Object::set_position(double new_x, double new_y) {
    _position[0] = new_x;
    _position[1] = new_y;
}

void Object::set_position(vec2d new_pos) {
    _position = new_pos;
}

/*
 * set_velocity()
 *
 * Sets the velocity of the object, either with two doubles, or as a vec2d.
 *
 * No checks are performed to allow for some freedom with other functions.
 */
void Object::set_velocity(double new_vx, double new_vy) {
    _velocity[0] = new_vx;
    _velocity[1] = new_vy;
}
void Object::set_velocity(vec2d new_v) {
    _velocity = new_v;
}


/*
 * set_mass()
 *
 * Sets the mass of the object, which must be greater than zero
 */
void Object::set_mass(double m) {
    if ( m > 0 ) {
        _mass = m;
    }
    else{
        std::cout << "[WARN] Attempting to set the mass of an object <= 0!";
    }
}

/*
 * set_radius()
 *
 * Sets the radius of the object, which must be greater than zero
 */
void Object::set_radius(double r){
    if( r > 0 ){
        _radius = r;
    } else{
        std::cerr << "[WARN] Tried to set radius of object " << this << "to invalid value" << r << std::endl;
    }
}

/*
 * set_colour()
 *
 * Set the colour of an object to the four value array (rgba), with values between and including 0 to 1
 */
void Object::set_colour(std::array<double, 4> colour){
    for ( int ii = 0; ii < 4; ii++ ) {
        if (colour[ii] < 0.0) {
            _colour[ii] = 0.0;
            std::cerr << "[WARN] Tried to set a colour value of object " << this << " to invalid value" << colour[ii] << " colour value was forced to 0" << std::endl;
        } else if ( colour[ii] <= 1.0 ) {
            _colour[ii] = colour[ii];
        } else {
            _colour[ii] = 1.0;
            std::cerr << "[WARN] Tried to set a colour value of object " << this << " to invalid value" << colour[ii] << " colour value was forced to 1" << std::endl;
        }
    }
};

/*
 * get_colour_glm()
 *
 * Return the colour of this object as a glm::vec4 object.
 */
glm::vec4 Object::get_colour_glm(){
    return glm::vec4(_colour[0],_colour[1],_colour[2],_colour[3]);
}


/*
 * calc_new_pos_vel()
 *
 * Calculate the new position of the object by doing a Euler algorithm time step.
 */
std::array<vec2d, 2> Object::calc_new_pos_vel(std::vector<Object*> &objects, Physics &physics) {
    // Calculate the acceleration
    vec2d acceleration = physics.net_acceleration(objects, this);

    return physics.de_solver(acceleration, this);
};

void Object::on_collide (Object* target, Physics &physics) {
    // Do nothing yet, allow inherited classes to use a different implementation

}



//// Child classes Object

/*
 * Player has a different on_collide handler
 */
void Player::on_collide (Object* target, Physics &physics) {
    // Set the property that this player collided, game logic will call end_game
    this->i_collided = true;
}

/*
 * Player has a different handler for accelerations, because it can respond to keyboard input. It basically
 * injects an acceleration superimposed on the attractive forces due to the other objects.
 */
std::array<vec2d, 2> Player::calc_new_pos_vel (std::vector<Object*> &objects, Physics &physics) {

    vec2d acceleration = physics.net_acceleration(objects, this);

    // Get access to the users keyboard input
    GLFWwindow* window = glfwGetCurrentContext();
    vec2d input = {{0}};
    double thruster_a = this->thruster_force / this->mass;

    if(joystick) {
        int count = 0;
        const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
        switch (count) {
            default:
            case 2:
                input[1] = -axes[1] * thruster_a;
            case 1:
                input[0] = axes[0] * thruster_a;
            case 0:
                break;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        input[1] = thruster_a;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        input[1] = -thruster_a;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        input[0] = -thruster_a;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        input[0] = thruster_a;
    }

    // Add the additional thruster acceleration to the calculated acceleration
    acceleration = add(acceleration, input);

    return physics.de_solver(acceleration, this);
};