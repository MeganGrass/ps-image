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

	if (m_Texture) { m_Texture.reset(); m_Texture = nullptr; }

	if (Render->NormalState() && m_DXTexture) { m_DXTexture.reset(); }

	m_Palette = 0;

	m_Filename.clear();
	m_SelectedFile = 0;
	m_Files.clear();

	m_LastKnownFileCount = 0;

	b_ImageOnDisk = false;
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
			b_ImageOnDisk = false;
		}

		m_LastKnownFileCount = m_Files.size();

		m_SelectedFile = std::clamp(m_SelectedFile, (size_t)0, (size_t)m_Files.empty() ? 0 : m_Files.size() - 1);
		m_Files[m_SelectedFile].second = Texture().Size();

		m_Palette = std::clamp(m_Palette, (uint16_t)0, (uint16_t)Texture().GetPaletteMaxIndex());

		m_LastKnownBitsPerPixel = Texture().GetDepth();
		m_LastKnownWidth = Texture().GetWidth();
		m_LastKnownHeight = Texture().GetHeight();
		m_LastKnownPaletteCount = Texture().GetPaletteCount();

		Texture().Str.hWnd = Window->Get();

		Texture().UpdateTransparencyFlags();

		if (Render->NormalState())
		{
			m_DXTexture.reset(Render->CreateTexture(m_Texture, m_Palette, Texture().TransparencyFlags(), Texture().TransparencyColor()));

			m_DXTexture->GetLevelDesc(0, &m_TextureDesc);
		}
	}
	else { Close(); }
}

void Global_Application::SetPalette(std::uint16_t iPalette)
{
	if (!m_Texture || !m_Texture->GetCF()) { return; }

	m_Palette = std::clamp((iPalette > m_Texture->GetPaletteCount()) ? (uint16_t)0 : iPalette, (uint16_t)0, Texture().GetPaletteMaxIndex());

	b_RequestTextureReset = true;
}

void Global_Application::SetTexture(std::size_t iFile)
{
	if (m_Files.empty()) { return; }

	m_SelectedFile = std::clamp(iFile, (size_t)0, (size_t)m_Files.empty() ? 0 : m_Files.size() - 1);

	m_Palette = 0;

	m_Texture.reset();

	m_Texture = std::make_unique<Sony_PlayStation_Texture>();

	m_Texture->Str.hWnd = Window->Get();

	m_Texture->OpenTIM(m_Filename, File()[m_SelectedFile].first);

	b_RequestTextureReset = true;
}

void Global_Application::Search(std::filesystem::path Filename)
{
	Close();

	auto This = std::shared_ptr<Global_Application>(this, [](Global_Application*) {});

	auto ThreadWork = std::thread([This, Filename]()
		{
			This->m_ProgressBar = 0.0f;
			This->b_Searching = true;

			This->SearchModalCb = [&](float Progress, bool& b_Status) -> void
				{
					This->m_ProgressBar = Progress;
					b_Status = This->b_Searching;
				};

			std::function<void(std::filesystem::path, std::vector<std::pair<std::uintmax_t, std::uintmax_t>>&)> OnSearchCompleteCb =
				[&](std::filesystem::path Filename, std::vector<std::pair<std::uintmax_t, std::uintmax_t>>& FileList) -> void
				{
					This->OnSearchComplete(Filename, FileList);
				};

			std::unique_ptr<Sony_PlayStation_Texture> External = std::make_unique<Sony_PlayStation_Texture>();

			External->Str.hWnd = This->Window->Get();

			External->Search(Filename, 0, This->SearchModalCb, OnSearchCompleteCb);
		});

	SearchModalFunc = [This]() -> void { This->SearchModal(); };

	ThreadWork.detach();
}

void Global_Application::OnSearchComplete(std::filesystem::path Filename, std::vector<std::pair<std::uintmax_t, std::uintmax_t>>& FileList)
{
	b_Searching = false;

	if (FileList.empty()) { Str.Message(L"Sony PlayStation Texture Images (*.TIM) were not found in \"%ws\"", Filename.filename().wstring().c_str()); return; }

	Close();

	m_Files = FileList;

	m_Filename = Filename;

	SetTexture(m_PostSearchSelectedFile);

	b_ImageOnDisk = true;

	SearchModalFunc = []() {};

	SearchModalCb = [](float Progress, bool& Execute) {};
}

bool Global_Application::Create(void)
{
	std::unique_ptr<Sony_PlayStation_Texture> External = std::make_unique<Sony_PlayStation_Texture>();

	External->Str.hWnd = Window->Get();

	if (!External->Create(m_CreateInfo)) { return false; }

	Close();

	m_Texture = std::make_unique<Sony_PlayStation_Texture>();

	m_Texture->Str.hWnd = Window->Get();

	if (!m_Texture->OpenTIM(External)) { Close(); return false; }

	b_RequestTextureReset = true;

	return true;
}

void Global_Application::MovePalette(std::uint16_t iPalette, bool Right)
{
	if (!m_Texture) { return; }

	if (Texture().MovePalette(iPalette, Right))
	{
		std::uint16_t OldPalette = m_Palette;

		if (Right) { ++m_Palette; }
		else if (m_Palette) { --m_Palette; }

		if (OldPalette != m_Palette) { b_RequestTextureReset = true; }
	}
}

void Global_Application::CopyPalette(void)
{
	if (!m_Texture) { return; }
	Texture().CopyPalette(m_ClipboardPalette, m_Palette);
}

void Global_Application::PastePalette(void)
{
	if (!m_Texture || m_ClipboardPalette.empty()) { return; }
	if (Texture().PastePalette(m_ClipboardPalette, m_Palette)) { b_RequestTextureReset = true; }
}

void Global_Application::AddPalette(void)
{
	if (!m_Texture) { return; }
	Texture().AddPalette();
	m_Palette = std::clamp(Texture().GetPaletteMaxIndex(), (uint16_t)0, Texture().GetPaletteMaxIndex());
	b_RequestTextureReset = true;
}

void Global_Application::InsertPalette(void)
{
	if (!m_Texture) { return; }
	Texture().InsertPalette(++m_Palette);
	b_RequestTextureReset = true;
}

void Global_Application::DeletePalette(std::uint16_t iPalette, bool b_All)
{
	if (!m_Texture) { return; }
	if (Texture().DeletePalette(iPalette, b_All))
	{
		if (m_Palette) { --m_Palette; }
		b_RequestTextureReset = true;
	}
}

void Global_Application::DeletePixels(void)
{
	if (!m_Texture) { return; }
	Texture().SetWidth(0);
	Texture().SetHeight(0);
	b_RequestTextureReset = true;
}

void Global_Application::SetExternalPaletteFilename(void)
{
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetOpenFilename(
		{ L"All files", L"Sony Texture Image", L"Sony Texture CLUT", L"Bitmap Graphic", L"Microsoft RIFF Palette", L"Portable Network Graphics" },
		{ L"*.*", L"*.tim", L"*.clt", L"*.bmp", L"*.pal", L"*.png" }
	); Filename.has_value()) { m_CreateInfo.Palette = Filename.value(); }

	CoUninitialize();
}

void Global_Application::SetExternalPixelFilename(void)
{
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (auto Filename = Window->GetOpenFilename(
		{ L"All files", L"Sony Texture Image", L"Sony Texture Pixels", L"Bitmap Graphic", L"Portable Network Graphics", L"Joint Photographic Experts Group" },
		{ L"*.*", L"*.tim", L"*.pxl", L"*.bmp", L"*.png", L"*.jpg;*.jpeg" }
	); Filename.has_value()) { m_CreateInfo.Pixel = Filename.value(); }

	CoUninitialize();
}

void Global_Application::RawIO(std::filesystem::path Filename, ImageIO Flags, std::uintmax_t pSource, std::uint16_t iPalette, ImageType SaveAllType)
{
	if (!m_Texture) { return; }

	StringW m_FileExt = FS.GetFileExtension(Filename);
	Str.ToUpper(m_FileExt);

	bool b_IsFile = FS.IsFile(Filename);
	bool b_PathExists = FS.Exists(Filename);

	bool b_Raw = std::to_underlying(Flags) & std::to_underlying(ImageIO::Raw);
	bool b_Palette = std::to_underlying(Flags) & std::to_underlying(ImageIO::Palette);
	bool b_Pixel = std::to_underlying(Flags) & std::to_underlying(ImageIO::Pixel);
	bool b_PaletteSingle = std::to_underlying(Flags) & std::to_underlying(ImageIO::PaletteSingle);
	bool b_PaletteAdd = std::to_underlying(Flags) & std::to_underlying(ImageIO::PaletteAdd);
	bool b_OpenOnComplete = std::to_underlying(Flags) & std::to_underlying(ImageIO::OpenOnComplete);
	bool b_Truncate = std::to_underlying(Flags) & std::to_underlying(ImageIO::Truncate);
	bool b_Write = std::to_underlying(Flags) & std::to_underlying(ImageIO::Write);
	bool b_Import = std::to_underlying(Flags) & std::to_underlying(ImageIO::Import);
	bool b_SaveAs = std::to_underlying(Flags) & std::to_underlying(ImageIO::SaveAs);
	bool b_SaveAll = std::to_underlying(Flags) & std::to_underlying(ImageIO::SaveAll);

	if (!b_Palette && !b_Pixel) { Str.Message(L"I/O Error: cannot ignore both palette and pixel data"); return; }

	if (b_Pixel && (!Texture().GetWidth() || !Texture().GetHeight()))
	{
		Str.Message(L"I/O Error: texture has no resolution (%d x %d) for r/w raw pixels", Texture().GetWidth(), Texture().GetHeight());
		return;
	}

	if (b_Write)
	{
		if (!m_Texture || !m_Texture->IsValid()) { Str.Message(L"I/O Error: texture does not contain palette or pixel data"); return; }

		std::uintmax_t pPixel = pSource;

		if (b_Palette)
		{
			pPixel += b_PaletteSingle ? Texture().GetPaletteColorMax() : Texture().GetPaletteSize();
			if (pPixel % Texture().GetPaletteColorMax()) { pPixel = pPixel += (Texture().GetPaletteColorMax() - (pPixel % Texture().GetPaletteColorMax())); }
		}

		if (!Filename.has_extension()) { Filename += L".raw"; }

		if (b_Palette && !Texture().WritePalette(Filename, pSource, iPalette, !b_PaletteSingle, b_Truncate)) { return; }

		if (b_Pixel && !Texture().WritePixels(Filename, pPixel, b_Truncate)) { return; }

		if (b_Palette && b_Pixel)
		{
			Str.Message(L"Raw data saved to \"%ws\"\r\n\r\nPalette (0x%llx bytes) written to offset 0x%llx\r\n\r\nPixel (0x%llx bytes) written to offset 0x%llx",
				Filename.filename().wstring().c_str(),
				b_Palette && b_PaletteSingle ? Texture().GetPaletteSingleSize() : b_Palette ? Texture().GetPaletteSize() : 0, b_Palette ? pSource : 0,
				b_Pixel ? Texture().GetPixels().size() : 0, b_Pixel ? pPixel : 0);
		}

		return;
	}

	if (b_Import)
	{
		if (!m_Texture) { Str.Message(L"I/O Error: cannot import data, texture is not open"); return; }

		if (b_Palette && b_Pixel) { Str.Message(L"I/O Error: cannot simultaneously import raw palette and pixel data"); return; }

		if (b_Import && (!b_IsFile || !b_PathExists)) { Str.Message(L"I/O Error: cannot import data, \"%ws\" does not exist", Filename.c_str()); return; }

		bool b_Add = Texture().GetCF();

		std::uint16_t iLastPalette = Texture().GetPaletteCount();

		if (b_Palette && Texture().ReadPalette(Filename, pSource, b_PaletteSingle ? 1 : Texture().GetPaletteCountMax(), b_PaletteAdd, b_PaletteSingle, iPalette))
		{
			b_PaletteAdd ? m_Palette = iLastPalette : 0;
		}

		if (b_Pixel && Texture().ReadPixels(Filename, pSource, Texture().GetWidth(), Texture().GetHeight())) {}

		b_ImageOnDisk = false;

		b_RequestTextureReset = true;

		return;
	}
}

void Global_Application::TextureIO(std::filesystem::path Filename, ImageIO Flags, std::uintmax_t pSource, std::uint16_t iPalette, ImageType SaveAllType)
{
	StringW m_FileExt = FS.GetFileExtension(Filename);
	Str.ToUpper(m_FileExt);

	bool b_IsDirectory = FS.IsDirectory(Filename);
	bool b_PathExists = FS.Exists(Filename);

	bool b_Raw = std::to_underlying(Flags) & std::to_underlying(ImageIO::Raw);
	bool b_Palette = std::to_underlying(Flags) & std::to_underlying(ImageIO::Palette);
	bool b_Pixel = std::to_underlying(Flags) & std::to_underlying(ImageIO::Pixel);
	bool b_PaletteSingle = std::to_underlying(Flags) & std::to_underlying(ImageIO::PaletteSingle);
	bool b_PaletteAdd = std::to_underlying(Flags) & std::to_underlying(ImageIO::PaletteAdd);
	bool b_OpenOnComplete = std::to_underlying(Flags) & std::to_underlying(ImageIO::OpenOnComplete);
	bool b_Truncate = std::to_underlying(Flags) & std::to_underlying(ImageIO::Truncate);
	bool b_Write = std::to_underlying(Flags) & std::to_underlying(ImageIO::Write);
	bool b_Import = std::to_underlying(Flags) & std::to_underlying(ImageIO::Import);
	bool b_SaveAs = std::to_underlying(Flags) & std::to_underlying(ImageIO::SaveAs);
	bool b_SaveAll = std::to_underlying(Flags) & std::to_underlying(ImageIO::SaveAll);

	if (!b_Palette && !b_Pixel) { Str.Message(L"I/O Error: cannot ignore both palette and pixel data"); return; }

	bool b_SupportedSaveType = false;
	!b_SupportedSaveType ? b_SupportedSaveType = std::to_underlying(SaveAllType) & std::to_underlying(ImageType::TIM) : false;
	!b_SupportedSaveType ? b_SupportedSaveType = std::to_underlying(SaveAllType) & std::to_underlying(ImageType::CLT) : false;
	!b_SupportedSaveType ? b_SupportedSaveType = std::to_underlying(SaveAllType) & std::to_underlying(ImageType::PXL) : false;
	!b_SupportedSaveType ? b_SupportedSaveType = std::to_underlying(SaveAllType) & std::to_underlying(ImageType::BMP) : false;
	!b_SupportedSaveType ? b_SupportedSaveType = std::to_underlying(SaveAllType) & std::to_underlying(ImageType::PAL) : false;
#ifdef LIB_PNG
	!b_SupportedSaveType ? b_SupportedSaveType = std::to_underlying(SaveAllType) & std::to_underlying(ImageType::PNG) : false;
#endif
#ifdef LIB_JPEG
	!b_SupportedSaveType ? b_SupportedSaveType = std::to_underlying(SaveAllType) & std::to_underlying(ImageType::JPG) : false;
#endif

	bool b_KnownFileExt = false;
	!b_KnownFileExt ? b_KnownFileExt = (m_FileExt == L".TM2") ? true : false : false;
	!b_KnownFileExt ? b_KnownFileExt = (m_FileExt == L".TIM") ? true : false : false;
	!b_KnownFileExt ? b_KnownFileExt = (m_FileExt == L".CLT") ? true : false : false;
	!b_KnownFileExt ? b_KnownFileExt = (m_FileExt == L".PXL") ? true : false : false;
	!b_KnownFileExt ? b_KnownFileExt = (m_FileExt == L".BS") ? true : false : false;
	!b_KnownFileExt ? b_KnownFileExt = (m_FileExt == L".BMP") ? true : false : false;
	!b_KnownFileExt ? b_KnownFileExt = (m_FileExt == L".PAL") ? true : false : false;
	!b_KnownFileExt ? b_KnownFileExt = (m_FileExt == L".PNG") ? true : false : false;
	!b_KnownFileExt ? b_KnownFileExt = (m_FileExt == L".JPG" || m_FileExt == L".JPEG") ? true : false : false;

	bool b_DefaultFileType = (m_FileExt == L".TIM");

	if ((!b_KnownFileExt || b_DefaultFileType) && !b_Import && !b_Write && !b_SaveAs && !b_SaveAll) { m_PostSearchSelectedFile = 0; Search(Filename); return; }

	if ((m_FileExt == L".TM2") && b_SaveAs) { Str.Message(L"I/O Error: TIM2 output is not yet supported"); return; }

	if (b_SaveAll)
	{
		if (!b_SupportedSaveType) { Str.Message(L"I/O Error: cannot save all as unknown file type"); return; }

		if (!m_Texture || m_Files.empty()) { Str.Message(L"I/O Error: cannot export files, file list is empty"); return; }

		if (!b_IsDirectory || !b_PathExists) { Str.Message(L"I/O Error: cannot export files, \"%ws\" is not a directory", Filename.c_str()); return; }

		auto This = std::shared_ptr<Global_Application>(this, [](Global_Application*) {});

		auto ThreadWork = std::thread([This, Filename, pSource, iPalette, SaveAllType, b_Palette, b_Pixel, b_Truncate]() -> void
			{
				std::filesystem::path Directory = Filename / This->m_Filename.stem();

				std::size_t iCnt = 0;

				for (iCnt = 0; iCnt < This->m_Files.size(); iCnt++)
				{
					if (std::to_underlying(SaveAllType) & std::to_underlying(ImageType::TIM))
					{
						std::make_unique<Sony_PlayStation_Texture>(This->m_Filename, (size_t)This->File()[iCnt].first)->SaveTIM(
							Directory / This->Str.FormatCStyle(L"%ws_%04d_0x%08llx.tim", This->m_Filename.stem().wstring().c_str(), iCnt, This->File()[iCnt].first), NULL, b_Palette, b_Pixel, b_Truncate);
					}
					if (std::to_underlying(SaveAllType) & std::to_underlying(ImageType::CLT))
					{
						std::make_unique<Sony_PlayStation_Texture>(This->m_Filename, (size_t)This->File()[iCnt].first)->SaveCLT(
							Directory / This->Str.FormatCStyle(L"%ws_%04d_0x%08llx.clt", This->m_Filename.stem().wstring().c_str(), iCnt, This->File()[iCnt].first), NULL, b_Truncate);
					}
					if (std::to_underlying(SaveAllType) & std::to_underlying(ImageType::PXL))
					{
						std::make_unique<Sony_PlayStation_Texture>(This->m_Filename, (size_t)This->File()[iCnt].first)->SavePXL(
							Directory / This->Str.FormatCStyle(L"%ws_%04d_0x%08llx.pxl", This->m_Filename.stem().wstring().c_str(), iCnt, This->File()[iCnt].first), NULL, b_Truncate);
					}
					if (std::to_underlying(SaveAllType) & std::to_underlying(ImageType::BMP))
					{
						std::make_unique<Sony_PlayStation_Texture>(This->m_Filename, (size_t)This->File()[iCnt].first)->SaveBMP(
							Directory / This->Str.FormatCStyle(L"%ws_%04d_0x%08llx.bmp", This->m_Filename.stem().wstring().c_str(), iCnt, This->File()[iCnt].first), NULL, iPalette, b_Truncate);
					}
					if (std::to_underlying(SaveAllType) & std::to_underlying(ImageType::PAL))
					{
						std::make_unique<Sony_PlayStation_Texture>(This->m_Filename, (size_t)This->File()[iCnt].first)->SavePAL(
							Directory / This->Str.FormatCStyle(L"%ws_%04d_0x%08llx.pal", This->m_Filename.stem().wstring().c_str(), iCnt, This->File()[iCnt].first), NULL, b_Truncate);
					}
#ifdef LIB_PNG
					if (std::to_underlying(SaveAllType) & std::to_underlying(ImageType::PNG))
					{
						std::make_unique<Sony_PlayStation_Texture>(This->m_Filename, (size_t)This->File()[iCnt].first)->SavePNG(
							Directory / This->Str.FormatCStyle(L"%ws_%04d_0x%08llx.png", This->m_Filename.stem().wstring().c_str(), iCnt, This->File()[iCnt].first), NULL, iPalette, b_Truncate);
					}
#endif
#ifdef LIB_JPEG
					if (std::to_underlying(SaveAllType) & std::to_underlying(ImageType::JPG))
					{
						std::make_unique<Sony_PlayStation_Texture>(This->m_Filename, (size_t)This->File()[iCnt].first)->SaveJPEG(
							Directory / This->Str.FormatCStyle(L"%ws_%04d_0x%08llx.jpg", This->m_Filename.stem().wstring().c_str(), iCnt, This->File()[iCnt].first), NULL, iPalette, b_Truncate);
					}
#endif

					This->m_ProgressBar = ((float)(iCnt * 100) / (float)This->m_Files.size()) / 100.0f;
				}

				This->m_ProgressBar = ((float)(iCnt * 100) / (float)This->m_Files.size()) / 100.0f;

#ifdef _WINDOWS
				ShellExecute(NULL, L"open", Directory.c_str(), NULL, NULL, SW_SHOWDEFAULT);
#endif
			});

		SaveAllFunc = [This]() -> void { This->SaveAllModal(); };

		ThreadWork.detach();

		return;
	}

	if (b_SaveAs)
	{
		if (!m_Texture) { return; }

		if (!m_Texture->IsValid()) { Str.Message(L"I/O Error: texture does not contain palette or pixel data"); return; }

		if (b_KnownFileExt)
		{
			std::unique_ptr<Sony_PlayStation_Texture> External = std::make_unique<Sony_PlayStation_Texture>();

			External->Str.hWnd = Window->Get();

			if (!External->OpenTIM(m_Texture)) { return; }

			if (!b_Palette) { External->DeletePalette(NULL, true); }

			if (!b_Pixel) { External->SetWidth(NULL); External->SetHeight(NULL); }

			if (m_FileExt == L".TIM" && !External->SaveTIM(Filename, pSource, b_Palette, b_Pixel, b_Truncate)) { return; }
			if (m_FileExt == L".CLT" && !External->SaveCLT(Filename, pSource, b_Truncate)) { return; }
			if (m_FileExt == L".PXL" && !External->SavePXL(Filename, pSource, b_Truncate)) { return; }
			if (m_FileExt == L".BMP" && !External->SaveBMP(Filename, pSource, iPalette, b_Truncate)) { return; }
			if (m_FileExt == L".PAL" && !External->SavePAL(Filename, pSource, b_Truncate)) { return; }
#ifdef LIB_PNG
			if (m_FileExt == L".PNG" && !External->SavePNG(Filename, pSource, iPalette, b_Truncate)) { return; }
#endif
#ifdef LIB_JPEG
			if ((m_FileExt == L".JPG" || m_FileExt == L".JPEG") && !External->SaveJPEG(Filename, pSource, iPalette, b_Truncate)) { return; }
#endif

			if (b_OpenOnComplete)
			{
				m_Filename = Filename;

				TextureIO(Filename, ImageIO::All, pSource, iPalette);
			}

			return;
		}

		b_Raw = true;
		b_Import = false;
		b_Write = true;
	}

	if (!b_KnownFileExt || b_Raw) { RawIO(Filename, Flags, pSource, iPalette, SaveAllType); return; }

	std::unique_ptr<Sony_PlayStation_Texture> External = std::make_unique<Sony_PlayStation_Texture>();

	External->Str.hWnd = Window->Get();

	if (m_FileExt == L".TM2" && !External->OpenTIM2(Filename, pSource, b_Palette, b_Pixel)) { return; }
	if (m_FileExt == L".TIM" && !External->OpenTIM(Filename, pSource, b_Palette, b_Pixel)) { return; }
	if (m_FileExt == L".CLT" && !External->OpenCLT(Filename, pSource)) { return; }
	if (m_FileExt == L".PXL" && !External->OpenPXL(Filename, pSource)) { return; }
	if (m_FileExt == L".BS" && !External->OpenBS(Filename, pSource, m_BistreamWidth, m_BistreamHeight)) { return; }
	if (m_FileExt == L".BMP" && !External->OpenBMP(Filename, pSource)) { return; }
	if (m_FileExt == L".PAL" && !External->OpenPAL(Filename, pSource)) { return; }
#ifdef LIB_PNG
	if (m_FileExt == L".PNG" && !External->OpenPNG(Filename, pSource)) { return; }
#endif
#ifdef LIB_JPEG
	if ((m_FileExt == L".JPG" || m_FileExt == L".JPEG") && !External->OpenJPEG(Filename, pSource)) { return; }
#endif

	if (!b_Palette) { External->DeletePalette(NULL, true); }

	if (!b_Pixel) { External->SetWidth(NULL); External->SetHeight(NULL); }

	if (!External->IsValid()) { Str.Message(L"I/O Error: \"%ws\" does not contain required palette or pixel data", Filename.filename().wstring().c_str()); return; }

	if (b_Import || b_Write)
	{
		if (b_Palette && b_Import && (b_PaletteAdd || b_PaletteSingle) && !External->GetCF())
		{
			Str.Message(L"I/O Error: \"%ws\" does not contain palette data", Filename.filename().wstring().c_str()); return;
		}

		if (b_Pixel && !(b_PaletteAdd || b_PaletteSingle) && m_Texture && (Texture().GetDepth() != External->GetDepth()))
		{
			if (!Window->Question(L"Depth (%d) mismatch with (%d) of \"%ws\"\r\n\r\nContinue?",
				Texture().GetDepth(), External->GetDepth(), Filename.filename().wstring().c_str())) { return; }
		}

		if (b_Pixel && !(b_PaletteAdd || b_PaletteSingle) && m_Texture && (Texture().GetWidth() != External->GetWidth() || Texture().GetHeight() != External->GetHeight()))
		{
			if (!Window->Question(L"Resolution (%d x %d) mismatch with (%d x %d) of \"%ws\"\r\n\r\nContinue?",
				Texture().GetWidth(), Texture().GetHeight(), External->GetWidth(), External->GetHeight(), Filename.filename().wstring().c_str())) { return; }
		}
	}

	if (b_Import && b_Write)
	{
		if (m_Files.empty()) { Str.Message(L"I/O Error: cannot write to empty file list"); return; }

		if (!External->SaveTIM(m_Filename, File()[m_SelectedFile].first, b_Palette, b_Pixel, b_Truncate)) { return; }

		Search(m_Filename);

		return;
	}

	if (b_Write)
	{
	}

	if (b_Import)
	{
		if (!m_Texture) { Str.Message(L"I/O Error: cannot import data, texture is not open"); return; }

		if ((b_PaletteAdd || b_PaletteSingle) && !External->GetCF())
		{
			Str.Message(L"I/O Error: \"%ws\" does not contain palette data", Filename.filename().wstring().c_str());
			return;
		}

		std::vector<Sony_Pixel_16bpp> Palette = Texture().ConvertPalette(External->GetPalette(), External->GetPaletteColorMax(), Texture().GetPaletteColorMax());

		if (b_PaletteAdd)
		{
			bool b_Add = Texture().GetCF();

			std::uint16_t iLastPalette = Texture().GetPaletteCount();

			Texture().AddPalette(Palette);

			if (iLastPalette) { m_Palette = iLastPalette; }
		}
		else if (b_PaletteSingle)
		{
			if (m_Texture->GetPalette().empty() && (b_PaletteAdd || b_PaletteSingle))
			{
				Texture().SetPaletteWidth(Texture().GetPaletteColorMax());
				Texture().SetPaletteHeight(1);
				Texture().SetCF(true);
			}

			for (std::uint16_t i = 0; i < Texture().GetPaletteColorMax(); i++)
			{
				Texture().GetPalette()[(size_t)Texture().GetPalettePtr(iPalette) + i] = Palette[i];
			}
		}
		else if (b_Palette)
		{
			Texture().SetPaletteX(External->GetPaletteX());
			Texture().SetPaletteY(External->GetPaletteY());
			Texture().SetPaletteWidth(External->GetPaletteWidth());
			Texture().SetPaletteHeight(External->GetPaletteHeight());
			Texture().UpdatePaletteDataSize();
			Texture().GetPalette() = Palette;
		}

		uint16_t OldDepth = Texture().GetDepth();
		
		if (b_Pixel)
		{
			Texture().SetDepth(External->GetDepth());
			Texture().SetPixelX(External->GetPixelX());
			Texture().SetPixelY(External->GetPixelY());
			Texture().SetWidth(External->GetWidth());
			Texture().SetHeight(External->GetHeight());
			Texture().UpdatePixelDataSize();
			Texture().GetPixels() = External->GetPixels();
		}

		b_ImageOnDisk = false;

		b_RequestTextureReset = true;

		return;
	}

	{
		Close();

		m_Texture = std::make_unique<Sony_PlayStation_Texture>();

		m_Texture->Str.hWnd = Window->Get();

		if (!m_Texture->OpenTIM(External, b_Palette, b_Pixel)) { Close(); return; }

		b_ImageOnDisk = true;

		m_Filename = Filename;

		m_Files.resize(1);

		b_RequestTextureReset = true;
	}
}

void Global_Application::TextureIO(ImageIO Flags, std::uintmax_t pSource, std::uint16_t iPalette, ImageType SaveAllType)
{
	bool b_Import = std::to_underlying(Flags) & std::to_underlying(ImageIO::Import);
	bool b_Write = std::to_underlying(Flags) & std::to_underlying(ImageIO::Write);
	bool b_SaveAs = std::to_underlying(Flags) & std::to_underlying(ImageIO::SaveAs);
	bool b_SaveAll = std::to_underlying(Flags) & std::to_underlying(ImageIO::SaveAll);

	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return; }

	if (b_SaveAll)
	{
		if (auto Directory = Window->GetFileDirectory(); Directory.has_value())
		{ TextureIO(Directory.value(), Flags, pSource, iPalette, SaveAllType); }
	}
	else if ((b_SaveAs || b_Write) && !b_Import)
	{
		if (!m_Texture) { return; }

		if (m_Texture && !Texture().GetCF() && (!Texture().GetWidth() || !Texture().GetHeight())) { Str.Message(L"I/O Error: texture does not contain palette or pixel data"); return; }

		if (auto Filename = Window->GetSaveFilename(
			{ L"All files",
			  L"Sony Texture Image", L"Sony Texture CLUT", L"Sony Texture Pixels",
			  L"Bitmap Graphic", L"Microsoft RIFF Palette", L"Portable Network Graphics", L"Joint Photographic Experts Group" },
			{ L"*.*", L"*.tim", L"*.clt", L"*.pxl", L"*.bmp", L"*.pal", L"*.png", L"*.jpg;*.jpeg" }
		); Filename.has_value())
		{ TextureIO(Filename.value(), Flags, pSource, iPalette, SaveAllType); }
	}
	else
	{
		if (auto Filename = Window->GetOpenFilename(
			{ L"All files",
			  L"Sony Texture Image", L"Sony Texture CLUT", L"Sony Texture Pixels", L"Sony Bitstream", L"Sony Texture Image 2",
			  L"Bitmap Graphic", L"Microsoft RIFF Palette", L"Portable Network Graphics", L"Joint Photographic Experts Group" },
			{ L"*.*", L"*.tim", L"*.clt", L"*.pxl", L"*.bs", L"*.tm2", L"*.bmp", L"*.pal", L"*.png", L"*.jpg;*.jpeg" }
		); Filename.has_value())
		{ TextureIO(Filename.value(), Flags, pSource, iPalette, SaveAllType); }
	}

	CoUninitialize();
}