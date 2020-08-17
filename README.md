# opengl-view-projection-transformations

## description
- A simple implementation of viewing and projection transformations in opengl.

## roadmap
- Problems in `ps.md` are solved.

## code
- `ps.md` contains the problem statement.
- `data/` contains .obj, .mtl, texture files ...
- `ext/` contains glfw library.
- `src/` contains all the source code.
- `github/` contains demonstrations.

## documentation
- The documentation for the code is itself.

## usage

### how to run? [linux]
- Assumes some opengl implementation is installed.
- Open a terminal at project root (the directory containing this file).
- Use the following for compilation which produces executable `HW2c`.
  - `mkdir build`
  - `cd build`
  - `cmake ..`
  - `make`
- Use `./HW2c` to run.

### controls
- `Up` `Down` translate along/opposite the camera direction respectively.
- `Left` `Right` to rotate camera left/right respectively about the vertical.
- Resizing window does not distort the image but changes the field of view.

## demonstration
Implementation is illustrated in the GIF below.
