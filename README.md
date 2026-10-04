# Satellite Orbit Simulator

A terminal-only, three-dimensional Earth orbit simulator using the existing
velocity Verlet integration in `functions.cpp`. No SDL or graphics libraries
are required. A C++17 compiler is sufficient.

## Compile and run

```sh
g++ -O2 -std=c++17 -Wall -Wextra -Wpedantic main.cpp functions.cpp -o satellite-orbit
./satellite-orbit < examples/circular-orbit.txt > orbit.csv
./satellite-orbit --help
```

With GNU Make, `make` builds the same executable and `make clean` removes it.
On Windows with MinGW, compile with `-o satellite-orbit.exe`; in Command Prompt,
run `satellite-orbit.exe < examples\circular-orbit.txt > orbit.csv`.

## Input

The first line contains `satellite_count step_count dt_seconds`. Each subsequent
satellite row contains `x y z vx vy vz`: positions in metres and velocities in
metres per second. All six components are required, including zero components.
The example runs one satellite for 600 steps of 10 seconds.

The former SDL input contained only `satellite_count dt_seconds`; add the step
count to make the run finite. The older terminal input used four components;
insert `z` and `vz` to use the three-dimensional model.

## Output

Standard output is CSV: `step,time_s,satellite,x_km,y_km,z_km,vx_m_s,vy_m_s,vz_m_s,radius_km`.
Satellite IDs start at 1. Step 0 records the initial state, followed by one row
per satellite per completed step. Radius is the distance from Earth's centre,
not altitude. Errors go to standard error and return a non-zero exit status.
A numerical failure during a run can leave a partial CSV.

Input must have finite values, a positive time step and non-zero initial radii.
Runs are limited to 10000 satellites and 10000000 output rows. Zero steps is
allowed for inspecting initial states. End interactive input with EOF
(Ctrl+D on Unix, Ctrl+Z then Enter in Windows Command Prompt), or use a file.

The model retains point-mass Earth gravity (`GM = 3.986e14`) without atmosphere,
surface collisions or satellite interactions. Smaller time steps improve
accuracy. Window rendering, star fields, camera rotation and space-bar pause
are replaced by finite terminal runs; stop a run with Ctrl+C.
