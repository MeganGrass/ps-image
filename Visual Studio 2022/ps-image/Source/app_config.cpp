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

			if (Str.ToUpper(Args[0]) == L"M_CREATEBPP")
			{
				switch (std::stoull(Args[1]))
				{
				case 4:
					m_CreateInfo.Depth = 4;
					break;
				case 8:
					m_CreateInfo.Depth = 8;
					break;
				case 16:
				default:
					m_CreateInfo.Depth = 16;
					break;
				case 24:
					m_CreateInfo.Depth = 24;
					break;
				}
			}
			if (Str.ToUpper(Args[0]) == L"M_CREATEWIDTH")
			{
				m_CreateInfo.Width = std::stoi(Args[1]);
				AdjustWidthInput(m_CreateInfo.Width);
			}
			if (Str.ToUpper(Args[0]) == L"M_CREATEHEIGHT")
			{
				m_CreateInfo.Height = std::stoi(Args[1]);
				AdjustHeightInput(m_CreateInfo.Height);
			}
			if (Str.ToUpper(Args[0]) == L"M_CREATEPALETTECOUNT")
			{
				m_CreateInfo.nPalette = std::stoi(Args[1]);
			}
			if (Str.ToUpper(Args[0]) == L"M_CREATEPALETTETYPE")
			{
				m_CreateInfo.PaletteType = ImageType(std::stoi(Args[1]));
			}
			if (Str.ToUpper(Args[0]) == L"M_CREATEPIXELTYPE")
			{
				m_CreateInfo.PixelType = ImageType(std::stoi(Args[1]));
			}
			if (Str.ToUpper(Args[0]) == L"M_CREATEPALETTEPTR")
			{
				m_CreateInfo.pPalette = std::stoull(Args[1], nullptr, 16);
			}
			if (Str.ToUpper(Args[0]) == L"M_CREATEPIXELPTR")
			{
				m_CreateInfo.pPixel = std::stoull(Args[1], nullptr, 16);
			}
			if (Str.ToUpper(Args[0]) == L"M_CREATEPALETTEFILENAME")
			{
				m_CreateInfo.Palette = Args[1];
			}
			if (Str.ToUpper(Args[0]) == L"M_CREATEPIXELFILENAME")
			{
				m_CreateInfo.Pixel = Args[1];
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

	Text->AddLine(L"m_CreateBpp\t%d\r", m_CreateInfo.Depth);
	Text->AddLine(L"m_CreateWidth\t%d\r", m_CreateInfo.Width);
	Text->AddLine(L"m_CreateHeight\t%d\r", m_CreateInfo.Height);
	Text->AddLine(L"m_CreatePaletteCount\t%d\r", m_CreateInfo.nPalette);
	Text->AddLine(L"m_CreatePaletteType\t%d\r", std::to_underlying(m_CreateInfo.PaletteType));
	Text->AddLine(L"m_CreatePixelType\t%d\r", std::to_underlying(m_CreateInfo.PixelType));
	Text->AddLine(L"m_CreatePalettePtr\t0x%x\r", m_CreateInfo.pPalette);
	Text->AddLine(L"m_CreatePixelPtr\t0x%x\r", m_CreateInfo.pPixel);
	Text->AddLine(L"m_CreatePaletteFilename\t\"%ws\"\r", m_CreateInfo.Palette.wstring().c_str());
	Text->AddLine(L"m_CreatePixelFilename\t\"%ws\"\r", m_CreateInfo.Pixel.wstring().c_str());

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