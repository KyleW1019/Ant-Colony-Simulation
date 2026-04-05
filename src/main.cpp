#include <iostream>
#include <SDL2/SDL.h>
#include <cstdlib>
#include <cmath>
#include <vector>

struct Ant {
    float x;
    float y;
    float speed;
    float dirX;
    float dirY;
    bool hasFood;

    void update() {
        x += dirX * speed;
        y += dirY * speed;
    }
    void checkBoundaries() {
        if(int(x) >= 800){
            x = 0;
        }
        if(int(y) >= 600){
            y = 0;
        }
        if (x < 0) {
            x = 800;
        }
        if (y < 0) {
            y = 600;
        }
    }
    void randomizeMovement(){
        float angle = (rand() % 360) * (M_PI / 180.0f);
        dirX = cos(angle);
        dirY = sin(angle);
    }
};

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

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool running = true;
    srand(SDL_GetTicks());

    std::vector<Ant> ants;
    for (int i = 0; i < 10; i++) {
        Ant ant;
        ant.x = 400;
        ant.y = 300;
        ant.speed = 2.0f;
        ant.hasFood = false;
        ant.randomizeMovement();
        ants.push_back(ant);
    }
    //srand(SDL_GetTicks());
    //ant.randomizeMovement();

    int frameCount = 0;

    while(running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT){
                running = false;
            }
        }
        //ant.x += ant.speed;
        for (Ant& ant : ants) {
            ant.update();
            ant.checkBoundaries();
        }
        frameCount++;
        if (frameCount % 60 == 0) {
            for (Ant& ant : ants) {
                ant.randomizeMovement();
            }
        }
        //ant.checkBoundaries();
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (Ant& ant : ants) {
            SDL_Rect rect = {(int)ant.x, (int)ant.y, 20, 20};
            SDL_RenderFillRect(renderer, &rect);
        }
        //SDL_RenderFillRect(renderer, &rect);
        SDL_RenderPresent(renderer);
        SDL_Delay(8);
    }

    std::cout << "Hello, Ant Colony!\n";
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit(); //shuts down all SDL2 subsystems and releases memory and system resources 
    return 0;
}