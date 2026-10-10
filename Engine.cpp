#include <SDL3/SDL.h>
#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <deque>
#include "satellite.h"
#include "stars.h"
#define M_PI 3.14159265358979323846
using namespace std;

constexpr int kScreenWidth = 640;
constexpr int kScreenHeight = 480;

void applyYaw(satellite &s, double yaw) // Rotation on the y axis
{
    double old_x = s.pos_x, old_z = s.pos_z;
    s.pos_x = old_x * cos(yaw) + old_z * sin(yaw);
    s.pos_z = -old_x * sin(yaw) + old_z * cos(yaw);
}

void applyPitch(satellite &s, double pitch) // Rotation on the x axis
{
    double old_y = s.pos_y, old_z = s.pos_z;
    s.pos_y = old_y * cos(pitch) - old_z * sin(pitch);
    s.pos_z = old_y * sin(pitch) + old_z * cos(pitch);
}

int main()
{
    int n; /// Create a satellite
    double dt, yaw = 0.0, pitch = 0.0;
    cin >> n >> dt;
    vector<deque<satellite>> trail(n);
    vector<satellite> v(n);
    vector<star> stars(1000); 
    mt19937 gen(42); ///Number generator
    uniform_real_distribution<double> dist(-1e11, 1e11);
    for (int i = 0; i < stars.size(); i++)
    {
        ///Gives random values to x, y, and z
        stars[i].x = dist(gen);
        stars[i].y = dist(gen);
        stars[i].z = dist(gen);
    }
    for (int i = 0; i < n; i++)
        cin >> v[i].pos_x >> v[i].pos_y >> v[i].pos_z >> v[i].vel_x >> v[i].vel_y >> v[i].vel_z;
    double r = sqrt(v[0].pos_x * v[0].pos_x + v[0].pos_y * v[0].pos_y + v[0].pos_z * v[0].pos_z);
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

    bool running = true, paused = false;
    SDL_Event event;
    constexpr double GM = 3.986e14;
    double T = 2 * M_PI * sqrt((r * r * r) / GM); /// Calculte orbital period
    int steps = (int)(T / dt);                    /// No. of steps in one orbital period
    while (running)                               // Main loop that checks for events such as the user closing the window
    {
        if (!paused) /// Pause using space
        {
            for (int i = 0; i < n; i++)
                update(v[i], dt);
        }
        const bool *state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_LEFT])
            yaw += 0.02;
        if (state[SDL_SCANCODE_RIGHT])
            yaw -= 0.02;
        if (state[SDL_SCANCODE_UP])
            pitch += 0.02;
        if (state[SDL_SCANCODE_DOWN])
            pitch -= 0.02;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        for (int i = 0; i < stars.size(); i++)
        {
            double view_starz = stars[i].z - camera_z;
            double screen_starsx = (stars[i].x / view_starz) * focal_length + 320;
            double screen_starsy = 240 - (stars[i].y / view_starz) * focal_length;
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); ///Draw the stars
            SDL_RenderPoint(renderer, screen_starsx, screen_starsy);
        }
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        double earth_r = (6371000.0 / (0 - camera_z)) * focal_length;
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

        for (double i = 0; i <= 2 * M_PI; i += 2 * M_PI / 100)
        {
            /// Calculate the current angle
            double x1 = 320 + earth_r * cos(i);
            double y1 = 240 + earth_r * sin(i);
            double x2 = 320 + earth_r * cos(i + 2 * M_PI / 100);
            double y2 = 240 + earth_r * sin(i + 2 * M_PI / 100);
            SDL_RenderLine(renderer, x1, y1, x2, y2); /// Draw Earth
        }

        for (int i = 0; i < n; i++)
        {
            satellite rotated = v[i]; /// Copy so physics position is never modified
            applyYaw(rotated, yaw);   /// Rotate copy for display only
            applyPitch(rotated, pitch);
            /// Perspective projection
            double view_z = rotated.pos_z - camera_z;
            double screen_x = (rotated.pos_x / view_z) * focal_length + 320;
            double screen_y = 240 - (rotated.pos_y / view_z) * focal_length;
            trail[i].push_front(v[i]);
            if (view_z > 0)
            {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_FRect rect = {(float)screen_x - 3, (float)screen_y - 3, 6, 6};
                SDL_RenderFillRect(renderer, &rect); /// Draw the satellite
            }

            if (trail[i].size() > steps)
                trail[i].pop_back();
            for (int j = 0; j < trail[i].size() - 1; j++)
            {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                satellite r1 = trail[i][j]; /// Copy trail points for display rotation
                satellite r2 = trail[i][j + 1];
                /// Perspective projection for the trail
                applyYaw(r1, yaw);
                applyYaw(r2, yaw);
                applyPitch(r1, pitch);
                applyPitch(r2, pitch);
                double view_tz1 = r1.pos_z - camera_z;
                double view_tz2 = r2.pos_z - camera_z;
                double screen_tx = (r1.pos_x / view_tz1) * focal_length + 320;
                double screen_ty = 240 - (r1.pos_y / view_tz1) * focal_length;
                double screen_tx2 = (r2.pos_x / view_tz2) * focal_length + 320;
                double screen_ty2 = 240 - (r2.pos_y / view_tz2) * focal_length;
                SDL_RenderLine(renderer, screen_tx, screen_ty, screen_tx2, screen_ty2);
            }
        }

        SDL_RenderPresent(renderer); /// Show the render
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_SPACE)
                paused = !paused;
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window); /// Cleans up resources by destroying the window and quitting SDL
    SDL_Quit();
    return 0;
}