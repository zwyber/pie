//
// Created by paul on 8/1/16.
//

#ifndef PIE_GITHUB_FRAMEWORK_H
#include "simulation.h"
#endif

Object::Object () : mass(_mass), bouncyness(_bouncyness), radius(_radius), position(_position), velocity(_velocity) {
    // Nothing to do, just the bindings
}


void Object::set_bouncyness(double bouncyness) {
    if ( bouncyness > 0 && bouncyness <= 1 ) {
        this->_bouncyness = bouncyness;
    }
    else {
        // Invalid
        std::cerr << "Invalid bouncyness entered, must be between 0 and including 1" << std::endl;
    }

}

/*
 * set_position()
 *
 * Sets the position of the object to the new two double input parameters.
 */
void Object::set_position(double new_x, double new_y) {
    // IMPLEMENT INPUT SANITATION CHECKS HERE?
    _position[0] = new_x;
    _position[1] = new_y;
}

void Object::set_position(vec2d new_pos) {
    _position = new_pos;
}

/*
 * set_velocity()
 *
 * Sets the velocity of the object
 */
void Object::set_velocity(double new_vx, double new_vy) {
    // IMPLEMENT INPUT SANITATION CHECKS HERE?
    _velocity[0] = new_vx;
    _velocity[1] = new_vy;
}
void Object::set_velocity(vec2d new_v) {
    _velocity = new_v;
}



void Object::set_mass(double m) {
    if ( m > 0 ) {
        _mass = m;
    }
    else{
        std::cout << "Attempting to set the mass of an object <= 0!";
    }
}

void Object::set_radius(double r){
    if( r > 0 ){
        _radius = r;
    } else{
        std::cerr << "[WARN] Tried to set radius of object " << this << "to invalid value" << r << std::endl;
    }
}


void Object::set_colour(std::array<double, 4> Colour){
    for(int ii = 0; ii < colour.size(); ii++){
        if(Colour[ii]<0.0){
            colour[ii] = 0.0;
            std::cerr << "[WARN] Tried to set a colour value of object " << this << " to invalid value" << Colour[ii] << " colour value was forced to 0" << std::endl;
        }else if(Colour[ii]<=1.0) {
            colour[ii] = Colour[ii];
        }else{
            colour[ii] = 1.0;
            std::cerr << "[WARN] Tried to set a colour value of object " << this << " to invalid value" << Colour[ii] << " colour value was forced to 1" << std::endl;
        }
    }
};
std::array<double, 4> Object::get_colour(){
    return colour;
};
glm::vec4 Object::get_colour_glm(){
    return glm::vec4(colour[0],colour[1],colour[2],colour[3]);
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


void Object::lose_energy(double factor, Physics &physics) {
    physics.lose_energy(this, factor);
}

void Object::on_collide (Object* target, Physics &physics) {
    // Do nothing yet
    //physics.lose_collision_energy(this, target, 0.05);

}



//// Child classes Object


void Player::on_collide (Object* target, Physics &physics) {
    // Make the other object slow down!
    target->lose_energy(0, physics);

    // Scream that we collided, e.g. the game should end!
    this->i_collided = true;
}