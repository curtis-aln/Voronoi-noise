# Voronoi Noise

> A C++ implementation of the Voronoi noise algorithm, rendered with SFML.

<!-- Replace the line below with your actual screenshot/gif -->
![Voronoi Noise Demo](assets/demo.gif)

---

## Table of Contents

- [About](#about)
- [How It Works](#how-it-works)
- [Preview](#preview)
- [Dependencies](#dependencies)
- [Building](#building)
- [Usage](#usage)
- [Configuration](#configuration)
- [Contributing](#contributing)

---

## About

**Voronoi Noise** is a C++ implementation of the Voronoi noise algorithm — a technique for procedurally generating organic, cell-like patterns. Seed points are scattered randomly across a 2D plane, and each pixel is coloured based on its distance (Euclidean) to the nearest seed point, producing a grayscale noise texture.

This project is primarily a learning resource for understanding procedural generation concepts and how Voronoi diagrams can be used to generate noise textures. It is built with CMake and uses SFML for rendering.

---

## How It Works

1. A set of seed points are scattered at random positions across the canvas.
2. For every pixel, the Euclidean distance to each seed point is calculated.
3. The pixel is shaded based on how close it is to its nearest neighbour — closer pixels appear darker (or lighter, depending on mapping).
4. The result is a smooth, cellular grayscale texture characteristic of Voronoi noise.

<!-- Optionally insert a diagram here -->
<!-- ![How it works](assets/diagram.png) -->

---

## Preview

<!-- Replace these placeholders with your actual images/gifs once uploaded -->

| Output Example | Description |
|---|---|
| ![Screenshot 1](assets/screenshot1.png) | Basic Voronoi noise output |
| ![Animated](assets/demo.gif) | Real-time generation |

---

## Dependencies

| Dependency | Version | Notes |
|---|---|---|
| [CMake](https://cmake.org/) | 3.15+ | Build system |
| [SFML](https://www.sfml-dev.org/) | 2.5+ | Rendering & window management |
| C++ Compiler | C++17 | MSVC (Visual Studio 2019+) recommended on Windows |

---

## Building

### Prerequisites

Make sure you have the following installed before building:

- [CMake 3.15+](https://cmake.org/download/)
- [SFML 2.5+](https://www.sfml-dev.org/download.php) — ensure SFML is findable by CMake (e.g. via `CMAKE_PREFIX_PATH` or installed to a standard location)
- A C++17-compatible compiler (MSVC recommended on Windows)

### Steps

**1. Clone the repository**

```bash
git clone https://github.com/curtis-aln/Voronoi-noise.git
cd Voronoi-noise
```

**2. Create a build directory**

```bash
mkdir build
cd build
```

**3. Configure the project with CMake**

```bash
cmake ..
```

If SFML is not found automatically, provide the path explicitly:

```bash
cmake .. -DCMAKE_PREFIX_PATH="C:/path/to/SFML"
```

**4. Build the project**

```bash
cmake --build . --config Release
```

**5. Run the executable**

The compiled binary will be in the `build/Release/` (or `build/Debug/`) directory depending on your configuration:

```bash
./VoronoiNoise        # Linux/macOS
VoronoiNoise.exe      # Windows
```

> **Note:** Make sure the SFML DLLs (e.g. `sfml-graphics-2.dll`, `sfml-window-2.dll`, etc.) are in the same directory as the executable on Windows, or that SFML is installed to a location on your system `PATH`.

---

## Usage

Once built, simply run the executable. A window will open displaying the generated Voronoi noise texture.

```bash
./VoronoiNoise
```

The noise is generated immediately on launch. Close the window to exit.

---

## Configuration

Parameters are currently hardcoded in the source. To change the output, open the relevant source file in `src/` and modify the constants at the top:

| Parameter | Description |
|---|---|
| `SEED_COUNT` | Number of randomly placed seed points |
| `WINDOW_WIDTH` / `WINDOW_HEIGHT` | Output resolution |
| `SEED` | Random seed for reproducible results |

After changing any parameter, rebuild the project using the steps in the [Building](#building) section.

---

## Contributing

Contributions are welcome! If you'd like to improve the implementation, add new features, or fix a bug:

1. Fork the repository
2. Create a new branch (`git checkout -b feature/your-feature`)
3. Commit your changes (`git commit -m 'Add your feature'`)
4. Push to the branch (`git push origin feature/your-feature`)
5. Open a Pull Request

Please keep PRs focused and include a clear description of what has changed and why.

---

## Acknowledgements

- [Voronoi diagram — Wikipedia](https://en.wikipedia.org/wiki/Voronoi_diagram) — background reading on the algorithm
- [SFML documentation](https://www.sfml-dev.org/documentation/) — rendering library used in this project