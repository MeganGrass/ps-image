# PS-Image
Multi-purpose utility application for Sony PlayStation (PS1/PSone) images

## Supported File Formats
- Sony PlayStation Texture Image (*.TIM) *(4bpp, 8bpp and 16bpp supported)*
- Sony PlayStation Bitstream (*.BS) *(Version 1/2 supported)*

## Accessibility Features
- DPI Awareness
- Image Scaling *(1x-128x)*
- TrueType Font support *(8px-256px)*
- Borderless fullscreen mode/Resizable window
- Adjustable window color
- Basic keyboard and mouse input support
- Detailed windows with informative tooltips
- Pixel coordinates on mouse hover
- Modern Open/Save modals
- Toolbar *(interactive 64x64 icons created from modifiable PNG)*
- Statusbar *(with basic texture information)*
- Configuration loaded/saved on boot/exit

## Basic Features
##### **File Open + Drag-n-Drop**
- Automatically scan any file for PlayStation Texture Images
##### **File Browser**
- Intuitive file browser with extensive right-click features
##### **File Conversion**
- Convert images to Bitmap Graphic (*.BMP) format
- Color depth is retained in conversion

## Advanced Features
##### **File Creator**
- Create PlayStation Texture Images (with options to load raw pixel+palette data from any location in any file type on creation)
##### **File Modifier**
- Extract/Replace any PlayStation Texture Image in any file type
##### **Raw Data**
- Extract/Replace raw pixel+palette data in PlayStation Texture Images
##### **Transparency**
- Enable/Disable preview of color transparency
- Preview transparent pixels as any color (default: magic pink)
- 4bpp, 8bpp and 16bpp modes supported
##### **Dither**
- Preview textures as they would appear on a real Sony PlayStation (1994) with a built-in [Dithering Filter](https://en.wikipedia.org/wiki/Dither) (optional)
- All depth modes supported

## Texture Modification
##### **Color Lookup Table (CLUT)**
- Modify and preview any palette in real-time
- Add, Insert, Delete, Move, Copy and Paste entire palettes
- Palettes are [15-bit](https://en.wikipedia.org/wiki/High_color) high color [indexes](https://en.wikipedia.org/wiki/Indexed_color) with truecolor [transparency](https://en.wikipedia.org/wiki/Palette_(computing)#Transparency_in_palettes) (superblack pixel) support
- 4bpp *(16 color palette)* and  8bpp *(256 color palette)* modes supported

## Bitstream Images
Support for Bitstream images is extremely limited, with only decoding support being reserved for future features

Any resolution supported ([640x480](https://en.wikipedia.org/wiki/PlayStation_technical_specifications#:~:text=Resolutions) max)

Bitstreams images are decoded to 32bpp

## Keyboard/Mouse
| Option | Description                       |
| :-------- | :-------------------------------- |
| `CTRL+O` | Open (any file type) |
| `CTRL+R` | Open Raw Sony PlayStation Texture Image data |
| `CTRL+N` | Create New Sony PlayStation Texture Image (*.TIM) |
| `CTRL+S` | Save Sony PlayStation Texture Image (*.TIM) |
| `DOWN/UP` | View previous/next texture (when file count > 1) |
| `LEFT/RIGHT` | View previous/next color lookup table (color palette) |
| `CTRL+MOUSEWHEEL` | Adjust image zoom |
| `F11` | Enter/Exit fullscreen mode |
| `CTRL+?` | About this application |
| `ESC` | Exit Application |

## Images
![General Usage](/images/ps-image_00.jpg?raw=true "General Usage")
![Dithering Off, image scaled to 65.5x, mouse cursor hovering pixel (21, 5)](/images/ps-image_02.jpg?raw=true "Dithering Off, image scaled to 65.5x, mouse cursor hovering pixel (21, 5)")
![Dithering On, image scaled to 65.5x, mouse cursor hovering pixel (21, 6)](/images/ps-image_01.jpg?raw=true "Dithering On, image scaled to 65.5x, mouse cursor hovering pixel (21, 6)")
![Open RAW data prompt](/images/ps-image_03.jpg?raw=true "Open RAW data prompt")

## TODO
- General code cleanup, optimization, commenting, etc.
- Pixel editor
- Blit functions
- VRAM features
- File import (*.BMP, *.PNG, etc.)
- (?) Sony PlayStation Bitstream (*.BS) creation
- (?) Sony PlayStation Texture Model (*.TMD) support

## Build
- [Microsoft Visual Studio](https://visualstudio.microsoft.com) 2022+
- [Microsoft Direct-X](https://www.nuget.org/packages/Microsoft.DXSDK.D3DX)
- Clone the repo, install *Microsoft.DXSDK.D3DX* nuget package into the project and build
```
git clone https://github.com/MeganGrass/ps-image.git
```

## Acknowledgements
[Bitstream Decoder](https://github.com/XProger/OpenResident)

[Dither Matrix](https://psx-spx.consoledev.net/graphicsprocessingunitgpu/#24bit-rgb-to-15bit-rgb-dithering-enabled-in-texpage-attribute)


## Copyrights
Sony PlayStation is Copyright (©) [Sony Computer Entertainment Inc.](https://sonyinteractive.com/)

[ImGui](https://github.com/ocornut/imgui) is Copyright (©) 2014-2025 Omar Cornut

[Direct-X](https://learn.microsoft.com/en-us/windows/win32/directx) is Copyright (©) 1995-2021 Microsoft Corporation

## Changelog
Version 1.0.0.0 (January 1, 2025)
- Initial release
