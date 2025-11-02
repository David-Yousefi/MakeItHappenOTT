# MakeItHappenOTT - Custom Artwork Guide

This guide explains how to customize the visual appearance of your MakeItHappenOTT plugin with custom artwork.

## Overview

The plugin supports custom artwork including:
- Background images
- Knob filmstrips (animated knob graphics)

## Directory Structure

Create an `assets` folder in the same directory as your plugin:

```
MakeItHappenOTT.app/
├── Contents/
│   └── MacOS/
│       ├── MakeItHappenOTT
│       └── assets/              <- Create this folder
│           ├── background.png   <- Your background image
│           └── knob.png         <- Your knob filmstrip
```

## Background Image

### Specifications
- **Filename**: `background.png`
- **Recommended size**: 900×470 pixels
- **Format**: PNG (supports transparency)

### Tips
- Design your background to match the plugin's 900×470 window size
- Leave space for knobs and text elements
- Use dark backgrounds for better contrast with white text
- Consider adding visual sections for the three bands (Low, Mid, High)

## Knob Filmstrip

### Specifications
- **Filename**: `knob.png`
- **Format**: Vertical filmstrip (all frames stacked vertically)
- **Number of frames**: 100 (configurable in code)
- **Recommended size**: 64×6400 pixels (64px wide × 64px per frame × 100 frames)

### Creating a Filmstrip

A filmstrip is a single vertical image containing multiple frames of your knob at different rotation positions:

```
Frame 0   (min value)  ┐
Frame 1                │
Frame 2                │
...                    │  All frames in one
Frame 98               │  vertical image
Frame 99  (max value)  ┘
```

### Tips for Knob Design
- Frame 0 should show the knob at minimum position (typically 7 o'clock)
- Frame 99 should show the knob at maximum position (typically 5 o'clock)
- Each frame should smoothly transition to the next
- Use anti-aliasing for smooth edges
- Consider adding a pointer or indicator line on the knob
- Match the style to your background image

### Tools for Creating Filmstrips
- **Photoshop**: Use actions to rotate and export each frame
- **After Effects**: Create rotation animation and export as image sequence, then combine
- **GIMP**: Use script-fu to generate rotated versions
- **KnobMan** (Windows): Free tool specifically for creating knob filmstrips
- **3D Software** (Blender, Cinema 4D): Render realistic 3D knobs

## Example Workflow

1. **Design your background** (900×470px)
   - Create sections for Low, Mid, High bands
   - Add labels, lines, and decorative elements
   - Export as `background.png`

2. **Create your knob filmstrip**
   - Design a single knob (64×64px recommended)
   - Rotate it 100 times from min to max position
   - Stack all frames vertically
   - Export as `knob.png` (64×6400px)

3. **Install the artwork**
   ```bash
   # For macOS standalone:
   mkdir -p "MakeItHappenOTT.app/Contents/MacOS/assets"
   cp background.png "MakeItHappenOTT.app/Contents/MacOS/assets/"
   cp knob.png "MakeItHappenOTT.app/Contents/MacOS/assets/"
   ```

4. **Launch the plugin** - Your artwork will be loaded automatically!

## Advanced Customization

### Changing the Number of Frames

If you want to use a different number of frames (e.g., 64 instead of 100), edit `src/PluginEditor.cpp`:

```cpp
ottLookAndFeel.setKnobImage(knobImage, 64); // Change 100 to 64
```

Then rebuild the plugin.

### Different Knobs per Band

To use different colored or styled knobs for each band, you would need to:
1. Create separate LookAndFeel classes for each band
2. Load different filmstrips for each
3. Apply them to the respective sliders

This requires code modification but provides maximum customization.

## Color-Coded Knobs (Default)

If you don't provide custom artwork, the plugin uses procedurally-drawn knobs with color-coding:
- **Low band**: Cyan (`#00D4FF`)
- **Mid band**: Green (`#00FF88`)
- **High band**: Orange (`#FF6600`)
- **Depth**: Blue (`#4A9EFF`)

## Troubleshooting

**Artwork not loading?**
- Check that the `assets` folder is in the correct location
- Verify filenames are exactly `background.png` and `knob.png` (case-sensitive on macOS/Linux)
- Ensure images are valid PNG files
- Check image dimensions match specifications

**Knob animation looks jumpy?**
- Increase the number of frames in your filmstrip
- Ensure each frame has smooth incremental rotation

**Background looks stretched?**
- Create background at exact plugin size (900×470px)
- Use `fillDestination` placement maintains aspect ratio

## Resources

- [KnobGallery](http://www.g200kg.com/en/webknobman/gallery.php) - Free knob designs
- [Plugin Boutique Freebies](https://www.pluginboutique.com/articles/tag/Freebies) - Sometimes includes GUI resources
- Create your own using Photoshop, GIMP, or Blender

---

Happy designing! 🎨
