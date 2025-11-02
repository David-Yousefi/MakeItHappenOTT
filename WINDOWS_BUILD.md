# Building MakeItHappenOTT for Windows

This guide explains how to build the plugin for Windows, creating a VST3 file that Windows users can install.

## Prerequisites

### On Windows:
1. **Visual Studio 2019 or newer** (Community Edition is free)
   - Download from: https://visualstudio.microsoft.com/downloads/
   - During installation, select "Desktop development with C++"

2. **CMake 3.24 or higher**
   - Download from: https://cmake.org/download/
   - During installation, select "Add CMake to system PATH"

3. **Git** (optional, for cloning the repository)
   - Download from: https://git-scm.com/download/win

## Building on Windows

### Step 1: Open Command Prompt or PowerShell
Navigate to the project directory:
```cmd
cd C:\path\to\MakeItHappenOTT
```

### Step 2: Create Build Directory
```cmd
mkdir build
cd build
```

### Step 3: Generate Visual Studio Project
```cmd
cmake ..
```

### Step 4: Build the Plugin
```cmd
cmake --build . --config Release
```

### Step 5: Find Your Plugin
After building, the VST3 will be located at:
```
build\MakeItHappenOTT_artefacts\VST3\MakeItHappenOTT.vst3
```

The plugin is also automatically installed to:
```
C:\Program Files\Common Files\VST3\MakeItHappenOTT.vst3
```

## Distributing to Windows Users

### What to Send:
Send Windows users the entire `MakeItHappenOTT.vst3` **folder** (not just a single file). VST3 plugins are actually folders containing multiple files.

### Installation Instructions for Windows Users:

1. **Download the plugin** - You'll receive a folder called `MakeItHappenOTT.vst3`

2. **Copy to VST3 folder** - Copy the entire `MakeItHappenOTT.vst3` folder to one of these locations:

   **Option A (System-wide):**
   ```
   C:\Program Files\Common Files\VST3\
   ```

   **Option B (User-specific):**
   ```
   C:\Users\YourUsername\AppData\Local\Programs\Common\VST3\
   ```

3. **Restart your DAW** - Close and reopen your DAW (Cubase, FL Studio, Ableton, etc.)

4. **Scan for plugins** - Most DAWs will automatically detect the new plugin. If not, manually scan your VST3 folder.

5. **Load the plugin** - Look for "MakeItHappenOTT" in your plugin list under Effects

## Creating a ZIP for Distribution

To make it easy for Windows users:

1. Navigate to: `build\MakeItHappenOTT_artefacts\VST3\`
2. Right-click on `MakeItHappenOTT.vst3` folder
3. Select "Send to > Compressed (zipped) folder"
4. Send the resulting `MakeItHappenOTT.vst3.zip` file to Windows users

## Cross-Platform Building (from Mac)

Unfortunately, you cannot directly build Windows binaries from macOS. You need to:

1. **Use a Windows machine** or **Windows VM** (Parallels, VMware, VirtualBox)
2. **Use a cloud build service** like GitHub Actions
3. **Ask a Windows user** to build it for you

### Option: GitHub Actions (Automated)
You can set up GitHub Actions to automatically build for Windows when you push code. This requires creating a `.github/workflows/build.yml` file in your repository.

## Troubleshooting

### Build Error: "CMake not found"
- Reinstall CMake and make sure "Add to PATH" is checked

### Build Error: "MSVC not found"
- Install Visual Studio with C++ development tools

### Plugin Not Showing in DAW
- Make sure you copied the entire `MakeItHappenOTT.vst3` **folder**, not just files inside it
- Check that your DAW supports VST3 plugins
- Try rescanning plugins in your DAW

### Missing DLL Errors
- If users get "MSVCP140.dll" or similar errors, they need to install:
  - Visual C++ Redistributable: https://aka.ms/vs/17/release/vc_redist.x64.exe

## Support

For build issues or questions, please open an issue on the GitHub repository.
