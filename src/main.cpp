#include <iostream>
#include <SDL2/SDL.h>


int main(int argc, char* argv[]) {
    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        std::cerr << "SDL failed to initialize" << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Ant Colony Simulator",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_SHOWN
    );

    bool running = true;
    while(running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT){
                running = false;
            }
        }
    }

    std::cout << "Hello, Ant Colony!\n";
        
    SDL_Quit(); //shuts down all SDL2 subsystems and releases memory and system resources 
    return 0;
}