#include <iostream>
#include <vector>
#include <cmath>
#include "satellite.h"
using namespace std;

int main()
{

    int n, t;  /// No. of satellites, and no. of time steps
    double dt; /// Delta time (difference in time between steps)
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
}