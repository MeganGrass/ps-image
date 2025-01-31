/*
*
*	Megan Grass
*	December 14, 2024
*
*/

#include "app.h"

void Global_Application::Close(void)
{
	if (b_Shutdown) { return; }

	if (m_Texture)
	{
		m_Texture.reset();
		m_Texture = nullptr;
	}

	if (Image)
	{
		Image.reset();
		Image = nullptr;
	}

	if (Render->NormalState() && DXTexture)
	{
		DXTexture->Release();
		DXTexture = nullptr;
	}

	m_Palette = 0;

	m_Filename.clear();
	m_SelectedFile = 0;
	m_Files.clear();
}

void Global_Application::ResetTexture(void)
{
	if (m_Texture && m_Texture->IsOpen())
	{
		if (m_Files.empty())
		{
			m_Filename = L"untitled.tim";
			m_Files.resize(1);
			m_Files[0].first = 0;
			m_SelectedFile = 0;
		}

		m_SelectedFile = std::clamp(m_SelectedFile, size_t(0), size_t(m_Files.empty() ? 0 : m_Files.size() - 1));
		m_Files[m_SelectedFile].second = Texture().Size();

		m_Palette = std::clamp(m_Palette, uint16_t(0), uint16_t(Texture().GetClutMax()));

		m_LastKnownBitsPerPixel = Texture().GetDepth();
		m_LastKnownWidth = Texture().GetWidth();
		m_LastKnownHeight = Texture().GetHeight();
		m_LastKnownPaletteCount = int16_t(Texture().GetPalette().size());

		Texture().Str.hWnd = Window->Get();

		Texture().UpdateTransparency();

		if (Render->NormalState())
		{
			if (DXTexture)
			{
				DXTexture->Release();
			}

			DXTexture = Render->CreateTexture(m_Texture, m_Palette, Texture().TransparencyFlags(), Texture().TransparentColor());

			DXTexture->GetLevelDesc(0, &m_TextureDesc);
		}
	}
	else
	{
		Close();
	}
}

void Global_Application::SetTexture(std::size_t iFile)
{
	if (m_Files.empty()) { return; }

	m_SelectedFile = std::clamp(iFile, size_t(0), size_t(m_Files.empty() ? 0 : m_Files.size() - 1));

	m_Palette = 0;

	m_Texture.reset();

	m_Texture = std::make_unique<Sony_PlayStation_Texture>(m_Filename, m_Files[m_SelectedFile].first);

	ResetTexture();
}

void Global_Application::SetPalette(std::uint16_t iPalette)
{
	m_Palette = std::clamp(iPalette, uint16_t(0), Texture().GetClutMax());

	ResetTexture();
}

void Global_Application::Open(std::filesystem::path Filename)
{
	StringW Extension = FS.GetFileExtension(Filename);

	Close();

	if (Str.ToUpper(Extension) == L".BS")
	{
		OpenBitstream(Filename);
		return;
	}

	SearchModalFunc = [&](std::filesystem::path Filename) -> void
		{
			SearchModal(Filename);
		};

	SearchModalFunc(Filename);

	if (m_Files.empty())
	{
		Str.Message(L"Sony PlayStation textures were not found in \"%ws\"", Filename.filename().wstring().c_str());
		return;
	}

	m_Filename = Filename;

	m_Texture = std::make_unique<Sony_PlayStation_Texture>(m_Filename, m_Files[m_SelectedFile].first);

	ResetTexture();
}

void Global_Application::OpenBitstream(std::filesystem::path Filename)
{
	StdFile m_Input{ Filename, FileAccessMode::Read, true, false };
	if (!m_Input.IsOpen())
	{
		Str.Message(L"Error, could not open \"%ws\"", Filename.filename().wstring().c_str());
		return;
	}

	struct BitStream_Header
	{
		std::uint16_t Length;
		std::uint16_t ID;
		std::uint16_t Q_Scale;
		std::uint16_t Version;
	};

	BitStream_Header Header{};

	Close();

	m_LastKnownBitsPerPixel = 32;
	m_LastKnownWidth = m_BistreamWidth;
	m_LastKnownHeight = m_BistreamHeight;

	std::vector<std::uint8_t> Output(m_BistreamWidth * m_BistreamHeight * 4);

	std::vector<std::uint8_t> Input((size_t)m_Input.Size());

	m_Input.Read(0, Input.data(), Input.size());

	std::memcpy(&Header, &Input.data()[0], sizeof(BitStream_Header));

	m_Bitstream->mdec_decode(&Input.data()[8], Header.Version, m_BistreamWidth, m_BistreamHeight, Header.Q_Scale, &Output.data()[0]);

	Image = std::make_unique<Standard_Image>(ImageFormat::BMP, m_BistreamWidth, m_BistreamHeight, 32);

	size_t pOut = 0;
	for (std::uint16_t y = 0; y < m_BistreamHeight; y++)
	{
		for (std::uint16_t x = 0; x < m_BistreamWidth; x++, pOut += 4)
		{
			std::uint32_t Color = Output[pOut + 2] | (Output[pOut + 1] << 8) | (Output[pOut + 0] << 16) | (Output[pOut + 3] << 24);
			Image->SetPixel(x, y, Color);
		}
	}

	if (Render->NormalState())
	{
		DXTexture = Render->CreateTexture(Image);

		DXTexture->GetLevelDesc(0, &m_TextureDesc);
	}

	m_Filename = Filename;
}

void Global_Application::Open(void)
{
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetOpenFilename({ L"Sony Texture Image", L"Sony Bitstream", L"All files" }, { L"*.tim", L"*.bs", L"*.*" }); Filename.has_value())
	{
		CoUninitialize();

		StringW Ext = FS.GetFileExtension(Filename.value());

		if (Str.ToUpper(Ext) == L".BS")
		{
			OpenBitstream(Filename.value());
		}
		else
		{
			Open(Filename.value());
		}
	}
}

void Global_Application::SetRawPixelFilename(void)
{
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetOpenFilename({ L"Sony Texture Image", L"All files" }, { L"*.tim", L"*.*" }); Filename.has_value())
	{
		CoUninitialize();

		m_RawPixelFilename = Filename.value();
	}
}

void Global_Application::SetRawPaletteFilename(void)
{
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetOpenFilename({ L"Sony Texture Image", L"All files" }, { L"*.tim", L"*.*" }); Filename.has_value())
	{
		CoUninitialize();

		m_RawPaletteFilename = Filename.value();
	}
}

bool Global_Application::Create(std::uint32_t Depth, std::uint16_t Width, std::uint16_t Height, std::uintmax_t pPixel, std::uintmax_t pPalette)
{
	std::unique_ptr<Sony_PlayStation_Texture> TmpTexture = std::make_unique<Sony_PlayStation_Texture>(Depth, Width, Height, NULL);

	TmpTexture->Str.hWnd = Window->Get();

	if (b_RawExternalPalette)
	{
		if (!TmpTexture->ImportPalette(m_RawPaletteFilename, pPalette, uint16_t(m_RawPaletteCount)))
		{
			return false;
		}
	}
	else if (b_RawExternalPaletteFromTIM)
	{
		if (!TmpTexture->ImportPaletteFromTIM(m_RawPaletteFilename, pPalette))
		{
			return false;
		}
	}

	if (b_RawExternalPixels)
	{
		if (!TmpTexture->ImportPixels(m_RawPixelFilename, pPixel))
		{
			return false;
		}
	}
	else if (b_RawExternalPixelsFromTIM)
	{
		if (!TmpTexture->ImportPixelsFromTIM(m_RawPixelFilename, pPixel, true))
		{
			return false;
		}
	}

	{
		Close();

		m_Texture = std::make_unique<Sony_PlayStation_Texture>(TmpTexture->GetPixels().empty() ? Depth : TmpTexture->GetDepth(), Width, Height, NULL);

		if (!TmpTexture->GetPixels().empty())
		{
			if (!Texture().ImportPixelsFromTIM(TmpTexture, true))
			{
				return false;
			}
		}

		if (m_RawPaletteCount && (!b_RawExternalPalette && !b_RawExternalPaletteFromTIM))
		{
			for (auto i = 0; i < m_RawPaletteCount; i++)
			{
				Texture().AddPalette();
			}
		}
		else if (!TmpTexture->GetPalette().empty())
		{
			if (!Texture().ImportPaletteFromTIM(TmpTexture))
			{
				return false;
			}
		}

		ResetTexture();
	}

	return true;
}

void Global_Application::SaveAs(void)
{
	if (m_Texture && !m_Texture->IsValid())
	{
		Str.Message("Error, palette and pixel data is empty");
		return;
	}

	if (!Image && !m_Texture)
	{
		return;
	}

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetSaveFilename({
		L"Sony Texture Image",
		L"Bitmap Graphic",
		/*L"Joint Photographic Experts Group",
		L"Truevision Advanced Raster Graphics Adapter",
		L"DirectDraw Surface",
		L"Portable Pixmap Format",
		L"Device-independent Bitmap"*/
		},
						{
							L"*.tim",
							L"*.bmp",
							/*L"*.jpg",
							L"*.tga",
							L"*.dds",
							L"*.ppm",
							L"*.dib"*/
						}); Filename.has_value())
	{
		CoUninitialize();

		StringW Ext = FS.GetFileExtension(Filename.value());

		if (Str.ToUpper(Ext) == L".TIM")
		{
			if (m_Texture)
			{
				if (Texture().Save(Filename.value()))
				{
					Open(Filename.value());
				}
			}
			else
			{
				Str.Message(L"Error, conversion to TIM not yet supported");
			}
		}

		if (Str.ToUpper(Ext) == L".BMP")
		{
			if (m_Texture)
			{
				Texture().SaveAsBitmap(Filename.value(), m_Palette);
			}
			else if (Image)
			{
				Image->SaveAsBitmap(Filename.value());
			}
			else
			{
				std::cout << "Error, no image data to save" << std::endl;
			}
		}
	}
}

void Global_Application::SaveAsBitmap(std::size_t iFile)
{
	if (m_Files.empty()) { return; }

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetSaveFilename({ L"Bitmap Graphic" }, { L"*.bmp" }); Filename.has_value())
	{
		CoUninitialize();

		if (m_Texture)
		{
			Texture().SaveAsBitmap(Filename.value(), m_Palette);
		}
		else if (Image)
		{
			Image->SaveAsBitmap(Filename.value());
		}
		else
		{
			std::cout << "Error, no image data to save" << std::endl;
		}
	}
}

void Global_Application::Export(std::size_t iFile)
{
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetSaveFilename({ L"Sony Texture Image" }, { L"*.tim" }); Filename.has_value())
	{
		CoUninitialize();

		Texture().Save(Filename.value());
	}
}

void Global_Application::Import(std::size_t iFile)
{
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetOpenFilename({ L"Sony Texture Image" }, { L"*.tim" }); Filename.has_value())
	{
		CoUninitialize();

		std::unique_ptr<Sony_PlayStation_Texture> ExternalTexture = std::make_unique<Sony_PlayStation_Texture>();

		ExternalTexture->Str.hWnd = Window->Get();

		ExternalTexture->Open(Filename.value());

		if (!ExternalTexture->IsOpen())
		{
			return;
		}

		if ((Texture().GetDepth() != ExternalTexture->GetDepth()))
		{
			if (!Window->Question(L"Texture depth (%d) does not match \"%ws\" depth (%d)\r\n\r\nContinue?",
				Texture().GetDepth(),
				Filename.value().filename().wstring().c_str(),
				ExternalTexture->GetDepth()))
			{
				return;
			}
		}
		
		if ((Texture().GetWidth() != ExternalTexture->GetWidth()) || (Texture().GetHeight() != ExternalTexture->GetHeight()))
		{
			if (!Window->Question(L"Texture resolution (%d x %d) does not match \"%ws\" resolution (%d x %d)\r\n\r\nContinue?",
				Texture().GetWidth(), Texture().GetHeight(),
				Filename.value().filename().wstring().c_str(),
				ExternalTexture->GetWidth(), ExternalTexture->GetHeight()))
			{
				return;
			}
		}

		ExternalTexture->Save(m_Filename, m_Files[m_SelectedFile].first);

		Open(m_Filename);

		SetTexture(iFile);
	}
}

void Global_Application::ExportPixels(void)
{
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetSaveFilename({ L"Raw Pixel Data" }, { L"*.pix" }); Filename.has_value())
	{
		CoUninitialize();

		Texture().ExportPixels(Filename.value());
	}
}

void Global_Application::ImportPixels(void)
{
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetOpenFilename({ L"All files" }, { L"*.*" }); Filename.has_value())
	{
		CoUninitialize();

		if (Texture().ImportPixels(Filename.value(), 0))
		{
			ResetTexture();
		}
	}
}

void Global_Application::ExportPixelsToTIM(void)
{
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetSaveFilename({ L"Sony Texture Image" }, { L"*.tim" }); Filename.has_value())
	{
		CoUninitialize();

		Texture().ExportPixelsToTIM(Filename.value());
	}
}

void Global_Application::ImportPixelsFromTIM(void)
{
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetOpenFilename({ L"Sony Texture Image" }, { L"*.tim" }); Filename.has_value())
	{
		CoUninitialize();

		if (Texture().ImportPixelsFromTIM(Filename.value(), 0, false))
		{
			ResetTexture();
		}
	}
}

void Global_Application::MovePalette(std::size_t iPalette, bool Right)
{
	if (Texture().MovePalette(iPalette, Right))
	{
		std::uint16_t OldPalette = m_Palette;

		if (Right) { ++m_Palette; }
		else if (m_Palette) { --m_Palette; }

		if (OldPalette != m_Palette)
		{
			ResetTexture();
		}
	}
}

void Global_Application::AddPalette(void)
{
	Texture().AddPalette();

	m_Palette = std::clamp(uint16_t(Texture().GetClutMax()), uint16_t(0), Texture().GetClutMax());

	ResetTexture();
}

void Global_Application::AddPaletteFromFile(void)
{
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetOpenFilename({ L"Sony Texture Image", L"Microsoft RIFF Palette", L"All Files" }, { L"*.tim", L"*.pal", L"*.*" }); Filename.has_value())
	{
		CoUninitialize();

		std::uint32_t iLastPalette = Texture().GetClutCount();

		if (Texture().AddPalette(Filename.value()))
		{
			m_Palette = std::clamp(uint16_t(iLastPalette), uint16_t(0), Texture().GetClutMax());

			ResetTexture();
		}
	}
}

void Global_Application::InsertPalette(void)
{
	Texture().InsertPalette(++m_Palette);

	ResetTexture();
}

void Global_Application::DeletePalette(std::size_t iPalette)
{
	if (Texture().DeletePalette(iPalette))
	{
		if (m_Palette) { --m_Palette; }

		ResetTexture();
	}
}

void Global_Application::DeleteAllPalettes(void)
{
	Texture().DeleteAllPalettes();

	ResetTexture();
}

void Global_Application::DeleteAllPixels(void)
{
	Texture().DeleteAllPixels();

	ResetTexture();
}

void Global_Application::CopyPalette(void)
{
	ClipboardPalette = Texture().GetPalette()[m_Palette];
}

void Global_Application::PastePalette(void)
{
	if (ClipboardPalette.empty()) { return; }

	if (Texture().ImportPalette(ClipboardPalette, m_Palette))
	{
		ResetTexture();
	}
}

void Global_Application::ExportPalette(void)
{
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetSaveFilename({ L"Raw Palette Data" }, { L"*.dat" }); Filename.has_value())
	{
		CoUninitialize();

		Texture().ExportPalette(Filename.value());
	}
}

void Global_Application::ImportPalette(void)
{
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetOpenFilename({ L"All files" }, { L"*.*" }); Filename.has_value())
	{
		CoUninitialize();

		if (Texture().ImportPalette(Filename.value(), 0, uint16_t(MAX_TIM_PALETTE)))
		{
			m_Palette = 0;

			ResetTexture();
		}
	}
}

void Global_Application::ExportPalette(std::size_t iPalette)
{
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetSaveFilename({ L"Raw Palette Data" }, { L"*.dat" }); Filename.has_value())
	{
		CoUninitialize();

		Texture().ExportPalette(Filename.value(), iPalette);
	}
}

void Global_Application::ImportPalette(std::size_t iPalette)
{
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetOpenFilename({ L"All files" }, { L"*.*" }); Filename.has_value())
	{
		CoUninitialize();

		if (Texture().ImportPalette(Filename.value(), 0, size_t(iPalette)))
		{
			ResetTexture();
		}
	}
}

void Global_Application::ExportPaletteToTIM(void)
{
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetSaveFilename({ L"Sony Texture Image" }, { L"*.tim" }); Filename.has_value())
	{
		CoUninitialize();

		Texture().ExportPaletteToTIM(Filename.value());
	}
}

void Global_Application::ImportPaletteFromTIM(void)
{
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetOpenFilename({ L"Sony Texture Image" }, { L"*.tim" }); Filename.has_value())
	{
		CoUninitialize();

		if (Texture().ImportPaletteFromTIM(Filename.value()))
		{
			ResetTexture();
		}
	}
}

void Global_Application::ExportPaletteToPAL(void)
{
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetSaveFilename({ L"Microsoft RIFF Palette" }, { L"*.pal" }); Filename.has_value())
	{
		CoUninitialize();

		Texture().ExportMicrosoftPalette(Filename.value(), m_Palette);
	}
}

void Global_Application::ImportPaletteFromPAL(void)
{
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetOpenFilename({ L"Microsoft RIFF Palette" }, { L"*.pal" }); Filename.has_value())
	{
		CoUninitialize();

		if (Texture().ImportMicrosoftPalette(Filename.value(), m_Palette))
		{
			ResetTexture();
		}
	}
}

void Global_Application::ExportAllTextures(void)
{
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Directory = Window->GetFileDirectory(); Directory.has_value())
	{
		CoUninitialize();

		for (std::size_t i = 0; i < m_Files.size(); i++)
		{
			std::make_unique<Sony_PlayStation_Texture>(m_Filename, m_Files[i].first)->Save(
				Directory.value() / m_Filename.stem() / Str.FormatCStyle(L"%ws_%08d_%08llx.tim", m_Filename.stem().wstring().c_str(), i, m_Files[i].first));
		}
	}
}

void Global_Application::ExportAllTexturesToBitmap(void)
{
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Directory = Window->GetFileDirectory(); Directory.has_value())
	{
		CoUninitialize();

		for (std::size_t i = 0; i < m_Files.size(); i++)
		{
			std::make_unique<Sony_PlayStation_Texture>(m_Filename, m_Files[i].first)->GetBitmap()->SaveAsBitmap(
				Directory.value() / m_Filename.stem() / Str.FormatCStyle(L"%ws_%08d_%08llx.bmp", m_Filename.stem().wstring().c_str(), i, m_Files[i].first));
		}
	}
}