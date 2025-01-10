# game-framework
A work in progress game engine/framework.

### Tested with: 
    - -std=c++23
    - Compiler: g++
    - Version: g++.exe (Rev2, Built by MSYS2 project) 14.2.0
    - To use the prebuilt libs you have to use the ucrt MSYS2 mingw build for g++ 14.2.0

### [SFML](https://www.sfml-dev.org/index.php)
    - Version 3.0.0

### [TGUI](https://tgui.eu/)
    - Version: 1.7

### [Box2D](https://box2d.org/)
    - Version: 2.4

### [thread pool](https://github.com/bshoshany/thread-pool?tab=readme-ov-file#installing-the-library)
    - Version: 4.1.0

### [cpp-Utilities](https://github.com/finjosh/cpp-Utilities)

### [cpp-Networking-Library](https://github.com/finjosh/cpp-Networking-Library)

# Class breakdown
  - This excludes Managers

| File | Brief Description  |
| --- | --- |
| `Vector2.hpp` | Vector2 class with some helper functions and conversion functions to and from tgui, sfml, and box2d vectors |
| `Rotation.hpp` | Rotation class with some helper functions and conversion functions to and from box2d Rotations |
| `Color.hpp` | Color class with some helper functions and conversion functions to and from sfml and tgui colors |
| `Transform.hpp` | 2D Transform class with some helper functions and conversion functions to and from box2d transforms |
| `EngineSettings.hpp` | Variable definitions for the engine |
| `Object.hpp` | The base object class storing information every object has (Transform, id, enabled, and children) |
| `UpdateInterface.hpp` | An interface that can be derived from to implement update functions |
| `WindowHandler.hpp` | A simple wrapper for a SFML window with a few helper functions |
| `Input.hpp` | Easy to use implementation for simple input handling. Has basic functionality with just sf::Keyboard::Key and sf::Mouse::Button. Also implements Input::Action(s) which are compared by name. Input::Action::Event are how actions change state, each Action can have multiple events which can also have multiple keys/buttons |
| `DrawableObject.hpp` | An interface for drawable objects which can be derived from to implement drawing |
| `Renderer.hpp` | A object implementation of the DrawableObject interface for SFML shapes |
| `Camera.hpp` | A camera object for drawing to the screen. Cameras can be layered (this does not affect what is drawn to the camera) |
| `Canvas.hpp` | A object class used for creating UI in screen ~~and global~~ space. Currently only functional in screen space (until required nothing will be done) |
| `Collider.hpp` | A collider object class that can be derived from or added as a child to an object (although not set up properly yet). It also stores the Contact/Collision Data and PreSolve Contact/Collision Data classes |
| `Fixture.hpp` | A simple wrapper around the box2d b2Fixture class that links in with the Collider class |
| `WorldHandler.hpp` | A simple wrapper than handles the box2d world |
| `Joint.hpp` | Currently not implemented (until required nothing will be done) |
| `NetworkObject.hpp` | A very simple implementation of objects which will be used in a multiplayer game (Not finished) |
| `NetworkTypes.hpp` | Used for initializing network types so that they can be created over the network (Not finished) |
| `SettingBase.hpp` | The base class for a setting |
| `Settings.hpp` | A Settings "Manager" which handles the creation and management of settings |
| `SettingsUI.hpp` | Derived from Settings.hpp and is an object which creates UI for the settings that are added to it |
| Setting Classes | setting types that are derived from SettingBase and implemented in SettingsUI |
