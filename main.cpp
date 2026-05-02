#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

struct satellite
{
    double pos_x, pos_y;
    double vel_x, vel_y;
    double acc_x, acc_y;
};

void update(satellite &s, double dt)
{
    const double GM = 3.986e14; /// Gravitational constant

    s.vel_x += 0.5 * s.acc_x * dt; /// Update the velocity of the satellite in the x and y directions
    s.vel_y += 0.5 * s.acc_y * dt;

    s.pos_x += s.vel_x * dt; /// Update the position of the satellite in the x and y directions
    s.pos_y += s.vel_y * dt;

    double r = sqrt(pow(s.pos_x, 2) + pow(s.pos_y, 2));
    s.acc_x = -GM * s.pos_x / pow(r, 3); /// Update the acceleration of the satellite in the x and y directions
    s.acc_y = -GM * s.pos_y / pow(r, 3);
    
    s.vel_x += 0.5 * s.acc_x * dt; /// Update the velocity of the satellite in the x and y directions again after calculating the new acceleration
    s.vel_y += 0.5 * s.acc_y * dt;
}

void init(satellite &s)
{
    constexpr double GM = 3.986e14; /// Gravitational constant

    double r = sqrt(pow(s.pos_x, 2) + pow(s.pos_y, 2));
    s.acc_x = -GM * s.pos_x / pow(r, 3); /// Calculate the initial acceleration of the satellite in the x and y directions
    s.acc_y = -GM * s.pos_y / pow(r, 3);
}

int main()
{

    int n, t;
    double dt, r;
    cin >> n >> t >> dt;
    vector<satellite> v(n);
    for (int i = 0; i < n; i++)
        cin >> v[i].pos_x >> v[i].pos_y >> v[i].vel_x >> v[i].vel_y;

    for (int i = 0; i < n; i++)
        init(v[i]);

    for (int j = 0; j < t; j++) /// Loop through each time step
    {
        for (int i = 0; i < n; i++) /// Loop through each satellite
            update(v[i], dt);

        for (int i = 0; i < n; i++) /// Loop through each satellite to print its position
        {
            double r = sqrt(pow(v[i].pos_x, 2) + pow(v[i].pos_y, 2)); /// Calculate the distance from the centre of the Earth to the satellite
            cout << v[i].pos_x / 1000 << " " << v[i].pos_y / 1000 << " " << r / 1000 << "km\n";
        }

        cout << "\n";
    }

    return 0;
}