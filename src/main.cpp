#include <iostream>
#include <SDL2/SDL.h>
#include <cstdlib>
#include <cmath>
#include <vector>

struct Home {
    float x;
    float y;
    void setHome(float x1, float y1){
        x = x1;
        y = y1;
    }
};

struct Pheromone {
    float cells[200][150];

    void deposit(float x, float y) {
        int gridX = (int)(x / 4);
        int gridY = (int)(y / 4);
        if (gridX < 0 || gridX >= 200 || gridY < 0 || gridY >= 150) return;
        cells[gridX][gridY] += 0.5f;
        if (cells[gridX][gridY] > 1.0f) {
            cells[gridX][gridY] = 1.0f;
        }
    }
    void decay() {
        for (int x = 0; x < 200; x++) {
            for (int y = 0; y < 150; y++) {
                cells[x][y] *= 0.99;
            }
        }
    }
    void draw(SDL_Renderer* renderer) {
        for (int x = 0; x < 200; x++) {
            for (int y = 0; y < 150; y++) {

                if (cells[x][y] > 0.01f) {
                    SDL_SetRenderDrawColor(renderer, 255, 200, 0, (int)(cells[x][y] * 255));
                    SDL_Rect rect = {x * 4, y * 4, 4, 4};
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }
    }
    Pheromone() {
        for (int x = 0; x < 200; x++)
            for (int y = 0; y < 150; y++)
                cells[x][y] = 0.0f;
    }
};

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
        if (hasFood) return;
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
    void goHome(Home home) {
        float angle = atan2(home.y - y, home.x - x);
        dirX = cos(angle);
        dirY = sin(angle);
    }
    void followPheromone(Pheromone& grid) {
        float bestStrength = 0.0f;
        float bestDirX = dirX;
        float bestDirY = dirY;

        for (int i = -2; i <= 2; i++) {
            float angle = atan2(dirY, dirX) + i * 0.5f;
            float checkX = x + cos(angle) * 20;
            float checkY = y + sin(angle) * 20;

            int gridX = (int)(checkX / 4);
            int gridY = (int)(checkY / 4);

            if (gridX < 0 || gridX >= 200 || gridY < 0 || gridY >= 150) continue;

            float strength = grid.cells[gridX][gridY];
            if (strength > bestStrength) {
                bestStrength = strength;
                bestDirX = cos(angle);
                bestDirY = sin(angle);
            }
        }

        if (bestStrength > 0.05f) {
            dirX = bestDirX;
            dirY = bestDirY;
        }
    };
};

struct Food {
    float x;
    float y;

    void spawnRandom() {
        x = rand() % 800;
        y = rand() % 600;
    }
};



float calcDist(float antx, float anty, float foodx, float foody){
    float distx = antx - foodx;
    float disty = anty - foody;
    return sqrt((distx*distx) + (disty*disty));
}

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
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    bool running = true;
    srand(SDL_GetTicks());

    std::vector<Ant> ants;
    for (int i = 0; i < 100; i++) {
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
    Food food;
    food.spawnRandom();

    Home home;
    home.setHome(400, 300);
    int frameCount = 0;

    Pheromone pheromone;

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
            if (!ant.hasFood) {
                ant.followPheromone(pheromone);
            } else {
                ant.goHome(home);
            }
        }
        frameCount++;
        if (frameCount % 60 == 0) {
            for (Ant& ant : ants) {
                if(!ant.hasFood){
                    ant.randomizeMovement();
                }
            }
        }

        pheromone.decay();

        for (Ant& ant:ants) {
            if(calcDist(ant.x, ant.y, food.x, food.y) < 15) {
                ant.hasFood = true;
                ant.goHome(home);
                //pheromone.deposit(ant.x, ant.y);
            }
        }

        //ant.checkBoundaries();
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        //SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (Ant& ant : ants) {
            if(ant.hasFood) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                if(calcDist(ant.x, ant.y, home.x, home.y) < 15){
                    ant.hasFood = false;
                }
                pheromone.deposit(ant.x, ant.y);
            }
        
            else {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            }
            SDL_Rect rect = {(int)ant.x, (int)ant.y, 20, 20};
            SDL_RenderFillRect(renderer, &rect);
        }
        SDL_Rect foodRect = {(int)food.x, (int)food.y, 10, 10};
        SDL_Rect homeRect = {(int)home.x, (int)home.y, 10, 10};
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &foodRect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderFillRect(renderer, &homeRect);
        pheromone.draw(renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(8);


    }

    std::cout << "Hello, Ant Colony!\n";
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit(); //shuts down all SDL2 subsystems and releases memory and system resources 
    return 0;
}