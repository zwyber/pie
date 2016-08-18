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
Universe::Universe() : height(_Height), width(_Width) {
    this->_Width = 640;
    this->_Height = 480;
}

Universe::Universe(double Width, double Height) : height(_Height), width(_Width){
    this->_Width = Width;
    this->_Height = Height;
}

/*
 * add_object(Object &obj)
 *
 * Add a copy of obj to the objects list of the current universe. Returns the id of the object
 * in the universe.
 */
void Universe::add_object (Object* &obj) {
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
        }
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
        _Width = width;
    }else{
        std::cerr << "[WARN] Tried to set resize universe width to invalid value " << width << ". Resize was not executed for width." << std::endl;
    }
    if(height > 0){
        _Height = height;
    }else{
        std::cerr << "[WARN] Tried to set resize universe height to invalid value " << height << ". Resize was not executed for height." << std::endl;
    }
    // Optional add some code here that keeps objects within universe
}


// Object


/*
 * set_position()
 *
 * Sets the position of the object to the new two double input parameters.
 */
void Object::set_position(double new_x, double new_y) {
    // IMPLEMENT INPUT SANITATION CHECKS HERE?
    position[0] = new_x;
    position[1] = new_y;
}

void Object::set_position(vec2d new_pos) {
    position = new_pos;
}

/*
 * get_position()
 *
 * Gets the position of the object, returns array of two doubles.
 */
vec2d Object::get_position () {
    return position;
}

/*
 * set_velocity()
 *
 * Sets the velocity of the object
 */
void Object::set_velocity(double new_vx, double new_vy) {
    // IMPLEMENT INPUT SANITATION CHECKS HERE?
    velocity[0] = new_vx;
    velocity[1] = new_vy;
}
void Object::set_velocity(vec2d new_v) {
    velocity = new_v;
}

/*
 * get_velocity()
 *
 * Returns the object's velocity as an array of two doubles
 */
vec2d Object::get_velocity () {
    return velocity;
}



double Object::get_mass() {
    return mass;
}

void Object::set_mass(double m) {
    if ( m > 0 ) {
        mass = m;
    }
    else{
        std::cout << "Attempting to set the mass of an object <= 0!";
    }
}

double Object::get_radius() {
    return radius;
}

void Object::set_radius(double r){
    if( r > 0 ){
        radius = r;
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

bool Physics::check_collision(Object* A, Object* B) {
    vec2d A_v = A->get_velocity();
    vec2d A_x = A->get_position();

    vec2d B_v = B->get_velocity();
    vec2d B_x = B->get_position();

    double A_r = A->get_radius();
    double B_r = B->get_radius();

    // Check if objects are overlapping and moving aay from each other
    if(len(sub(A_x,B_x)) < (A_r+B_r) && A_x != B_x && dot(sub(A_v,B_v),sub(B_x,A_x)) > 0){
        return true;
    } else{
        return false;
    }
}

void Physics::resolve_collision(Object* A, Object* B) {
    // Using the math from Wikipedia: https://en.wikipedia.org/wiki/Elastic_collision
    vec2d A_v = A->get_velocity();
    vec2d A_x = A->get_position();
    double A_r = A->get_radius();

    vec2d B_v = B->get_velocity();
    vec2d B_x = B->get_position();
    double B_r = B->get_radius();

    double A_m = A->get_mass();
    double B_m = B->get_mass();

    double coeff; // Coefficient of restitution, the smallest of the two
    if ( A->bouncyness < B->bouncyness )
        coeff = A->bouncyness;
    else
        coeff = B->bouncyness;

    /*
     * For the collision you can have 100% elastic or 100% inelastic. The final velocity vector is a
     * linear combination of the two, where the bouncyness coefficient determines how much elastic
     * the collision is.
     *
     * Not completely sure energy is conserved with this method :S
     */

    // Initialise the vectors
    vec2d A_v_e_new; // Object A, velocity, elastic, new
    vec2d A_v_i_new; // Object A, velocity, inelastic, new
    vec2d B_v_e_new; // etc
    vec2d B_v_i_new;

    // Horrible long expression which calculates the new velocity! It is the equation on Wikipedia for the
    // vector notation of resolving a collision: https://en.wikipedia.org/wiki/Elastic_collision
    A_v_e_new = sub(A_v, cmult(sub(A_x, B_x),
                                     (coeff) * (2 * B_m / (A_m + B_m)) * dot(sub(A_v, B_v), sub(A_x, B_x)) /
                                     len_squared(sub(A_x, B_x))));
    B_v_e_new = sub(B_v, cmult(sub(B_x, A_x),
                                     (coeff) * (2 * A_m / (A_m + B_m)) * dot(sub(B_v, A_v), sub(B_x, A_x)) /
                                     len_squared(sub(B_x, A_x))));

    // Fully inelastic expression, very simple derivation:
    /*
     * p momentum, v velocity, m mass, ' final state
     * p1' + p2' = p1 + p2 // Conservation of momentum
     * v1' = v2'           // Same final velocity
     * --> v1' = p1 + p2/(m1 + m2)
     */
    A_v_i_new = cmult(add(cmult(A_x, A_m), cmult(B_x, B_m)), 1/(A_m + B_m));
    B_v_i_new = A_v_i_new; // Same velocity, fully joined together in motion

    // Create a linear combination of the two types of collision
    vec2d A_v_new = add(cmult(A_v_e_new, coeff), cmult(A_v_i_new, (1-coeff)));
    vec2d B_v_new = add(cmult(B_v_e_new, coeff), cmult(B_v_i_new, (1-coeff)));

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

/*
 * physics_runtime_iteration()
 *
 * Call this function to do an update of position and velocity of all objects in this universe.
 * The idea is that the main runtime can call this function to perform the subset of functions
 * which is needed to do the physics engine.
 */
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
        vec2d pos = objects[ii]->get_position();
        double r = objects[ii]->get_radius();

        // If it exceeds the world
        if (    pos[0] - r < -this->_Width/2  || pos[0] + r > this->_Width/2 || \
                pos[1] - r < -this->_Height/2 || pos[1] + r > this->_Height/2 ) {
            // Make a mirror object at the
        }

        //// Durp wall collision is rather ugly now, put in subroutine

        // Colliding in the west wall
        if ( pos[0] - r < -this->_Width/2 ) {
            // Do the wall collision
            physics.wall_collision(objects[ii], this->_Width, this->_Height, 4);
        }

        // Colliding into the east wall
        if ( pos[0] + r > this->_Width/2 ) {
            // Do the wall collision
            physics.wall_collision(objects[ii], this->_Width, this->_Height, 2);
        }

        // Collide into the north wall
        if ( pos[1] + r > this->_Height/2 ) {
            // Do the wall collision
            physics.wall_collision(objects[ii], this->_Width, this->_Height, 1);
        }

        // Collide into the south wall
        if ( pos[1] - r < -this->_Height/2 ) {
            // Do the wall collision
            physics.wall_collision(objects[ii], this->_Width, this->_Height, 3);
        }
    }

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


void Physics::wall_collision(Object* X, double width, double height, int wall) {
    // Create a mirror object
    Object mirror;
    mirror.set_mass(X->get_mass());
    mirror.set_radius(X->get_radius());
    mirror.bouncyness = X->bouncyness;

    // Get velocity of the object
    vec2d pos = X->get_position();
    vec2d vel = X->get_velocity();

    switch (wall) {
        case 1: // North
            mirror.set_velocity(vel[0], -1 * std::abs(vel[1]) );
            mirror.set_position(pos[0], height/2 + std::abs(height/2 - pos[1]) );
            break;
        case 2: // East
            mirror.set_velocity(-1 * std::abs(vel[0]), vel[1]);
            mirror.set_position(width/2 + std::abs(width/2 - pos[0]), pos[1]);
            break;
        case 3: // South
            mirror.set_velocity(vel[0], std::abs(vel[1]) );
            mirror.set_position(pos[0], -height/2 - std::abs(pos[1] + height/2) );
            break;
        case 4: // West
            mirror.set_velocity(std::abs(vel[0]), vel[1]);
            mirror.set_position(-width/2 - std::abs(pos[0] + width/2), pos[1]);
            break;
    }



    // Perform a collision, only if sensible (prevents things getting stuck in a wall)
    if ( this->check_collision(&mirror, X) ) {
        this->resolve_collision(&mirror, X);
    }

    // Mirror object will clean up due to scope
}

double Physics::distance_between(Object* X, Object* Y) {
    vec2d pos_X = X-> get_position();
    vec2d pos_Y = Y-> get_position();
    double dist = std::sqrt( (pos_Y[0]-pos_X[0])*(pos_Y[0]-pos_X[0]) + (pos_Y[1]-pos_X[1])*(pos_Y[1]-pos_X[1]) );
    return dist;
}

vec2d Physics::acceleration (Object* X, Object* Y){
    double dist = this -> distance_between (X , Y);
    vec2d pos_X = X -> get_position();
    vec2d pos_Y = Y -> get_position();
    vec2d r = sub(pos_Y , pos_X);
    double mass = Y -> get_mass();
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
    double thruster_a = this->thruster_force / this->get_mass();
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
    vec2d velocity = me->get_velocity();
    vec2d position = me->get_position();

    vec2d velocity_half = add(velocity, cmult(acceleration, timestep/2));
    new_pos_vel[0] = add(position, cmult(velocity_half, timestep) );

    new_pos_vel[1] = add(velocity, cmult(acceleration, timestep));

    // Euler method
    //new_pos_vel[0] = add(position, cmult(velocity, time_step));
    //new_pos_vel[1] = add(velocity, cmult(acceleration, time_step));

    return new_pos_vel;
};


void Universe::simulate_one_time_unit(double fps) {

    // Call physics_runtime_iteration as many times as required to advance it one time "unit",
    // which is defined as 1/FPS

    int iterations = double(1.0/fps) / this->physics.timestep;
    for ( int ii = 0; ii < iterations; ++ii ) {
        this->physics_runtime_iteration();
    }

}

/*
 * clock_t begin = std::clock();


    clock_t end = std::clock();

    double delta = double(end - begin) / CLOCKS_PER_SEC;

    // If the execution was done within a second
    if (delta < 1) {
        unsigned int microseconds = int(double(1 - delta) * 1E6);
        usleep(microseconds);
    }
    else{
        std::cerr << "WARNING: SIMULATION CANNOT KEEP UP!";
    }
 */

void Physics::lose_energy(Object* me, double factor) {
    // Rescale the velocity factor
    me->set_velocity(cmult(me->get_velocity(), 1 - std::sqrt(factor)));
}

void Object::lose_energy(double factor, Physics &physics) {
    physics.lose_energy(this, factor);
}

void Object::on_collide (Object* target, Physics &physics) {
    // Do nothing yet
    target->lose_energy(0.2, physics);
}

void Player::on_collide (Object* target, Physics &physics) {
    // Make the other object slow down!
    target->lose_energy(0, physics);
}