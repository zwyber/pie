//
// Created by paul on 8/1/16.
//


#ifndef PIE_GITHUB_FRAMEWORK_H
#include "simulation.h"
#endif

/*
 * check_collision()
 *
 * Check if two objects are colliding, which is when their distance is less then the two radii together, but not moving
 * away from each other. Hence objects can overlap, but only when they are moving away from each other.
 */
bool Physics::check_collision(Object* A, Object* B) {
    // Check if objects are overlapping and moving away from each other
    if ( len(sub(A->position,B->position)) < (A->radius+B->radius) &&
            A->position != B->position && dot(sub(A->velocity,B->velocity), sub(B->position,A->position)) > 0) {
        return true;
    }
    else {
        return false;
    }
}

/*
 * resolve_collision()
 *
 * Do a physically acceptable object collision between objects A and B.
 */
void Physics::resolve_collision(Object* A, Object* B) {
    // Use the average bounciness coefficient
    double coeff = (A->bounciness + B->bounciness)/2.0;

    // The expression which calculates the new velocity! It is the equation on Wikipedia using
    // vector notation of resolving a collision: https://en.wikipedia.org/wiki/Elastic_collision
    vec2d A_v_new = sub(A->velocity, cmult(sub(A->position, B->position),
                                     ((coeff+1) * B->mass / (A->mass + B->mass))
                                     * dot(sub(A->velocity, B->velocity), sub(A->position, B->position))
                                     / len_squared(sub(A->position, B->position))));
    vec2d B_v_new = sub(B->velocity, cmult(sub(B->position, A->position),
                                     ((coeff+1) * A->mass / (A->mass + B->mass))
                                     * dot(sub(B->velocity, A->velocity), sub(B->position, A->position))
                                     / len_squared(sub(B->position, A->position))));

    // Push these new vectors to the objects
    A->set_velocity(A_v_new);
    B->set_velocity(B_v_new);

    // Now also move them apart slightly, such that they are just touching. They
    // are moved apart perpendicular to the plane of contact
    vec2d r = sub(B->position, A->position);
    double l = (A->radius + B->radius) - len(r);

    // Move A and B apart
    A->set_position(add(A->position, cmult(r, -l * A->mass / (A->mass + B->mass) )));
    B->set_position(add(B->position, cmult(r,  l * B->mass / (A->mass + B->mass) )));

}

/*
 * wall_collision()
 *
 * Prevent an object from passing through a wall by setting the velocity away from the wall
 * and moving it slightly to prevent wall contact.
 */
void Physics::wall_collision(Object* X, double width, double height, int wall) {
    /*
     * Wall collisions are resolved by the sign of the velocity component which is causing
     * the object to collide into a wall. In that sense it is a perfectly elastic collision.
     *
     * Then the object is then moved away from the wall, so next physics iteration it will not
     * collide again.
     *
     * Earlier versions of the code has inelastic collisions implemented as well, but that caused
     * a "cold wall" effect, where the objects would quickly get stuck onto a wall.
     */

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

/*
 * distance_between()
 *
 * Calculate the distance between two objects, but it will always be greater than 0.1 to prevent problems
 * with infinite forces.
 */
double Physics::distance_between(Object* X, Object* Y) {
    // For the case where any object is a default NULL object
    if ( X == NULL || Y == NULL ) {
        return 0;
    }

    // Calculate the distance between the two objects
    double dist = std::sqrt( ( Y->position[0] - X->position[0] ) * ( Y->position[0] - X->position[0])
                             + ( Y->position[1] - X->position[1] ) * ( Y->position[1] - X->position[1]) );

    // To prevent exerting too large forces when two objects are near, or something weird happened
    if ( dist <= 0 ) {
        dist = 0.1;
    }

    return dist;
}







// Calculates the acceleration of an object towards another object's gravitational force

vec2d Physics::acceleration (Object* X, Object* Y){
    double dist = this->distance_between (X , Y);
    vec2d pos_X = X->position;
    vec2d pos_Y = Y->position;
    vec2d r = sub(pos_Y , pos_X);
    double mass = Y->mass;
    vec2d acc = cmult(r,(this->G * mass/(dist*dist*dist)));
    return acc;
}

vec2d Physics::net_acceleration(std::vector<Object* > &objects, Object* me) {
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
