# PS-Image
Multi-purpose utility application for Sony PlayStation (PS1/PSone) textures and images

## Supported File Formats
- Sony PlayStation Texture Image (*.TIM) *(4bpp, 8bpp, 16bpp and 24bpp)*
- Sony PlayStation Bitstream (*.BS) *(Version 1/2)*

## Accessibility Features
- DPI Awareness
- Image Scaling *(1x-128x)*
- TrueType Font support *(8px-256px)*
- Borderless fullscreen mode/Resizable window
- Basic keyboard and mouse input support
- Modern Open/Save modals
- Toolbar *(interactive 64x64 icons created from modifiable PNG)* (optional)
- Statusbar *(with basic texture information)* (optional)
- Configuration loaded/saved on boot/exit

## Basic Features
##### **File Open Modal + Drag-n-Drop + Commandline**
- Automatically scan any file for PlayStation Texture Images
##### **File Conversion**
- Convert images to Bitmap Graphic (*.BMP) format
- Color depth is retained in conversion

## Advanced Features
##### **File Creation**
- Create and save new PlayStation Texture Images
- Import/Export pixel and/or palette data to/from any file (RAW)
- Import/Export pixel and/or palette data to/from PlayStation Texture Images
##### **File Modifier**
- Replace/Extract PlayStation Texture Images to/from any file type
##### **Semi/Transparency**
- Preview any PlayStation Texture Image with basic semi/fully transparency effects (optional)
- Preview any color separate from palette as semi/fully transparent (CLUT not required)
- Superblack support, where solid black pixels are fully transparent
- Superimposed support, where colors at palette index 0 are fully transparent (CLUT required)
- Transparency Control Bit (STP) support for Half (50%back + 50%texture), Full (100%back + 100%texture), Inverse (100%back - 100%texture) and Quarter (100%back + 25%texture) modes
- All bit depth modes supported (4bpp, 8bpp, 16bpp and 24bpp)
##### **Dither**
- Preview textures as they would appear on a real Sony PlayStation (1994) with a built-in [Dithering Filter](https://en.wikipedia.org/wiki/Dither) (optional)
##### **Filter**
- Preview textures with Point, Linear, Anisotropic or no filtering (default)

## Texture Modification
##### **Color Lookup Table (CLUT)**
- Modify and preview any palette in real-time
- Add, Insert, Delete, Move, Copy and Paste entire palettes
- Palettes are [15-bit](https://en.wikipedia.org/wiki/High_color) high color [indexes](https://en.wikipedia.org/wiki/Indexed_color) with [transparency](https://en.wikipedia.org/wiki/Palette_(computing)#Transparency_in_palettes) support
- 4bpp *(16 color palette)* and  8bpp/16bpp/24bpp *(256 color palette)* modes supported
- Microsoft RIFF Palette (*.PAL) support

## Bitstream Images
Support for Bitstream images is extremely limited, with only decoding support being reserved for future features

Any resolution supported ([640x480](https://en.wikipedia.org/wiki/PlayStation_technical_specifications#:~:text=Resolutions) max)

Bitstreams images are decoded to 32bpp raster data

## VRAM
Preliminary support for modifying both CLUT and Pixel coordinates is available (_"File"_ window right-click for pixels, _"Palette"_ window for palette)

Preview is not available

## Keyboard/Mouse
| Option | Description                       |
| :-------- | :-------------------------------- |
| `CTRL+O` | Open (any file type) |
| `CTRL+N` | Create Sony PlayStation Texture Image (*.TIM) |
| `CTRL+S` | Save Sony PlayStation Texture Image (*.TIM) |
| `DOWN/UP` | View previous/next texture (when file count > 1) |
| `LEFT/RIGHT` | View previous/next color lookup table (palette) |
| `CTRL+MOUSEWHEEL` | Adjust image zoom |
| `F11` | Enter/Exit fullscreen mode |
| `CTRL+?` | About this application |
| `ESC` | Exit Application |

## Commandline
| Option | Description                       |
| :-------- | :-------------------------------- |
| `-file "%s"` | Open (any file type) |

## Images
![General Usage](/images/ps-image_00.jpg?raw=true "General Usage")
![Image scaled to 64x, Dithering Off, Superblack pixels fully transparent, mouse cursor hovering pixel (21, 5)](/images/ps-image_01.jpg?raw=true "Image scaled to 64x, Dithering Off, Superblack pixels fully transparent, mouse cursor hovering pixel (21, 5)")
![Image scaled to 64x, Dithering On, Superblack pixels fully transparent, mouse cursor hovering pixel (21, 6)](/images/ps-image_02.jpg?raw=true "Image scaled to 64x, Dithering On, Superblack pixels fully transparent, mouse cursor hovering pixel (21, 6)")
![Open RAW data prompt](/images/ps-image_03.jpg?raw=true "Open RAW data prompt")

## TODO
- General code cleanup, optimization, commenting, etc.
- Pixel editor
- Blit functions
- VRAM features
- Multi-file support
- File import (*.BMP, *.PNG, etc.)
- (?) Mixed Mode support
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
