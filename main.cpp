#include <SDL2/SDL.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <string>

// Output filename constant (easy to change in one place)
constexpr const char* OUTPUT_FILENAME = "mouse_data.txt";

// Using struct to organize mouse data events with timestamp
struct MouseEvent {
    int x, y;
    bool LeftClick, RightClick;
    std::chrono::steady_clock::time_point timestamp;  // High-resolution timestamp
};

// Helper function to format timestamp as milliseconds since program start
std::string FormatTimestamp(const std::chrono::steady_clock::time_point& tp, 
                           const std::chrono::steady_clock::time_point& startTime) {
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(tp - startTime);
    return std::to_string(duration.count());
}

void SaveData(const MouseEvent &data, const std::chrono::steady_clock::time_point& startTime) {
    std::ofstream outFile(OUTPUT_FILENAME, std::ios_base::app);
    
    // CSV format: timestamp_ms,x,y,left_click,right_click
    outFile << FormatTimestamp(data.timestamp, startTime) << ","
            << data.x << ","
            << data.y << ","
            << (data.LeftClick ? 1 : 0) << ","
            << (data.RightClick ? 1 : 0) << std::endl;
    outFile.close();
}

// Helper function to write CSV header if file is empty
void WriteHeaderIfNeeded() {
    std::ofstream file(OUTPUT_FILENAME, std::ios::ate | std::ios::in);
    if (file.is_open() && file.tellp() == 0) {
        file << "timestamp_ms,x,y,left_click,right_click\n";
    }
    // File auto-closes when going out of scope
}

int main() {
    // Record program start time for relative timestamps
    auto startTime = std::chrono::steady_clock::now();
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL Error: " 
                  << SDL_GetError() << std::endl; 
        return -1;
    }

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

    SDL_Rect mouseArea = {0, 0, 200, 200};
    SDL_Event event;
    MouseEvent currentEvent = {0, 0, false, false, startTime};

    // Write CSV header on first run (only if file is empty)
    WriteHeaderIfNeeded();

    bool running = true;
    
    // Event loop
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;  // Exit loop gracefully
                break;
            }

            // Capture timestamp at event occurrence
            auto eventTime = std::chrono::steady_clock::now();

            if (event.type == SDL_MOUSEMOTION) {
                if (event.motion.x >= mouseArea.x && event.motion.x <= mouseArea.x + mouseArea.w &&
                    event.motion.y >= mouseArea.y && event.motion.y <= mouseArea.y + mouseArea.h) {
                    currentEvent.x = event.motion.x;
                    currentEvent.y = event.motion.y;
                    currentEvent.timestamp = eventTime;
                    std::cout << "Mouse moved to (" << currentEvent.x << ", " << currentEvent.y << ")" << std::endl;
                }
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                currentEvent.timestamp = eventTime;
                if (event.button.button == SDL_BUTTON_LEFT) {
                    currentEvent.LeftClick = true;
                    std::cout << "Left button pressed!" << std::endl;
                }
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    currentEvent.RightClick = true;
                    std::cout << "Right button pressed!" << std::endl;
                }
            }

            if (event.type == SDL_MOUSEBUTTONUP) {
                currentEvent.timestamp = eventTime;
                if (event.button.button == SDL_BUTTON_LEFT) {
                    currentEvent.LeftClick = false;
                    std::cout << "Left button released!" << std::endl;
                }
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    currentEvent.RightClick = false;
                    std::cout << "Right button released!" << std::endl;
                }
            }

            // Save data with timestamp after each event
            SaveData(currentEvent, startTime);
        }
        
        // Small sleep to reduce CPU usage
        SDL_Delay(1);
    }

    // 🎯 CLEANUP & CONFIRMATION MESSAGE
    std::cout << "\n✅ Data saved to: " << OUTPUT_FILENAME << std::endl;
    std::cout << "📊 Columns: timestamp_ms, x, y, left_click, right_click" << std::endl;
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}