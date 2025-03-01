# PS-Image
Multi-purpose utility application for Sony PlayStation (PS1/PSone) textures

## Supported File Formats
- Sony PlayStation Texture Image (.TIM) *(4/8/16/24bpp)*
- Sony PlayStation Texture CLUT (.CLT) *(16bpp color palette)*
- Sony PlayStation Texture Pixels (.PXL) *(4/8/16/24bpp pixels)*
- Sony PlayStation Bitstream (.BS) *(Version 1/2)*
- Sony PlayStation 2 Texture Map (.TM2/*.CL2) *(16/24/32bpp palette Storage Mode 1/2, 4/8/16/24/32bpp pixels)*
- Bitmap Graphic (.BMP) *(4/8bpp palette, 16-bit high color and 24/32bpp A/RGB)*
- Microsoft RIFF Palette (.PAL) *(32bpp ARGB palette)*
- Portable Network Graphics (.PNG) *(4/8bpp palette and 24/32bpp A/RGB)*
- Joint Photographic Experts Group (.JPG)

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
- Import and export supplemental file types (BMP, PNG, etc.)

(*) Palette colors from supplemental formats are converted from 24-bit (8:8:8) to 15-bit (5:5:5) on import (vice-versa on export)

(*) Color depth (bits per pixel) is not modified on export

## Advanced Features
##### **File Creation**
- Create and save new PlayStation Texture Images
- Import and export raw data from any file, pixel and/or palette data for all supported file types

##### **File Modifier**
- Import and export PlayStation Texture Images to/from any file type
##### **Semi-Transparency Processing**
- Preview any PlayStation Texture Image with basic semi-transparency effects (optional)
- Preview any color separate from palette as semi-transparent (CLUT not required)
- Superblack support: solid black pixels (5:5:5:1/8:8:8) are completely transparent
- Superimposed support: color at palette index 0 is semi-transparent (CLUT required)
- Transparency Control Bit (STP) support
- Semi-Transparency Rate (ABR) support
- All bit depth modes supported (4bpp, 8bpp, 16bpp and 24bpp)

| Rate | Description                       |
| :-------- | :-------------------------------- |
| `Half` | 50%back + 50%texture |
| `Full` | 100%back + 100%texture |
| `Inverse` | 100%back - 100%texture |
| `Quarter` | 100%back + 25%texture |

##### **Dither**
- Preview textures as they would appear on a real Sony PlayStation (1994) with a built-in [Dithering Filter](https://en.wikipedia.org/wiki/Dither) (optional)
##### **Filter**
- Preview textures with Point, Linear, Anisotropic or no filtering (default)

## Texture Modification
##### **Color Lookup Table (CLUT)**
- Modify and preview multiple palettes in real-time
- Add, Insert, Delete, Move, Copy and Paste entire palettes
- Palettes are [15-bit](https://en.wikipedia.org/wiki/High_color) high color [indexes](https://en.wikipedia.org/wiki/Indexed_color) with [semi/transparency](https://en.wikipedia.org/wiki/Palette_(computing)#Transparency_in_palettes) support (5:5:5:1)
- 4bpp *(16 color palette)* and 8bpp *(256 color palette)* modes supported

##### **Pixels**
- Enable or disable semi-transparency (STP flag) for individual palette colors and pixels (4bpp/8bpp/16bpp only)

## Bitstream Images
Support for Bitstream images is extremely limited, with only decoding support being reserved for future features

Any resolution supported ([640x480](https://en.wikipedia.org/wiki/PlayStation_technical_specifications#:~:text=Resolutions) max)

Bitstreams images are decoded to 32bpp raster data

## VRAM
Preliminary support for modifying both CLUT and Pixel coordinates is available in _"File Properties"_ window

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
- (?) Mixed Mode support
- (?) Sony PlayStation Bitstream (.BS) creation
- (?) Sony PlayStation Texture Model (.TMD) support

## Build
- [Microsoft Visual Studio](https://visualstudio.microsoft.com) 2022+
- [Microsoft Direct-X](https://www.nuget.org/packages/Microsoft.DXSDK.D3DX)
- Clone the repo
- Install *Microsoft.DXSDK.D3DX* nuget package into the project
- Build/Link libjpeg-turbo libraries
- Compile
```
git clone https://github.com/MeganGrass/ps-image.git
```

## Acknowledgements
[Bitstream Decoder](https://github.com/XProger/OpenResident)

[Dither Matrix](https://psx-spx.consoledev.net/graphicsprocessingunitgpu/#24bit-rgb-to-15bit-rgb-dithering-enabled-in-texpage-attribute)

[CLT and PXL file](https://problemkaputt.de/psxspx-cdrom-file-video-texture-image-tim-pxl-clt-sony.htm)

[PS2 TIM2 Palette De-Swizzle](https://github.com/bartlomiejduda/ReverseBox/blob/main/reversebox/image/swizzling/swizzle_ps2.py)

[PSX.dev (Discord)](https://discord.com/invite/psx-dev-642647820683444236)


## Copyrights
Sony PlayStation is Copyright (©) [Sony Computer Entertainment Inc.](https://sonyinteractive.com/)

[ImGui](https://github.com/ocornut/imgui) is Copyright (©) 2014-2025 Omar Cornut

[Direct-X](https://learn.microsoft.com/en-us/windows/win32/directx) is Copyright (©) 1995-2021 Microsoft Corporation

[libjpeg-turbo](https://github.com/libjpeg-turbo/libjpeg-turbo)

[libpng](https://github.com/pnggroup/libpng)

[zlib](https://github.com/madler/zlib)
