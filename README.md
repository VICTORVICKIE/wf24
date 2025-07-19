# WF24 - 24-Hour Watchface Application

A beautiful 24-hour watchface application built with LVGL graphics library and SDL2, featuring a cosmic-themed design with manual time controls.

## 🌟 Features

- **24-Hour Clock Display**: Elegant circular clock face with 24-hour format
- **Cosmic Design**: Dark gradient background with gold accents
- **Saturn Needle**: Custom animated needle pointing to current time
- **Manual Time Controls**: Interactive UI for setting custom times
- **Range Mode**: Direct control over needle position (0-144 range)
- **Real-time Updates**: Automatic time synchronization or manual override
- **Keyboard Navigation**: Full keyboard support for UI interaction

## 🎨 Design

The application features a sophisticated cosmic color palette:
- **Deep Space Background**: Radial gradient from indigo-blue to pure black
- **Cosmic Latte**: Warm off-white for text and accents
- **Gold Accents**: Elegant gold borders and highlights
- **Dark Gray**: Subtle UI elements and controls

## 🏗️ Architecture

### Project Structure
```
wf24/
├── src/
│   ├── main.c          # Application entry point
│   ├── ui.c            # UI implementation
│   ├── ui.h            # UI interface
│   └── saturn_v.c      # Saturn needle image data
├── vendor/
│   ├── lvgl/           # LVGL graphics library
│   └── sdl2/           # SDL2 library
├── bin/                # Build output
├── CMakeLists.txt      # Build configuration
└── lv_conf.h           # LVGL configuration
```

### Code Organization

**Clean Architecture:**
- `main.c`: Application initialization and main loop
- `ui.c`: All UI logic and components (static functions for encapsulation)
- `ui.h`: Public interface for UI module
- Modular design with clear separation of concerns

**Key Components:**
- **Clock Dial**: 24-hour scale with 144 ticks (6 ticks per hour)
- **Manual Controls**: Time input, range input, and reset functionality
- **Timer System**: Automatic updates every 10 seconds
- **Event Handling**: Keyboard and mouse input support

## 🚀 Building

### Prerequisites
- CMake 3.12.4 or higher
- C/C++ compiler (GCC, Clang, or MSVC)
- SDL2 library (included in vendor/)

### Build Instructions

```bash
# Clone the repository
git clone <repository-url>
cd wf24

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make

# Run the application
./bin/main
```

### Windows Build
```cmd
# Using build.bat
build.bat

# Or manually
mkdir build
cd build
cmake ..
cmake --build . --config Debug
```

## 🎮 Usage

### Controls
- **Tab/Arrow Keys**: Navigate between UI elements
- **Enter**: Activate buttons
- **Time Input**: Enter time in HH:MM format (00:00 to 23:59)
- **Range Input**: Enter value 0-144 for direct needle control
- **Reset Button**: Return to system time

### Features
1. **System Time Mode**: Automatically displays current system time
2. **Manual Time Mode**: Set custom time using HH:MM format
3. **Range Mode**: Direct control over needle position (0-144)
4. **Reset**: Return to system time and clear manual settings

## ⚙️ Configuration

### UI Constants
The application uses well-defined constants for easy customization:

```c
const int32_t DIMENSION = 466;                    // Display size
const int32_t TOTAL_HOURS = 24;                  // 24-hour format
const int32_t TOTAL_ANGLE = 360;                 // Full circle
const int32_t INTERVAL = 1;                      // Update interval
const int32_t TOTAL_TICKS = 144;                 // Clock ticks
const float ANGLE_PER_TICK = 2.5f;              // Degrees per tick
```

### Color Palette
```c
palette_black = LV_COLOR_MAKE(0, 0, 0);
palette_dark_gray = LV_COLOR_MAKE(24, 24, 24);
palette_gold = LV_COLOR_MAKE(255, 215, 0);
palette_cosmic_latte = LV_COLOR_MAKE(255, 248, 231);
```

## 🔧 Development

### Adding New Features
1. **UI Components**: Add to `ui.c` as static functions
2. **Public Interface**: Declare in `ui.h` if needed externally
3. **Initialization**: Add to `ui_init()` function
4. **Constants**: Add to UI constants section in `ui.c`

### Code Style
- **C99 Standard**: Modern C with clean syntax
- **Static Functions**: Internal functions are static for encapsulation
- **Constants**: UPPERCASE naming for constants
- **Comments**: Clear documentation for complex logic

### Debugging
The application prints UI constants at startup:
```
=== UI CONSTANTS ===
DIMENSION: 466
TOTAL_HOURS: 24
TOTAL_ANGLE: 360
...
==================
```

## 📦 Dependencies

- **LVGL v8.3+**: Graphics library for UI components
- **SDL2**: Cross-platform multimedia library
- **CMake**: Build system
- **C99/C++17**: Programming language standards

## 🎯 Future Enhancements

- [ ] Multiple watchface themes
- [ ] Weather integration
- [ ] Customizable color schemes
- [ ] Animation effects
- [ ] Touch screen support
- [ ] Configuration file support

## 📄 License

[Add your license information here]

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## 📞 Support

For issues and questions, please open an issue on the repository.

---

**Built with ❤️ using LVGL and SDL2**
