#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include "satellite.h"

namespace {
void usage(std::ostream &out) {
    out << "Usage: satellite-orbit [--help] < input.txt > orbit.csv\n"
        << "Input: satellite_count step_count dt_seconds, followed by one\n"
        << "x y z vx vy vz row per satellite (metres and metres/second).\n"
        << "Output: CSV with the initial state and each completed step.\n";
}

bool valid(const satellite &s) {
    const double radius = std::hypot(std::hypot(s.pos_x, s.pos_y), s.pos_z);
    return std::isfinite(radius) && radius > 0.0 &&
           std::isfinite(s.vel_x) && std::isfinite(s.vel_y) &&
           std::isfinite(s.vel_z) && std::isfinite(s.acc_x) &&
           std::isfinite(s.acc_y) && std::isfinite(s.acc_z);
}

void write_state(const std::vector<satellite> &satellites, long long step, double time) {
    for (std::size_t i = 0; i < satellites.size(); ++i) {
        const auto &s = satellites[i];
        const double radius = std::hypot(std::hypot(s.pos_x, s.pos_y), s.pos_z);
        std::cout << step << ',' << time << ',' << i + 1 << ','
                  << s.pos_x / 1000.0 << ',' << s.pos_y / 1000.0 << ','
                  << s.pos_z / 1000.0 << ',' << s.vel_x << ',' << s.vel_y << ','
                  << s.vel_z << ',' << radius / 1000.0 << '\n';
    }
}
} // namespace

int main(int argc, char **argv) {
    if (argc == 2 && std::string(argv[1]) == "--help") {
        usage(std::cout);
        return 0;
    }
    if (argc != 1) {
        usage(std::cerr);
        return 1;
    }

    // Bound allocation and output size; reject invalid input before printing CSV.
    long long count, steps;
    double dt;
    if (!(std::cin >> count >> steps >> dt) || count < 1 || count > 10000 ||
        steps < 0 || steps > 10000000 || !std::isfinite(dt) || dt <= 0.0 ||
        !std::isfinite(steps * dt) || count > 10000000 / (steps + 1)) {
        std::cerr << "Invalid header: use 1..10000 satellites, non-negative steps,\n"
                  << "a finite positive time step, and at most 10000000 output rows.\n";
        return 1;
    }
    std::vector<satellite> satellites(static_cast<std::size_t>(count));
    for (auto &s : satellites) {
        if (!(std::cin >> s.pos_x >> s.pos_y >> s.pos_z
                       >> s.vel_x >> s.vel_y >> s.vel_z) || !valid(s)) {
            std::cerr << "Invalid satellite: supply six finite values and a non-zero radius.\n";
            return 1;
        }
        init(s);
        if (!valid(s)) {
            std::cerr << "Initial state exceeds the numerical range of the physics model.\n";
            return 1;
        }
    }
    std::cin >> std::ws;
    if (!std::cin.eof()) {
        std::cerr << "Unexpected input after the satellite rows.\n";
        return 1;
    }

    std::cout << std::setprecision(std::numeric_limits<double>::max_digits10)
              << "step,time_s,satellite,x_km,y_km,z_km,vx_m_s,vy_m_s,vz_m_s,radius_km\n";
    write_state(satellites, 0, 0.0);
    for (long long step = 1; step <= steps; ++step) {
        for (auto &s : satellites) {
            update(s, dt);
            if (!valid(s)) {
                std::cerr << "Simulation became singular or non-finite at step " << step << ".\n";
                return 1;
            }
        }
        write_state(satellites, step, step * dt);
        if (!std::cout) {
            std::cerr << "Could not write simulation output.\n";
            return 1;
        }
    }
    std::cout.flush();
    return std::cout ? 0 : 1;
}
