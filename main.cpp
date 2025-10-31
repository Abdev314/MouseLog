#include <SDL2/SDL.h>
#include <iostream>
#include <fstream>

// Using struct to organize mouse data events 
struct MouseEvent {
    int x, y;
    bool LeftClick, RightClick;
};

void SaveData(const MouseEvent &data) {
    std::ofstream outFile("mouse_data.txt", std::ios_base::app);
    outFile << "X: " << data.x << " Y: " << data.y << " Left Click: " 
            << data.LeftClick << " Right Click: " << data.RightClick << std::endl;
    outFile.close();
}

int main() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL Error: " 
                  << SDL_GetError() << std::endl; 
        return -1;
    }

    // Create a window with a specific size (600x600) and set it to be visible
    SDL_Window* window = SDL_CreateWindow("Mouse Event Detection", 
                                         SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                         200, 200, SDL_WINDOW_SHOWN);

    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL Error: " 
                  << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL Error: " 
                  << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Set the default mouse area (inside the window)
    SDL_Rect mouseArea = {0, 0, 200, 200};  // Screen area is the whole window (600x600)
    
    SDL_Event event;
    MouseEvent currentEvent = {0, 0, false, false};  // Initial values for mouse data

    // Event loop
    while (true) {
        while (SDL_PollEvent(&event)) {
            // Check for window close event
            if (event.type == SDL_QUIT) {
                std::cout << "Window closed!" << std::endl;
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                SDL_Quit();
                return 0;
            }

            // Mouse motion event
            if (event.type == SDL_MOUSEMOTION) {
                // If the mouse is within the defined area (600x600 window)
                if (event.motion.x >= mouseArea.x && event.motion.x <= mouseArea.x + mouseArea.w &&
                    event.motion.y >= mouseArea.y && event.motion.y <= mouseArea.y + mouseArea.h) {
                    currentEvent.x = event.motion.x;
                    currentEvent.y = event.motion.y;
                    std::cout << "Mouse moved to (" << currentEvent.x << ", " << currentEvent.y << ")" << std::endl;
                }
            }

            // Mouse button down event (button press)
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    currentEvent.LeftClick = true;
                    std::cout << "Left button pressed!" << std::endl;
                }
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    currentEvent.RightClick = true;
                    std::cout << "Right button pressed!" << std::endl;
                }
            }

            // Mouse button up event (button release)
            if (event.type == SDL_MOUSEBUTTONUP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    currentEvent.LeftClick = false;
                    std::cout << "Left button released!" << std::endl;
                }
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    currentEvent.RightClick = false;
                    std::cout << "Right button released!" << std::endl;
                }
            }

            // Save data to file after each event
            SaveData(currentEvent);
        }
    }

    SDL_Quit();  // Quit SDL
    return 0;
}
