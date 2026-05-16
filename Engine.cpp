#include <SDL3/SDL.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "satellite.h"
#define M_PI 3.14159265358979323846
using namespace std;

constexpr int kScreenWidth = 640;
constexpr int kScreenHeight = 480;

int main()
{
    int n; /// Create a satellite
    double dt;
    cin >> n >> dt;
    vector<satellite> v(n);
    for (int i = 0; i < n; i++)
        cin >> v[i].pos_x >> v[i].pos_y >> v[i].vel_x >> v[i].vel_y;
    for (int i = 0; i < n; i++)
        init(v[i]);
    double scale = 640.0 / 16000000.0; /// Adapt scale
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
    while (running) /// Main loop that checks for events sssuch as the user closing the window
    {
        for (int i = 0; i < n; i++)
            update(v[i], dt);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int i = 0; i < n; i++)
        {
            double view_z = v[i].pos_z - camera_z;
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

            double screen_x = (v[i].pos_x / view_z) * focal_length + 320;
            double screen_y = 240 - (v[i].pos_y / view_z) * focal_length;
            if (view_z > 0)
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