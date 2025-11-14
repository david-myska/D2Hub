# D2Hub

[![Buy Me a Coffee](https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png)](https://buymeacoffee.com/davidmyska)
[![License](https://img.shields.io/badge/License-CC%20BY--NC%204.0-lightgrey.svg)](https://creativecommons.org/licenses/by-nc/4.0/)


## Table of Contents

- [For Users](#for-users)
  - [Features](#features)
  - [Important Notice](#important-notice)
  - [Getting Started](#getting-started)
- [For Developers](#for-developers)
  - [Requirements](#requirements)
  - [Building](#building)
  - [Code Structure](#code-structure)
- [License](#license)
- [Why Godot for the UI?](#why-godot-for-the-ui)

## For Users

**D2Hub** is an external tool that adds advanced features to the Diablo II mod [MedianXL](https://www.median-xl.com/):

- Advanced **loot filtering** (based on attributes)
- **Achievements**
- Gameplay **statistics** (Damage meter, experience & item tracking)
- Game window **overlay**
- ...and more

### Features

- **Non-intrusive:** Does not modify the game or its files.
  - With little changes usable for other mods or vanilla Diablo II.
- **External tool:** Works by reading the game’s memory.
  - Requires **admin** rights
  - Does **not** run as part of the game's process.
  - In case of issues, your running game will **not** be affected
- **Singleplayer Focus:** Designed for singleplayer, LAN is supported with limited functionality.
- **Windows Only:** D2Hub is supported only on Windows.

### Important Notice

> [!WARNING]
> **Antivirus**  
>  - This tool might trigger your antivirus. This is a **false positive**.
>  - Reading process memory (main principle of this tool) is behavioral pattern common in viruses.

> [!CAUTION]
> **Multiplayer & Anticheat**  
>  - Use **only in singleplayer or LAN**.  
>  - Do **not** use in online multiplayer—accessing the game’s process can trigger anticheat systems resulting in **PERMANENT BAN**.  
>  - The tool does **not** attempt to hide its activity and is easily detectable.

### Getting Started

1. Download the latest release from the [Releases](#) page.
2. Extract & start D2Hub.
3. Go through the initial setup.
4. Start MedianXL.
5. Attach D2Hub to the game process.
   - Either manually or via auto-attach setting.
6. Setup Overlay from Settings tab.
   - Overlay works only when the game is in windowed mode. (default MedianXL fullscreen works fine)
   - Highly dependent on your chosen graphics driver. (default MedianXL setting works fine.
7. Enjoy!

> [!NOTE]
> Overlay is visible only when the game window is in focus.

**Missing:**  
- Screenshots or UI examples

---

## For Developers

### Requirements

- CMake 3.16+
- [Godot](https://godotengine.org/) (for UI integration)
  - minimal version 4.5
  - 32bit (required since it is accessing memory of Diablo 2 which is 32bit)
- C++20 compatible compiler
- Python 3.4+ (required by godot-cpp for binding generation)

Following dependencies are fetched automatically by CMake during the build process:
- [godot-cpp](https://github.com/godotengine/godot-cpp)
- [spdlog](https://github.com/gabime/spdlog)
- [GameExtSuite](https://github.com/david-myska/GameExtSuite)
- [ProcessMemoryAccess](https://github.com/david-myska/ProcessMemoryAccess)

### Building

1. Clone this repository.
   ```
   git clone https://github.com/david-myska/D2Hub.git
   ```
2. Run CMake to configure the backend:
   
   ```bash
   # Enter the project directory
   cd D2Hub
   
   # Choose one preset:
   #   Debug32, Release32

   cmake --preset <preset>
   cmake --build build/<preset>
   cmake --install build/<preset>
   ```
   
   - This builds the C++ backend plugin and deploys it to folder where frontend expects it.
3. Build the frontend using the Godot editor (manual process for now).

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

Godot was chosen for the tool’s UI because I wanted to experiment with building a Godot C++ plugin and I like Godot’s powerful UI capabilities. I am aware this choice is pretty unconventional.

---
