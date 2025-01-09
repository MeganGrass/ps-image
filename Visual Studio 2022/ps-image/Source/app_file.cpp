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

	TextureInfo.clear();

	if (Texture)
	{
		Texture.reset();
		Texture = nullptr;
	}

	if (Image)
	{
		Image.reset();
		Image = nullptr;
	}

	if (DXTexture)
	{
		DXTexture->Release();
		DXTexture = nullptr;
	}

	m_Palette = 0;

	m_Filename.clear();
	m_SelectedFile = 0;
	m_Files.clear();
}

void Global_Application::ResetTexture(bool bTransparency)
{
	if (Image)
	{
		Image.reset();
		Image = nullptr;
	}

	if (DXTexture)
	{
		DXTexture->Release();
		DXTexture = nullptr;
	}

	if (!Texture)
	{
		if (!m_Files.empty())
		{
			Texture = std::make_unique<Sony_PlayStation_Texture>(m_Filename, m_Files[m_SelectedFile].first);
			if (!Texture->IsOpen())
			{
				Texture.reset();
				Texture = nullptr;
			}
		}
		else
		{
			Texture.reset();
			Texture = nullptr;
		}
	}

	if ((Texture) && (Texture->IsOpen()))
	{
		Texture->TransparencySuperimposed() = b_TransparencySuperimposed;

		if (Texture->GetDepth() > 8)
		{
			if (Texture->GetPalette().empty())
			{
				Texture->TransparencySuperimposed() = b_TransparencySuperimposed = false;
			}
			Texture->STP16Bpp() = b_Transparency;
		}
		else if (Texture->GetPalette().empty())
		{
			Texture->TransparencySuperimposed() = b_TransparencySuperimposed = b_Transparency = false;
		}

		TextureInfo = Texture->About();

		Image = Texture->GetBitmap();

		if (!Texture->GetPalette().empty())
		{
			Texture->UpdateBitmapPalette(Image, m_Palette);
		}

		DXTexture = Render->CreateTexture(Image, bTransparency, Texture->GetTransparentColor());
	}
	else
	{
		Close();
	}
}

void Global_Application::SetTexture(std::size_t iFile)
{
	if (m_Files.empty()) { return; }

	if (!Texture) { return; }

#if defined(_WIN64)
	m_SelectedFile = std::clamp(iFile, 0ULL, m_Files.size() - 1);
#else
	m_SelectedFile = std::clamp(iFile, 0U, m_Files.size() - 1);
#endif

	m_Palette = 0;

	Texture = std::make_unique<Sony_PlayStation_Texture>(m_Filename, m_Files[m_SelectedFile].first);

	ResetTexture(b_Transparency);
}

void Global_Application::SetPalette(std::uint32_t iPalette)
{
	if (!Texture) { return; }

	if (!Texture->GetClutSize()) { return; }

	iPalette = std::clamp(iPalette, 0U, Texture->GetClutSize() - 1);

	Texture->UpdateBitmapPalette(Image, m_Palette = iPalette);

	if (DXTexture)
	{
		DXTexture->Release();
		DXTexture = nullptr;
	}

	DXTexture = Render->CreateTexture(Image, b_Transparency, Texture->GetTransparentColor());
}

void Global_Application::Open(std::filesystem::path Filename)
{
	Close();

	StringW FileExtension = FS.GetFileExtension(Filename).wstring();
	Str.ToUpper(FileExtension);

	if (FileExtension == L".BS")
	{
		std::uintmax_t Offset = NULL;
		std::size_t FileSize = static_cast<std::size_t>(FS.FileSize(Filename));

		std::vector<std::uint8_t> BS(FileSize);
		StdFile m_Input{ Filename, FileAccessMode::Read_Ex, true, false };
		m_Input.Read(Offset, BS.data(), FileSize);

		struct BitStream_Header
		{
			std::uint16_t Length;
			std::uint16_t ID;
			std::uint16_t Q_Scale;
			std::uint16_t Version;
		};

		BitStream_Header Header{};

		std::vector<std::uint8_t> BMP(m_BistreamWidth * m_BistreamHeight * 4);
		std::memcpy(&Header, &BS.data()[0], sizeof(BitStream_Header));
		Bitstream->mdec_decode(&BS.data()[8], Header.Version, m_BistreamWidth, m_BistreamHeight, Header.Q_Scale, &BMP.data()[0]);

		Image = std::make_unique<Standard_Image>();
		Image->Create(ImageFormat::BMP, m_BistreamWidth, m_BistreamHeight, 32, 0);

		size_t pBmp = 0;
		for (int32_t y = 0; y < m_BistreamHeight; y++)
		{
			for (int32_t x = 0; x < m_BistreamWidth; x++, pBmp += 4)
			{
				uint32_t Color = BMP[pBmp + 2] | (BMP[pBmp + 1] << 8) | (BMP[pBmp + 0] << 16) | (BMP[pBmp + 3] << 24);
				Image->SetPixel(x, y, Color);
			}
		}

		DXTexture = Render->CreateTexture(Image);

		m_Filename = Filename;

		b_Transparency = b_TransparencySuperimposed = false;

		return;
	}

	std::unique_ptr<Sony_PlayStation_Texture> TextureUtility = std::make_unique<Sony_PlayStation_Texture>();
	m_Files = TextureUtility->Search(Filename);

	m_SelectedFile = 0;

	if (m_Files.empty())
	{
		Window->Message("Sony PlayStation textures were not found in %s", Filename.filename().string().c_str());
		m_Files.clear();
		return;
	}

	m_Filename = Filename;

	Texture = std::make_unique<Sony_PlayStation_Texture>(m_Filename, m_Files[m_SelectedFile].first);

	ResetTexture(b_Transparency);
}

void Global_Application::Open(void)
{
	if (SUCCEEDED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
	{
		if (auto Filename = Window->GetOpenFilename({ L"Sony Texture Image", L"Sony Bitstream", L"All files" }, { L"*.tim", L"*.bs", L"*.*" }); Filename.has_value())
		{
			Open(Filename.value());
		}

		CoUninitialize();
	}
}

void Global_Application::SetRawPixelFilename(void)
{
	if (SUCCEEDED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
	{
		StringW ButtonDesc = L"";
		StringW ButtonFilter = L"";

		b_RawExternalPalette ? ButtonDesc = L"All files" : b_RawExternalPaletteFromTIM ? ButtonDesc = L"Sony Texture Image" : ButtonDesc = L"All files";
		b_RawExternalPalette ? ButtonFilter = L"*.*" : b_RawExternalPaletteFromTIM ? ButtonFilter = L"*.tim" : ButtonFilter = L"*.*";

		if (auto Filename = Window->GetOpenFilename({ ButtonDesc.c_str() }, { ButtonFilter.c_str() }); Filename.has_value())
		{
			m_RawPixelFilename = Filename.value();
		}

		CoUninitialize();
	}
}

void Global_Application::SetRawPaletteFilename(void)
{
	if (SUCCEEDED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
	{
		StringW ButtonDesc = L"";
		StringW ButtonFilter = L"";

		b_RawExternalPalette ? ButtonDesc = L"All files" : b_RawExternalPaletteFromTIM ? ButtonDesc = L"Sony Texture Image" : ButtonDesc = L"All files";
		b_RawExternalPalette ? ButtonFilter = L"*.*" : b_RawExternalPaletteFromTIM ? ButtonFilter = L"*.tim" : ButtonFilter = L"*.*";

		if (auto Filename = Window->GetOpenFilename({ ButtonDesc.c_str() }, { ButtonFilter.c_str() }); Filename.has_value())
		{
			m_RawPaletteFilename = Filename.value();
		}

		CoUninitialize();
	}
}

bool Global_Application::Create(std::uint32_t Depth, std::uint16_t Width, std::uint16_t Height, std::uintmax_t pPixel, std::uintmax_t pPalette)
{
	if ((b_RawExternalPixels || b_RawExternalPixelsFromTIM))
	{
		if (m_RawPixelFilename.empty())
		{
			Window->Message(L"Error, pixel data file is not selected");
			return false;
		}

		if (!FS.Exists(m_RawPixelFilename))
		{
			Window->Message(L"Error, pixel data file \"%ws\" not found", m_RawPixelFilename.filename().wstring().c_str());
			return false;
		}
	}

	if ((b_RawExternalPalette || b_RawExternalPaletteFromTIM))
	{
		if (m_RawPaletteFilename.empty())
		{
			Window->Message(L"Error, palette data file is not selected");
			return false;
		}

		if (!FS.Exists(m_RawPaletteFilename))
		{
			Window->Message(L"Error, palette data file \"%ws\" not found", m_RawPaletteFilename.filename().wstring().c_str());
			return false;
		}
	}

	std::unique_ptr<Sony_PlayStation_Texture> TmpTexture = std::make_unique<Sony_PlayStation_Texture>(Depth, Width, Height, static_cast<std::uint16_t>(m_RawPaletteCount));

	TmpTexture->TransparencySuperimposed() = b_TransparencySuperimposed;

	TmpTexture->STP16Bpp() = b_Transparency;

	if ((b_RawExternalPixels) && (TmpTexture->GetPixels().size() > FS.FileSize(m_RawPixelFilename)))
	{
		Window->Message(L"Error, required pixel size (%llx bytes) is larger than \"%ws\" file size (%llx bytes)", TmpTexture->GetPixels().size(), m_RawPixelFilename.filename().wstring().c_str(), FS.FileSize(m_RawPixelFilename));
		return false;
	}

	if (b_RawExternalPalette)
	{
		StdFile m_Input{ m_RawPaletteFilename, FileAccessMode::Read_Ex, true, false };
		if (!m_Input.IsOpen())
		{
			Window->Message(L"Error, could not open file \"%ws\" for reading palette data", m_RawPaletteFilename.filename().wstring().c_str());
			return false;
		}

		std::uintmax_t PaletteFileSize = m_Input.Size();

		std::uintmax_t ClutSize = Depth == 4 ? 0x20 : Depth == 8 ? 0x200 : 0x200;

		if ((ClutSize * m_RawPaletteCount) > PaletteFileSize)
		{
			Window->Message(L"Error, required CLUT size (%X bytes) is larger than \"%ws\" file size (%X bytes)", (ClutSize * m_RawPaletteCount), m_RawPaletteFilename.filename().wstring().c_str(), PaletteFileSize);
			return false;
		}

		for (std::size_t i = 0; i < TmpTexture->GetPalette().size(); i++, pPalette += ClutSize)
		{
			TmpTexture->ImportPalette(m_Input, pPalette);
		}
	}
	else if (b_RawExternalPaletteFromTIM)
	{
		std::unique_ptr<Sony_PlayStation_Texture> ExternalTexture = std::make_unique<Sony_PlayStation_Texture>();

		ExternalTexture->Open(m_RawPaletteFilename);
		if (!ExternalTexture->IsOpen())
		{
			return false;
		}

		if (ExternalTexture->GetPalette().empty())
		{
			Window->Message(L"Error, no Color Lookup Tables found in \"%ws\"", m_RawPaletteFilename.filename().wstring().c_str());
			return false;
		}

		if (!b_RawImportAllPalettesFromTIM)
		{
			if (m_RawPaletteCount > ExternalTexture->GetPalette().size())
			{
				Window->Message(L"Error, CLUT Amount (%d) is larger than the amount of Color Lookup Tables (%d) located in \"%ws\"", m_RawPaletteCount, ExternalTexture->GetPalette().size(), m_RawPaletteFilename.filename().wstring().c_str());
				return false;
			}

			if (m_RawPaletteID > ExternalTexture->GetPalette().size() - 1)
			{
				Window->Message(L"Error, CLUT ID (%d) is out of range for Color Lookup Tables (%d max) located in \"%ws\"", m_RawPaletteID, ExternalTexture->GetPalette().size() - 1, m_RawPaletteFilename.filename().wstring().c_str());
				return false;
			}

			if (m_RawPaletteID + m_RawPaletteCount > ExternalTexture->GetPalette().size())
			{
				Window->Message(L"Error, CLUT ID (%d) + CLUT Amount (%d) is out of range for Color Lookup Tables (%d max) located in \"%ws\"", m_RawPaletteID, m_RawPaletteCount, ExternalTexture->GetPalette().size() - 1, m_RawPaletteFilename.filename().wstring().c_str());
				return false;
			}
		}

		std::uintmax_t nClut = b_RawImportAllPalettesFromTIM ? nClut = ExternalTexture->GetPalette().size() : nClut = m_RawPaletteCount;

		TmpTexture.reset();

		TmpTexture = std::make_unique<Sony_PlayStation_Texture>(Depth, Width, Height, static_cast<std::uint16_t>(nClut));

		TmpTexture->TransparencySuperimposed() = b_TransparencySuperimposed;

		TmpTexture->STP16Bpp() = b_Transparency;

		if (!b_RawImportAllPalettesFromTIM)
		{
			std::size_t x = static_cast<std::size_t>(m_RawPaletteID);

			for (std::size_t i = 0; i < m_RawPaletteCount; i++, x++)
			{
				if (ExternalTexture->GetDepth() != Depth)
				{
					TmpTexture->GetPalette()[i] = ExternalTexture->GetPalette(Depth)[x];
				}
				else
				{
					TmpTexture->GetPalette()[i] = ExternalTexture->GetPalette()[x];
				}
			}
		}
		else
		{
			if (ExternalTexture->GetDepth() != Depth)
			{
				TmpTexture->GetPalette() = ExternalTexture->GetPalette(Depth);
			}
			else
			{
				TmpTexture->GetPalette() = ExternalTexture->GetPalette();
			}
		}
	}

	std::pair<std::uintmax_t, std::uintmax_t> m_File = std::make_pair(0ULL, static_cast<std::uintmax_t>(TmpTexture->Size()));
	m_Files.push_back(m_File);

	bool b_TextureReset = false;

	if (b_RawExternalPixels)
	{
		StdFile m_Input{ m_RawPixelFilename, FileAccessMode::Read_Ex, true, false };
		if (!m_Input.IsOpen())
		{
			Window->Message(L"Error, could not open pixel file: \"%ws\"", m_RawPixelFilename.filename().wstring().c_str());
			return false;
		}

		TmpTexture->ImportPixels(m_Input, pPixel, static_cast<std::uint32_t>(TmpTexture->GetPixels().size()));

		TextureInfo = TmpTexture->About();
		Image = TmpTexture->GetBitmap();
		DXTexture = Render->CreateTexture(Image, b_Transparency, TmpTexture->GetTransparentColor());

		b_TextureReset = true;
	}
	else if (b_RawExternalPixelsFromTIM)
	{
		std::vector<std::vector<Sony_Texture_16bpp>> RequestedPalette = TmpTexture->GetPalette();

		TmpTexture.reset();

		TmpTexture = std::make_unique<Sony_PlayStation_Texture>(m_RawPixelFilename);
		if (!TmpTexture->IsOpen())
		{
			return false;
		}

		if (!RequestedPalette.empty())
		{
			TmpTexture->SetCF(true);
			TmpTexture->ImportPalette(RequestedPalette);
		}

		m_Files[0].second = static_cast<std::uintmax_t>(TmpTexture->Size());

		TextureInfo = TmpTexture->About();
		Image = TmpTexture->GetBitmap();
		DXTexture = Render->CreateTexture(Image, b_Transparency, TmpTexture->GetTransparentColor());

		b_TextureReset = true;
	}
	else
	{
		TextureInfo = TmpTexture->About();
		Image = TmpTexture->GetBitmap();
		DXTexture = Render->CreateTexture(Image, b_Transparency, TmpTexture->GetTransparentColor());

		b_TextureReset = true;
	}

	if (b_TextureReset)
	{
		Texture.reset();

		Texture = std::make_unique<Sony_PlayStation_Texture>(Depth, Width, Height, static_cast<std::uint16_t>(m_RawPaletteCount));

		Texture->TransparencySuperimposed() = b_TransparencySuperimposed;

		Texture->STP16Bpp() = b_Transparency;

		if (TmpTexture->GetPalette().empty())
		{
			Texture->SetCF(false);
		}
		else
		{
			Texture->SetCF(true);
			Texture->ImportPalette(TmpTexture->GetPalette());
		}

		Texture->ImportPixels(TmpTexture->GetPixels());

		m_Files.resize(1);

		m_Filename = L"untitled.tim";

		ResetTexture(b_Transparency = false);

		return true;
	}

	return false;
}

void Global_Application::SaveAs(void)
{
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
		StringW Ext = FS.GetFileExtension(Filename.value());

		if (Str.ToUpper(Ext) == L".TIM")
		{
			if (Texture)
			{
				if (Texture->GetPalette().empty() && Texture->GetPixels().empty())
				{
					Str.Message("PlayStation Texture: Error, palette and pixel data is empty");
				}
				else
				{
					StdFile m_Input{ Filename.value(), FileAccessMode::Write_Ex, true, true };
					if (!m_Input.IsOpen())
					{
						Window->Message("Error, could not open file \"%ws\" for writing pixel data", Filename.value().filename().wstring().c_str());
						return;
					}
					Texture->Save(m_Input);
				}
			}
			else
			{
				std::cout << "Error, conversion to TIM not supported" << std::endl;
			}
		}

		if (Str.ToUpper(Ext) == L".BMP")
		{
			if (Texture && !Texture->GetPixels().empty())
			{
				Texture->GetBitmap().get()->SaveAsBitmap(Filename.value());
			}
			else if (Image && (Image->GetWidth() && Image->GetHeight()))
			{
				Image->SaveAsBitmap(Filename.value());
			}
			else
			{
				std::cout << "Error, no image data to save" << std::endl;
			}
		}
	}

	CoUninitialize();
}

void Global_Application::SaveAsBitmap(std::size_t iFile)
{
	if (m_Files.empty()) { return; }

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

#if defined(_WIN64)
	iFile = std::clamp(iFile, 0ULL, m_Files.size() - 1);
#else
	iFile = std::clamp(iFile, 0U, m_Files.size() - 1);
#endif

	if (auto Filename = Window->GetSaveFilename({ L"Bitmap Graphic" }, { L"*.bmp" }); Filename.has_value())
	{
		if (Texture)
		{
			std::unique_ptr<Sony_PlayStation_Texture> TextureUtility = std::make_unique<Sony_PlayStation_Texture>(m_Filename, m_Files[iFile].first);
			TextureUtility->GetBitmap().get()->SaveAsBitmap(Filename.value());
		}
		else
		{
			Image->SaveAsBitmap(Filename.value());
		}
	}

	CoUninitialize();
}

void Global_Application::Export(std::size_t iFile)
{
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetSaveFilename({ L"Sony Texture Image" }, { L"*.tim" }); Filename.has_value())
	{
		if (Texture)
		{
			StdFile m_Input{ Filename.value(), FileAccessMode::Write_Ex, true, true };
			if (!m_Input.IsOpen())
			{
				Window->Message("Error, could not open file \"%ws\" for writing pixel data", Filename.value().filename().wstring().c_str());
				return;
			}
			Texture->Save(m_Input);
		}
	}

	CoUninitialize();
}

void Global_Application::Import(std::size_t iFile)
{
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetOpenFilename({ L"Sony Texture Image" }, { L"*.tim" }); Filename.has_value())
	{
		std::unique_ptr<Sony_PlayStation_Texture> ExternalTexture = std::make_unique<Sony_PlayStation_Texture>(Filename.value());
		if (!ExternalTexture->IsOpen())
		{
			return;
		}

		// need width and height for resize and/or conversion
		if ((Texture->GetWidth() != ExternalTexture->GetWidth()) || (Texture->GetHeight() != ExternalTexture->GetHeight()))
		{
			Str.Message(L"Error, texture resolution does not match, resize not yet supported");
			return;
		}

		// conversion not yet supported
		if ((Texture->GetDepth() != ExternalTexture->GetDepth()))
		{
			Str.Message(L"Error, texture depth does not match, conversion not yet supported");
			return;
		}

		/*if ((Texture->GetDepth() != ExternalTexture->GetDepth()))
		{
			if (!Window->Question(L"Texture depth (%d) does not match \"%ws\" depth (%d)\r\n\r\nContinue?",
				Texture->GetDepth(),
				Filename.value().filename().wstring().c_str(),
				ExternalTexture->GetDepth()))
			{
				return;
			}
		}*/

		/*if ((Texture->GetWidth() != ExternalTexture->GetWidth()) || (Texture->GetHeight() != ExternalTexture->GetHeight()))
		{
			if (!Window->Question(L"Texture resolution (%d x %d) does not match \"%ws\" resolution (%d x %d)\r\n\r\nContinue?",
				Texture->GetWidth(), Texture->GetHeight(),
				Filename.value().filename().wstring().c_str(),
				ExternalTexture->GetWidth(), ExternalTexture->GetHeight()))
			{
				return;
			}
		}*/

		std::size_t OldSelectedFile = m_SelectedFile;

		ExternalTexture->Save(m_Filename, m_Files[m_SelectedFile].first);

		Open(m_Filename);

#if defined(_WIN64)
		m_SelectedFile = std::clamp(OldSelectedFile, 0ULL, m_Files.size() - 1);
#else
		m_SelectedFile = std::clamp(OldSelectedFile, 0U, m_Files.size() - 1);
#endif

		SetTexture(m_SelectedFile);
	}

	CoUninitialize();
}

void Global_Application::ExportPixels(void)
{
	if (!Texture) { return; }

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetSaveFilename({ L"Raw Pixel Data" }, { L"*.pix" }); Filename.has_value())
	{
		StdFile m_Input{ Filename.value(), FileAccessMode::Write_Ex, true, true };
		if (!m_Input.IsOpen())
		{
			Window->Message("Error, could not open file \"%ws\" for writing pixel data", Filename.value().filename().wstring().c_str());
			return;
		}

		m_Input.Write(0, Texture->GetPixels().data(), Texture->GetPixels().size());
	}

	CoUninitialize();
}

void Global_Application::ImportPixels(void)
{
	if (!Texture) { return; }

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetOpenFilename({ L"All files" }, { L"*.*" }); Filename.has_value())
	{
		StdFile m_Input{ Filename.value(), FileAccessMode::Read_Ex, true, false };
		if (!m_Input.IsOpen())
		{
			Window->Message("Error, could not open file \"%ws\" for reading pixel data", Filename.value().filename().wstring().c_str());
			return;
		}

		// need width and height for resize
		if (Texture->GetPixels().empty() || Texture->GetPixels().size() > m_Input.Size())
		{
			Str.Message(L"Error, texture resolution does not match, resize not yet supported");
			return;
		}

		/*if (Texture->GetPixels().size() != m_Input.Size())
		{
			if (!Window->Question(L"Error, pixel size (%X bytes) does not match \"%ws\" file size (%X bytes)\r\n\r\nContinue?", Texture->GetPixels().size(), Filename.value().filename().wstring().c_str(), m_Input.Size()))
			{
				return;
			}
		}*/

		std::vector<std::uint8_t> Pixels(static_cast<std::size_t>(m_Input.Size()));

		m_Input.Read(0, Pixels.data(), Pixels.size());

		if (Texture->GetPixels().size() < Pixels.size())
		{
			std::memcpy(Texture->GetPixels().data(), Pixels.data(), Texture->GetPixels().size());
		}
		else
		{
			std::memcpy(Texture->GetPixels().data(), Pixels.data(), Pixels.size());
		}

		ResetTexture(b_Transparency);
	}

	CoUninitialize();

}

void Global_Application::ExportPixelsToTIM(void)
{
	if (!Texture) { return; }

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetSaveFilename({ L"Sony Texture Image" }, { L"*.tim" }); Filename.has_value())
	{
		bool bClutFlag = Texture->GetCF();

		Texture->SetCF(false);

		StdFile m_Input{ Filename.value(), FileAccessMode::Write_Ex, true, true };
		if (!m_Input.IsOpen())
		{
			Window->Message("Error, could not open file \"%ws\" for writing pixel data", Filename.value().filename().wstring().c_str());
			return;
		}

		Texture->Save(m_Input);

		Texture->SetCF(bClutFlag);
	}

	CoUninitialize();
}

void Global_Application::ImportPixelsFromTIM(void)
{
	if (!Texture) { return; }

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetOpenFilename({ L"Sony Texture Image" }, { L"*.tim" }); Filename.has_value())
	{
		std::unique_ptr<Sony_PlayStation_Texture> ExternalTexture = std::make_unique<Sony_PlayStation_Texture>(Filename.value());
		if (!ExternalTexture->IsOpen())
		{
			return;
		}

		if ((Texture->GetDepth() != ExternalTexture->GetDepth()))
		{
			Window->Message("Error, texture depth (%d) does not match \"%ws\" depth (%d)", Texture->GetDepth(), Filename.value().filename().wstring().c_str(), ExternalTexture->GetDepth());
			return;
		}

		if ((Texture->GetWidth() != ExternalTexture->GetWidth()) || (Texture->GetHeight() != ExternalTexture->GetHeight()))
		{
			if (!Window->Question(L"Warning, texture resolution (%d x %d) does not match \"%ws\" resolution (%d x %d)\r\n\r\nContinue?",
				Texture->GetWidth(), Texture->GetHeight(),
				Filename.value().filename().wstring().c_str(),
				ExternalTexture->GetWidth(), ExternalTexture->GetHeight()))
			{
				return;
			}
		}

		Texture->GetPixels().clear();

		Texture->SetWidth(ExternalTexture->GetWidth());
		Texture->SetHeight(ExternalTexture->GetHeight());

		Texture->GetPixels() = ExternalTexture->GetPixels();

		ResetTexture(b_Transparency);
	}

	CoUninitialize();
}

void Global_Application::AddPalette(void)
{
	if (!Texture) { return; }

	Texture->AddPalette();

	SetPalette(Texture->GetClutSize() - 1);
}

void Global_Application::InsertPalette(void)
{
	if (!Texture) { return; }

	Texture->InsertPalette(++m_Palette);

	SetPalette(m_Palette);
}

void Global_Application::DeletePalette(void)
{
	if (!Texture) { return; }

	if (Texture->GetPalette().empty()) { return; }

	Texture->DeletePalette(m_Palette);

	if (Texture->GetPalette().empty())
	{
		m_Palette = 0;
		m_Files[m_SelectedFile].second = static_cast<std::uintmax_t>(Texture->Size());
		ResetTexture(b_Transparency = false);
	}
	else
	{
		SetPalette(--m_Palette);
	}
}

void Global_Application::CopyPalette(void)
{
	if (!Texture) { return; }

	if (Texture->GetPalette().empty()) { return; }

	ClipboardPalette.resize(Texture->GetPalette()[m_Palette].size() * sizeof(Sony_Texture_16bpp));

	std::memcpy(ClipboardPalette.data(), Texture->GetPalette()[m_Palette].data(), ClipboardPalette.size());
}

void Global_Application::PastePalette(void)
{
	if (!Texture) { return; }

	if (Texture->GetPalette().empty()) { return; }

	if (ClipboardPalette.empty()) { return; }

	for (std::size_t x = 0; x < Texture->GetPalette()[m_Palette].size(); x++)
	{
		if (x >= ClipboardPalette.size()) { break; }
		Texture->GetPalette()[m_Palette][x] = ClipboardPalette[x];
	}

	SetPalette(m_Palette);
}

void Global_Application::ExportPalette(void)
{
	if (!Texture) { return; }

	if (Texture->GetPalette().empty()) { return; }

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetSaveFilename({ L"Raw Palette Data" }, { L"*.dat" }); Filename.has_value())
	{
		Texture->ExportPalette(Filename.value());
	}

	CoUninitialize();
}

void Global_Application::ImportPalette(void)
{
	if (!Texture) { return; }

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetOpenFilename({ L"All files" }, { L"*.*" }); Filename.has_value())
	{
		StdFile m_Input{ Filename.value(), FileAccessMode::Read_Ex, true, false };
		if (!m_Input.IsOpen())
		{
			Window->Message("Error, could not open file \"%ws\" for reading palette data", Filename.value().filename().wstring().c_str());
			return;
		}

		std::vector<std::uint8_t> Source(static_cast<size_t>(m_Input.Size()));

		m_Input.Read(0, Source.data(), Source.size());

		std::vector<std::vector<Sony_Texture_16bpp>> _Palette = Texture->ConvertToPalette(Source);

		if (!_Palette.empty())
		{
			m_Palette = 0;

			Texture->ImportPalette(_Palette);

			ResetTexture(b_Transparency);
		}
	}

	CoUninitialize();
}

void Global_Application::ExportPalette(std::size_t iPalette)
{
	if (!Texture) { return; }

	if (Texture->GetPalette().empty()) { return; }

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	std::uint32_t Depth = Texture->GetDepth();

	iPalette = std::clamp(static_cast<size_t>(iPalette), static_cast<size_t>(0), static_cast<size_t>(Texture->GetClutSize() - 1));

	if (auto Filename = Window->GetSaveFilename({ L"Raw Palette Data" }, { L"*.dat" }); Filename.has_value())
	{
		StdFile m_Input{ Filename.value(), FileAccessMode::Write_Ex, true, true };
		if (!m_Input.IsOpen())
		{
			Window->Message("Error, could not open file \"%ws\" for writing palette data", Filename.value().filename().wstring().c_str());
			return;
		}

		m_Input.Write(0, Texture->GetPalette()[iPalette].data(), Texture->GetPalette()[iPalette].size() * sizeof(Sony_Texture_16bpp));
	}

	CoUninitialize();
}

void Global_Application::ImportPalette(std::size_t iPalette)
{
	if (!Texture) { return; }

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	std::uint32_t Depth = Texture->GetDepth();

	if (Texture->GetPalette().empty())
	{
		Texture->SetCF(true);
		Texture->AddPalette();
		iPalette = m_Palette = 0;
	}

	iPalette = std::clamp(static_cast<size_t>(iPalette), static_cast<size_t>(0), static_cast<size_t>(Texture->GetClutSize() - 1));

	if (auto Filename = Window->GetOpenFilename({ L"All files" }, { L"*.*" }); Filename.has_value())
	{
		StdFile m_Input{ Filename.value(), FileAccessMode::Read_Ex, true, false };
		if (!m_Input.IsOpen())
		{
			Window->Message("Error, could not open file \"%ws\" for reading palette data", Filename.value().filename().wstring().c_str());
			return;
		}

		std::vector<std::uint8_t> Source(static_cast<size_t>(m_Input.Size()));

		m_Input.Read(0, Source.data(), Source.size());

		std::vector<std::vector<Sony_Texture_16bpp>> _Palette = Texture->ConvertToPalette(Source);

		if (!_Palette.empty())
		{
			Texture->ImportPalette(_Palette[0], iPalette);

			ResetTexture(b_Transparency);
		}
	}

	CoUninitialize();
}

void Global_Application::ExportPaletteToTIM(void)
{
	if (!Texture) { return; }

	std::uint32_t Depth = Texture->GetDepth();

	if (Texture->GetPalette().empty()) { return; }

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetSaveFilename({ L"Sony Texture Image" }, { L"*.tim" }); Filename.has_value())
	{
		std::unique_ptr<Sony_PlayStation_Texture> ExternalTexture = std::make_unique<Sony_PlayStation_Texture>(Depth, NULL, NULL, static_cast<std::uint16_t>(Texture->GetClutSize()));
		if (!ExternalTexture->IsOpen())
		{
			return;
		}

		ExternalTexture->GetClutX() = Texture->GetClutX();
		ExternalTexture->GetClutY() = Texture->GetClutY();

		ExternalTexture->ImportPalette(Texture->GetPalette());

		StdFile m_Input{ Filename.value(), FileAccessMode::Write_Ex, true, true };
		if (!m_Input.IsOpen())
		{
			Window->Message("Error, could not open file \"%ws\" for writing palette data", Filename.value().filename().wstring().c_str());
			return;
		}

		ExternalTexture->Save(m_Input);
	}

	CoUninitialize();
}

void Global_Application::ImportPaletteFromTIM(void)
{
	if (!Texture) { return; }

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetOpenFilename({ L"Sony Texture Image" }, { L"*.tim" }); Filename.has_value())
	{
		std::unique_ptr<Sony_PlayStation_Texture> ExternalTexture = std::make_unique<Sony_PlayStation_Texture>(Filename.value());
		if (!ExternalTexture->IsOpen())
		{
			return;
		}

		Texture->SetCF(true);

		if ((Texture->GetDepth() != ExternalTexture->GetDepth()))
		{
			Texture->ImportPalette(ExternalTexture->GetPalette(ExternalTexture->GetDepth()));
		}
		else
		{
			Texture->ImportPalette(ExternalTexture->GetPalette());
		}

		if (Texture->GetPalette().empty())
		{
			Texture->SetCF(false);
		}

		ResetTexture(b_Transparency);
	}

	CoUninitialize();
}

void Global_Application::ExportPaletteToPAL(void)
{
	if (!Texture) { return; }

	if (Texture->GetPalette().empty()) { return; }

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetSaveFilename({ L"Microsoft RIFF Palette" }, { L"*.pal" }); Filename.has_value())
	{
		Texture->ExportMicrosoftPalette(Filename.value(), m_Palette);
	}

	CoUninitialize();
}

void Global_Application::ImportPaletteFromPAL(void)
{
	if (!Texture) { return; }

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetOpenFilename({ L"Microsoft RIFF Palette" }, { L"*.pal" }); Filename.has_value())
	{
		if (Texture->ImportMicrosoftPalette(Filename.value(), m_Palette))
		{
			ResetTexture(b_Transparency);
		}
	}

	CoUninitialize();
}

void Global_Application::ExportAllTextures(void)
{
	if (m_Files.empty()) { return; }

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Directory = Window->GetFileDirectory(); Directory.has_value())
	{
		for (std::size_t i = 0; i < m_Files.size(); i++)
		{
			std::filesystem::path Filename = Directory.value() / m_Filename.stem() / Str.FormatCStyle(L"%ws_%08d_%08llx.tim", m_Filename.stem().wstring().c_str(), i, m_Files[i].first);
			std::unique_ptr<Sony_PlayStation_Texture> TextureUtility = std::make_unique<Sony_PlayStation_Texture>(m_Filename, m_Files[i].first);
			TextureUtility->Save(Filename);
		}
	}

	CoUninitialize();
}

void Global_Application::ExportAllTexturesToBitmap(void)
{
	if (m_Files.empty()) { return; }

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Directory = Window->GetFileDirectory(); Directory.has_value())
	{
		for (std::size_t i = 0; i < m_Files.size(); i++)
		{
			std::filesystem::path Filename = Directory.value() / m_Filename.stem() / Str.FormatCStyle(L"%ws_%08d_%08llx.bmp", m_Filename.stem().wstring().c_str(), i, m_Files[i].first);
			std::unique_ptr<Sony_PlayStation_Texture> TextureUtility = std::make_unique<Sony_PlayStation_Texture>(m_Filename, m_Files[i].first);
			TextureUtility->GetBitmap().get()->SaveAsBitmap(Filename);
		}
	}

	CoUninitialize();
}