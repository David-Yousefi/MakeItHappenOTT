# Make It Happen OTT

A professional multiband OTT (Over-The-Top) compressor plugin built with JUCE.

![Plugin Screenshot](docs/screenshot.png)

## Features

- **3-Band Multiband Processing** with Linkwitz-Riley 4th-order crossover filters
  - Low: <250Hz
  - Mid: 250Hz - 2kHz
  - High: >2kHz

- **Dual Compression per Band**
  - Downward compression (reduces loud signals)
  - Upward compression (boosts quiet signals)

- **Independent Controls per Band**
  - Threshold (Down & Up)
  - Ratio (Down & Up)
  - Attack time
  - Release time
  - Output gain
  - **Stereo Width** (0-200%) - Mid-Side stereo width control per band
  - **Solo** - Listen to individual bands in isolation

- **Global Controls**
  - **Depth** (0-100%) - Wet/dry mix for parallel processing
  - **Time** (0-1000%) - Global time scaling
  - **Gain Match** - Automatic level matching between dry and wet signals
  - Input/Output Gain

- **Custom Artwork Support** - Add your own background images and knob designs

- **Real-time Spectrum Display** - Visual feedback for each frequency band

- **Multiple Formats**
  - VST3
  - Audio Unit (AU)
  - Standalone application

## Installation

### macOS

The plugin will be automatically installed to:
- **AU**: `~/Library/Audio/Plug-Ins/Components/MakeItHappenOTT.component`
- **VST3**: `~/Library/Audio/Plug-Ins/VST3/MakeItHappenOTT.vst3`
- **Standalone**: Available in the build artifacts

### Windows

Copy the `MakeItHappenOTT.vst3` folder to:
- **VST3**: `C:\Program Files\Common Files\VST3\`

For detailed Windows build and installation instructions, see [WINDOWS_BUILD.md](WINDOWS_BUILD.md)

### Building from Source

#### Requirements
- CMake 3.24 or higher
- C++20 compatible compiler
- macOS: Xcode Command Line Tools
- Windows: Visual Studio 2019 or newer
- Linux: GCC 9+ or Clang 10+

#### Build Instructions

```bash
# Clone the repository
git clone https://github.com/yourusername/MakeItHappenOTT.git
cd MakeItHappenOTT

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build (Release mode recommended)
cmake --build . --config Release

# Plugins will be automatically installed to your system
```

## Usage

### Quick Start

1. Load the plugin in your DAW or launch the standalone application
2. Adjust the **Depth** slider to set the amount of OTT processing (0-100%)
3. Fine-tune each frequency band:
   - **Downward compression**: Controls how much loud signals are reduced
   - **Upward compression**: Controls how much quiet signals are boosted
   - **Attack/Release**: Shape the compression response time
   - **Gain**: Final output level for each band

### Tips for Best Results

- **Subtle Processing**: Start with Depth around 30-50% for transparent enhancement
- **Aggressive Processing**: Push Depth to 80-100% for the classic "OTT sound"
- **Attack Times**:
  - Fast (0.1-2ms) for transient preservation
  - Slow (5-20ms) for smoother, pumping effect
- **Release Times**:
  - Short (10-50ms) for tight control
  - Long (100-500ms) for more natural dynamics
- **Per-Band Gains**: Use to rebalance frequency response after compression

### Common Use Cases

- **Vocal Enhancement**: Gentle upward compression on mids, moderate downward on highs
- **Bass Thickening**: Strong compression on lows with fast attack
- **Master Bus Glue**: Light compression across all bands with moderate depth
- **EDM/Electronic**: Aggressive settings with high depth for "pumping" effect

## Customization

You can fully customize the plugin's appearance! See [ARTWORK_GUIDE.md](ARTWORK_GUIDE.md) for details on:
- Creating custom background images
- Designing knob filmstrips
- Installation and setup

## Technical Details

### Signal Flow

```
Input → Crossover Filter → Low Band → Downward/Upward Compression → Gain →
                         → Mid Band → Downward/Upward Compression → Gain → Sum → Depth Mix → Output
                         → High Band → Downward/Upward Compression → Gain →
```

### Crossover Filters
- Type: Linkwitz-Riley 4th order
- Frequencies: 250Hz, 2kHz
- Phase coherent reconstruction (bands sum flat)

### Compression Algorithm
- Envelope follower with adjustable attack/release
- Independent downward and upward compression
- Ratio range: 1:1 to 20:1
- Threshold range: -60dB to 0dB

### Parameters

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Depth | 0-100% | 50% | Wet/dry mix |
| Low/Mid/High Thresh Down | -60 to 0 dB | -20 dB | Downward compression threshold |
| Low/Mid/High Ratio Down | 1:1 to 20:1 | 3:1 | Downward compression ratio |
| Low/Mid/High Thresh Up | -60 to 0 dB | -40 dB | Upward compression threshold |
| Low/Mid/High Ratio Up | 1:1 to 20:1 | 2:1 | Upward compression ratio |
| Low/Mid/High Attack | 0.1-100 ms | 1 ms | Envelope attack time |
| Low/Mid/High Release | 10-1000 ms | 100 ms | Envelope release time |
| Low/Mid/High Gain | -12 to +12 dB | 0 dB | Output gain |

## Project Structure

```
MakeItHappenOTT/
├── CMakeLists.txt           # Build configuration
├── src/
│   ├── PluginProcessor.h    # DSP processing
│   ├── PluginProcessor.cpp
│   ├── PluginEditor.h       # GUI
│   └── PluginEditor.cpp
├── README.md
└── ARTWORK_GUIDE.md
```

## Development

### Code Architecture

- **PluginProcessor**: Handles all audio processing, parameter management, and DSP
- **PluginEditor**: Manages the GUI, custom graphics, and user interaction
- **OTTLookAndFeel**: Custom JUCE LookAndFeel class for styled knobs

### Key Features in Code

- JUCE AudioProcessorValueTreeState for parameter automation
- Envelope followers for smooth compression response
- Custom LookAndFeel with support for filmstrip images
- Optional external asset loading

## License

[Your chosen license here - e.g., MIT, GPL, etc.]

## Credits

- Built with [JUCE Framework](https://juce.com/)
- Developed by [Your Name]
- Inspired by the classic Xfer OTT compressor

## Support

For issues, feature requests, or questions:
- Open an issue on GitHub
- Contact: [your email]

## Changelog

### Version 0.0.1 (Initial Release)
- 3-band multiband compression
- Dual upward/downward compression per band
- Custom artwork support
- VST3, AU, and Standalone formats
- Professional UI with color-coded bands

---

Made with ❤️ using JUCE
