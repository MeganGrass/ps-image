/*
*
*	Megan Grass
*	December 14, 2024
*
*/

#pragma once

#include <Resource.h>

#include <std_window.h>

#include <std_dx9.h>

#include <sony_texture.h>

#include <sony_bitstream.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>
#include <imgui_stdlib.h>

typedef class Global_Application Global;

extern std::unique_ptr<Global_Application> G;

extern std::function<void()> CreateModalFunc;

extern LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
extern LRESULT CALLBACK RenderProc(HWND, UINT, WPARAM, LPARAM);

class Global_Application final {
private:

    String TextureInfo;

    std::unique_ptr<Sony_PlayStation_Texture> Texture;
    std::unique_ptr<Sony_PlayStation_Bitstream> Bitstream;

    std::unique_ptr<Standard_Image> Image;

    IDirect3DTexture9* ToolbarIcons;
    IDirect3DTexture9* DXTexture;

    std::filesystem::path m_Filename;
    std::vector<std::pair<std::uintmax_t, std::uintmax_t>> m_Files;
    std::size_t m_SelectedFile;

    std::uint32_t m_Palette;

    std::int32_t m_MousePixelX;
    std::int32_t m_MousePixelY;

    bool b_FontChangeRequested;

    bool b_RawCreateNew;
    bool b_Raw4bpp;
    bool b_Raw8bpp;
    bool b_Raw16bpp;
    bool b_RawExternalPalette;
    bool b_RawExternalPaletteFromTIM;
    bool b_RawImportAllPalettesFromTIM;
    std::int32_t m_RawWidth;
    std::int32_t m_RawHeight;
    std::size_t m_RawPaletteCount;
    std::size_t m_RawPaletteID;
	std::uintmax_t m_RawPixelPtr;
	std::uintmax_t m_RawPalettePtr;
    String m_RawPixelPtrStr;
    String m_RawPalettePtrStr;
    std::filesystem::path m_RawPaletteFilename;

	std::vector<Sony_Texture_16bpp> ClipboardPalette;

	std::int32_t m_BistreamWidth;
	std::int32_t m_BistreamHeight;

	float m_FontSizeMin;
	float m_FontSizeMax;

	float m_ImageZoomMin;
	float m_ImageZoomMax;
	float m_ImageZoom;

    bool b_Dithering;
	bool b_Transparency;
	bool b_Transparency16bpp;

	bool b_ViewToolbar;
	bool b_ViewStatusbar;
	bool b_ViewWindowOptions;
	bool b_ViewBitstreamOptions;
    bool b_ViewFileWindow;
    bool b_ViewPaletteWindow;
    bool b_ViewImageOptions;
    bool b_ViewVRAMWindow;

    bool b_ImageWindowTitleBar;
    bool b_ImageWindowBackground;

	bool b_OpenLastFileOnBoot;

	int m_BootWidth;
	int m_BootHeight;
	bool b_BootMaximized;
	bool b_BootFullscreen;

	void OpenConfig(void);
	std::filesystem::path GetConfigFilename(void) const { return Window->GetUserDocumentsDir() / VER_INTERNAL_NAME_STR / L"config.ini"; }

    void Close(void);
    void SetTexture(std::size_t iTexture);
    void SetPalette(std::uint32_t iPalette);
    void Open(std::filesystem::path Filename);
    void Create(std::filesystem::path Filename);
	void CreateModal(void);
	void Replace(void);
    void ExtractPixelData(void);
	void ReplacePixelData(void);
	void AddPaletteData(void);
	void InsertPaletteData(void);
	void DeletePaletteData(void);
	void CopyPaletteData(void);
	void PastePaletteData(void);
    void ExtractPaletteData(std::size_t iPalette);
    void ReplacePaletteData(std::size_t iPalette);
    void ExtractAllTextures(void);
    void ExtractAllTexturesAsBitmap(void);

	void Tooltip(String Tip);
	void TooltipOnHover(String Tip);
    void MainMenu(void);
	void Toolbar(void);
	float GetToolbarHeight(void) { return 64.0f + ImGui::GetStyle().FramePadding.y * 4.0f; }	// 64.0f is the height of the toolbar icons
    void Statusbar(void);
	void Options(void);
    void File(void);
    void Palette(void);
	void BitstreamSettings(void);
    void ImageSettings(void);
    void ImageWindow(void);

    void Draw(void);
    void Update(void);
    void DragAndDrop(StrVecW Files);

public:

    std::unique_ptr<Standard_Window> Window;
    std::unique_ptr<Standard_DirectX_9> Render;

    Global_Application(void) :
        Window(std::make_unique<Standard_Window>()),
        Render(std::make_unique<Standard_DirectX_9>()),
        TextureInfo(),
        Texture(nullptr),
        Bitstream(std::make_unique<Sony_PlayStation_Bitstream>()),
        Image(nullptr),
        ToolbarIcons(nullptr),
        DXTexture(nullptr),
        m_Filename(),
        m_Files(),
        m_SelectedFile(0),
        m_Palette(0),
        m_MousePixelX(0),
        m_MousePixelY(0),
        b_FontChangeRequested(false),
        b_RawCreateNew(false),
        b_Raw4bpp(false),
        b_Raw8bpp(false),
        b_Raw16bpp(true),
        b_RawExternalPalette(false),
        b_RawExternalPaletteFromTIM(false),
        b_RawImportAllPalettesFromTIM(false),
        m_RawWidth(0),
        m_RawHeight(0),
        m_RawPaletteCount(0),
        m_RawPaletteID(0),
		m_RawPixelPtr(0),
		m_RawPalettePtr(0),
        m_RawPixelPtrStr("0"),
        m_RawPalettePtrStr("0"),
        m_RawPaletteFilename(),
		ClipboardPalette(),
		m_BistreamWidth(320),
		m_BistreamHeight(240),
		m_FontSizeMin(8.0f),
		m_FontSizeMax(256.0f),
		m_ImageZoomMin(1.0f),
		m_ImageZoomMax(128.0f),
		m_ImageZoom(1.0f),
        b_Dithering(false),
        b_Transparency(false),
		b_Transparency16bpp(false),
		b_ViewToolbar(true),
        b_ViewStatusbar(true),
        b_ViewWindowOptions(false),
		b_ViewBitstreamOptions(false),
        b_ViewFileWindow(false),
        b_ViewPaletteWindow(false),
        b_ViewImageOptions(false),
        b_ViewVRAMWindow(false),
        b_ImageWindowTitleBar(true),
		b_ImageWindowBackground(true),
		b_OpenLastFileOnBoot(false),
		m_BootWidth(0),
		m_BootHeight(0),
		b_BootMaximized(false),
		b_BootFullscreen(false)
    {
#if defined(_WIN64)
        m_RawWidth = 64;
        m_RawHeight = 64;
#else
        m_RawWidth = 32;
        m_RawHeight = 32;
#endif
    }
    ~Global_Application(void) = default;

    void About(void) const;

	void SaveConfig(void);

    int Main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow);
};
