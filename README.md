# D2Hub

## For Users

**D2Hub** is an external tool that adds advanced features to the Diablo II mod [MedianXL](https://www.median-xl.com/):

- Advanced loot filtering (based on attributes)
- Achievements
- Gameplay statistics (Damage meter, experience & item tracking)
- Game window overlay
- ...and more

### Features

- **Non-intrusive:** Does not modify the game or its files.
  - With little changes usable for other mods or vanilla Diablo II.
- **External tool:** Works by reading the game’s memory.
- **Singleplayer Focus:** Designed for singleplayer and LAN play (with limited functionality).
- **Windows Only:** D2Hub is supported only on Windows.

### Important Notice

- **Antivirus Warning:**  
  Because this tool reads the game’s memory, some antivirus programs may flag it as suspicious. This is a false positive.
- **Multiplayer & Anticheat Warning:**  
  - Use **only in singleplayer or LAN**.  
  - Do **not** use in online multiplayer—accessing the game’s process can trigger anticheat systems resulting in **PERMANENT BAN**.  
  - The tool does **not** attempt to hide its activity and is easily detectable.

### Getting Started

1. Download the latest release from the [Releases](#) page.
2. Start MedianXL & D2Hub (*in any order*).
3. Go through the initial setup in D2Hub.
4. Attach D2Hub to the game process.
  - Either manually or via auto-attach setting.
5. Setup Overlay
  - Overlay works only when the game is in windowed mode. (default MedianXL fullscreen works fine)
  - Highly dependent on your chosen graphics driver. (default MedianXL setting works fine)

**Missing:**  
- Screenshots or UI examples

---

## For Developers

### Requirements

- CMake 3.16+
- [Godot](https://godotengine.org/) (for UI integration)
  - recomended version 4.4.1
- C++20 compatible compiler

Following dependencies are fetched automatically by CMake during the build process:
- [godot-cpp](https://github.com/godotengine/godot-cpp)
- [spdlog](https://github.com/gabime/spdlog)
- [GameEnhancer](https://github.com/david-myska/GameEnhancer)
- [ProcessMemoryAccess TODO](https://github.com/david-myska/GameEnhancer)

### Building

1. Clone this repository and its dependencies.
2. Run CMake to configure the backend:
   
   cmake -S . -B build
   cmake --build build
   
  - This builds the C++ backend plugin and deploys it to folder where frontend expects it.
3. Build the frontend using the Godot editor (manual process for now).

**Missing:**  
- for Diablo II build as 32bit!!!

### Code Structure

- **`src/backend`**  
  Contains all C++ code for the plugin used by the Godot UI.  
  - Output: `d2hub_backend.dll`  
  - Built using CMake and a C++20 compatible compiler.

- **`src/frontend`**  
  Contains Godot code (scenes, GDScript) that defines and controls the UI.  
  - Output: `D2Hub.exe`  
  - Built using the Godot editor (manual build; not yet automated via script or CMake).

---

## License

This project is licensed under the **Creative Commons Attribution-NonCommercial 4.0 International (CC BY-NC 4.0)** license.

- You may use, modify, and share this code freely for non-commercial purposes.
- **Attribution required:** If you use this code, you must credit this repository and provide a link to it in your code or software.
- Commercial use is **not permitted**.

See the [CC BY-NC 4.0 summary](https://creativecommons.org/licenses/by-nc/4.0/) and [full legal code](https://creativecommons.org/licenses/by-nc/4.0/legalcode) for details.

---


## Why Godot for the UI?

Godot was chosen for the tool’s UI because I wanted to experiment with building a Godot C++ plugin and I appreciate Godot’s powerful UI capabilities. While this is unconventional for external game tools, it allowed for rapid development and a flexible user interface.

---
