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
    std::chrono::steady_clock::time_point timestamp;
};

std::string FormatTimestamp(
    const std::chrono::steady_clock::time_point& tp,
    const std::chrono::steady_clock::time_point& startTime
) {
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(tp - startTime);

    return std::to_string(duration.count());
}

void SaveData(
    const MouseEvent &data,
    const std::chrono::steady_clock::time_point& startTime
) {
    std::ofstream outFile(OUTPUT_FILENAME, std::ios_base::app);

    outFile << FormatTimestamp(data.timestamp, startTime) << ","
            << data.x << ","
            << data.y << ","
            << (data.LeftClick ? 1 : 0) << ","
            << (data.RightClick ? 1 : 0) << std::endl;

    outFile.close();
}

void WriteHeaderIfNeeded() {
    std::ofstream file(OUTPUT_FILENAME, std::ios::ate | std::ios::in);

    if (file.is_open() && file.tellp() == 0) {
        file << "timestamp_ms,x,y,left_click,right_click\n";
    }
}

int main(int argc, char* argv[]) {
    // Check for help flag before initializing SDL
    if (argc > 1 &&
        (std::string(argv[1]) == "--help" ||
         std::string(argv[1]) == "-h")) {

        std::cout << "Mouse Event Detection\n"
                  << "=====================\n\n"
                  << "A simple SDL2 program that tracks mouse movement and "
                     "left/right button events and saves them to a CSV file.\n\n"
                  << "Usage:\n"
                  << "  " << argv[0] << "\n\n"
                  << "Description:\n"
                  << "  Opens a small SDL2 window, detects mouse movement and "
                     "button events,\n"
                  << "  and saves the captured data with timestamps to "
                     "mouse_data.txt.\n\n"
                  << "Options:\n"
                  << "  -h, --help    Show this help message and exit.\n";

        return 0;
    }

    // Record program start time for relative timestamps
    auto startTime = std::chrono::steady_clock::now();

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL Error: "
                  << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Mouse Event Detection",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        200,
        200,
        SDL_WINDOW_SHOWN
    );

    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL Error: "
                  << SDL_GetError() << std::endl;

        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL Error: "
                  << SDL_GetError() << std::endl;

        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_Rect mouseArea = {0, 0, 200, 200};
    SDL_Event event;

    MouseEvent currentEvent = {
        0,
        0,
        false,
        false,
        startTime
    };

    WriteHeaderIfNeeded();

    bool running = true;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }

            auto eventTime = std::chrono::steady_clock::now();

            if (event.type == SDL_MOUSEMOTION) {
                if (event.motion.x >= mouseArea.x &&
                    event.motion.x <= mouseArea.x + mouseArea.w &&
                    event.motion.y >= mouseArea.y &&
                    event.motion.y <= mouseArea.y + mouseArea.h) {

                    currentEvent.x = event.motion.x;
                    currentEvent.y = event.motion.y;
                    currentEvent.timestamp = eventTime;

                    std::cout
                        << "Mouse moved to ("
                        << currentEvent.x << ", "
                        << currentEvent.y << ")"
                        << std::endl;
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

            SaveData(currentEvent, startTime);
        }

        SDL_Delay(1);
    }

    std::cout << "\nData saved to: "
              << OUTPUT_FILENAME << std::endl;

    std::cout << "Columns: timestamp_ms, x, y, left_click, right_click"
              << std::endl;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
