# 🖱️ Mouse Event Detection — C++ SDL2 Project

> 🧠 *A C++ command-line tool that tracks mouse events like movement, clicks, and more using SDL2.*  

![C++](https://img.shields.io/badge/Language-C%2B%2B-blue?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows%20%7C%20macOS-blue?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Active-brightgreen?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)

---

## 🌟 Overview

The **Mouse Event Detection** project is a **C++ command-line application** that tracks mouse events using the SDL2 library.  
It detects mouse movements and button presses (left and right), and saves the event data (coordinates and button statuses) into a text file (`mouse_data.txt`).  
This project is perfect for **C++ learners** exploring:
- 🧩 Mouse event handling with SDL2
- 🧠 Event-driven programming
- 🖥️ Cross-platform C++ applications
- 📂 File I/O operations

---

## 🧩 Features

✅ Tracks **mouse motion** inside a defined window area  
✅ Detects **left** and **right mouse button** presses and releases  
✅ Saves mouse event data (coordinates & button states) to a file  
✅ Handles **SDL2** initialization and error handling  
✅ **Cross-platform**: Works on **Linux**, **macOS**, and **Windows**  
✅ 100% **pure C++** with no external dependencies other than SDL2

---

## 🖥️ How It Works

1. The program initializes SDL2 and creates a window of **200x200** pixels.  
2. It listens for **mouse events** like movement and button clicks.  
3. On each event, it saves the **mouse position** and **button state** (left/right) to a file.  
4. The program runs in a loop until the window is closed.


---

## 🚀 How to Run

### 1. Install SDL2

#### On Linux (Ubuntu/Debian-based systems):
```bash
sudo apt-get install libsdl2-dev
```
#### On macOS (using Homebrew):
```bash
brew install sdl2
```
### On Windows:

>>Download the SDL2 development libraries from the SDL2 website

>>Follow the instructions for setting up SDL2 in your C++ IDE (e.g., Visual Studio or MinGW).


---

### 2.Compile the Code

### On Linux/macOS:
```bash
g++ -o MouseEventDetector main.cpp -lSDL2
```

---- 

### 3. Run the Program

### On Linux/macOS:
```bash
./MouseEventDetector
```
### On Windows:

Run MouseEventDetector.exe from your IDE or the command prompt.






### 🔮 Possible Future Improvements

This project can be expanded with more features, such as:

✅Customizable Mouse Area: Allowing users to define the mouse tracking area inside the window.

✅Gesture Recognition: Recognizing mouse gestures like dragging and zooming.

✅Data Analytics: Visualizing the tracked mouse data in real-time.

✅Game Integration: Using mouse events for games or simulations.



### 📝 License

This project is open-source and licensed under the **MIT License**. See the [LICENSE](LICENSE) file for more details.



#SDL2
#C++
#MouseTracking
#InputEvents
#OpenSource


