# OpenGLThingy

Me messing around with modern OpenGL. A couple of animated quads and a rotating textured cube, with ImGui controls.

https://github.com/user-attachments/assets/6f912ff5-c5fe-49a4-8147-b25e15b3b1e3

## Build

### Linux

Install the dev packages, then run `./build.sh`:

```bash
# Fedora
sudo dnf install glfw-devel glew-devel mesa-libGL-devel gcc-c++ cmake

# Ubuntu/Debian
sudo apt install libglfw3-dev libglew-dev libgl1-mesa-dev build-essential cmake

# Arch
sudo pacman -S glfw-x11 glew mesa cmake gcc
```

### Windows

GLFW and GLEW are bundled under `Dependencies/`. Run `build.bat` (needs VS 2022 + CMake).

## Run

From the repo root:

```bash
./build/OpenGLThingy            # Linux
build\Debug\OpenGLThingy.exe    # Windows
```

Shaders and textures are loaded with paths relative to the working directory, so run from the repo root (not from inside `build/`).

## License

Do whatever you want with it, it's not like i own anything in this :)
