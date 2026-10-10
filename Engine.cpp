#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <deque>
#include "satellite.h"
#include "stars.h"
#include "shapes.h"
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

void drawMesh(SDL_Renderer *renderer, const Mesh &mesh,
              Vertex3 position, double modelScale,
              double yaw, double pitch,
              double camera_z, double focal_length)
{
    vector<SDL_FPoint> projected(mesh.vertices.size());
    vector<bool> visible(mesh.vertices.size(), false);

    for (size_t i = 0; i < mesh.vertices.size(); ++i)
    {
        const Vertex3 &vertex = mesh.vertices[i];

        // Convert a model vertex into a position
        satellite point{};
        point.pos_x = position.x + vertex.x * modelScale;
        point.pos_y = position.y + vertex.y * modelScale;
        point.pos_z = position.z + vertex.z * modelScale;

        // Apply the same view rotation used by orbit trails
        applyYaw(point, yaw);
        applyPitch(point, pitch);

        double view_z = point.pos_z - camera_z;
        if (view_z <= 1.0)
            continue;
        projected[i] = {
            static_cast<float>(320.0 + point.pos_x / view_z * focal_length),
            static_cast<float>(240.0 - point.pos_y / view_z * focal_length)};
        visible[i] = true;
    }

    for (const auto &edge : mesh.edges)
    {
        int a = edge.first;
        int b = edge.second;

        if (visible[a] && visible[b])
        {
            SDL_RenderLine(renderer,
                           projected[a].x, projected[a].y,
                           projected[b].x, projected[b].y);
        }
    }
}

int main(int argc, char *argv[])
{
    int n; /// Create a satellite
    double dt, yaw = 0.0, pitch = 0.0;
    cout<<"No. of satellites ";
    cin>> n;
    cout<<"Delta time ";
    cin>> dt;
    vector<deque<satellite>> trail(n);
    vector<satellite> v(n);
    vector<star> stars(1000);
    mt19937 gen(42); /// Number generator
    uniform_real_distribution<double> dist(-1e11, 1e11);
    for (int i = 0; i < stars.size(); i++)
    {
        /// Gives random values to x, y, and z
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
    double focal_length = 400.0;

    if (!SDL_Init(SDL_INIT_VIDEO)) /// Check if SDL initialisation was successful and prints an error message if it was not
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
    double T = 2 * M_PI * sqrt((r * r * r) / GM);          /// Calculte orbital period
    int steps = (int)(T / dt);                             /// No. of steps in one orbital period
    Mesh earthMesh = makeLatLongSphere(6371000.0, 12, 24); /// create the meshes
    Mesh satelliteMesh = makeSatelliteModel();
    bool dragging = false;
    const double mouseSensitivity = 0.005; // mouse sensitivity
    double earthYaw = 0.0;
    while (running) // Main loop that checks for events such as the user closing the window
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
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); /// Draw the stars
            SDL_RenderPoint(renderer, screen_starsx, screen_starsy);
        }
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        SDL_SetRenderDrawColor(renderer, 0, 100, 255, 255);

        drawMesh(renderer, earthMesh,
                 {0.0, 0.0, 0.0}, 1.0,
                 yaw, pitch, camera_z, focal_length);

        for (int i = 0; i < n; i++)
        {
            trail[i].push_front(v[i]);

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

            drawMesh(renderer, satelliteMesh,
                     {v[i].pos_x, v[i].pos_y, v[i].pos_z},
                     200000.0,
                     yaw, pitch, camera_z, focal_length);
            if (trail[i].size() > steps)
                trail[i].pop_back();
            for (size_t j = 0; j + 1 < trail[i].size(); ++j)
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
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
                event.button.button == SDL_BUTTON_LEFT)
                dragging = true;

            if (event.type == SDL_EVENT_MOUSE_BUTTON_UP &&
                event.button.button == SDL_BUTTON_LEFT)
                dragging = false;

            if (event.type == SDL_EVENT_MOUSE_MOTION && dragging)
            {
                yaw += event.motion.xrel * mouseSensitivity;
                pitch += event.motion.yrel * mouseSensitivity;

                // Stop the view flipping upside down.
                const double pitchLimit = M_PI / 2.0 - 0.01;

                if (pitch > pitchLimit)
                    pitch = pitchLimit;
                if (pitch < -pitchLimit)
                    pitch = -pitchLimit;
            }

            if (event.type == SDL_EVENT_WINDOW_FOCUS_LOST)
                dragging = false;
            if (event.type == SDL_EVENT_MOUSE_WHEEL)
            {
                double scroll = event.wheel.y;

                if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
                    scroll = -scroll;

                focal_length *= pow(1.1, scroll);

                if (focal_length < 100.0)
                    focal_length = 100.0;

                if (focal_length > 2000.0)
                    focal_length = 2000.0;
            }
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