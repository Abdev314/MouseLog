#include <SDL2/SDL.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <cmath>
#include <iomanip>

// Output filenames
constexpr const char* FILENAME = "mouse_data.csv";      // For analysis
constexpr const char* LOG_FILENAME = "mouse_debug.log";     // For human reading

struct MouseEvent {
    int x, y;
    bool LeftClick, RightClick;
    std::chrono::steady_clock::time_point timestamp;
    double velocity_px_s = 0.0;
    int click_duration_ms = 0;
};

std::string FormatTimestamp(const std::chrono::steady_clock::time_point& tp, 
                           const std::chrono::steady_clock::time_point& startTime) {
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(tp - startTime);
    return std::to_string(duration.count());
}

// Save to file
void SaveToCSV(const MouseEvent &data, const std::chrono::steady_clock::time_point& startTime) {
    std::ofstream outFile(FILENAME, std::ios_base::app);
    outFile << FormatTimestamp(data.timestamp, startTime) << ","
            << data.x << ","
            << data.y << ","
            << (data.LeftClick ? 1 : 0) << ","
            << (data.RightClick ? 1 : 0) << ","
            << std::fixed << std::setprecision(1) << data.velocity_px_s << ","
            << data.click_duration_ms << std::endl;
    outFile.close();
}

// Save to human-readable log 
void SaveToLog(const MouseEvent &data, const std::chrono::steady_clock::time_point& startTime) {
    std::ofstream logFile(LOG_FILENAME, std::ios_base::app);
    
    logFile << "[T+" << FormatTimestamp(data.timestamp, startTime) << "ms] "
            << "x: " << data.x << ", "
            << "y: " << data.y << ", "
            << "Left: " << (data.LeftClick ? "▼" : "▲") << ", "
            << "Right: " << (data.RightClick ? "▼" : "▲") << ", "
            << "Speed: " << std::fixed << std::setprecision(1) << data.velocity_px_s << " px/s, "
            << "Hold: " << data.click_duration_ms << "ms"
            << std::endl;
    
    logFile.close();
}

void WriteCSVHeaderIfNeeded() {
    std::ofstream file(FILENAME, std::ios::ate | std::ios::in);
    if (file.is_open() && file.tellp() == 0) {
        file << "timestamp_ms,x,y,left_click,right_click,velocity_px_s,click_duration_ms\n";
    }
}

void WriteLogHeaderIfNeeded() {
    std::ofstream file(LOG_FILENAME, std::ios::ate | std::ios::in);
    if (file.is_open() && file.tellp() == 0) {
        file << "# Mouse Event Log - Human Readable Format\n";
        file << "# Format: [T+XXXms] x: N, y: N, Left: ▼/▲, Right: ▼/▲, Speed: N.N px/s, Hold: Nms\n";
        file << "# ▼ = pressed, ▲ = released\n\n";
    }
}

int main() {
    auto startTime = std::chrono::steady_clock::now();
    
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
    
    MouseEvent currentEvent = {0, 0, false, false, startTime, 0.0, 0};
    MouseEvent lastEvent = currentEvent;
    
    std::chrono::steady_clock::time_point leftDownTime, rightDownTime;
    bool leftPressed = false, rightPressed = false;

    WriteCSVHeaderIfNeeded();
    WriteLogHeaderIfNeeded();
    
    bool running = true;
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }

            auto eventTime = std::chrono::steady_clock::now();

            if (event.type == SDL_MOUSEMOTION) {
                if (event.motion.x >= mouseArea.x && event.motion.x <= mouseArea.x + mouseArea.w &&
                    event.motion.y >= mouseArea.y && event.motion.y <= mouseArea.y + mouseArea.h) {
                    
                    auto dt_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        eventTime - lastEvent.timestamp).count();
                    double dx = event.motion.x - lastEvent.x;
                    double dy = event.motion.y - lastEvent.y;
                    double distance = std::sqrt(dx*dx + dy*dy);
                    double velocity = (dt_ms > 0) ? (distance / dt_ms * 1000.0) : 0.0;
                    
                    currentEvent.x = event.motion.x;
                    currentEvent.y = event.motion.y;
                    currentEvent.timestamp = eventTime;
                    currentEvent.velocity_px_s = velocity;
                    currentEvent.click_duration_ms = 0;
                    
                    // 👁️ Human-readable console output with labels
                    std::cout << "🖱️  [T+" << FormatTimestamp(eventTime, startTime) << "ms] "
                              << "x: " << currentEvent.x << ", y: " << currentEvent.y
                              << " | Speed: " << std::fixed << std::setprecision(1) 
                              << velocity << " px/s" << std::endl;
                }
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                currentEvent.timestamp = eventTime;
                currentEvent.velocity_px_s = lastEvent.velocity_px_s;
                currentEvent.click_duration_ms = 0;
                
                if (event.button.button == SDL_BUTTON_LEFT) {
                    currentEvent.LeftClick = true;
                    leftDownTime = eventTime;
                    leftPressed = true;
                    std::cout << "🔘 [T+" << FormatTimestamp(eventTime, startTime) << "ms] Left: ▼ PRESSED" << std::endl;
                }
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    currentEvent.RightClick = true;
                    rightDownTime = eventTime;
                    rightPressed = true;
                    std::cout << "🔘 [T+" << FormatTimestamp(eventTime, startTime) << "ms] Right: ▼ PRESSED" << std::endl;
                }
            }

            if (event.type == SDL_MOUSEBUTTONUP) {
                currentEvent.timestamp = eventTime;
                currentEvent.velocity_px_s = lastEvent.velocity_px_s;
                
                if (event.button.button == SDL_BUTTON_LEFT) {
                    currentEvent.LeftClick = false;
                    if (leftPressed) {
                        currentEvent.click_duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                            eventTime - leftDownTime).count();
                        leftPressed = false;
                        std::cout << "🔘 [T+" << FormatTimestamp(eventTime, startTime) << "ms] Left: ▲ RELEASED (Hold: " 
                                  << currentEvent.click_duration_ms << "ms)" << std::endl;
                    }
                }
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    currentEvent.RightClick = false;
                    if (rightPressed) {
                        currentEvent.click_duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                            eventTime - rightDownTime).count();
                        rightPressed = false;
                        std::cout << "🔘 [T+" << FormatTimestamp(eventTime, startTime) << "ms] Right: ▲ RELEASED (Hold: " 
                                  << currentEvent.click_duration_ms << "ms)" << std::endl;
                    }
                }
            }

            // Save to both outputs
            SaveToCSV(currentEvent, startTime);
            SaveToLog(currentEvent, startTime);
            
            lastEvent = currentEvent;
        }
        
        SDL_Delay(1);
    }

    // CLEANUP & CONFIRMATION MESSAGE
    std::cout << "\n✅ Session complete!" << std::endl;
    std::cout << "📊 Analysis-ready data: " << FILENAME << std::endl;
    std::cout << "👁️ Human-readable log:  " << LOG_FILENAME << " (labeled format for quick review)" << std::endl;
    std::cout << "\n💡 Pro tips:" << std::endl;
    std::cout << "   • Open " << FILENAME << " in Excel to sort/filter by velocity or click duration" << std::endl;
    std::cout << "   • View " << LOG_FILENAME << " in any text editor to scan events visually" << std::endl;
    std::cout << "   • Look for 'Speed: >1500 px/s' for fast flicks, 'Hold: >300ms' for hesitation" << std::endl;
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}