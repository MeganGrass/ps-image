/*
*
*	Megan Grass
*	December 14, 2024
*
*/

#include "app.h"

void Global_Application::Close(void)
{
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

void Global_Application::SetTexture(std::size_t iFile)
{
	if (m_Files.empty()) { return; }

	if (!Texture) { return; }

#if defined(_WIN64)
	m_SelectedFile = std::clamp(iFile, 0ULL, m_Files.size() - 1);
#else
	m_SelectedFile = std::clamp(iFile, 0U, m_Files.size() - 1);
#endif

	Texture.reset();
	Image.reset();

	m_Palette = 0;

	if (DXTexture)
	{
		DXTexture->Release();
		DXTexture = nullptr;
	}

	Texture = std::make_unique<Sony_PlayStation_Texture>(m_Filename, m_Files[m_SelectedFile].first);

	b_Transparency16bpp ? Texture->SetSTP16Bpp(true) : Texture->SetSTP16Bpp(false);

	if (Texture->IsOpen())
	{
		TextureInfo = Texture->Print();
		Image = Texture->GetBitmap();
		DXTexture = Render->CreateTexture(Image, b_Transparency, Texture->GetTransparentColor());
	}
	else
	{
		Close();
	}
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

	Standard_String Str;
	Standard_FileSystem FS;

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

		return;
	}

	std::unique_ptr<Sony_PlayStation_Texture> TextureUtility = std::make_unique<Sony_PlayStation_Texture>();
	m_Files = TextureUtility->Search(Filename);

	if (m_Files.empty())
	{
		Window->Message("Sony PlayStation textures were not found in %s", Filename.filename().string().c_str());
		m_Files.clear();
		return;
	}

	m_Filename = Filename;

	Texture = std::make_unique<Sony_PlayStation_Texture>(m_Filename, m_Files[m_SelectedFile].first);

	b_Transparency16bpp ? Texture->SetSTP16Bpp(true) : Texture->SetSTP16Bpp(false);

	if (Texture->IsOpen())
	{
		TextureInfo = Texture->Print();
		Image = Texture->GetBitmap();
		DXTexture = Render->CreateTexture(Image, b_Transparency, Texture->GetTransparentColor());
	}
	else
	{
		Close();
	}
}

void Global_Application::Create(std::filesystem::path Filename)
{
	Standard_FileSystem FS;

	bool b_Ready = true;

	std::unique_ptr<Sony_PlayStation_Texture> ExternalTexture = std::make_unique<Sony_PlayStation_Texture>();

	std::uintmax_t pPixel = m_RawPixelPtr;
	std::uintmax_t pPalette = m_RawPalettePtr;

	std::uint32_t Depth = b_Raw4bpp ? 4 : b_Raw8bpp ? 8 : b_Raw16bpp ? 16 : 0;

	Close();

	Texture = std::make_unique<Sony_PlayStation_Texture>(Depth, static_cast<std::uint16_t>(m_RawWidth), static_cast<std::uint16_t>(m_RawHeight), static_cast<std::uint16_t>(m_RawPaletteCount));

	b_Transparency16bpp ? Texture->SetSTP16Bpp(true) : Texture->SetSTP16Bpp(false);

	if (!b_RawCreateNew)
	{
		std::uintmax_t FileSize = FS.FileSize(Filename);
		if (Texture->GetPixels().size() > FileSize)
		{
			Window->Message(L"Error, pixel size (%X bytes) is larger than \"%ws\" file size (%X bytes)", Texture->GetPixels().size(), Filename.filename().wstring().c_str(), FileSize);
			b_Ready = false;
		}
	}

	if ((Depth == 4) || (Depth == 8))
	{
		if (b_RawExternalPaletteFromTIM)
		{
			ExternalTexture->Open(m_RawPaletteFilename);
			if (!ExternalTexture->IsOpen())
			{
				b_Ready = false;
			}

			if ((b_Ready) && (ExternalTexture->GetPalette().empty()))
			{
				Window->Message(L"Error, no Color Lookup Tables found in \"%ws\"", m_RawPaletteFilename.filename().wstring().c_str());
				b_Ready = false;
			}

			if ((b_Ready) && (!b_RawImportAllPalettesFromTIM))
			{
				if (m_RawPaletteCount > ExternalTexture->GetPalette().size())
				{
					Window->Message(L"Error, CLUT Amount (%d) is larger than the amount of Color Lookup Tables (%d) located in \"%ws\"", m_RawPaletteCount, ExternalTexture->GetPalette().size(), m_RawPaletteFilename.filename().wstring().c_str());
					b_Ready = false;
				}

				if (m_RawPaletteID > ExternalTexture->GetPalette().size() - 1)
				{
					Window->Message(L"Error, CLUT ID (%d) is out of range for Color Lookup Tables (%d max) located in \"%ws\"", m_RawPaletteID, ExternalTexture->GetPalette().size() - 1, m_RawPaletteFilename.filename().wstring().c_str());
					b_Ready = false;
				}

				if (m_RawPaletteID + m_RawPaletteCount > ExternalTexture->GetPalette().size())
				{
					Window->Message(L"Error, CLUT ID (%d) + CLUT Amount (%d) is out of range for Color Lookup Tables (%d max) located in \"%ws\"", m_RawPaletteID, m_RawPaletteCount, ExternalTexture->GetPalette().size() - 1, m_RawPaletteFilename.filename().wstring().c_str());
					b_Ready = false;
				}
			}

			if (b_Ready)
			{
				Texture.reset();
				std::size_t nClut = b_RawImportAllPalettesFromTIM ? nClut = ExternalTexture->GetPalette().size() : nClut = m_RawPaletteCount;
				Texture = std::make_unique<Sony_PlayStation_Texture>(Depth, static_cast<std::uint16_t>(m_RawWidth), static_cast<std::uint16_t>(m_RawHeight), static_cast<std::uint16_t>(nClut));

				b_Transparency16bpp ? Texture->SetSTP16Bpp(true) : Texture->SetSTP16Bpp(false);

				if (!b_RawImportAllPalettesFromTIM)
				{
					std::size_t x = m_RawPaletteID;
					for (std::size_t i = 0; i < m_RawPaletteCount; i++, x++)
					{
						if (ExternalTexture->GetDepth() != Depth)
						{
							Texture->GetPalette()[i] = ExternalTexture->GetConvertedPalette(Depth)[x];
						}
						else
						{
							Texture->GetPalette()[i] = ExternalTexture->GetPalette()[x];
						}
					}
				}
				else
				{
					if (ExternalTexture->GetDepth() != Depth)
					{
						Texture->GetPalette() = ExternalTexture->GetConvertedPalette(Depth);
					}
					else
					{
						Texture->GetPalette() = ExternalTexture->GetPalette();
					}
				}
			}
		}
		else if (b_RawCreateNew && !b_RawExternalPalette) {}
		else
		{
			StdFile m_Input{ b_RawExternalPalette ? m_RawPaletteFilename : b_RawCreateNew ? m_RawPaletteFilename : Filename, FileAccessMode::Read_Ex, true, false };
			if (!m_Input.IsOpen())
			{
				Window->Message(L"Error, could not open file \"%ws\" for reading palette data", m_RawPaletteFilename.filename().wstring().c_str());
				b_Ready = false;
			}

			std::uintmax_t PaletteFileSize = m_Input.Size();
			std::uintmax_t ClutSize = Depth == 4 ? 0x20 : Depth == 8 ? 0x200 : 0;
			if ((ClutSize * m_RawPaletteCount) > PaletteFileSize)
			{
				Window->Message(L"Error, CLUT size (%X bytes) is larger than \"%ws\" file size (%X bytes)", (ClutSize * m_RawPaletteCount), m_RawPaletteFilename.filename().wstring().c_str(), PaletteFileSize);
				b_Ready = false;
			}

			if (b_Ready)
			{
				for (std::size_t i = 0; i < Texture->GetPalette().size(); i++, pPalette += ClutSize)
				{
					b_Ready = Texture->ReadPalette(m_Input, pPalette, i);
				}
			}
		}
	}

	if (!b_Ready)
	{
		Texture.reset();
		Texture = nullptr;
	}
	else if (b_RawCreateNew)
	{
		m_Filename = Filename;

		TextureInfo = Texture->Print();
		Image = Texture->GetBitmap();
		DXTexture = Render->CreateTexture(Image, b_Transparency, Texture->GetTransparentColor());

		ImGui::CloseCurrentPopup();
		CreateModalFunc = []() {};
	}
	else
	{
		StdFile m_Input{ Filename, FileAccessMode::Read_Ex, true, false };
		if (!m_Input.IsOpen())
		{
			Texture.reset();
			Texture = nullptr;
			Window->Message(L"Error, could not open pixel file: \"%ws\"", Filename.filename().wstring().c_str());
		}
		else
		{
			std::filesystem::path Dir = FS.GetDirectory(Filename);
			StringW FileStem = FS.GetFileStem(Filename);
			m_Filename = Dir / FileStem += L".tim";

			Texture->ReadPixels(m_Input, pPixel, static_cast<std::uint32_t>(Texture->GetPixels().size()));

			TextureInfo = Texture->Print();
			Image = Texture->GetBitmap();
			DXTexture = Render->CreateTexture(Image, b_Transparency, Texture->GetTransparentColor());

			ImGui::CloseCurrentPopup();
			CreateModalFunc = []() {};
		}
	}
}

void Global_Application::Replace(void)
{
	if (!Texture) { return; }

	if (m_Files.empty()) { return; }

	if (auto Filename = Window->GetOpenFilename({ L"Sony Texture Image" }, { L"*.tim" }); Filename.has_value())
	{
		std::unique_ptr<Sony_PlayStation_Texture> ExternalTexture = std::make_unique<Sony_PlayStation_Texture>(Filename.value());
		if (!ExternalTexture->IsOpen())
		{
			return;
		}

		if (Texture->Size() != ExternalTexture->Size())
		{
			if (!Window->Question(L"Error, texture size (%X bytes) does not match \"%ws\" texture size (%X bytes), continue?", Texture->Size(), Filename.value().filename().wstring().c_str(), ExternalTexture->Size()))
			{
				return;
			}
		}

		StdFile m_Input{ m_Filename, FileAccessMode::Read_Ex, true, false };
		if (!m_Input.IsOpen())
		{
			Window->Message("Error, could not open file \"%ws\" for writing pixel data", m_Filename.filename().string().c_str());
			return;
		}

		std::vector<std::uint8_t> Pixels(Texture->Size(), 0);
		m_Input.Write(m_Files[m_SelectedFile].first, Pixels.data(), Pixels.size());

		ExternalTexture->Save(m_Input, m_Files[m_SelectedFile].first);

		Open(m_Filename);
	}
}

void Global_Application::ExtractPixelData(void)
{
	if (!Texture) { return; }

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

}

void Global_Application::ReplacePixelData(void)
{
	if (!Texture) { return; }

	if (auto Filename = Window->GetOpenFilename({ L"All files" }, { L"*.*" }); Filename.has_value())
	{
		StdFile m_Input{ Filename.value(), FileAccessMode::Read_Ex, true, false };
		if (!m_Input.IsOpen())
		{
			Window->Message("Error, could not open file \"%ws\" for reading pixel data", Filename.value().filename().wstring().c_str());
			return;
		}

		if (Texture->GetPixels().size() != m_Input.Size())
		{
			if (!Window->Question(L"Error, pixel size (%X bytes) does not match \"%ws\" file size (%X bytes), continue?", Texture->GetPixels().size(), Filename.value().filename().wstring().c_str(), m_Input.Size()))
			{
				return;
			}
		}

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

		if (DXTexture)
		{
			DXTexture->Release();
			DXTexture = nullptr;
		}

		Image.reset();
		TextureInfo = Texture->Print();
		Image = Texture->GetBitmap();
		DXTexture = Render->CreateTexture(Image, b_Transparency, Texture->GetTransparentColor());
	}

}

void Global_Application::AddPaletteData(void)
{
	if (!Texture) { return; }

	Texture->AddPalette();

	SetPalette(Texture->GetClutSize() - 1);
}

void Global_Application::InsertPaletteData(void)
{
	if (!Texture) { return; }

	Texture->InsertPalette(++m_Palette);

	SetPalette(m_Palette);
}

void Global_Application::DeletePaletteData(void)
{
	if (!Texture) { return; }

	if (Texture->GetPalette().empty()) { return; }

	if (Texture->GetClutSize() == 1) { return; }

	Texture->DeletePalette(m_Palette);

	SetPalette(--m_Palette);
}

void Global_Application::CopyPaletteData(void)
{
	if (!Texture) { return; }

	if (Texture->GetPalette().empty()) { return; }

	ClipboardPalette.resize(Texture->GetPalette()[m_Palette].size());

	std::memcpy(ClipboardPalette.data(), Texture->GetPalette()[m_Palette].data(), ClipboardPalette.size());
}

void Global_Application::PastePaletteData(void)
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

void Global_Application::ExtractPaletteData(std::size_t iPalette)
{
	if (!Texture) { return; }

	std::uint32_t Depth = Texture->GetDepth();

	if ((Depth != 4) && (Depth != 8)) { return; }

	iPalette = std::clamp(static_cast<size_t>(iPalette), static_cast<size_t>(0), static_cast<size_t>(Texture->GetClutSize() - 1));

	if (auto Filename = Window->GetSaveFilename({ L"Raw Palette Data" }, { L"*.dat" }); Filename.has_value())
	{
		StdFile m_Input{ Filename.value(), FileAccessMode::Write_Ex, true, true };
		if (!m_Input.IsOpen())
		{
			Window->Message("Error, could not open file \"%ws\" for writing palette data", Filename.value().filename().wstring().c_str());
			return;
		}

		m_Input.Write(0, Texture->GetPalette()[iPalette].data(), Texture->GetPalette()[iPalette].size());
	}
}

void Global_Application::ReplacePaletteData(std::size_t iPalette)
{
	if (!Texture) { return; }

	std::uint32_t Depth = Texture->GetDepth();

	if ((Depth != 4) && (Depth != 8)) { return; }

	iPalette = std::clamp(static_cast<size_t>(iPalette), static_cast<size_t>(0), static_cast<size_t>(Texture->GetClutSize() - 1));

	if (auto Filename = Window->GetOpenFilename({ L"All files" }, { L"*.*" }); Filename.has_value())
	{
		StdFile m_Input{ Filename.value(), FileAccessMode::Read_Ex, true, false };
		if (!m_Input.IsOpen())
		{
			Window->Message("Error, could not open file \"%ws\" for reading palette data", Filename.value().filename().wstring().c_str());
			return;
		}

		if (Texture->GetPalette()[iPalette].size() != m_Input.Size())
		{
			if (!Window->Question(L"Error, palette size (%X bytes) does not match \"%ws\" file size (%X bytes), continue?", Texture->GetPalette()[iPalette].size(), Filename.value().filename().wstring().c_str(), m_Input.Size()))
			{
				return;
			}
		}

		std::vector<std::uint8_t> Palette(static_cast<std::size_t>(m_Input.Size()));
		m_Input.Read(0, Palette.data(), Palette.size());

		if (Texture->GetPalette()[iPalette].size() < Palette.size())
		{
			std::memcpy(Texture->GetPalette()[iPalette].data(), Palette.data(), Texture->GetPalette()[iPalette].size());
		}
		else
		{
			std::memcpy(Texture->GetPalette()[iPalette].data(), Palette.data(), Palette.size());
		}

		if (DXTexture)
		{
			DXTexture->Release();
			DXTexture = nullptr;
		}

		Image.reset();
		TextureInfo = Texture->Print();
		Image = Texture->GetBitmap();
		DXTexture = Render->CreateTexture(Image, b_Transparency, Texture->GetTransparentColor());
	}
}

void Global_Application::ExtractAllTextures(void)
{
	if (m_Files.empty()) { return; }

	Standard_String Str;

	if (auto Directory = Window->GetFileDirectory(); Directory.has_value())
	{
		for (std::size_t i = 0; i < m_Files.size(); i++)
		{
			std::filesystem::path Filename = Directory.value() / m_Filename.stem() / Str.FormatCStyle(L"%ws_%08d_%08llx.tim", m_Filename.stem().wstring().c_str(), i, m_Files[i].first);
			std::unique_ptr<Sony_PlayStation_Texture> TextureUtility = std::make_unique<Sony_PlayStation_Texture>(m_Filename, m_Files[i].first);
			TextureUtility->Save(Filename);
		}
	}
}

void Global_Application::ExtractAllTexturesAsBitmap(void)
{
	if (m_Files.empty()) { return; }

	Standard_String Str;

	if (auto Directory = Window->GetFileDirectory(); Directory.has_value())
	{
		for (std::size_t i = 0; i < m_Files.size(); i++)
		{
			std::filesystem::path Filename = Directory.value() / m_Filename.stem() / Str.FormatCStyle(L"%ws_%08d_%08llx.bmp", m_Filename.stem().wstring().c_str(), i, m_Files[i].first);
			std::unique_ptr<Sony_PlayStation_Texture> TextureUtility = std::make_unique<Sony_PlayStation_Texture>(m_Filename, m_Files[i].first);
			TextureUtility->GetBitmap().get()->SaveAsBitmap(Filename);
		}
	}
}