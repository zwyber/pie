//
// Created by paul on 8/1/16.
//


#ifndef PIE_GITHUB_FRAMEWORK_H
#include "simulation.h"
#endif


bool Physics::check_collision(Object* A, Object* B) {
    vec2d A_v = A->velocity;
    vec2d A_x = A->position;

    vec2d B_v = B->velocity;
    vec2d B_x = B->position;

    double A_r = A->radius;
    double B_r = B->radius;

    // Check if objects are overlapping and moving aay from each other
    if(len(sub(A_x,B_x)) < (A_r+B_r) && A_x != B_x && dot(sub(A_v,B_v),sub(B_x,A_x)) > 0){
        return true;
    } else{
        return false;
    }
}

void Physics::resolve_collision(Object* A, Object* B) {
    // Using the math from Wikipedia: https://en.wikipedia.org/wiki/Elastic_collision
    vec2d A_v = A->velocity;
    vec2d A_x = A->position;
    double A_r = A->radius;

    vec2d B_v = B->velocity;
    vec2d B_x = B->position;
    double B_r = B->radius;

    double A_m = A->mass;
    double B_m = B->mass;

    double coeff; // Coefficient of restitution, the smallest of the two
    // (for now take the coefficient of restitution as a linear combination because it wouldn't make sense that the
    // energy absorbed would be equal when most bouncyness or inelastic.
    coeff = (A->bouncyness + B->bouncyness)/2.0;
    //if ( A->bouncyness < B->bouncyness )
    //    coeff = A->bouncyness;
    //else
    //    coeff = B->bouncyness;

    /*
     * For the collision you can have 100% elastic or 100% inelastic. The final velocity vector is a
     * linear combination of the two, where the bouncyness coefficient determines how much elastic
     * the collision is.
     *
     * Not completely sure energy is conserved with this method :S
     */

    // Initialise the vectors
    //vec2d A_v_e_new; // Object A, velocity, elastic, new
    //vec2d A_v_i_new; // Object A, velocity, inelastic, new
    //vec2d B_v_e_new; // etc
    //vec2d B_v_i_new;

    // Horrible long expression which calculates the new velocity! It is the equation on Wikipedia for the
    // vector notation of resolving a collision: https://en.wikipedia.org/wiki/Elastic_collision
    vec2d A_v_new = sub(A_v, cmult(sub(A_x, B_x),
                                     ((coeff+1) * B_m / (A_m + B_m)) * dot(sub(A_v, B_v), sub(A_x, B_x)) /
                                    len_squared(sub(A_x, B_x))));
    vec2d B_v_new = sub(B_v, cmult(sub(B_x, A_x),
                                     ((coeff+1) * A_m / (A_m + B_m)) * dot(sub(B_v, A_v), sub(B_x, A_x)) /
                                     len_squared(sub(B_x, A_x))));

    // Fully inelastic expression, very simple derivation:
    /*
     * p momentum, v velocity, m mass, ' final state
     * p1' + p2' = p1 + p2 // Conservation of momentum
     * v1' = v2'           // Same final velocity
     * --> v1' = p1 + p2/(m1 + m2)
     */
    //A_v_i_new = cmult(add(cmult(A_v, A_m), cmult(B_v, B_m)), 1/(A_m + B_m));
    //B_v_i_new = A_v_i_new; // Same velocity, fully joined together in motion

    // Create a linear combination of the two types of collision
    //vec2d A_v_new = add(cmult(A_v_e_new, coeff), cmult(A_v_i_new, (1-coeff)));
    //vec2d B_v_new = add(cmult(B_v_e_new, coeff), cmult(B_v_i_new, (1-coeff)));

    // Push these new vectors to the objects
    A->set_velocity(A_v_new);
    B->set_velocity(B_v_new);

    // Now also move them apart slightly, such that they are just touching. They
    // are moved apart perpendicular to the plane of contact
    vec2d r = sub(B_x, A_x);
    double l = (A_r + B_r) - len(r);

    // Move A and B apart
    A->set_position(add(A_x, cmult(r, -l*A_m/(A_m + B_m))));
    B->set_position(add(B_x, cmult(r, l*B_m/(A_m + B_m))));

}

void Physics::  wall_collision(Object* X, double width, double height, int wall) {

    switch (wall) {
        case 1: // North
            X->set_velocity(X->velocity[0], -1 * std::abs(X->velocity[1]) );
            X->set_position(X->position[0], height/2 - std::abs(height/2 - X->position[1]) );
            break;
        case 2: // East
            X->set_velocity(-1 * std::abs(X->velocity[0]), X->velocity[1]);
            X->set_position(width/2 - std::abs(width/2 - X->position[0]), X->position[1]);
            break;
        case 3: // South
            X->set_velocity(X->velocity[0], std::abs(X->velocity[1]) );
            X->set_position(X->position[0], -height/2 + std::abs(X->position[1] + height/2) );
            break;
        case 4: // West
            X->set_velocity(std::abs(X->velocity[0]), X->velocity[1]);
            X->set_position(-width/2 + std::abs(X->position[0] + width/2), X->position[1]);
            break;
    }

}

double Physics::distance_between(Object* X, Object* Y) {
    vec2d pos_X = X->position;
    vec2d pos_Y = Y->position;
    double dist = std::sqrt( (pos_Y[0]-pos_X[0])*(pos_Y[0]-pos_X[0]) + (pos_Y[1]-pos_X[1])*(pos_Y[1]-pos_X[1]) );

    if ( dist <= 0 ) {
        dist = 0.1;
    }

    return dist;
}

vec2d Physics::acceleration (Object* X, Object* Y){
    double dist = this->distance_between (X , Y);
    vec2d pos_X = X->position;
    vec2d pos_Y = Y->position;
    vec2d r = sub(pos_Y , pos_X);
    double mass = Y->mass;
    vec2d acc = cmult(r,(this->G * mass/(dist*dist*dist)));
    return acc;
}

vec2d Physics::net_acceleration(std::vector<Object *> &objects, Object* me) {
    // Calculate the acceleration
    vec2d acceleration = {0,0};
    // Loop through all objects
    for (int ii = 0; ii < objects.size(); ++ii ) {
        // Make sure you are not calculating yourself
        if ( objects[ii] == me ) {
            // This is myself, skip this loop iteration
            continue;
        }

        vec2d this_acc = this->acceleration(me, objects[ii]);
        // Here add up the contribution to the acceleration
        acceleration = add(acceleration, this_acc);
    }

    return acceleration;
}

std::array<vec2d, 2> Player::calc_new_pos_vel (std::vector<Object*> &objects, Physics &physics) {

    vec2d acceleration = physics.net_acceleration(objects, this);

    // Get access to the users keyboard input
    GLFWwindow* window = glfwGetCurrentContext();
    vec2d input = {{0}};
    double thruster_a = this->thruster_force / this->mass;
    if ( glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS ) {
        input[1] = thruster_a;
    }
    if ( glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS ) {
        input[1] = -thruster_a;
    }
    if ( glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS ) {
        input[0] = -thruster_a;
    }
    if ( glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS ) {
        input[0] = thruster_a;
    }

    acceleration = add(acceleration, input);

    return physics.de_solver(acceleration, this);
};

std::array<vec2d, 2> Physics::de_solver (vec2d &acceleration, Object* me) {
    // Initialize the result array
    std::array<vec2d, 2> new_pos_vel = {{0}};

    // Midpoint method :)
    vec2d velocity = me->velocity;
    vec2d position = me->position;

    vec2d velocity_half = add(velocity, cmult(acceleration, timestep/2));
    new_pos_vel[0] = add(position, cmult(velocity_half, timestep) );

    new_pos_vel[1] = add(velocity, cmult(acceleration, timestep));

    // Euler method
    //new_pos_vel[0] = add(position, cmult(velocity, time_step));
    //new_pos_vel[1] = add(velocity, cmult(acceleration, time_step));

    return new_pos_vel;
};

void Physics::lose_energy(Object* me, double factor) {
    // Rescale the velocity factor
    me->set_velocity(cmult(me->velocity, 1 - std::sqrt(factor)));
}

void Physics::lose_collision_energy(Object* A, Object* B, double factor) {
    // First determine the relative velocities towards the two objects.
    vec2d relBtoA = sub(A->velocity, B->velocity);

    // Subtract a bit of velocity
    A->set_velocity(sub(A->velocity, cmult(relBtoA, factor)));
    B->set_velocity(sub(B->velocity, cmult(relBtoA, -1*factor)));

}