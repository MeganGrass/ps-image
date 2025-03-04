/*
*
*	Megan Grass
*	December 14, 2024
*
*/

#pragma once

#include <resource.h>

#include <std_window.h>

#include <std_dx9.h>

#include <sony_texture.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>
#include <imgui_stdlib.h>

typedef class Global_Application Global;

extern std::unique_ptr<Global_Application> G;

extern LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
extern LRESULT CALLBACK RenderProc(HWND, UINT, WPARAM, LPARAM);

class Global_Application final {
private:

	Standard_String Str;

	Standard_FileSystem FS;

	String m_ConfigStr;

	std::function<void()> CreateModalFunc;

	std::function<void()> SearchModalFunc;
	std::function<void(float, bool&)> SearchModalCb;

	std::function<void()> SaveAllFunc;

	std::unique_ptr<ImGuiContext, decltype(&ImGui::DestroyContext)> Context;

	std::unique_ptr<IDirect3DTexture9, IDirect3DTexture9Delete> m_DXTexture;
	std::unique_ptr<IDirect3DTexture9, IDirect3DTexture9Delete> m_ToolbarIcons;
	D3DSURFACE_DESC m_TextureDesc;
	D3DTEXTUREFILTERTYPE m_TextureFilter;

	std::filesystem::path m_Filename;
	std::vector<std::pair<std::uintmax_t, std::uintmax_t>> m_Files;
	std::size_t m_SelectedFile;
	std::size_t m_PostSearchSelectedFile;

	std::unique_ptr<Sony_PlayStation_Texture> m_Texture;

	Sony_Texture_Create_Ex m_CreateInfo;

	std::vector<Sony_Pixel_16bpp> m_ClipboardPalette;
	DWORD m_ClipboardColor;

	std::uint16_t m_Palette;

	std::uint16_t m_BistreamWidth;
	std::uint16_t m_BistreamHeight;

	std::uint16_t m_MousePixelX;
	std::uint16_t m_MousePixelY;

	std::uint16_t m_LastKnownBitsPerPixel;
	std::uint16_t m_LastKnownWidth;
	std::uint16_t m_LastKnownHeight;
	std::uint16_t m_LastKnownPaletteCount;
	std::size_t m_LastKnownFileCount;

	int m_BootWidth;
	int m_BootHeight;
	bool b_BootMaximized;
	bool b_BootFullscreen;
	bool b_ToolbarIconsOnBoot;
	bool b_OpenLastFileOnBoot;

	bool b_RequestTextureReset;
	bool b_RequestFontChange;
	bool b_Shutdown;
	bool b_ForceShutdown;

	bool b_ViewToolbar;
	bool b_ViewStatusbar;
	bool b_ViewWindowOptions;
	bool b_ViewBitstreamOptions;
	bool b_ViewFileWindow;
	bool b_ViewPaletteWindow;
	bool b_ViewImageOptions;
	bool b_ViewVRAMWindow;

	float m_FontSizeMin;
	float m_FontSizeMax;

	float m_ImageZoomMin;
	float m_ImageZoomMax;
	float m_ImageZoom;

	bool b_ImageOnDisk;

	float m_ProgressBar;
	bool b_Searching;

	bool b_Dithering;

	[[nodiscard]] constexpr auto Texture() noexcept -> Sony_PlayStation_Texture&
	{
		if (m_Texture.get() == nullptr)
		{
			m_Texture = std::make_unique<Sony_PlayStation_Texture>(m_LastKnownBitsPerPixel, m_LastKnownWidth, m_LastKnownHeight, m_LastKnownPaletteCount);
		}
		return *m_Texture.get();
	}

	[[nodiscard]] constexpr auto File() noexcept -> std::vector<std::pair<std::uintmax_t, std::uintmax_t>>&
	{
		if (m_Files.empty())
		{
			m_Files.resize(m_LastKnownFileCount);
		}
		return m_Files;
	}

	void OpenConfig(void);
	std::filesystem::path GetConfigFilename(void) const { return Window->GetUserDocumentsDir() / VER_INTERNAL_NAME_STR / L"config.ini"; }
	std::filesystem::path GetImGuiConfigFilename(void) const { return Window->GetUserDocumentsDir() / VER_INTERNAL_NAME_STR / L"imgui.ini"; }
	std::filesystem::path GetToolbarIconFilename(void) const { return Window->GetUserDocumentsDir() / VER_INTERNAL_NAME_STR / L"icons.png"; }

	void Close(void);
	void ResetTexture(void);
	void SetPalette(std::uint16_t iPalette);
	void SetTexture(std::size_t iTexture);
	void Search(std::filesystem::path Filename);
	void SearchModal(void);
	void OnSearchComplete(std::filesystem::path Filename, std::vector<std::pair<std::uintmax_t, std::uintmax_t>>& FileList);
	bool Create(void);
	void CreateModal(void);
	void SaveAllModal(void) const;
	void MovePalette(std::uint16_t iPalette, bool Right);
	void AddPalette(void);
	void InsertPalette(void);
	void DeletePalette(std::uint16_t iPalette, bool b_All = false);
	void DeletePixels(void);
	void CopyPalette(void);
	void PastePalette(void);
	void SetExternalPaletteFilename(void);
	void SetExternalPixelFilename(void);
	void RawIO(std::filesystem::path Filename, ImageIO Flags = ImageIO::All, std::uintmax_t pSource = 0, std::uint16_t iPalette = 0, ImageType SaveAllType = ImageType::TIM);
	void TextureIO(std::filesystem::path Filename, ImageIO Flags = ImageIO::All, std::uintmax_t pSource = 0, std::uint16_t iPalette = 0, ImageType SaveAllType = ImageType::TIM);
	void TextureIO(ImageIO Flags = ImageIO::All, std::uintmax_t pSource = 0, std::uint16_t iPalette = 0, ImageType SaveAllType = ImageType::TIM);

	void Tooltip(String Tip);
	void TooltipOnHover(String Tip);
	bool ScrollOnHover(void* Input, ImGuiDataType DataType, std::uintmax_t Step, std::uintmax_t Min, std::uintmax_t Max) const;
	bool ScrollFloatOnHover(void* Input, ImGuiDataType DataType, double Step, double Min, double Max) const;
	void AdjustWidthInput(std::uint16_t& Width);
	void AdjustHeightInput(std::uint16_t& Height);

	float GetToolbarHeight(void) { return 64.0f + ImGui::GetStyle().FramePadding.y * 4.0f; }	// 64.0f = height of toolbar icons

	void MainMenu(void);
	void Toolbar(void);
	void Statusbar(void);
	void Options(void);
	void FileBrowser(void);
	void Palette(void);
	void BitstreamSettings(void);
	void ImageSettings(void);
	void ImageWindow(void);

	void Draw(void);
	void Update(void);
	void DragAndDrop(StrVecW Files);
	void Commandline(StrVecW Args);

public:

	std::unique_ptr<Standard_Window> Window;
	std::unique_ptr<Standard_DirectX_9> Render;

	Global_Application(void) :
		Window(std::make_unique<Standard_Window>()),
		Render(std::make_unique<Standard_DirectX_9>()),
		Str(),
		FS(),
		m_ConfigStr(),
		m_Texture(nullptr),
		m_ToolbarIcons(nullptr),
		m_DXTexture(nullptr),
		m_TextureDesc(),
		m_TextureFilter(D3DTEXF_NONE),
		m_Filename(),
		m_Files(),
		m_SelectedFile(0),
		m_PostSearchSelectedFile(0),
		m_Palette(0),
		m_MousePixelX(0),
		m_MousePixelY(0),
		b_RequestFontChange(false),
		b_RequestTextureReset(false),
		m_CreateInfo(),
		m_ClipboardColor(0),
		m_ClipboardPalette(),
		m_LastKnownBitsPerPixel(16),
		m_LastKnownWidth(1024),
		m_LastKnownHeight(512),
		m_LastKnownPaletteCount(0),
		m_LastKnownFileCount(0),
		m_BistreamWidth(320),
		m_BistreamHeight(240),
		m_FontSizeMin(8.0f),
		m_FontSizeMax(256.0f),
		m_ImageZoomMin(1.0f),
		m_ImageZoomMax(128.0f),
		m_ImageZoom(1.0f),
		m_ProgressBar(1.0f),
		b_Searching(false),
		b_ImageOnDisk(false),
		b_Dithering(false),
		b_ViewToolbar(true),
		b_ViewStatusbar(true),
		b_ViewWindowOptions(false),
		b_ViewBitstreamOptions(false),
		b_ViewFileWindow(false),
		b_ViewPaletteWindow(false),
		b_ViewImageOptions(false),
		b_ViewVRAMWindow(false),
		b_ToolbarIconsOnBoot(false),
		b_OpenLastFileOnBoot(false),
		m_BootWidth(0),
		m_BootHeight(0),
		b_BootMaximized(false),
		b_BootFullscreen(false),
		b_Shutdown(false),
		b_ForceShutdown(false),
		Context(nullptr, &ImGui::DestroyContext)
	{
		CreateModalFunc = []() {};
		SearchModalFunc = []() {};
		SearchModalCb = [](float, bool&) {};
		SaveAllFunc = []() {};
	}
	~Global_Application(void) = default;

	void About(void) const;

	void Controls(void) const;

	void Shutdown(void);

	void SaveConfig(void);

	int Main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow);
};