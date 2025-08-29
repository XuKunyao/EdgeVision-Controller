# VSCode Setup Guide for Intelligent Door Lock System

## Project Overview

This Intelligent Door Lock System is a Qt-based application with facial recognition capabilities. The project consists of two main components:

- **Face_Server**: Server-side application for face recognition processing
- **Face_DoorLock**: Client-side application for door lock control

## Prerequisites

### System Requirements
- Ubuntu 20.04+ or similar Linux distribution
- Visual Studio Code
- Qt 5.15+ development tools
- OpenCV 4.6+ libraries
- C++ compiler (GCC recommended)

### Required VSCode Extensions

The following extensions are recommended and configured in `.vscode/extensions.json`:

1. **C/C++** (Microsoft) - Language support and IntelliSense
2. **CMake Tools** (Microsoft) - CMake integration
3. **Qt for Python** (Sean Wu) - Qt framework support
4. **CMake** (twxs) - CMake language support
5. **C/C++ Extension Pack** (Microsoft) - Complete C++ development package

## Installation Steps

### 1. Install Qt Development Tools

```bash
sudo apt update
sudo apt install -y qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools qtcreator build-essential pkg-config
```

### 2. Install OpenCV Libraries

```bash
sudo apt install -y libopencv-dev libopencv-contrib-dev
```

### 3. Verify Installation

```bash
# Check Qt installation
qmake --version

# Check OpenCV installation
pkg-config --modversion opencv4
```

## Project Configuration

### VSCode Configuration Files

The project includes pre-configured VSCode settings:

- **`.vscode/c_cpp_properties.json`**: IntelliSense configuration with Qt and OpenCV paths
- **`.vscode/tasks.json`**: Build tasks for both Face_Server and Face_DoorLock
- **`.vscode/extensions.json`**: Recommended extensions

### Build Variants

The project provides multiple build configurations:

1. **Original .pro files**: Uses relative paths for dependencies
   - `Face_Server.pro` - Updated with relative paths
   - `Face_DoorLock.pro` - Updated with relative paths

2. **Linux-compatible versions**: Uses system OpenCV
   - `Face_Server_linux.pro` - For full SeetaFace builds
   - `Face_DoorLock_linux.pro` - For full SeetaFace builds

3. **Demo versions**: OpenCV only (SeetaFace disabled)
   - `Face_Server_demo.pro` - Working demo without SeetaFace
   - Ready to build and test

## Building the Project

### Using VSCode Tasks

1. Open the project in VSCode
2. Press `Ctrl+Shift+P` to open Command Palette
3. Type "Tasks: Run Task" and select it
4. Choose from available tasks:
   - **Make Face_Server (Demo)** - Build server demo (recommended for testing)
   - **Build Face_Server** - Generate Makefile only
   - **Clean Face_Server** - Clean build files

### Using Terminal

```bash
# Navigate to Face_Server directory
cd Face_Server/Face_Server

# Build demo version (recommended for testing)
qmake Face_Server_demo.pro
make

# Build full version (requires SeetaFace libraries compiled for Linux)
qmake Face_Server_linux.pro
make
```

## Running the Application

### Face_Server Demo
```bash
cd Face_Server/Face_Server
./Face_Server_demo
```

Note: The demo version will start but face recognition features will be disabled with appropriate debug messages.

## Dependencies Status

### ✅ Working Dependencies
- **Qt 5.15**: Fully configured and working
- **OpenCV 4.6**: System libraries properly linked
- **C++ Compilation**: All source files compile successfully

### ⚠️ Partial Dependencies
- **SeetaFace2**: Currently only Windows libraries are available
  - Headers are accessible
  - Libraries need to be compiled for Linux
  - Conditional compilation implemented (`DISABLE_SEETAFACE`)

## SeetaFace Integration

### Current Status
The included SeetaFace libraries are compiled for Windows (`.dll.a` files). To enable full face recognition:

1. **Compile SeetaFace for Linux**:
   ```bash
   cd SeetaFace2
   mkdir build && cd build
   cmake ..
   make -j$(nproc)
   make install
   ```

2. **Update library paths** in `.pro` files to point to Linux-compiled libraries

3. **Use the full versions**:
   - `Face_Server_linux.pro`
   - `Face_DoorLock_linux.pro`

### Model Files
Update model file paths in `qfaceobject.cpp` from Windows paths:
```cpp
// Change from:
seeta::ModelSetting FDmode("E:/Program/Linux/...", seeta::ModelSetting::CPU, 0);

// To Linux paths:
seeta::ModelSetting FDmode("./models/fd_2_00.dat", seeta::ModelSetting::CPU, 0);
```

## Development Workflow

### 1. Code Editing
- IntelliSense provides full code completion for Qt and OpenCV
- Error highlighting for C++ syntax
- Integrated debugging support

### 2. Building
- Use `Ctrl+Shift+P` → "Tasks: Run Build Task" (or `Ctrl+Shift+B`)
- Build output appears in integrated terminal
- Problem matcher highlights errors in Problems panel

### 3. Running
- Use VSCode terminal or external terminal
- GUI applications will open in desktop environment

### 4. Debugging
- Set breakpoints in VSCode
- Use F5 to start debugging (requires launch configuration)

## Troubleshooting

### Common Issues

1. **Qt headers not found**
   ```bash
   # Install Qt development packages
   sudo apt install qtbase5-dev qtbase5-dev-tools
   ```

2. **OpenCV not found**
   ```bash
   # Install OpenCV development libraries
   sudo apt install libopencv-dev libopencv-contrib-dev
   ```

3. **SeetaFace compilation errors**
   - Use demo version (`Face_Server_demo.pro`) to test Qt/OpenCV setup
   - Compile SeetaFace separately for Linux

### Build Flags

The project uses these important flags:
- `DISABLE_SEETAFACE`: Disables SeetaFace functionality for demo builds
- Standard Qt flags: `QT_WIDGETS_LIB`, `QT_GUI_LIB`, etc.
- OpenCV 4 compatibility

## Architecture Notes

### Project Structure
```
├── Face_Server/Face_Server/     # Server application
│   ├── *.cpp, *.h              # Source files
│   ├── *.ui                    # Qt Designer forms
│   ├── *.pro                   # qmake project files
│   └── Face_Server_demo        # Compiled demo executable
├── Face_DoorLock/Face_DoorLock/ # Client application
├── OpenCV_install/             # Pre-compiled OpenCV (Windows)
├── SeetaFace_install/          # Pre-compiled SeetaFace (Windows)
└── .vscode/                    # VSCode configuration
```

### Key Components
- **QFaceObject**: Face recognition interface (conditionally compiled)
- **Face_ServerWin**: Main server window
- **RegisterWin**: Face registration interface
- **SelectWin**: Face selection interface

## Next Steps

1. **Test the demo build** to ensure Qt/OpenCV integration works
2. **Compile SeetaFace for Linux** if facial recognition is required
3. **Configure network settings** for client-server communication
4. **Set up database** for face registration storage
5. **Test on target ARM hardware** with appropriate cross-compilation

## Contributing

When making changes:
1. Update both original and Linux-compatible `.pro` files
2. Maintain conditional compilation for SeetaFace
3. Test builds with both `make` and VSCode tasks
4. Update this documentation for any new dependencies

---

**Note**: This setup demonstrates VSCode compatibility with the Qt/OpenCV portion of the project. Full facial recognition functionality requires compiling SeetaFace libraries for the target Linux environment.