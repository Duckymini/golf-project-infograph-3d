# Golf Project – 3D Simulation

Project carried out as part of the course **Computer Graphics 3D (CSC_43043_EP)**.  
Author: **Pierre Bernadet**

## Description

This project consists of creating an **interactive 3D golf game**.  
It combines a recent personal interest in golf with several **computer graphics techniques**: procedural terrain generation, real-time rendering, physics simulation, and user interface.

The main objective is to simulate the motion of a golf ball on a generated course, including collision handling, friction, rebounds, and victory conditions.

---

## Environment

- **Textured terrain** with height function based on:
  - combinations of Gaussian hills,
  - Perlin noise attenuated with a cosine smoothing function,
  - smoothstep for smooth transitions.
- **Green** (flat area around the hole).
- **Water surface** animated with Perlin noise and transparency.
- **Skybox** built from a panoramic image mapped on a cube.
- **Vegetation**: grass using billboards, trees imported from `.obj` meshes.
- **Flag** animated with a sinusoidal shader to simulate wind.
- **Hole** represented by a black disk at the center of the green.

---

## Physics Simulation

- **Motion integration** with gravitational acceleration.  
- **Ball-terrain collisions** with position correction and rebounds.  
- **Friction** depending on terrain or green.  
- **Special conditions**:
  - Ball outside terrain → reset.  
  - Ball in hole → win detected.  
- **Shooting interface**:
  - Directional arrow with color gradient according to power.  
  - Controls:  
    - `A` / `D` → adjust horizontal angle  
    - `W` / `S` → adjust vertical angle  
    - `Q` / `E` → change shooting power  
    - `Space` → shoot  

---

## Extensions

- Three clubs available: **7 iron**, **wedge**, **putter** (different speeds and shooting angles).  
- Camera modes: **free view** or **automatic tracking** of the ball.  
- Real-time interface with **ImGui** (shot counter).  
- Congratulatory message displayed when the ball enters the hole.  
- Attempted but not completed: splash effect in water, impact effect on ground.

---

## Demonstration

A demonstration video is available in the repository (managed with **Git LFS** as it exceeds 100 MB).

---

## Build and Run

### Requirements
- OpenGL  
- ImGui  
- Standard C++ libraries (GLFW/GLUT, GLM, stb_image, etc.)  

### Launch the game
```bash
cd golf
make
./golf
