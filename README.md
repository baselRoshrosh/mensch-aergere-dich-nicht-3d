# Mensch ärgere dich nicht – 3D Simulation (OpenGL + C++)

This project is a 3D simulation of the classic German board game *Mensch ärgere dich nicht*.  
It was developed as part of the Computer Graphics course at Humboldt University of Berlin.

The application uses modern OpenGL with shaders, transformations, and texture mapping to render the board, background, and player pieces in 3D.

## Features

- 3D-rendered game board and pieces
- Background rendering using shaders and texture mapping
- Basic camera and projection setup
- MVP (Model-View-Projection) transformations
- Mipmapping for texture quality improvement
- Modular structure with clean separation of concerns

## Project Structure

- `src/` – C++ source files (Board, Figur, main program, shader utilities)
- `shaders/` – GLSL shader programs
- `textures/` – Textures for board and background
- `include/` – Header files for GLAD and stb_image
- `lib/` – GLAD library source
- `CMakeLists.txt` – CMake configuration file
- `Mensch_aergere_dich_nicht.pdf` – Project presentation and documentation

## How to Build and Run

Requirements:
- OpenGL 3.3 or higher
- CMake
- GLFW
- GLAD
- stb_image

### Build Steps (Linux/macOS/Windows with CMake)

```bash
mkdir build
cd build
cmake ..
make
./GLSample
```

## Controls

- The simulation currently runs automatically without real user gameplay.
- Figures are placed statically, and textures are rendered on the board and background.

## Notes
- Relative paths are used for shaders and textures.
- All external libraries (GLAD, stb_image) are included locally in the project.
- This project demonstrates computer graphics concepts but does not implement full game logic (dice rolling, player turns, etc.).

## License

This project was created as part of a university course and is intended for educational and demonstration purposes.

## Screenshots

![Screenshot 2025-04-26 020352](https://github.com/user-attachments/assets/d23b2497-558f-4cdb-9d43-69e31dbf953a)

![Screenshot 2025-04-26 020411](https://github.com/user-attachments/assets/b3c723b4-4333-4404-84e6-e1d24c6f2482)
