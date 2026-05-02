#include <SDL3/SDL.h>
#include <iostream>
using namespace std;
constexpr int kScreenWidth = 640;
constexpr int kScreenHeight = 480;
int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cout << "SDL could not initialise!" << SDL_GetError() << endl;
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Simple SDL Window", kScreenWidth, kScreenHeight, SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        cout << "The window could not be created" << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }
    bool running = true;
    SDL_Event event;
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}