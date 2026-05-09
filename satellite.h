#pragma once ///Prevent it from being included multiple times

struct satellite
{
    double pos_x, pos_y, pos_z;
    double vel_x, vel_y, vel_z;
    double acc_x, acc_y, acc_z;
};

void update(satellite &s, double dt);
void init(satellite &s);