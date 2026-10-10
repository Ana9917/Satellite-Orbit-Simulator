# Satellite Orbit Simulator

A C++17 simulator that displays satellites, orbit trails and a wireframe Earth using SDL3.

## Requirements

- CMake 3.21 or newer.
- A C++17 compiler (GCC/MinGW, Clang or MSVC).
- The SDL3 development package, version 3.2 or newer, built for the same compiler and architecture as this project. SDL2 cannot be substituted.

Install SDL3 using a package manager or build and install it using the [official SDL CMake instructions](https://wiki.libsdl.org/SDL3/README-cmake). A development package contains headers, libraries and an SDL3 CMake package configuration; a DLL alone is insufficient.

## Configure, build and run

Run these commands from the repository directory:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
cmake --build build --config Release --target run
```

The run target builds the simulator if necessary, then launches it in the terminal. It also works with generators that place executables in configuration-specific directories, such as Visual Studio.

If CMake cannot find SDL3, pass its installation prefix when configuring:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:/SDL3"
```

Replace `C:/SDL3` with your SDL3 installation directory. Alternatively, set `-DSDL3_DIR="path/to/directory/containing/SDL3Config.cmake"`.

### Windows with MinGW

With CMake and MinGW on PATH, and a matching MinGW SDL3 development installation:

```sh
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:/SDL3"
cmake --build build
cmake --build build --target run
```

Use a fresh build directory when switching compiler or generator. When linking shared SDL3 on Windows, CMake copies SDL3.dll beside the executable after building.

### VS Code

Install the CMake Tools and C/C++ extensions. Use **CMake: Select a Kit**, **CMake: Configure**, then **CMake: Build**. Select `SatelliteOrbit` as the launch target. Run the `run` build target from a terminal when entering satellite data interactively.

## Simulation input

Enter the number of satellites, the time step in seconds, then one line per satellite with:

```text
pos_x pos_y pos_z vel_x vel_y vel_z
```

Positions are in metres and velocities in metres per second. Use at least one satellite, a positive time step and non-zero initial distance from Earth's centre.

For example, one satellite with a time step of 10 seconds:

```text
1
10
7000000 0 0 0 7546 0
```

Controls:
- Arrow keys or left-button dragging: rotate the view.
- Mouse wheel: zoom.
- Space: pause or resume.
- Close the window: quit.

## Source files and generated files

`Engine.cpp` contains the graphical application's `main()`. CMake compiles it together with `functions.cpp` (physics) and `shapes.cpp` (meshes) into one executable.

The former `main.cpp` was a separate console-only simulation and is not needed by the graphical application. It has been removed, along with the tracked `main.exe` and `Engine.exe`. Previous versions remain in Git history.

Build output belongs in `build/` and is ignored by Git. Adding a filename to `.gitignore` does not untrack a file that is already committed. To retain an obsolete file locally while removing it from the repository:

```sh
git rm --cached main.cpp
git commit -m "Stop tracking the obsolete console entry point"
```

To delete a tracked file both locally and from the repository, use `git rm main.cpp` instead. Push the commit to update GitHub.
