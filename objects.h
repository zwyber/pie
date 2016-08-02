//
// Created by paul on 8/1/16.
//

#ifndef PIE_GITHUB_OBJECTS_H
#define PIE_GITHUB_OBJECTS_H

// Type definitions
typedef unsigned id_type;





class Universe;
class Object;

class Object {
private:
    std::array<double, 2> position = {{ 0 }};
    std::array<double, 2> velocity = {{ 0 }};
    double mass = 0;
    id_type id = 0;

public:
    id_type get_id();
    void set_id(Universe &universe);

    void set_position(double new_x, double new_y);
    std::array<double, 2> get_position ();

    void set_velocity(double new_vx, double new_vy);
    std::array<double, 2> get_velocity ();
};








class Universe {

private:
    int new_object_id = 0;


public:
    Universe();

    std::vector<Object> objects = {};
    // std::vector<Object>* get_objects();

    id_type add_object (Object &obj, Universe &me);
    void remove_object_by_index(int obj_index);

    Object* get_object_by_index(int index);
    Object* get_object_by_id(int id);

    id_type give_new_object_id();

    void update_id_to_object_map();
    std::map<id_type, Object*> id_to_object_map;
};

const Universe* THIS_UNIVERSE;

class Player;


#include "objects.cpp"

#endif //PIE_GITHUB_OBJECTS_H
