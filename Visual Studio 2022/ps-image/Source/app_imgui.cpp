/*
*
*	Megan Grass
*	December 14, 2024
*
*/

#include "app.h"

void Global_Application::Tooltip(String Tip)
{
	ImGui::SameLine(); ImGui::TextDisabled("(?)");
	if (ImGui::BeginItemTooltip())
	{
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(Tip.c_str());
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void Global_Application::TooltipOnHover(String Tip)
{
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
	{
		ImGui::BeginTooltip();
		ImGui::Text(Tip.c_str());
		ImGui::EndTooltip();
	}
}

void Global_Application::MainMenu(void)
{
	bool b_MenuPaletteEnabled = false;
	if ((Texture) && (!Texture->GetPalette().empty()))
	{
		b_MenuPaletteEnabled = true;
	}

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File##MenuWindow"))
		{
			if (ImGui::MenuItem("Open##FileMenu", "CTRL+O"))
			{
				if (auto Filename = Window->GetOpenFilename({ L"Sony Texture Image", L"Sony Bitstream", L"All files" }, { L"*.tim", L"*.bs", L"*.*" }); Filename.has_value())
				{
					Open(Filename.value());
				}
			}
			if (ImGui::MenuItem("Open RAW##FileMenu", "CTRL+R"))
			{
				CreateModalFunc = [this]()
					{
						b_RawCreateNew = false;
						CreateModal();
					};
			}
			ImGui::Separator();
			if (ImGui::MenuItem("New##FileMenu", "CTRL+N"))
			{
				CreateModalFunc = [this]()
					{
						b_RawCreateNew = true;
						CreateModal();
					};
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Close##FileMenu", NULL, nullptr, Image ? true : false))
			{
				Close();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Save##FileMenu", "CTRL+S", nullptr, Texture ? true : false))
			{
				if (auto Filename = Window->GetSaveFilename({ L"Sony Texture Image" }, { L"*.tim" }); Filename.has_value())
				{
					Texture->Save(Filename.value());
				}
			}
			if (ImGui::MenuItem("Save As Bitmap##FileMenu", NULL, nullptr, Image ? true : false))
			{
				if (Texture)
				{
					if (auto Filename = Window->GetSaveFilename({ L"Bitmap Graphic" }, { L"*.bmp" }); Filename.has_value())
					{
						Texture->GetBitmap().get()->SaveAsBitmap(Filename.value());
					}
				}
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Save All##FileMenu", NULL, nullptr, m_Files.size() > 1 ? true : false))
			{
				ExtractAllTextures();
			}
			if (ImGui::MenuItem("Save All As Bitmap##FileMenu", NULL, nullptr, m_Files.size() > 1 ? true : false))
			{
				ExtractAllTexturesAsBitmap();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Extract Pixel Data##FileMenu", NULL, nullptr, Texture ? true : false))
			{
				ExtractPixelData();
			}
			if (ImGui::MenuItem("Replace Pixel Data##FileMenu", NULL, nullptr, Texture ? true : false))
			{
				ReplacePixelData();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Extract Palette Data##FileMenu", NULL, nullptr, b_MenuPaletteEnabled))
			{
				ExtractPaletteData(m_Palette);
			}
			if (ImGui::MenuItem("Replace Palette Data##FileMenu", NULL, nullptr, b_MenuPaletteEnabled))
			{
				ReplacePaletteData(m_Palette);
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit##FileMenu", "ESC"))
			{
				G->SaveConfig();
				DestroyWindow(Window->Get());
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View##MenuWindow"))
		{
			if (ImGui::MenuItem("Fullscreen##ViewMenu", "F11"))
			{
				Window->AutoFullscreen();
			}
			ImGui::Separator();
			ImGui::MenuItem("Window Options##ViewMenu", NULL, &b_ViewWindowOptions);
			ImGui::Separator();
			ImGui::MenuItem("Image Options##ViewMenu", NULL, &b_ViewImageOptions); // , Image ? true : false);
			ImGui::MenuItem("Palette Editor##ViewMenu", NULL, &b_ViewPaletteWindow, b_MenuPaletteEnabled);
			ImGui::MenuItem("File Properties##ViewMenu", NULL, &b_ViewFileWindow, m_Files.empty() ? false : true);
			ImGui::Separator();
			ImGui::MenuItem("Bitstream Options##ViewMenu", NULL, &b_ViewBitstreamOptions);
			ImGui::Separator();
			if (ImGui::MenuItem("Next Palette##ViewMenu", "RIGHT", nullptr, b_MenuPaletteEnabled))
			{
				SetPalette(++m_Palette);
			}
			if (ImGui::MenuItem("Prev Palette##ViewMenu", "LEFT", nullptr, b_MenuPaletteEnabled))
			{
				SetPalette(--m_Palette);
			}
			if (ImGui::MenuItem("Next Image##ViewMenu", "DOWN", nullptr, m_Files.size() > 1 ? true : false))
			{
				SetTexture(++m_SelectedFile);
			}
			if (ImGui::MenuItem("Prev Image##ViewMenu", "UP", nullptr, m_Files.size() > 1 ? true : false))
			{
				SetTexture(--m_SelectedFile);
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help##MenuWindow"))
		{
			if (ImGui::MenuItem("Controls##HelpMenu", NULL))
			{
				Standard_String Str;
				String AboutStr = Str.FormatCStyle("Esc -- Exit Application");
				AboutStr += Str.FormatCStyle("\r\nF11 -- Fullscreen");
				AboutStr += Str.FormatCStyle("\r\nCTRL+O -- Open Sony Texture Image");
				AboutStr += Str.FormatCStyle("\r\nCTRL+R -- Open Raw Sony Texture Image data");
				AboutStr += Str.FormatCStyle("\r\nCTRL+N -- Create Sony Texture Image");
				AboutStr += Str.FormatCStyle("\r\nCTRL+S -- Save Sony Texture Image");
				AboutStr += Str.FormatCStyle("\r\nLeft/Right -- Previous/Next Palette");
				AboutStr += Str.FormatCStyle("\r\nUp/Down -- Previous/Next Texture");
				AboutStr += Str.FormatCStyle("\r\nCTRL+Mousewheel -- Image Zoom");

				std::wstring WideStr = std::wstring(AboutStr.begin(), AboutStr.end());

				G->Window->MessageModal(L"Keyboard and Mouse Controls", L"", WideStr);
			}
			ImGui::Separator();
			if (ImGui::MenuItem("About##HelpMenu", "ALT+?"))
			{
				About();
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	CreateModalFunc();
}

void Global_Application::Toolbar(void)
{
	if (!b_ViewToolbar) { return; }

	if (ImGui::BeginViewportSideBar("Toolbar##ToolbarWindow",
		(ImGuiViewportP*)(void*)ImGui::GetMainViewport(),
		ImGuiDir_Up,
		GetToolbarHeight(),
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoDecoration))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

		ImVec4 ColorF = ImVec4(
			GetRValue(Window->GetColor()) + 32 / 255.0f,
			GetGValue(Window->GetColor()) + 32 / 255.0f,
			GetBValue(Window->GetColor()) + 32 / 255.0f,
			GetAValue(Window->GetColor()) + 32 / 255.0f);

		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ColorF);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ColorF);

		ImVec2 uv0 = ImVec2(0.0f, 0.0f);
		ImVec2 uv1 = ImVec2(64.0f / 320.0f, 64.0f / 64.0f);
		if (ImGui::ImageButton("Open##ToolbarOpen", (ImTextureID)(intptr_t)ToolbarIcons, ImVec2(64, 64), uv0, uv1))
		{
			if (auto Filename = Window->GetOpenFilename({ L"Sony Texture Image", L"Sony Bitstream", L"All files" }, { L"*.tim", L"*.bs", L"*.*" }); Filename.has_value())
			{
				Open(Filename.value());
			}
		}
		TooltipOnHover("Open File");

		ImGui::SameLine();
		uv0 = ImVec2(64.0f / 320.0f, 0.0f);
		uv1 = ImVec2(128.0f / 320.0f, 64.0f / 64.0f);
		if (ImGui::ImageButton("New##ToolbarNew", (ImTextureID)(intptr_t)ToolbarIcons, ImVec2(64, 64), uv0, uv1))
		{
			CreateModalFunc = [this]()
				{
					b_RawCreateNew = true;
					CreateModal();
				};
		}
		TooltipOnHover("Create New");

		ImGui::SameLine();
		uv0 = ImVec2(128.0f / 320.0f, 0.0f);
		uv1 = ImVec2(192.0f / 320.0f, 64.0f / 64.0f);
		if (ImGui::ImageButton("Close##ToolbarClose", (ImTextureID)(intptr_t)ToolbarIcons, ImVec2(64, 64), uv0, uv1))
		{
			Close();
		}
		TooltipOnHover("Close File");

		ImGui::SameLine();
		uv0 = ImVec2(192.0f / 320.0f, 0.0f);
		uv1 = ImVec2(256.0f / 320.0f, 64.0f / 64.0f);
		if (ImGui::ImageButton("Save##ToolbarSave", (ImTextureID)(intptr_t)ToolbarIcons, ImVec2(64, 64), uv0, uv1))
		{
			if (Texture)
			{
				if (auto Filename = Window->GetSaveFilename({ L"Sony Texture Image" }, { L"*.tim" }); Filename.has_value())
				{
					Texture->Save(Filename.value());
				}
			}
		}
		TooltipOnHover("Save as Sony Texture Image");

		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar();

		ImGui::End();
	}
}

void Global_Application::Statusbar(void)
{
	if (!b_ViewStatusbar) { return; }

	if (ImGui::BeginViewportSideBar("StatusBar##StatusBarWindow",
		(ImGuiViewportP*)(void*)ImGui::GetMainViewport(),
		ImGuiDir_Down,
		ImGui::GetFrameHeight(),
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar))
	{
		if (ImGui::BeginMenuBar())
		{
			if (Image)
			{
				Standard_String Str;

				float textWidth = ImGui::CalcTextSize(m_Filename.filename().string().c_str()).x;
				ImGui::Text(m_Filename.filename().string().c_str());
				ImGui::Indent(textWidth);

				textWidth = ImGui::CalcTextSize(Str.FormatCStyle("  |  %d x %d  |  %dbpp", Image->GetWidth(), Image->GetHeight(), Image->GetDepth()).c_str()).x;
				ImGui::Text(Str.FormatCStyle("  |  %d x %d  |  %dbpp", Image->GetWidth(), Image->GetHeight(), Image->GetDepth()).c_str());
				ImGui::Indent(textWidth);

				if ((Texture) && (Texture->GetClutSize()))
				{
					textWidth = ImGui::CalcTextSize(Str.FormatCStyle("palette: %d / %d ", m_Palette, Texture->GetClutSize() - 1).c_str()).x;
					ImGui::SetCursorPosX(ImGui::GetWindowWidth() - textWidth);
					ImGui::Text(Str.FormatCStyle("palette: %d / %d", m_Palette, Texture->GetClutSize() - 1).c_str());
				}
			}
			else
			{
				ImGui::Text("Ready");
			}
			ImGui::EndMenuBar();
		}
		ImGui::End();
	}
}

void Global_Application::Options(void)
{
	if (!b_ViewWindowOptions) { return; }

	Standard_String Str;

	ImGui::Begin("Window Options##OptionsWindow", &b_ViewWindowOptions, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);

	if (ImGui::BeginCombo("Font##OptionsWindow", Window->FontList()[Window->GetFontIndex()].filename().stem().string().c_str()))
	{
		for (std::size_t i = 0; i < Window->FontList().size(); i++)
		{
			bool b_IsSelected = (Window->GetFontIndex() == i);

			if (ImGui::Selectable(Window->FontList()[i].filename().stem().string().c_str(), b_IsSelected))
			{
				b_FontChangeRequested = true;
				Window->GetFontIndex() = i;
				Window->SetFont(Window->FontList()[Window->GetFontIndex()], Window->FontSize());
			}

			if (b_IsSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	{
		float OldFontSize = Window->FontSize();

		if (ImGui::ArrowButton("FontSizeSub##OptionsWindow", ImGuiDir_Left))
		{
			--Window->FontSize();
		}
		TooltipOnHover("Decrease the font size");

		ImGui::SameLine();

		if (ImGui::ArrowButton("FontSizeAdd##OptionsWindow", ImGuiDir_Right))
		{
			++Window->FontSize();
		}
		TooltipOnHover("Increase the font size");

		ImGui::SameLine();
		ImGui::Text("Font Size: %.1f", OldFontSize);

		Window->FontSize() = std::clamp(Window->FontSize(), m_FontSizeMin, m_FontSizeMax);

		if (OldFontSize != Window->FontSize())
		{
			b_FontChangeRequested = true;
		}
	}

	ImGui::Checkbox("Tool Bar##OptionsWindow", &b_ViewToolbar);

	ImGui::Checkbox("Status Bar##OptionsWindow", &b_ViewStatusbar);

	auto OldRed = GetRValue(Window->GetColor());
	auto OldGreen = GetGValue(Window->GetColor());
	auto OldBlue = GetBValue(Window->GetColor());

	ImVec4 ColorF = ImVec4(
		GetRValue(Window->GetColor()) / 255.0f,
		GetGValue(Window->GetColor()) / 255.0f,
		GetBValue(Window->GetColor()) / 255.0f,
		GetAValue(Window->GetColor()) / 255.0f);

	ImGui::ColorEdit3(Str.FormatCStyle("Options##WindowColor").c_str(), (float*)&ColorF, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoOptions);
	ImGui::SameLine(); ImGui::Text("Window Color");

	COLORREF Color = RGB(
		static_cast<BYTE>(ColorF.x * 255.0f),
		static_cast<BYTE>(ColorF.y * 255.0f),
		static_cast<BYTE>(ColorF.z * 255.0f));

	auto Red = GetRValue(Color);
	auto Green = GetGValue(Color);
	auto Blue = GetBValue(Color);

	if ((Red != OldRed) || (Green != OldGreen) || (Blue != OldBlue))
	{
		Window->SetColor(GetRValue(Color), GetGValue(Color), GetBValue(Color), true);
	}

	ImGui::Checkbox("Open File on Boot##OptionsWindow", &b_OpenLastFileOnBoot);
	TooltipOnHover("Reopen the previous file on boot");

	if (ImGui::Button("Reset W/H##OptionsWindow"))
	{
		int DefaultWidth = 1024 + static_cast<int>(ImGui::GetFrameHeightWithSpacing()) * 2;
		int DefaultHeight = 512 + static_cast<int>(ImGui::GetFrameHeightWithSpacing()) * 4 + 160;
		RECT Rect = { 0, 0, DefaultWidth, DefaultHeight };
		Window->Resize(&Rect);
	}
	TooltipOnHover("Reset the width and height");

	ImGui::SetWindowSize(ImVec2(ImGui::GetItemRectMax().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetItemRectMax().y + ImGui::GetStyle().WindowPadding.y));

	ImGui::End();
}

void Global_Application::File(void)
{
	if (!b_ViewFileWindow) { return; }

	if (m_Files.empty()) { return; }

	Standard_String Str;

	static int nColumn = 2;

	ImGui::Begin("File##FileWindow", &b_ViewFileWindow, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);

	if (ImGui::BeginTable("Data##FileData", nColumn,
		ImGuiTableFlags_ContextMenuInBody |
		ImGuiTableFlags_Borders |
		ImGuiTableFlags_RowBg |
		ImGuiTableFlags_BordersH |
		ImGuiTableFlags_BordersV |
		ImGuiTableFlags_BordersInner |
		ImGuiTableFlags_BordersOuter |
		ImGuiTableFlags_BordersInnerH |
		ImGuiTableFlags_BordersInnerV |
		ImGuiTableFlags_BordersOuterH |
		ImGuiTableFlags_BordersOuterV))
	{
		ImGui::TableSetupColumn("Offset##FileOffset");
		ImGui::TableSetupColumn("Size##FileSize");
		ImGui::TableHeadersRow();

		std::size_t OldSelectedFile = m_SelectedFile;

		for (std::size_t Row = 0; Row < m_Files.size(); Row++)
		{
			ImGui::TableNextRow();

			for (int Column = 0; Column < nColumn; Column++)
			{
				ImGui::PushID(Column);

				ImGui::TableSetColumnIndex(0);
				if (ImGui::Selectable(Str.FormatCStyle("0x%llX##FileOffset%d", m_Files[Row].first, Row).c_str(), m_SelectedFile == Row, ImGuiSelectableFlags_SpanAllColumns))
				{
					m_SelectedFile = Row;
				}

				if (ImGui::BeginPopupContextItem(Str.FormatCStyle("0x%llX##FileOffset%d", m_Files[Row].first, Row).c_str()))
				{
					if (m_SelectedFile != Row) { SetTexture(m_SelectedFile = Row); }

					if (Texture)
					{
						ImGui::Text(TextureInfo.c_str());

						if (ImGui::Button("Extract##FileWindow", ImVec2(ImGui::CalcTextSize("Extract##FileWindow").x, 0)))
						{
							if (auto Filename = Window->GetSaveFilename({ L"Sony Texture Image" }, { L"*.tim" }); Filename.has_value())
							{
								Texture->Save(Filename.value());
							}
						}
						TooltipOnHover("Extract the Sony PlayStation texture image");

						ImGui::SameLine();

						if (ImGui::Button("Replace##FileWindow", ImVec2(ImGui::CalcTextSize("Replace##FileWindow").x, 0)))
						{
							Replace();
						}
						TooltipOnHover("Replace the Sony PlayStation texture image");

						if (ImGui::Button("Extract Pixels##FileWindow", ImVec2(ImGui::CalcTextSize("Extract##FileWindow").x, 0)))
						{
							ExtractPixelData();
						}
						TooltipOnHover("Extract the pixel data to raw file data");

						ImGui::SameLine();

						if (ImGui::Button("Replace Pixels##FileWindow", ImVec2(ImGui::CalcTextSize("Replace##FileWindow").x, 0)))
						{
							ReplacePixelData();
						}
						TooltipOnHover("Replace the pixel data with raw file data");

						if (ImGui::Button("Save As Bitmap##FileWindow", ImVec2(ImGui::CalcTextSize("Extract##FileWindow").x, 0)))
						{
							if (auto Filename = Window->GetSaveFilename({ L"Bitmap Graphic" }, { L"*.bmp" }); Filename.has_value())
							{
								Texture->GetBitmap().get()->SaveAsBitmap(Filename.value());
							}
						}
						TooltipOnHover("Save the texture as a Bitmap file");
					}

					ImGui::EndPopup();
				}

				ImGui::TableSetColumnIndex(1);
				ImGui::Text(Str.FormatCStyle("0x%llX", m_Files[Row].second).c_str());

				ImGui::PopID();
			}
		}

		if (OldSelectedFile != m_SelectedFile) { SetTexture(m_SelectedFile); }

		ImGui::EndTable();
	}

	ImGui::SetWindowSize(ImVec2(ImGui::GetItemRectMax().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetItemRectMax().y + m_Files.size()));

	ImGui::End();
}

void Global_Application::Palette(void)
{
	if (!b_ViewPaletteWindow) { return; }

	if (!Texture) { return; }

	if (Texture->GetPalette().empty()) { return; }

	Standard_String Str;

	auto& io = ImGui::GetIO();
	std::uint16_t nColor = Texture->GetDepth() == 4 ? 16 : 256;
	std::uint32_t OldPalette = m_Palette;

	ImGui::Begin("Palette##PaletteWindow", &b_ViewPaletteWindow, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);

	float SliderIntWidth = ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("  ID##PaletteWindow").x / 3 - ImGui::GetStyle().ItemInnerSpacing.x * 2;
	ImGui::SetNextItemWidth(SliderIntWidth);
	ImGui::SliderInt("  ID##PaletteWindow", (int*)&m_Palette, 0, Texture->GetClutSize() - 1);
	Tooltip("Active Color Lookup Table\r\n\r\nLeft and Right arrow keys can also be used");
	if (OldPalette != m_Palette) { SetPalette(m_Palette); }

	{
		ImGui::BeginDisabled(!(Texture->GetClutSize() > 1));
		ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, false);

		if (ImGui::ArrowButton("MovePrev##PaletteWindow", ImGuiDir_Left))
		{
			std::uint32_t iPalette = std::clamp(m_Palette - 1, 0U, Texture->GetClutSize() - 1);
			std::vector<Sony_Texture_16bpp> NextPalette = Texture->GetPalette()[iPalette];
			Texture->GetPalette()[iPalette] = Texture->GetPalette()[m_Palette];
			Texture->GetPalette()[m_Palette] = NextPalette;
			SetPalette(iPalette);
		}
		TooltipOnHover(Str.FormatCStyle("Move the currently selected Color Lookup Table (%d) to the previous index (%d)", m_Palette, std::clamp(m_Palette - 1, 0U, Texture->GetClutSize() - 1)));

		ImGui::SameLine();

		if (ImGui::ArrowButton("MoveNext##PaletteWindow", ImGuiDir_Right))
		{
			std::uint32_t iPalette = std::clamp(m_Palette + 1, 0U, Texture->GetClutSize() - 1);
			std::vector<Sony_Texture_16bpp> NextPalette = Texture->GetPalette()[iPalette];
			Texture->GetPalette()[iPalette] = Texture->GetPalette()[m_Palette];
			Texture->GetPalette()[m_Palette] = NextPalette;
			SetPalette(iPalette);
		}
		TooltipOnHover(Str.FormatCStyle("Move the currently selected Color Lookup Table (%d) to the next index (%d)", m_Palette, std::clamp(m_Palette + 1, 0U, Texture->GetClutSize() - 1)));

		ImGui::SameLine();
		ImGui::Text("Move");
		Tooltip("Reorganize the Color Lookup Table index using the left and right buttons");

		ImGui::PopItemFlag();
		ImGui::EndDisabled();
	}

	if (ImGui::Button("Extract##PaletteWindow"))
	{
		ExtractPaletteData(m_Palette);
	}
	TooltipOnHover(Str.FormatCStyle("Extract the currently selected Color Lookup Table (%d) to raw file data", m_Palette));

	ImGui::SameLine();

	if (ImGui::Button("Replace##PaletteWindow"))
	{
		ReplacePaletteData(m_Palette);
	}
	TooltipOnHover(Str.FormatCStyle("Replace the currently selected Color Lookup Table (%d) with raw file data", m_Palette));

	ImGui::SameLine();

	if (ImGui::Button("Copy##PaletteWindow"))
	{
		CopyPaletteData();
	}
	TooltipOnHover(Str.FormatCStyle("Copy the currently selected Color Lookup Table (%d) to the clipboard", m_Palette));

	ImGui::SameLine();

	if (ImGui::Button("Paste##PaletteWindow"))
	{
		PastePaletteData();
	}
	TooltipOnHover(Str.FormatCStyle("Paste the clipboard to the currently selected Color Lookup Table (%d)", m_Palette));

	ImGui::SameLine();

	if (ImGui::Button("Add##PaletteWindow"))
	{
		AddPaletteData();
	}
	TooltipOnHover(Str.FormatCStyle("Append a new (blank) Color Lookup Table after the max (%d)", Texture->GetClutSize() - 1));

	ImGui::SameLine();

	if (ImGui::Button("Insert##PaletteWindow"))
	{
		InsertPaletteData();
	}
	TooltipOnHover(Str.FormatCStyle("Insert a new (blank) Color Lookup Table after the currently selected (%d)", m_Palette));

	ImGui::SameLine();

	if (ImGui::Button("Delete##PaletteWindow"))
	{
		DeletePaletteData();
	}
	TooltipOnHover(Str.FormatCStyle("Delete the currently selected Color Lookup Table (%d)", m_Palette));

	for (std::uint16_t x = 0; x < nColor; x++)
	{
		float Alpha = 1.0f;

		if ((!Texture->GetPalette()[m_Palette][x].R) && (!Texture->GetPalette()[m_Palette][x].G) && (!Texture->GetPalette()[m_Palette][x].B) && (!Texture->GetPalette()[m_Palette][x].STP))
		{
			Alpha = 0.0f;
		}

		ImVec4 ColorF = ImVec4(
			Texture->Red(Texture->GetPalette()[m_Palette][x]) / 255.0f,
			Texture->Green(Texture->GetPalette()[m_Palette][x]) / 255.0f,
			Texture->Blue(Texture->GetPalette()[m_Palette][x]) / 255.0f,
			Alpha);

		if (x % 16) { ImGui::SameLine(); }

		ImGui::ColorEdit4(Str.FormatCStyle("##%d", x).c_str(), (float*)&ColorF, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoOptions);

		COLORREF Color = RGB(
			static_cast<BYTE>(ColorF.x * 255.0f),
			static_cast<BYTE>(ColorF.y * 255.0f),
			static_cast<BYTE>(ColorF.z * 255.0f));

		auto Red = GetRValue(Color);
		auto Green = GetGValue(Color);
		auto Blue = GetBValue(Color);

		if ((Red != Texture->Red(Texture->GetPalette()[m_Palette][x])) || (Green != Texture->Green(Texture->GetPalette()[m_Palette][x])) || (Blue != Texture->Blue(Texture->GetPalette()[m_Palette][x])))
		{
			Texture->GetPalette()[m_Palette][x].R = ((Red >> 3) & 0x1F);
			Texture->GetPalette()[m_Palette][x].G = ((Green >> 3) & 0x1F);
			Texture->GetPalette()[m_Palette][x].B = ((Blue >> 3) & 0x1F);
			SetPalette(m_Palette);
		}
	}

	ImGui::SetWindowSize(ImVec2(ImGui::GetItemRectMax().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetItemRectMax().y + ImGui::GetStyle().WindowPadding.y));

	ImGui::End();
}

void Global_Application::BitstreamSettings(void)
{
	if (!b_ViewBitstreamOptions) { return; }

	ImGui::Begin("Bitstream Options##BitstreamSettings", &b_ViewBitstreamOptions, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);

	ImGui::SetNextItemWidth(ImGui::CalcTextSize("Width##BitstreamSettings").x);
	if (ImGui::InputInt("Width##BitstreamSettings", &m_BistreamWidth))
	{
		m_BistreamWidth = std::clamp(m_BistreamWidth, 1, 640);
	}
	TooltipOnHover("Width of Sony Bitstream (*.bs) image");

	ImGui::SetNextItemWidth(ImGui::CalcTextSize("Width##BitstreamSettings").x);
	if (ImGui::InputInt("Height##BitstreamSettings", &m_BistreamHeight))
	{
		m_BistreamHeight = std::clamp(m_BistreamHeight, 1, 480);
	}
	TooltipOnHover("Height of Sony Bitstream (*.bs) image");

	Tooltip("Width and Height of Sony Bitstream (*.bs) image\r\n\r\nValues are used when opening a Bitstream image");

	ImGui::SetWindowSize(ImVec2(ImGui::GetItemRectMax().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetItemRectMax().y + ImGui::GetStyle().WindowPadding.y));

	ImGui::End();
}

void Global_Application::ImageSettings(void)
{
	if (!b_ViewImageOptions) { return; }

	ImGui::Begin("Image Options##UtilityWindow", &b_ViewImageOptions, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);

	ImGui::Checkbox("Dithering##ImageDithering", &b_Dithering);
	Tooltip("Sony PlayStation (1994) Dithering Pixel Shader\r\n\r\nPreview only; effects are not written to TIM data");

	ImGui::SliderFloat("  Zoom##ImageZoom", (float*)&m_ImageZoom, m_ImageZoomMin, m_ImageZoomMax);
	TooltipOnHover("CTRL + mousewheel can also be used");

	ImGui::Checkbox("Caption##ImageCaption", &b_ImageWindowTitleBar);
	Tooltip("Enable/Disable the \"Image\" window title bar");

	ImGui::Checkbox("Background##ImageBackground", &b_ImageWindowBackground);
	Tooltip("Enable/Disable the \"Image\" window background\r\n\r\nWhen disabled, main window background can be used for transparency (4/8bpp)");

	bool b_TransparencyControls = false;
	if ((Texture) && (b_Transparency16bpp || (Texture->GetDepth() == 4) || (Texture->GetDepth() == 8)))
	{
		b_TransparencyControls = true;
	}
	ImGui::BeginDisabled(!b_TransparencyControls);
	{
		Standard_String Str;

		bool OldTransparency = b_Transparency;
		ImGui::Checkbox("Transparency##ImageTransparency", &b_Transparency);
		Tooltip("Display Transparent Color\r\n\r\nPreview only; effects are not written to TIM data");

		Sony_Texture_16bpp TransparentColor = Texture->Create16bpp(Texture->GetTransparentColor(), false);

		auto OldRed = GetRValue(Texture->GetTransparentColor());
		auto OldGreen = GetGValue(Texture->GetTransparentColor());
		auto OldBlue = GetBValue(Texture->GetTransparentColor());

		ImVec4 ColorF = ImVec4(
			Texture->Red(TransparentColor) / 255.0f,
			Texture->Green(TransparentColor) / 255.0f,
			Texture->Blue(TransparentColor) / 255.0f,
			0.0f);

		ImGui::SameLine();
		ImGui::ColorEdit3(Str.FormatCStyle("##ImageSettingsTransparency").c_str(), (float*)&ColorF, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoOptions);
		ImGui::SameLine();
		Tooltip("Transparent Color represents solid black pixels in Sony Texture Image\r\n\r\nPreview only; effects are not written to TIM data");

		COLORREF Color = RGB(
			static_cast<BYTE>(ColorF.x * 255.0f),
			static_cast<BYTE>(ColorF.y * 255.0f),
			static_cast<BYTE>(ColorF.z * 255.0f));

		auto Red = GetRValue(Color);
		auto Green = GetGValue(Color);
		auto Blue = GetBValue(Color);

		if ((OldTransparency != b_Transparency) || (Red != OldRed) || (Green != OldGreen) || (Blue != OldBlue))
		{
			Texture->GetTransparentColor() = RGB(Red, Green, Blue);

			if (Texture->GetDepth() == 16)
			{
				Image.reset();

				if (DXTexture)
				{
					DXTexture->Release();
					DXTexture = nullptr;
				}

				Texture->SetSTP16Bpp(b_Transparency16bpp);

				Image = Texture->GetBitmap();
				DXTexture = Render->CreateTexture(Image, b_Transparency, Texture->GetTransparentColor());
			}
			else
			{
				SetPalette(m_Palette);
			}
		}
	}
	ImGui::EndDisabled();

	ImGui::SameLine();
	bool OldTransparency16bpp = b_Transparency16bpp;
	bool b_TransparencyControls16bpp = false;
	if ((Texture) && (Texture->GetDepth() == 16))
	{
		b_TransparencyControls16bpp = true;
	}
	ImGui::BeginDisabled(!b_TransparencyControls16bpp);
	ImGui::Checkbox("16bpp##ImageTransparency", &b_Transparency16bpp);
	Tooltip("Enable/Disable transparency for 16bpp textures\r\n\r\nPreview only; effects are not written to TIM data");
	ImGui::EndDisabled();

	if ((OldTransparency16bpp != b_Transparency16bpp) && ((Texture) && (Texture->GetDepth() == 16)))
	{
		Image.reset();

		if (DXTexture)
		{
			DXTexture->Release();
			DXTexture = nullptr;
		}

		Texture->SetSTP16Bpp(b_Transparency16bpp);

		Image = Texture->GetBitmap();
		DXTexture = Render->CreateTexture(Image, b_Transparency = b_Transparency16bpp, Texture->GetTransparentColor());
	}

	ImGui::SetWindowSize(ImVec2(ImGui::GetItemRectMax().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetItemRectMax().y + ImGui::GetStyle().WindowPadding.y));

	ImGui::End();
}

void Global_Application::ImageWindow(void)
{
	Standard_String Str;

	if (Image)
	{
		ImGui::SetNextWindowPos(b_ViewToolbar ? ImVec2(2, ImGui::GetFrameHeightWithSpacing() + GetToolbarHeight()) : ImVec2(2, ImGui::GetFrameHeightWithSpacing()));

		ImGuiWindowFlags UseTitleBar = b_ImageWindowTitleBar ? ImGuiWindowFlags_None : ImGuiWindowFlags_NoTitleBar;
		ImGuiWindowFlags UseBackground = b_ImageWindowBackground ? ImGuiWindowFlags_None : ImGuiWindowFlags_NoBackground;

		ImGui::Begin("Image##ImageWindow", nullptr,
			UseTitleBar |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_HorizontalScrollbar |
			UseBackground |
			//ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoBringToFrontOnFocus);

		auto ShaderCallback = [](const ImDrawList* parent_list, const ImDrawCmd* cmd)
			{
				D3DSURFACE_DESC Desc{};
				G->DXTexture->GetLevelDesc(0, &Desc);

				float TextureWidthF = static_cast<float>(Desc.Width);
				float TextureHeightF = static_cast<float>(Desc.Height);

				G->Render->Device()->SetPixelShaderConstantF(0, &TextureWidthF, 1);
				G->Render->Device()->SetPixelShaderConstantF(1, &TextureHeightF, 1);

				G->Render->Device()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
				G->Render->Device()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
				G->Render->Device()->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);

				G->Render->Device()->SetRenderState(D3DRS_WRAP0, 0);

				G->Render->TextureFiltering(D3DTEXF_NONE);

				G->Render->SetVertexShader(D3DFVF_VERTGT);

				if (G->b_Dithering)
				{
					G->Render->Device()->SetPixelShader(G->Render->PS1DitherPixelShader);
				}
				else
				{
					G->Render->Device()->SetPixelShader(G->Render->PassthroughPixelShader);
				}
			};

		ImGui::GetWindowDrawList()->AddCallback(ShaderCallback, nullptr);

		D3DSURFACE_DESC Desc{};
		G->DXTexture->GetLevelDesc(0, &Desc);

		float TextureWidthF = static_cast<float>(Desc.Width);
		float TextureHeightF = static_cast<float>(Desc.Height);

		ImVec2 uv0 = ImVec2(0.0f, static_cast<float>(Image->GetHeight()) / TextureHeightF);
		ImVec2 uv1 = ImVec2(static_cast<float>(Image->GetWidth()) / TextureWidthF, 0.0f);

		ImGui::Image((ImTextureID)(intptr_t)DXTexture, ImVec2(Image->GetWidth() * m_ImageZoom, Image->GetHeight() * m_ImageZoom), uv0, uv1);

		if (ImGui::IsItemHovered())
		{
			ImVec2 MousePos = ImGui::GetMousePos();
			ImVec2 ImagePos = ImGui::GetItemRectMin();

			float MouseX = (MousePos.x - ImagePos.x) / m_ImageZoom;
			float MouseY = (MousePos.y - ImagePos.y) / m_ImageZoom;

			if (MouseX >= 0 && MouseX < Image->GetWidth() && MouseY >= 0 && MouseY < Image->GetHeight())
			{
				m_MousePixelX = static_cast<std::int32_t>(MouseX);
				m_MousePixelY = static_cast<std::int32_t>(MouseY);

				ImGui::BeginTooltip();
				ImGui::Text("%d, %d", m_MousePixelX, m_MousePixelY);
				ImGui::EndTooltip();
			}
		}

		ImGui::GetWindowDrawList()->AddCallback(ImDrawCallback_ResetRenderState, nullptr);

		ImGui::SetWindowSize(ImVec2(ImGui::GetItemRectMax().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetItemRectMax().y + ImGui::GetStyle().WindowPadding.y));

		ImGui::End();
	}
}

void Global_Application::CreateModal(void)
{
	Standard_String Str;

	Standard_FileSystem FS;

	if (!ImGui::IsPopupOpen(b_RawCreateNew ? "Create" : "Open RAW"))
	{
		ImGui::OpenPopup(b_RawCreateNew ? "Create" : "Open RAW");
	}

	ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x - ImGui::GetWindowSize().x) / 2, (ImGui::GetIO().DisplaySize.y - ImGui::GetWindowSize().y) / 4));

	if (ImGui::BeginPopupModal(b_RawCreateNew ? "Create" : "Open RAW", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
	{
		if (ImGui::RadioButton("4-bit##CreateModal", b_Raw4bpp))
		{
			b_Raw4bpp = true;
			b_Raw8bpp = false;
			b_Raw16bpp = false;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("8-bit##CreateModal", b_Raw8bpp))
		{
			b_Raw4bpp = false;
			b_Raw8bpp = true;
			b_Raw16bpp = false;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("16-bit##CreateModal", b_Raw16bpp))
		{
			b_Raw4bpp = false;
			b_Raw8bpp = false;
			b_Raw16bpp = true;
		}

		ImGui::NewLine();
		ImGui::SetNextItemWidth(ImGui::CalcTextSize("Width##CreateModal").x);
		if (ImGui::InputInt("Width##CreateModal", &m_RawWidth))
		{
			m_RawWidth = std::clamp(m_RawWidth, 1, 1024);
		}

		ImGui::SetNextItemWidth(ImGui::CalcTextSize("Width##CreateModal").x);
		if (ImGui::InputInt("Height##CreateModal", &m_RawHeight))
		{
			m_RawHeight = std::clamp(m_RawHeight, 1, 512);
		}

		if (!b_RawCreateNew)
		{
			ImGui::NewLine();
			ImGui::SetNextItemWidth(ImGui::CalcTextSize("Width##CreateModal").x);
			if (ImGui::InputTextWithHint("Pixel Offset##CreateModal", "hexadecimal", &m_RawPixelPtrStr))
			{
				m_RawPixelPtr = std::stoull(m_RawPixelPtrStr, nullptr, 16);
			}
			TooltipOnHover("Value must be in hexadecimal format -- 0x prefix not required");
			Tooltip("Pointer to the pixel data in the file");
		}

		{
			ImGui::NewLine();

			static bool b_InputEnabled = true;
			if ((b_Raw4bpp) || (b_Raw8bpp)) { b_InputEnabled = false; }
			else { b_InputEnabled = true; }
			ImGui::BeginDisabled(b_InputEnabled);

			// CLUT Amount
			bool b_ClutAmountEnabled = b_RawExternalPalette ? false : b_RawImportAllPalettesFromTIM ? true : false;
			ImGui::BeginDisabled(b_ClutAmountEnabled);
			ImGui::SetNextItemWidth(ImGui::CalcTextSize("Width##CreateModal").x);
			ImGui::InputInt("CLUT Amount##CreateModal", (int*)&m_RawPaletteCount);
#if defined(_WIN64)
			m_RawPaletteCount = std::clamp(m_RawPaletteCount, 1ULL, 0xFFFFULL);
#else
			m_RawPaletteCount = std::clamp(m_RawPaletteCount, 1U, 0xFFFFU);
#endif
			Tooltip("Total count of Color Lookup Tables in the file\r\n\r\n4bpp = 16 colors (0x20 bytes) per CLUT\r\n\r\n8bpp = 256 colors (0x200 bytes) per CLUT");
			ImGui::EndDisabled();

			// CLUT Offset
			bool b_ClutOffsetEnabled = b_RawExternalPalette ? false : b_RawExternalPaletteFromTIM ? true : false;
			if (b_RawCreateNew && !b_RawExternalPalette) { b_ClutOffsetEnabled = true; }
			ImGui::BeginDisabled(b_ClutOffsetEnabled);
			ImGui::SetNextItemWidth(ImGui::CalcTextSize("Width##CreateModal").x);
			if (ImGui::InputTextWithHint("CLUT Offset##CreateModal", "hexadecimal", &m_RawPalettePtrStr))
			{
				m_RawPalettePtr = std::stoull(m_RawPalettePtrStr, nullptr, 16);
			}
			TooltipOnHover("Value must be in hexadecimal format -- 0x prefix not required");
			Tooltip("Pointer to the Color Lookup Table data in the file");
			ImGui::EndDisabled();

			// CLUT File
			ImGui::NewLine();
			if (ImGui::Checkbox("External CLUT File##CreateModal", &b_RawExternalPalette))
			{
				b_RawExternalPalette ? b_RawExternalPalette = true : b_RawExternalPalette = false;
				b_RawExternalPaletteFromTIM = false;
				b_RawExternalPalette ? b_RawImportAllPalettesFromTIM = false : b_RawImportAllPalettesFromTIM = true;
			}
			Tooltip("(Optional) \"CLUT Amount\" Color Lookup Tables are stored in another file at \"CLUT Offset\"");

			// TIM File
			if (ImGui::Checkbox("External TIM CLUT##CreateModal", &b_RawExternalPaletteFromTIM))
			{
				b_RawExternalPalette = false;
				b_RawExternalPaletteFromTIM ? b_RawExternalPaletteFromTIM = true : b_RawExternalPaletteFromTIM = false;
				b_RawExternalPaletteFromTIM ? b_RawImportAllPalettesFromTIM = true : b_RawImportAllPalettesFromTIM = false;
			}
			Tooltip("(Optional) Use Color Lookup Tables from TIM file");

			ImGui::BeginDisabled(!b_RawExternalPaletteFromTIM);
			if (ImGui::Checkbox("Import all CLUTs from TIM##CreateModal", &b_RawImportAllPalettesFromTIM))
			{
				b_RawImportAllPalettesFromTIM ? b_RawImportAllPalettesFromTIM = true : b_RawImportAllPalettesFromTIM = false;
			}
			Tooltip("(Optional) Import all Color Lookup Tables from TIM file");

			ImGui::BeginDisabled(b_RawImportAllPalettesFromTIM);
			ImGui::SetNextItemWidth(ImGui::CalcTextSize("Width##CreateModal").x);
			ImGui::InputInt("CLUT ID##CreateModal", (int*)&m_RawPaletteID);
			Tooltip("Color Lookup Table ID of TIM file\r\n\r\nIf \"CLUT Amount\" is greater than one (1), this value is starting index value");
#if defined(_WIN64)
			m_RawPaletteID = std::clamp(m_RawPaletteID, 0ULL, 0xFFFFULL);
#else
			m_RawPaletteID = std::clamp(m_RawPaletteID, 0U, 0xFFFFU);
#endif
			ImGui::EndDisabled();

			ImGui::EndDisabled();

			// Open File
			String ButtonName = "";
			StringW ButtonDesc = L"";
			StringW ButtonFilter = L"";
			bool b_EnableOpenExternal = b_RawExternalPalette ? false : b_RawExternalPaletteFromTIM ? false : true;
			ImGui::BeginDisabled(b_EnableOpenExternal);
			b_RawExternalPalette ? ButtonName = "Palette File##CreateModal" : b_RawExternalPaletteFromTIM ? ButtonName = "Palette TIM##CreateModal" : ButtonName = "...##CreateModal";
			b_RawExternalPalette ? ButtonDesc = L"All files" : b_RawExternalPaletteFromTIM ? ButtonDesc = L"Sony Texture Image" : ButtonDesc = L"All files";
			b_RawExternalPalette ? ButtonFilter = L"*.*" : b_RawExternalPaletteFromTIM ? ButtonFilter = L"*.tim" : ButtonFilter = L"*.*";
			if (ImGui::Button(ButtonName.c_str(), ImVec2(ImGui::CalcTextSize(ButtonName.c_str()).x, 0)))
			{
				if (auto Filename = Window->GetOpenFilename({ ButtonDesc.c_str() }, { ButtonFilter.c_str() }); Filename.has_value())
				{
					m_RawPaletteFilename = Filename.value();
				}
			}
			Tooltip("Filename of where Color Lookup Tables are stored");
			ImGui::Text("%ws", m_RawPaletteFilename.filename().wstring().c_str());
			ImGui::EndDisabled();

			ImGui::EndDisabled();
			ImGui::NewLine();
		}

		String ButtonTitle = b_RawCreateNew ? "Create##CreateModal" : "Open##CreateModal";
		if (ImGui::Button(ButtonTitle.c_str(), ImVec2(ImGui::CalcTextSize(ButtonTitle.c_str()).x, 0)))
		{
			if (b_RawCreateNew)
			{
				Create("untitled.tim");
			}
			else if (auto Filename = Window->GetOpenFilename({ L"All files" }, { L"*.*" }); Filename.has_value())
			{
				Create(Filename.value());
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel##CreateModal", ImVec2(ImGui::CalcTextSize("Cancel##CreateModal").x, 0)))
		{
			ImGui::CloseCurrentPopup();
			CreateModalFunc = []() {};
		}

		ImGui::SetWindowSize(ImVec2(ImGui::GetItemRectMax().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetItemRectMax().y + ImGui::GetStyle().WindowPadding.y));

		ImGui::EndPopup();
	}
}