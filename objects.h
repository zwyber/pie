//
// Created by paul on 8/1/16.
//

#ifndef PIE_GITHUB_OBJECTS_H
#define PIE_GITHUB_OBJECTS_H

#include<vector>
#include<array>
#include<iostream>

class Universe;
class Object;

class Object {
private:
    std::array<double, 2> position = {{ 0 }};
    std::array<double, 2> velocity = {{ 0 }};
    int id = -1;

public:
    int get_id();
    void set_id(Universe universe);

    void set_position(double new_x, double new_y);
    std::array<double, 2> get_position ();

    void set_velocity(double new_vx, double new_vy);
    std::array<double, 2> get_velocity ();
};

class Universe {

private:
    int new_object_id = 0;
    std::vector<Object> objects = {};

public:
    std::vector<Object> get_objects();

    void add_object (Object &obj, Universe &me);
    Object* get_object_by_index(int index);
    void remove_object (int obj_index);

    void debug_display_world ();

    int give_new_object_id();
};

const Universe* THIS_UNIVERSE;

class Player;


#include "objects.cpp"

#endif //PIE_GITHUB_OBJECTS_H
