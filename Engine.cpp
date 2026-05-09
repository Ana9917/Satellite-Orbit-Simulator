#include <SDL3/SDL.h>
#include <iostream>
#include <cmath>
#include "satellite.h"
#define M_PI 3.14159265358979323846
using namespace std;

constexpr int kScreenWidth = 640;
constexpr int kScreenHeight = 480;

int main()
{
    satellite s; /// Create a satellite
    s.pos_x = 6600000;
    s.pos_y = 0;
    s.pos_z = 2000000;
    s.vel_x = 0;
    s.vel_y = 7800;
    s.vel_z = 2000;
    double scale = 640.0 / 16000000.0; /// Adapt scale
    double dt = 100.0;                 /// Define delta time
    const double camera_z = -20000000.0;
    const double focal_length = 400.0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) /// Check if SDL initialisation was successful and prints an error message if it was not
    {
        cout << "SDL could not initialise!" << SDL_GetError() << endl;
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Simple SDL Window", kScreenWidth, kScreenHeight, SDL_WINDOW_RESIZABLE); /// Create a window with the specified title, width, height and flags
    if (!window)
    {
        cout << "The window could not be created" << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL); /// Create renderer
    if (!renderer)
    {
        cout << "The renderer could not be created" << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    bool running = true;
    SDL_Event event;
    init(s);
    while (running) /// Main loop that checks for events sssuch as the user closing the window
    {
        update(s, dt);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        double view_z = s.pos_z - camera_z;
        double earth_r = (6371000.0 / (0 - camera_z)) * focal_length;
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

        for (double i = 0; i <= 2 * M_PI; i += 2 * M_PI / 100)
        {
            /// Calculate the current angle
            double x1 = 320 + earth_r * cos(i);
            double y1 = 240 + earth_r * sin(i);
            double x2 = 320 + earth_r * cos(i + 2 * M_PI / 100);
            double y2 = 240 + earth_r * sin(i + 2 * M_PI / 100);
            SDL_RenderLine(renderer, x1, y1, x2, y2);
        }

        double screen_x = (s.pos_x / view_z) * focal_length + 320;
        double screen_y = 240 - (s.pos_y / view_z) * focal_length;
        if (view_z > 0)
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderPoint(renderer, (float)screen_x, (float)screen_y); /// Draw the point
        }
        SDL_RenderPresent(renderer); /// Show the render
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window); /// Cleans up resources by destroying the window and quitting SDL
    SDL_Quit();
    return 0;
}