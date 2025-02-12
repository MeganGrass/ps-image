/*
*
*	Megan Grass
*	December 14, 2024
*
*/

#include "app.h"

#include <std_text.h>

void Global_Application::OpenConfig(void)
{
	if (b_Shutdown) { return; }

	std::unique_ptr<StdText> Text = std::make_unique<StdText>(GetConfigFilename(), FileAccessMode::Read_Ex, TextFileBOM::UTF16_LE);
	if (!Text->IsOpen())
	{
		Str.Message("Error, could not open \"%ws\" app config", GetConfigFilename().filename().wstring().c_str());
		return;
	}

	for (std::size_t i = 0; i < Text->GetLineCount(); i++)
	{
		StrVecW Args = Text->GetArgsW(i);

		if (!Args.empty() && (Args.size() > 1))
		{
			if (Str.ToUpper(Args[0]) == L"M_FONT")
			{
				Window->SetFont(Args[1], Window->FontSize());
			}
			if (Str.ToUpper(Args[0]) == L"M_FONTSIZE")
			{
				Window->FontSize() = std::stof(Args[1]);
				Window->FontSize() = std::clamp(Window->FontSize(), m_FontSizeMin, m_FontSizeMax);
			}
			if (Str.ToUpper(Args[0]) == L"M_COLOR")
			{
				DWORD Color = std::stoul(Args[1], nullptr, 16);
				Window->SetColor(GetRValue(Color), GetGValue(Color), GetBValue(Color));
			}
			if (Str.ToUpper(Args[0]) == L"M_WIDTH")
			{
				m_BootWidth = std::stoi(Args[1]);
			}
			if (Str.ToUpper(Args[0]) == L"M_HEIGHT")
			{
				m_BootHeight = std::stoi(Args[1]);
			}
			if (Str.ToUpper(Args[0]) == L"B_MAXIMIZED")
			{
				b_BootMaximized = std::stoi(Args[1]);
			}
			if (Str.ToUpper(Args[0]) == L"B_FULLSCREEN")
			{
				b_BootFullscreen = std::stoi(Args[1]);
			}

			if (Str.ToUpper(Args[0]) == L"M_FILENAME")
			{
				m_Filename = Args[1];
			}

			if (Str.ToUpper(Args[0]) == L"B_RAWBPP")
			{
				switch (std::stoull(Args[1]))
				{
				case 4:
					b_Raw4bpp = true;
					b_Raw8bpp = false;
					b_Raw16bpp = false;
					b_Raw24bpp = false;
					break;
				case 8:
					b_Raw4bpp = false;
					b_Raw8bpp = true;
					b_Raw16bpp = false;
					b_Raw24bpp = false;
					break;
				case 16:
				default:
					b_Raw4bpp = false;
					b_Raw8bpp = false;
					b_Raw16bpp = true;
					b_Raw24bpp = false;
					break;
				case 24:
					b_Raw4bpp = false;
					b_Raw8bpp = false;
					b_Raw16bpp = false;
					b_Raw24bpp = true;
					break;
				}
			}
			if (Str.ToUpper(Args[0]) == L"B_RAWEXTERNALPIXELS")
			{
				b_RawExternalPixels = std::stoi(Args[1]);
			}
			if (Str.ToUpper(Args[0]) == L"B_RAWEXTERNALPIXELSFROMTIM")
			{
				b_RawExternalPixelsFromTIM = std::stoi(Args[1]);
			}
			if (Str.ToUpper(Args[0]) == L"B_RAWEXTERNALPALETTE")
			{
				b_RawExternalPalette = std::stoi(Args[1]);
			}
			if (Str.ToUpper(Args[0]) == L"B_RAWEXTERNALPALETTEFROMTIM")
			{
				b_RawExternalPaletteFromTIM = std::stoi(Args[1]);
			}
			if (Str.ToUpper(Args[0]) == L"M_RAWWIDTH")
			{
				m_RawWidth = std::stoi(Args[1]);
				AdjustWidthInput(m_RawWidth);
			}
			if (Str.ToUpper(Args[0]) == L"M_RAWHEIGHT")
			{
				m_RawHeight = std::stoi(Args[1]);
				AdjustHeightInput(m_RawHeight);
			}
			if (Str.ToUpper(Args[0]) == L"M_RAWPALETTECOUNT")
			{
				m_RawPaletteCount = std::stoi(Args[1]);
			}
			if (Str.ToUpper(Args[0]) == L"M_RAWPIXELPTR")
			{
				m_RawPixelPtr = std::stoull(Args[1], nullptr, 16);
			}
			if (Str.ToUpper(Args[0]) == L"M_RAWPALETTEPTR")
			{
				m_RawPalettePtr = std::stoull(Args[1], nullptr, 16);
			}
			if (Str.ToUpper(Args[0]) == L"M_RAWPIXELFILENAME")
			{
				m_RawPixelFilename = Args[1];
			}
			if (Str.ToUpper(Args[0]) == L"M_RAWPALETTEFILENAME")
			{
				m_RawPaletteFilename = Args[1];
			}

			if (Str.ToUpper(Args[0]) == L"M_BISTREAMWIDTH")
			{
				m_BistreamWidth = std::stoi(Args[1]);
			}
			if (Str.ToUpper(Args[0]) == L"M_BISTREAMHEIGHT")
			{
				m_BistreamHeight = std::stoi(Args[1]);
			}

			if (Str.ToUpper(Args[0]) == L"M_IMAGEZOOM")
			{
				m_ImageZoom = std::stof(Args[1]);
				m_ImageZoom = std::clamp(m_ImageZoom, m_ImageZoomMin, m_ImageZoomMax);
			}
			if (Str.ToUpper(Args[0]) == L"B_DITHERING")
			{
				b_Dithering = std::stoi(Args[1]);
			}
			if (Str.ToUpper(Args[0]) == L"M_FILTERING")
			{
				m_TextureFilter = (D3DTEXTUREFILTERTYPE)std::stoi(Args[1]);

				std::uintmax_t iTextureFilterType = m_TextureFilter;
				iTextureFilterType = std::clamp(iTextureFilterType, 0ULL, 3ULL);
				m_TextureFilter = (D3DTEXTUREFILTERTYPE)iTextureFilterType;
			}

			if (Str.ToUpper(Args[0]) == L"B_VIEWTOOLBAR")
			{
				b_ViewToolbar = std::stoi(Args[1]);
			}
			if (Str.ToUpper(Args[0]) == L"B_VIEWSTATUSBAR")
			{
				b_ViewStatusbar = std::stoi(Args[1]);
			}
			if (Str.ToUpper(Args[0]) == L"B_VIEWWINDOWOPTIONS")
			{
				b_ViewWindowOptions = std::stoi(Args[1]);
			}
			if (Str.ToUpper(Args[0]) == L"B_VIEWBITSTREAMOPTIONS")
			{
				b_ViewBitstreamOptions = std::stoi(Args[1]);
			}
			if (Str.ToUpper(Args[0]) == L"B_VIEWFILEWINDOW")
			{
				b_ViewFileWindow = std::stoi(Args[1]);
			}
			if (Str.ToUpper(Args[0]) == L"B_VIEWPALETTEWINDOW")
			{
				b_ViewPaletteWindow = std::stoi(Args[1]);
			}
			if (Str.ToUpper(Args[0]) == L"B_VIEWIMAGEOPTIONS")
			{
				b_ViewImageOptions = std::stoi(Args[1]);
			}
			if (Str.ToUpper(Args[0]) == L"B_VIEWVRAMWINDOW")
			{
				b_ViewVRAMWindow = std::stoi(Args[1]);
			}

			if (Str.ToUpper(Args[0]) == L"B_OPENLASTFILEONBOOT")
			{
				b_OpenLastFileOnBoot = std::stoi(Args[1]);
			}
		}
	}
}

void Global_Application::SaveConfig(void)
{
	if (b_Shutdown) { return; }

	std::unique_ptr<StdText> Text = std::make_unique<StdText>(GetConfigFilename(), FileAccessMode::Write_Ex, TextFileBOM::UTF16_LE);
	if (!Text->IsOpen())
	{
		Str.Message("Error, could not create \"%ws\" app config", GetConfigFilename().filename().wstring().c_str());
		return;
	}

	Text->AddLine(L"app\t\"%ws\"\r", Window->GetModuleStr().c_str());

	Text->AddLine(L"m_Font\t%ws\r", Window->GetFont().stem().wstring().c_str());
	Text->AddLine(L"m_FontSize\t%.0f\r", Window->FontSize());
	Text->AddLine(L"m_Color\t0x%x\r", Window->GetColor());
	Text->AddLine(L"m_Width\t%d\r", (Window->GetRect().right - Window->GetRect().left));
	Text->AddLine(L"m_Height\t%d\r", (Window->GetRect().bottom - Window->GetRect().top));
	Text->AddLine(L"b_Maximized\t%d\r", Window->IsMaximized());
	Text->AddLine(L"b_Fullscreen\t%d\r", Window->IsFullscreen());

	Text->AddLine(L"m_Filename\t\"%ws\"\r", m_Filename.wstring().c_str());

	Text->AddLine(L"b_RawBpp\t%d\r", b_Raw4bpp ? 4 : b_Raw8bpp ? 8 : b_Raw16bpp ? 16 : b_Raw24bpp ? 24 : 16);
	Text->AddLine(L"b_RawExternalPixels\t%d\r", b_RawExternalPixels);
	Text->AddLine(L"b_RawExternalPixelsFromTIM\t%d\r", b_RawExternalPixelsFromTIM);
	Text->AddLine(L"b_RawExternalPalette\t%d\r", b_RawExternalPalette);
	Text->AddLine(L"b_RawExternalPaletteFromTIM\t%d\r", b_RawExternalPaletteFromTIM);
	Text->AddLine(L"m_RawWidth\t%d\r", m_RawWidth);
	Text->AddLine(L"m_RawHeight\t%d\r", m_RawHeight);
	Text->AddLine(L"m_RawPaletteCount\t%d\r", m_RawPaletteCount);
	Text->AddLine(L"m_RawPixelPtr\t0x%llx\r", m_RawPixelPtr);
	Text->AddLine(L"m_RawPalettePtr\t0x%llx\r", m_RawPalettePtr);
	Text->AddLine(L"m_RawPixelFilename\t\"%ws\"\r", m_RawPixelFilename.wstring().c_str());
	Text->AddLine(L"m_RawPaletteFilename\t\"%ws\"\r", m_RawPaletteFilename.wstring().c_str());

	Text->AddLine(L"m_BistreamWidth\t%d\r", m_BistreamWidth);
	Text->AddLine(L"m_BistreamHeight\t%d\r", m_BistreamHeight);

	Text->AddLine(L"m_ImageZoom\t%f\r", m_ImageZoom);
	Text->AddLine(L"b_Dithering\t%d\r", b_Dithering);
	Text->AddLine(L"m_Filtering\t%d\r", m_TextureFilter);

	Text->AddLine(L"b_ViewToolbar\t%d\r", b_ViewToolbar);
	Text->AddLine(L"b_ViewStatusbar\t%d\r", b_ViewStatusbar);
	Text->AddLine(L"b_ViewWindowOptions\t%d\r", b_ViewWindowOptions);
	Text->AddLine(L"b_ViewBitstreamOptions\t%d\r", b_ViewBitstreamOptions);
	Text->AddLine(L"b_ViewFileWindow\t%d\r", b_ViewFileWindow);
	Text->AddLine(L"b_ViewPaletteWindow\t%d\r", b_ViewPaletteWindow);
	Text->AddLine(L"b_ViewImageOptions\t%d\r", b_ViewImageOptions);
	Text->AddLine(L"b_ViewVRAMWindow\t%d\r", b_ViewVRAMWindow);

	Text->AddLine(L"b_OpenLastFileOnBoot\t%d\r", b_OpenLastFileOnBoot);

	Text->FlushUTF16();
}