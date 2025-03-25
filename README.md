# OpenGL Picasa Demo

A modern, hardware-accelerated image viewer built with OpenGL... naaa-h, it's demo, i'm complete it, maybe..

## Features

- **Hardware-accelerated rendering** using modern OpenGL 3.3
- **Smooth image scaling and rotation** with proper aspect ratio preservation
- **Thumbnail gallery view** for browsing multiple images
- **Interactive UI elements** including buttons for navigation, zoom, and rotation
- **Drag and drop support** for panning images
- **Mouse wheel zooming** for intuitive image inspection
- **Keyboard shortcuts** for navigation and manipulation
- **Support for common image formats** including PNG, JPEG, BMP, and GIF

## Building Instructions

### Prerequisites

To build the project, you need the following dependencies:

```bash
sudo apt-get update
sudo apt-get install -y libglfw3-dev libglew-dev libglm-dev libstb-dev libpng-dev libjpeg-dev cmake build-essential
```

### Compilation

1. **Build the application**:
```bash
cd ~/opengl_picasa_cpp
mkdir -p build
cd build
cmake ..
make
```

## Usage Instructions

### Running the Application

```bash
./picasa [path_to_image_or_folder]
```

If you provide a path to an image, the application will open that image directly. If you provide a path to a folder, it will load all images in that folder and display them in the thumbnail view.

### Keyboard Controls

- **Left/Right Arrow Keys**: Navigate between images
- **Up/Down Arrow Keys**: Rotate image 90 degrees clockwise/counterclockwise
- **Mouse Drag**: Pan the image
- **Mouse Wheel**: Zoom in/out
- **Tab Key**: Toggle between thumbnail view and single image view
- **Space Key**: Reset view (zoom, rotation, position)
- **Escape Key**: Exit application

### Mouse Controls

- **Left-click and drag**: Pan the image
- **Mouse wheel**: Zoom in/out
- **Left-click on thumbnail**: Select and display that image
- **Double-click**: Reset zoom and rotation

### UI Controls

The application includes buttons at the bottom of the screen for:
- Previous/Next image navigation
- Rotation
- Zoom in/out
- Toggling between thumbnail and single image view

## Technical Details

This application is built using:
- OpenGL 3.3 for rendering
- GLFW for window management and input handling
- GLEW for OpenGL extension loading
- GLM for mathematics
- STB libraries for image loading and processing

## License

This project is open source and available under the MIT License.
