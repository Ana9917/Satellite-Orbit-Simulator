#include <iostream>
#include <vector>
#include <cmath>
#include "satellite.h"
using namespace std;

void update(satellite &s, double dt)
{
    const double GM = 3.986e14; /// Gravitational constant

    s.vel_x += 0.5 * s.acc_x * dt; /// Update the velocity of the satellite in the x, y, and z directions
    s.vel_y += 0.5 * s.acc_y * dt;
    s.vel_z += 0.5 * s.acc_z * dt;

    s.pos_x += s.vel_x * dt; /// Update the position of the satellite in the x, y and z directions
    s.pos_y += s.vel_y * dt;
    s.pos_z += s.vel_z * dt;

    double r = sqrt(
        s.pos_x * s.pos_x +
        s.pos_y * s.pos_y +
        s.pos_z * s.pos_z);
    double factor = -GM / (r * r * r);
    s.acc_x = factor * s.pos_x; /// Update the acceleration of the satellite in the x, y, and z directions
    s.acc_y = factor * s.pos_y;
    s.acc_z = factor * s.pos_z;

    s.vel_x += 0.5 * s.acc_x * dt; /// Update the velocity of the satellite in the x, y, and z directions again after calculating the new acceleration
    s.vel_y += 0.5 * s.acc_y * dt;
    s.vel_z += 0.5 * s.acc_z * dt;
}

void init(satellite &s)
{
    constexpr double GM = 3.986e14;

    double r = sqrt(
        s.pos_x * s.pos_x +
        s.pos_y * s.pos_y +
        s.pos_z * s.pos_z);

    double factor = -GM / (r * r * r);

    s.acc_x = factor * s.pos_x;
    s.acc_y = factor * s.pos_y;
    s.acc_z = factor * s.pos_z;
}