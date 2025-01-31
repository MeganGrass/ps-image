/*
*
*	Megan Grass
*	December 14, 2024
*
*/

#include "app.h"

void Global_Application::MainMenu(void)
{
	bool b_MenuPixelEnabled = false;
	if (m_Texture && !m_Texture->GetPixels().empty())
	{
		b_MenuPixelEnabled = true;
	}

	bool b_MenuPaletteEnabled = false;
	if (m_Texture && !m_Texture->GetPalette().empty())
	{
		b_MenuPaletteEnabled = true;
	}

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File##MenuWindow"))
		{
			if (ImGui::MenuItem("Open##FileMenu", "CTRL+O"))
			{
				Open();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Create##FileMenu", "CTRL+N"))
			{
				CreateModalFunc = [this]()
					{
						CreateModal();
					};
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Save As##FileMenu", "CTRL+S", nullptr, (b_MenuPixelEnabled || b_MenuPaletteEnabled)))
			{
				SaveAs();
			}
			ImGui::Separator();
			if (ImGui::BeginMenu("Delete", m_Texture ? true : false))
			{
				if (ImGui::MenuItem("Palette (Current)##FileMenuDelete", NULL, nullptr, b_MenuPaletteEnabled))
				{
					DeletePalette(m_Palette);
				}

				if (ImGui::MenuItem("Palette (All)##FileMenuDelete", NULL, nullptr, b_MenuPaletteEnabled))
				{
					DeleteAllPalettes();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Pixels##FileMenuDelete", NULL, nullptr, b_MenuPixelEnabled))
				{
					DeleteAllPixels();
				}

				ImGui::EndMenu();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Export All##FileMenu", NULL, nullptr, m_Files.size() > 1 ? true : false))
			{
				ExportAllTextures();
			}
			if (ImGui::MenuItem("Export All As Bitmap##FileMenu", NULL, nullptr, m_Files.size() > 1 ? true : false))
			{
				ExportAllTexturesToBitmap();
			}
			ImGui::Separator();
			if (ImGui::BeginMenu("Export", (b_MenuPixelEnabled || b_MenuPaletteEnabled)))
			{
				if (ImGui::MenuItem("Palette (Current)##FileMenuExport", NULL, nullptr, b_MenuPaletteEnabled))
				{
					ExportPalette(m_Palette);
				}
				if (ImGui::MenuItem("Palette (All)##FileMenuExport", NULL, nullptr, b_MenuPaletteEnabled))
				{
					ExportPalette();
				}
				if (ImGui::MenuItem("Palette As PAL##FileMenuExport", NULL, nullptr, b_MenuPaletteEnabled))
				{
					ExportPaletteToPAL();
				}
				if (ImGui::MenuItem("Palette As TIM##FileMenuExport", NULL, nullptr, b_MenuPaletteEnabled))
				{
					ExportPaletteToTIM();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Pixels##FileMenuExport", NULL, nullptr, b_MenuPixelEnabled))
				{
					ExportPixels();
				}
				if (ImGui::MenuItem("Pixels As TIM##FileMenuExport", NULL, nullptr, b_MenuPixelEnabled))
				{
					ExportPixelsToTIM();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Import", m_Texture ? true : false))
			{
				if (ImGui::MenuItem("Palette (Current)##FileMenuImport", NULL, nullptr, m_Texture ? true : false))
				{
					ImportPalette(m_Palette);
				}
				if (ImGui::MenuItem("Palette (All)##FileMenuExport", NULL, nullptr, m_Texture ? true : false))
				{
					ImportPalette();
				}
				if (ImGui::MenuItem("Palette From PAL##FileMenuExport", NULL, nullptr, m_Texture ? true : false))
				{
					ImportPaletteFromPAL();
				}
				if (ImGui::MenuItem("Palette From TIM##FileMenuImport", NULL, nullptr, m_Texture ? true : false))
				{
					ImportPaletteFromTIM();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Pixels##FileMenuImport", NULL, nullptr, (b_MenuPixelEnabled || b_MenuPaletteEnabled)))
				{
					ImportPixels();
				}
				if (ImGui::MenuItem("Pixels From TIM##FileMenuImport", NULL, nullptr, m_Texture ? true : false))
				{
					ImportPixelsFromTIM();
				}
				ImGui::EndMenu();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Close##FileMenu", NULL, nullptr, DXTexture ? true : false))
			{
				Close();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit##FileMenu", "ESC"))
			{
				b_ForceShutdown = true;
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
			ImGui::MenuItem("Image Options##ViewMenu", NULL, &b_ViewImageOptions);
			ImGui::MenuItem("Palette Editor##ViewMenu", NULL, &b_ViewPaletteWindow, m_Texture ? true : false);
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

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize("?").x - ImGui::GetFontSize() / 2);

		if (ImGui::BeginMenu("?##MenuWindow"))
		{
			if (ImGui::MenuItem("Controls##HelpMenu", NULL))
			{
				Controls();
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
	if (!b_ToolbarIconsOnBoot) { return; }

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
			Open();
		}
		TooltipOnHover("Open File");

		ImGui::SameLine();
		uv0 = ImVec2(64.0f / 320.0f, 0.0f);
		uv1 = ImVec2(128.0f / 320.0f, 64.0f / 64.0f);
		if (ImGui::ImageButton("New##ToolbarNew", (ImTextureID)(intptr_t)ToolbarIcons, ImVec2(64, 64), uv0, uv1))
		{
			CreateModalFunc = [this]()
				{
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
		if (ImGui::ImageButton("Save As##ToolbarSave", (ImTextureID)(intptr_t)ToolbarIcons, ImVec2(64, 64), uv0, uv1))
		{
			SaveAs();
		}
		TooltipOnHover("Save As");

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
			if (Render->NormalState() && DXTexture)
			{
				ImGui::Text(m_Filename.filename().string().c_str());

				ImGui::Indent(ImGui::CalcTextSize(m_Filename.filename().string().c_str()).x + ImGui::GetFontSize() / 2);

				ImGui::Text(Str.FormatCStyle("|  %d x %d  |  %dbpp", m_LastKnownWidth, m_LastKnownHeight, m_LastKnownBitsPerPixel).c_str());

				if (m_Texture)
				{
					if (Texture().GetPalette().empty() && Texture().GetPixels().empty())
					{
						ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize("Warning: palette and pixel data is empty").x - ImGui::GetFontSize() / 3);
						ImGui::Text("Warning: palette and pixel data is empty");
					}
					else if (Texture().GetDepth() < 16 && Texture().GetPalette().empty())
					{
						ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize("Warning: palette data is empty").x - ImGui::GetFontSize() / 3);
						ImGui::Text("Warning: palette data is empty");
					}
					else if (Texture().GetDepth() < 16 && Texture().GetPixels().empty())
					{
						ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize("Warning: pixel data is empty").x - ImGui::GetFontSize() / 3);
						ImGui::Text("Warning: pixel data is empty");
					}
					else if (!Texture().GetPalette().empty())
					{
						ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize(Str.FormatCStyle("palette: %d / %d", m_Palette, Texture().GetClutMax()).c_str()).x - ImGui::GetFontSize() / 3);
						ImGui::Text(Str.FormatCStyle("palette: %d / %d", m_Palette, Texture().GetClutMax()).c_str());
					}
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

	ImGui::Begin("Window Options##OptionsWindow", &b_ViewWindowOptions, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);

	if (ImGui::BeginCombo("Font##OptionsWindow", Window->FontList()[Window->GetFontIndex()].filename().stem().string().c_str()))
	{
		for (std::size_t i = 0; i < Window->FontList().size(); i++)
		{
			bool b_IsSelected = (Window->GetFontIndex() == i);

			if (ImGui::Selectable(Window->FontList()[i].filename().stem().string().c_str(), b_IsSelected))
			{
				b_FontChangeRequested = true;
				Window->SetFont(Window->FontList()[Window->GetFontIndex() = i], Window->FontSize());
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

	{
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
			BYTE(ColorF.x * 255.0f),
			BYTE(ColorF.y * 255.0f),
			BYTE(ColorF.z * 255.0f));

		auto Red = GetRValue(Color);
		auto Green = GetGValue(Color);
		auto Blue = GetBValue(Color);

		if ((Red != OldRed) || (Green != OldGreen) || (Blue != OldBlue))
		{
			Window->SetColor(GetRValue(Color), GetGValue(Color), GetBValue(Color), true);
		}
	}

	ImGui::BeginDisabled(!b_ToolbarIconsOnBoot);
	ImGui::Checkbox("Tool Bar##OptionsWindow", &b_ViewToolbar);
	ImGui::EndDisabled();

	ImGui::Checkbox("Status Bar##OptionsWindow", &b_ViewStatusbar);

	ImGui::Checkbox("Open File on Boot##OptionsWindow", &b_OpenLastFileOnBoot);
	TooltipOnHover("Reopen the previous file on boot");

	ImGui::SetWindowSize(ImVec2(ImGui::GetItemRectMax().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetItemRectMax().y + ImGui::GetStyle().WindowPadding.y));

	ImGui::End();
}

void Global_Application::FileBrowser(void)
{
	if (!b_ViewFileWindow) { return; }

	if (m_Files.empty()) { return; }

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

					if (m_Texture)
					{
						ImGui::Text(Texture().About().c_str());

						ImGui::BeginDisabled(Texture().GetPalette().empty());
						{
							ImGui::SetNextItemWidth(ImGui::CalcTextSize("VRAM X (Pixels)##FileWindow").x);
							if (ImGui::InputScalar("VRAM X (CLUT)##FileWindow", ImGuiDataType_U16, &Texture().ClutX()))
							{
								Texture().ClutX() = std::clamp(Texture().ClutX(), std::uint16_t(0), std::uint16_t(1024 - Texture().GetPaletteWidth()));
							}
							ScrollOnHover(&Texture().ClutX(), ImGuiDataType_U16, 16, 0, std::uint16_t(1024 - Texture().GetPaletteWidth()));
							TooltipOnHover("Value must be in decimal format");
							Tooltip(Str.FormatCStyle("Horizontal position of palette data in VRAM\r\n\r\nMouse scrollwheel can be used while hovering"));

							ImGui::SetNextItemWidth(ImGui::CalcTextSize("VRAM Y (Pixels)##FileWindow").x);
							if (ImGui::InputScalar("VRAM Y (CLUT)##FileWindow", ImGuiDataType_U16, &Texture().ClutY()))
							{
								Texture().ClutY() = std::clamp(Texture().ClutY(), std::uint16_t(0), std::uint16_t(512 - Texture().GetPaletteHeight()));
							}
							ScrollOnHover(&Texture().ClutY(), ImGuiDataType_U16, 16, 0, std::uint16_t(512 - Texture().GetPaletteHeight()));
							TooltipOnHover("Value must be in decimal format");
							Tooltip(Str.FormatCStyle("Vertical position of palette data in VRAM\r\n\r\nMouse scrollwheel can be used while hovering"));
						}
						ImGui::EndDisabled();

						ImGui::BeginDisabled(Texture().GetPixels().empty());
						{
							ImGui::SetNextItemWidth(ImGui::CalcTextSize("VRAM X (Pixels)##FileWindow").x);
							if (ImGui::InputScalar("VRAM X (Pixels)##FileWindow", ImGuiDataType_U16, &Texture().DataX()))
							{
								Texture().DataX() = std::clamp(Texture().DataX(), std::uint16_t(0), std::uint16_t(1024 - Texture().GetWidth()));
							}
							ScrollOnHover(&Texture().DataX(), ImGuiDataType_U16, 16, 0, std::uint16_t(1024 - Texture().GetWidth()));
							TooltipOnHover("Value must be in decimal format");
							Tooltip(Str.FormatCStyle("Horizontal position of pixel data in VRAM\r\n\r\nMouse scrollwheel can be used while hovering"));

							ImGui::SetNextItemWidth(ImGui::CalcTextSize("VRAM Y (Pixels)##FileWindow").x);
							if (ImGui::InputScalar("VRAM Y (Pixels)##FileWindow", ImGuiDataType_U16, &Texture().DataY()))
							{
								Texture().DataY() = std::clamp(Texture().DataY(), std::uint16_t(0), std::uint16_t(512 - Texture().GetHeight()));
							}
							ScrollOnHover(&Texture().DataY(), ImGuiDataType_U16, 16, 0, std::uint16_t(512 - Texture().GetHeight()));
							TooltipOnHover("Value must be in decimal format");
							Tooltip(Str.FormatCStyle("Vertical position of pixel data in VRAM\r\n\r\nMouse scrollwheel can be used while hovering"));
						}
						ImGui::EndDisabled();

						ImGui::BeginDisabled(Texture().GetPixels().empty() && Texture().GetPalette().empty());
						if (ImGui::Button("Export##FileWindow", ImVec2(ImGui::CalcTextSize("Save BMP##FileWindow").x, 0)))
						{
							Export(m_SelectedFile);
						}
						TooltipOnHover("Export this PlayStation Texture Image");
						ImGui::EndDisabled();

						ImGui::SameLine();

						if (ImGui::Button("Import##FileWindow", ImVec2(ImGui::CalcTextSize("Save BMP##FileWindow").x, 0)))
						{
							Import(m_SelectedFile);
						}
						TooltipOnHover(Str.FormatCStyle("Replace this PlayStation Texture Image\r\n\r\nData will be written to \"%ws\" beginning at offset 0x%llx",
							m_Filename.filename().wstring().c_str(),
							m_Files[m_SelectedFile].first));

						ImGui::BeginDisabled(Texture().GetPixels().empty());
						if (ImGui::Button("Export RAW##FileWindow", ImVec2(ImGui::CalcTextSize("Save BMP##FileWindow").x, 0)))
						{
							ExportPixels();
						}
						TooltipOnHover("Export pixel data to raw binary file");
						ImGui::EndDisabled();

						ImGui::SameLine();

						ImGui::BeginDisabled(Texture().GetPixels().empty());
						if (ImGui::Button("Import RAW##FileWindow", ImVec2(ImGui::CalcTextSize("Save BMP##FileWindow").x, 0)))
						{
							ImportPixels();
						}
						TooltipOnHover("Import pixel data from raw binary file");
						ImGui::EndDisabled();

						ImGui::BeginDisabled(Texture().GetPixels().empty());
						if (ImGui::Button("Export TIM##FileWindow", ImVec2(ImGui::CalcTextSize("Save BMP##FileWindow").x, 0)))
						{
							ExportPixelsToTIM();
						}
						TooltipOnHover("Export pixel data to PlayStation Texture Image\r\n\r\nPalette is ignored on export");
						ImGui::EndDisabled();

						ImGui::SameLine();

						if (ImGui::Button("Import TIM##FileWindow", ImVec2(ImGui::CalcTextSize("Save BMP##FileWindow").x, 0)))
						{
							ImportPixelsFromTIM();
						}
						TooltipOnHover("Import pixel data from PlayStation Texture Image\r\n\r\nPalette from PlayStation Texture Image is ignored on import");

						ImGui::BeginDisabled(Texture().GetPixels().empty());
						if (ImGui::Button("Save As Bitmap##FileWindow", ImVec2(ImGui::CalcTextSize("Save BMP##FileWindow").x, 0)))
						{
							SaveAsBitmap(m_SelectedFile);
						}
						TooltipOnHover("Export this PlayStation Texture Image to a Bitmap file");
						ImGui::EndDisabled();
					}

					ImGui::EndPopup();
				}

				ImGui::TableSetColumnIndex(1);
				ImGui::Text(Str.FormatCStyle("0x%llX", m_Files[Row].second).c_str());

				ImGui::PopID();
			}
		}

		if (OldSelectedFile != m_SelectedFile)
		{
			SetTexture(m_SelectedFile);
		}

		ImGui::EndTable();
	}

	ImGui::SetWindowSize(ImVec2(ImGui::GetItemRectMax().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetItemRectMax().y + m_Files.size()));

	ImGui::End();
}

void Global_Application::Palette(void)
{
	if (!b_ViewPaletteWindow) { return; }

	if (!m_Texture) { return; }

	std::uint16_t nColor = Texture().GetClutColorMax();

	ImGui::Begin("Palette##PaletteWindow", &b_ViewPaletteWindow, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);

	ImGui::BeginDisabled(Texture().GetPalette().empty());
	{
		std::uint32_t OldPalette = std::clamp(m_Palette, uint16_t(0), Texture().GetClutMax());
		float SliderIntWidth = ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("  ID##PaletteWindow").x / 3 - ImGui::GetStyle().ItemInnerSpacing.x * 2;
		ImGui::SetNextItemWidth(SliderIntWidth);
		if (ImGui::SliderInt("  ID##PaletteWindow", (int*)&m_Palette, 0, Texture().GetClutMax()))
		{
			m_Palette = std::clamp(m_Palette, uint16_t(0), Texture().GetClutMax());
		}
		ScrollOnHover(&m_Palette, ImGuiDataType_U32, 1, 0, Texture().GetClutMax());
		Tooltip("Active Color Lookup Table\r\n\r\nMousewheel (while hovering) and Left/Right arrow keys can also be used");
		if (OldPalette != m_Palette)
		{
			SetPalette(m_Palette);
		}

		ImGui::BeginDisabled(!(Texture().GetClutCount() > 1));
		{
			ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, false);

			if (ImGui::ArrowButton("MovePrev##PaletteWindow", ImGuiDir_Left))
			{
				MovePalette(m_Palette, ImGuiDir_Left);
			}
			TooltipOnHover(Str.FormatCStyle("Move the currently selected Color Lookup Table (%d) to the previous index (%d)", m_Palette, std::clamp(uint16_t(m_Palette - 1), uint16_t(0), Texture().GetClutMax())));

			ImGui::SameLine();

			if (ImGui::ArrowButton("MoveNext##PaletteWindow", ImGuiDir_Right))
			{
				MovePalette(m_Palette, ImGuiDir_Right);
			}
			TooltipOnHover(Str.FormatCStyle("Move the currently selected Color Lookup Table (%d) to the next index (%d)", m_Palette, std::clamp(uint16_t(m_Palette + 1), uint16_t(0), Texture().GetClutMax())));

			ImGui::SameLine();
			ImGui::Text("Move");
			Tooltip("Reorganize the Color Lookup Table index using the left and right buttons");

			ImGui::PopItemFlag();
		}
		ImGui::EndDisabled();
	}
	ImGui::EndDisabled();

	if (Texture().GetPalette().empty())
	{
		m_Palette = 0;
	}

	ImGui::BeginDisabled(Texture().GetPalette().empty());
	if (ImGui::Button("Copy##PaletteWindow"))
	{
		CopyPalette();
	}
	TooltipOnHover(Str.FormatCStyle("Copy the currently selected palette (%d) to the clipboard", m_Palette));
	ImGui::EndDisabled();

	ImGui::SameLine();

	ImGui::BeginDisabled(Texture().GetPalette().empty());
	if (ImGui::Button("Paste##PaletteWindow"))
	{
		PastePalette();
	}
	TooltipOnHover(Str.FormatCStyle("Paste the clipboard to the currently selected palette (%d)", m_Palette));
	ImGui::EndDisabled();

	ImGui::SameLine();

	if (ImGui::Button("Add##PaletteWindow"))
	{
		AddPalette();
	}
	TooltipOnHover(Str.FormatCStyle("Append a new palette after the max (%d)", Texture().GetClutMax()));

	ImGui::SameLine();

	if (ImGui::Button("Insert##PaletteWindow"))
	{
		InsertPalette();
	}
	TooltipOnHover(Str.FormatCStyle("Insert a new palette after the currently selected (%d)", m_Palette));

	ImGui::SameLine();

	ImGui::BeginDisabled(Texture().GetPalette().empty());
	if (ImGui::Button("Delete##PaletteWindow"))
	{
		DeletePalette(m_Palette);
	}
	TooltipOnHover(Str.FormatCStyle("Delete the currently selected palette (%d)", m_Palette));
	ImGui::EndDisabled();

	ImGui::SameLine();

	if (ImGui::Button("Add File##PaletteWindow"))
	{
		AddPaletteFromFile();
	}
	TooltipOnHover(Str.FormatCStyle("Append an existing palette after the max (%d)", Texture().GetClutMax()));

	ImGui::BeginDisabled(Texture().GetPalette().empty());
	if (ImGui::Button("Export (Current)##PaletteWindow", ImVec2(ImGui::CalcTextSize("Width##CreateModal").x, 0)))
	{
		ExportPalette(m_Palette);
	}
	TooltipOnHover(Str.FormatCStyle("Export the currently selected palette (%d) to raw file data", m_Palette));
	ImGui::EndDisabled();

	ImGui::SameLine();

	if (ImGui::Button("Import (Current)##PaletteWindow", ImVec2(ImGui::CalcTextSize("Width##CreateModal").x, 0)))
	{
		ImportPalette(m_Palette);
	}
	TooltipOnHover(Str.FormatCStyle("Replace the currently selected palette (%d) with raw file data", m_Palette));

	ImGui::BeginDisabled(Texture().GetPalette().empty());
	if (ImGui::Button("Export (All)##PaletteWindow", ImVec2(ImGui::CalcTextSize("Width##CreateModal").x, 0)))
	{
		ExportPalette();
	}
	TooltipOnHover(Str.FormatCStyle("Export all palettes to raw file data"));
	ImGui::EndDisabled();

	ImGui::SameLine();

	if (ImGui::Button("Import (All)##PaletteWindow", ImVec2(ImGui::CalcTextSize("Width##CreateModal").x, 0)))
	{
		ImportPalette();
		ImGui::End();
		return;
	}
	TooltipOnHover(Str.FormatCStyle("Replace all with palettes from raw file data"));

	ImGui::BeginDisabled(Texture().GetPalette().empty());
	if (ImGui::Button("Export TIM##PaletteWindow", ImVec2(ImGui::CalcTextSize("Width##CreateModal").x, 0)))
	{
		ExportPaletteToTIM();
	}
	TooltipOnHover(Str.FormatCStyle("Export all palettes to Sony Texture Image", m_Palette));
	ImGui::EndDisabled();

	ImGui::SameLine();

	if (ImGui::Button("Import TIM##PaletteWindow", ImVec2(ImGui::CalcTextSize("Width##CreateModal").x, 0)))
	{
		ImportPaletteFromTIM();
		ImGui::End();
		return;
	}
	TooltipOnHover(Str.FormatCStyle("Replace all with palettes from Sony Texture Image", m_Palette));

	ImGui::BeginDisabled(!Texture().GetCF());
	{
		ImGui::SetNextItemWidth(ImGui::CalcTextSize("VRAM X__##FileWindow").x);
		if (ImGui::InputScalar("VRAM X (CLUT)##FileWindow", ImGuiDataType_U16, &Texture().ClutX()))
		{
			Texture().ClutX() = std::clamp(Texture().ClutX(), std::uint16_t(0), std::uint16_t(1024 - Texture().GetPaletteWidth()));
		}
		ScrollOnHover(&Texture().ClutX(), ImGuiDataType_U16, 16, 0, uint16_t(1024 - Texture().GetPaletteWidth()));
		TooltipOnHover("Value must be in decimal format");
		Tooltip(Str.FormatCStyle("Horizontal position of palette data in VRAM\r\n\r\nMouse scrollwheel can be used while hovering"));

		ImGui::SetNextItemWidth(ImGui::CalcTextSize("VRAM Y__##FileWindow").x);
		if (ImGui::InputScalar("VRAM Y (CLUT)##FileWindow", ImGuiDataType_U16, &Texture().ClutY()))
		{
			Texture().ClutY() = std::clamp(Texture().ClutY(), std::uint16_t(0), std::uint16_t(512 - Texture().GetPaletteHeight()));
		}
		ScrollOnHover(&Texture().ClutY(), ImGuiDataType_U16, 16, 0, std::uint16_t(512 - Texture().GetPaletteHeight()));
		TooltipOnHover("Value must be in decimal format");
		Tooltip(Str.FormatCStyle("Vertical position of palette data in VRAM\r\n\r\nMouse scrollwheel can be used while hovering"));
	}
	ImGui::EndDisabled();

	ImGui::BeginDisabled(!Texture().GetCF());
	for (std::uint16_t x = 0; x < Texture().GetPaletteWidth(); x++)
	{
		if (x % 16) { ImGui::SameLine(); }

		ImVec4 ColorF = ImVec4(
			Texture().Red(Texture().GetPalette()[m_Palette][x]) / 255.0f,
			Texture().Green(Texture().GetPalette()[m_Palette][x]) / 255.0f,
			Texture().Blue(Texture().GetPalette()[m_Palette][x]) / 255.0f,
			1.0f);

		ImGui::ColorEdit3(Str.FormatCStyle("Palette Index [%d]##PaletteWindowIndex%d", x, x).c_str(), (float*)&ColorF, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoOptions);

		Sony_Texture_16bpp Trans1 = Texture().Create16bpp(uint8_t(ColorF.x * 255.0f), uint8_t(ColorF.y * 255.0f), uint8_t(ColorF.z * 255.0f), false);

		if ((Texture().GetPalette()[m_Palette][x].R != Trans1.R) || (Texture().GetPalette()[m_Palette][x].G != Trans1.G) || (Texture().GetPalette()[m_Palette][x].B != Trans1.B))
		{
			Texture().GetPalette()[m_Palette][x] = Trans1;

			ResetTexture();
		}
	}
	ImGui::EndDisabled();

	ImGui::SetWindowSize(ImVec2(ImGui::GetItemRectMax().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetItemRectMax().y + ImGui::GetStyle().WindowPadding.y));

	ImGui::End();
}

void Global_Application::BitstreamSettings(void)
{
	if (!b_ViewBitstreamOptions) { return; }

	ImGui::Begin("Bitstream Options##BitstreamSettings", &b_ViewBitstreamOptions, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);

	ImGui::SetNextItemWidth(ImGui::CalcTextSize("Width##BitstreamSettings").x);
	if (ImGui::InputScalar("Width##BitstreamSettings", ImGuiDataType_U16, &m_BistreamWidth))
	{
		m_BistreamWidth = std::clamp(m_BistreamWidth, (uint16_t)1, (uint16_t)640);
	}
	ScrollOnHover(&m_BistreamWidth, ImGuiDataType_U16, 16, 1, 640);
	TooltipOnHover("Width of Sony Bitstream (*.bs) image");

	ImGui::SetNextItemWidth(ImGui::CalcTextSize("Width##BitstreamSettings").x);
	if (ImGui::InputScalar("Height##BitstreamSettings", ImGuiDataType_U16, &m_BistreamHeight))
	{
		m_BistreamHeight = std::clamp(m_BistreamHeight, (uint16_t)1, (uint16_t)480);
	}
	ScrollOnHover(&m_BistreamHeight, ImGuiDataType_U16, 16, 1, 480);
	TooltipOnHover("Height of Sony Bitstream (*.bs) image");

	Tooltip("Width and Height of Sony Bitstream (*.bs) image\r\n\r\nValues are used when opening a Bitstream image");

	ImGui::SetWindowSize(ImVec2(ImGui::GetItemRectMax().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetItemRectMax().y + ImGui::GetStyle().WindowPadding.y));

	ImGui::End();
}

void Global_Application::ImageSettings(void)
{
	if (!b_ViewImageOptions) { return; }

	const char* TextureFilterTypes[] = { "None", "Point", "Linear", "Anisotropic" };

	ImGui::Begin("Image##UtilityWindow", &b_ViewImageOptions, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);

	ImGui::Checkbox("Dithering##UtilityWindow", &b_Dithering);
	Tooltip("Sony PlayStation (1994) Dithering Pixel Shader\r\n\r\nPreview only; effects are not written to TIM data");

	ImGui::SliderFloat("  Zoom##UtilityWindow", (float*)&m_ImageZoom, m_ImageZoomMin, m_ImageZoomMax);
	ScrollFloatOnHover(&m_ImageZoom, ImGuiDataType_Float, 0.25f, m_ImageZoomMin, m_ImageZoomMax);
	TooltipOnHover("Mouse scrollwheel may be used while hovering\r\n\r\nCTRL+Mousewheel can be used anytime");

	{
		if (ImGui::BeginCombo("Filtering##UtilityWindow", TextureFilterTypes[m_TextureFilter]))
		{
			std::uintmax_t iTextureFilterType = m_TextureFilter;
			iTextureFilterType = std::clamp(iTextureFilterType, 0ULL, 3ULL);
			m_TextureFilter = (D3DTEXTUREFILTERTYPE)iTextureFilterType;

			for (std::size_t i = 0; i < IM_ARRAYSIZE(TextureFilterTypes); i++)
			{
				bool b_IsSelected = (m_TextureFilter == i);

				if (ImGui::Selectable(TextureFilterTypes[i], b_IsSelected))
				{
					m_TextureFilter = (D3DTEXTUREFILTERTYPE)i;
				}
				if (b_IsSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	}

	if (m_Texture)
	{
		bool b_NoData = (Texture().GetPalette().empty() && Texture().GetPixels().empty());
		bool b_NoPalette = false;

		if (Texture().GetDepth() < 24)
		{
			b_NoPalette = Texture().GetPalette().empty();
		}

		if (Texture().GetDepth() <= 8)
		{
			b_NoData = (Texture().GetPalette().empty() || Texture().GetPixels().empty());
		}

		ImGui::BeginDisabled(b_NoData || Texture().TransparencySTP());
		{
			if (ImGui::Checkbox("Transparency##UtilityWindow", &Texture().Transparency()))
			{
				ResetTexture();
			}
			Tooltip("External color is semi/fully transparent");
		}
		ImGui::EndDisabled();

		ImGui::SameLine();

		ImGui::BeginDisabled((b_NoData && Texture().Transparency()) || Texture().TransparencySTP());
		{
			if (Texture().GetDepth() < 24)
			{
				Sony_Texture_16bpp Color0 = Texture().Create16bpp(Texture().TransparentColor(), false);

				ImVec4 ColorF = ImVec4(
					Texture().Red(Color0) / 255.0f,
					Texture().Green(Color0) / 255.0f,
					Texture().Blue(Color0) / 255.0f,
					0.0f);

				ImGui::ColorEdit3(Str.FormatCStyle("External Color##UtilityWindow").c_str(), (float*)&ColorF, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoOptions);
				Tooltip("External Color\r\n\r\nPreview only; effects are not written to TIM data");

				Sony_Texture_16bpp Color1 = Texture().Create16bpp(uint8_t(ColorF.x * 255.0f), uint8_t(ColorF.y * 255.0f), uint8_t(ColorF.z * 255.0f), false);

				if ((Color0.R != Color1.R) || (Color0.G != Color1.G) || (Color0.B != Color1.B))
				{
					Texture().TransparentColor() = (0x00 << 24) | (Texture().Blue(Color1) << 16) | (Texture().Green(Color1) << 8) | Texture().Red(Color1);

					ResetTexture();
				}
			}
			else
			{
				uint8_t Red = (Texture().TransparentColor() & 0xFF);
				uint8_t Green = ((Texture().TransparentColor() >> 8) & 0xFF);
				uint8_t Blue = ((Texture().TransparentColor() >> 16) & 0xFF);
				uint8_t Alpha = ((Texture().TransparentColor() >> 24) & 0xFF);

				ImVec4 ColorF = ImVec4(
					Red / 255.0f,
					Green / 255.0f,
					Blue / 255.0f,
					Alpha / 255.0f);

				ImGui::ColorEdit4(Str.FormatCStyle("External Color##UtilityWindow").c_str(), (float*)&ColorF, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoOptions);
				Tooltip("External Color\r\n\r\nPreview only; effects are not written to TIM data");

				uint8_t NewRed = uint8_t(ColorF.x * 255.0f);
				uint8_t NewGreen = uint8_t(ColorF.y * 255.0f);
				uint8_t NewBlue = uint8_t(ColorF.z * 255.0f);
				uint8_t NewAlpha = uint8_t(ColorF.w * 255.0f);

				if ((Red != NewRed) || (Green != NewGreen) || (Blue != NewBlue) || (Alpha != NewAlpha))
				{
					Texture().TransparentColor() = (NewAlpha << 24) | (NewBlue << 16) | (NewGreen << 8) | NewRed;

					ResetTexture();
				}
			}
		}
		ImGui::EndDisabled();

		ImGui::BeginDisabled(b_NoData || Texture().TransparencySTP());
		{
			if (ImGui::Checkbox("Superblack##UtilityWindow", &Texture().TransparencySuperblack()))
			{
				ResetTexture();
			}
			Tooltip("Solid black pixels are fully transparent");
		}
		ImGui::EndDisabled();

		ImGui::BeginDisabled(Texture().GetDepth() <= 8 ? b_NoData || Texture().TransparencySTP() : false || b_NoPalette || Texture().TransparencySTP() || Texture().GetDepth() == 24 ? Texture().GetPalette().empty() ? true : false : false);
		{
			if (ImGui::Checkbox("Superimposed##UtilityWindow", &Texture().TransparencySuperimposed()))
			{
				ResetTexture();
			}
			Tooltip("Color at palette index 0 is semi/fully transparent");
		}
		ImGui::EndDisabled();

		ImGui::BeginDisabled(b_NoData || Texture().GetDepth() == 24 ? true : false);
		{
			if (ImGui::Checkbox("STP Only##UtilityWindow", &Texture().TransparencySTP()))
			{
				ResetTexture();
			}
			Tooltip("STP flag determines if Transparency is used\r\n\r\n4bpp, 8bpp and 16bpp only");
		}
		ImGui::EndDisabled();

		ImGui::BeginDisabled(b_NoData);
		{
			if (ImGui::RadioButton("Full##UtilityWindow", Texture().TransparencyFull()))
			{
				Texture().TransparencyHalf() = false;
				Texture().TransparencyFull() = true;
				Texture().TransparencyInverse() = false;
				Texture().TransparencyQuarter() = false;

				ResetTexture();
			}
			TooltipOnHover("100/back + 100/texture");

			ImGui::SameLine();

			if (ImGui::RadioButton("Quarter##UtilityWindow", Texture().TransparencyQuarter()))
			{
				Texture().TransparencyHalf() = false;
				Texture().TransparencyFull() = false;
				Texture().TransparencyInverse() = false;
				Texture().TransparencyQuarter() = true;

				ResetTexture();
			}
			TooltipOnHover("100/back + 25/texture");

			ImGui::SameLine();

			if (ImGui::RadioButton("Half##UtilityWindow", Texture().TransparencyHalf()))
			{
				Texture().TransparencyHalf() = true;
				Texture().TransparencyFull() = false;
				Texture().TransparencyInverse() = false;
				Texture().TransparencyQuarter() = false;

				ResetTexture();
			}
			TooltipOnHover("50/back + 50/texture");

			ImGui::SameLine();

			if (ImGui::RadioButton("Inverse##UtilityWindow", Texture().TransparencyInverse()))
			{
				Texture().TransparencyHalf() = false;
				Texture().TransparencyFull() = false;
				Texture().TransparencyInverse() = true;
				Texture().TransparencyQuarter() = false;

				ResetTexture();
			}
			TooltipOnHover("100/back - 100/texture");
		}
		ImGui::EndDisabled();
	}

	//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::SetWindowSize(ImVec2(ImGui::GetItemRectMax().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetItemRectMax().y + ImGui::GetStyle().WindowPadding.y));

	ImGui::End();
}

void Global_Application::ImageWindow(void)
{
	if (Render->NormalState() && DXTexture)
	{
		RECT Rect = Window->GetRect();

		float ContentWidth = Rect.right - ImGui::GetStyle().WindowPadding.x * 2;

		float ContentHeightNothing = Rect.bottom - ImGui::GetStyle().WindowPadding.y * 6;
		float ContentHeightToolbar = Rect.bottom - ImGui::GetStyle().WindowPadding.y * 16;
		float ContentHeightStatusbar = Rect.bottom - ImGui::GetStyle().WindowPadding.y * 10;
		float ContentHeightToolbarStatusbar = Rect.bottom - ImGui::GetStyle().WindowPadding.y * 20;

		ImVec2 ContentSize;

		if (b_ViewStatusbar && b_ViewToolbar)
		{
			ContentSize = ImVec2(ContentWidth, ContentHeightToolbarStatusbar);
		}
		else if (b_ViewToolbar)
		{
			ContentSize = ImVec2(ContentWidth, ContentHeightToolbar);
		}
		else if (b_ViewStatusbar)
		{
			ContentSize = ImVec2(ContentWidth, ContentHeightStatusbar);
		}
		else
		{
			ContentSize = ImVec2(ContentWidth, ContentHeightNothing);
		}

		ImGui::SetNextWindowContentSize(ContentSize);

		ImGui::SetNextWindowSize(ImVec2(ContentSize.x + ImGui::GetStyle().WindowPadding.x * 2, ContentSize.y + ImGui::GetStyle().WindowPadding.y * 2));

		ImGui::SetNextWindowPos(b_ViewToolbar ? ImVec2(2, ImGui::GetFrameHeightWithSpacing() + GetToolbarHeight()) : ImVec2(2, ImGui::GetFrameHeightWithSpacing()));

		ImGui::Begin("ImageWindow##ImageWindow", nullptr,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoBringToFrontOnFocus);

		ContentSize = ImVec2(ImGui::GetWindowWidth() - ImGui::GetStyle().WindowPadding.x * 3, ImGui::GetWindowHeight() - ImGui::GetStyle().WindowPadding.y * 2);
		ImGui::BeginChild("Image##ImageWindow", ContentSize, false, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_HorizontalScrollbar);

		ImGui::GetWindowDrawList()->AddCallback([](const ImDrawList* parent_list, const ImDrawCmd* cmd)
			{
				float m_TextureWidth = float(G->m_TextureDesc.Width);
				float m_TextureHeight = float(G->m_TextureDesc.Height);

				G->Render->Device()->SetPixelShaderConstantF(0, &m_TextureWidth, 1);
				G->Render->Device()->SetPixelShaderConstantF(1, &m_TextureHeight, 1);

				G->Render->Device()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
				G->Render->Device()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
				G->Render->Device()->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);

				G->Render->Device()->SetRenderState(D3DRS_WRAP0, 0);

				G->Render->TextureFiltering(G->m_TextureFilter);

				G->Render->SetVertexShader(D3DFVF_VERTGT);

				if (G->b_Dithering)
				{
					G->Render->Device()->SetPixelShader(G->Render->PS1DitherPixelShader);
				}
				else
				{
					G->Render->Device()->SetPixelShader(G->Render->PassthroughPixelShader);
				}
			}, nullptr);

		ImVec2 uv0 = ImVec2(0.0f, float(m_LastKnownHeight) / float(m_TextureDesc.Height));
		ImVec2 uv1 = ImVec2(float(m_LastKnownWidth) / float(m_TextureDesc.Width), 0.0f);

		ImGui::Image((ImTextureID)(intptr_t)DXTexture, ImVec2(m_LastKnownWidth * m_ImageZoom, m_LastKnownHeight * m_ImageZoom), uv0, uv1);

		ImGui::GetWindowDrawList()->AddCallback(ImDrawCallback_ResetRenderState, nullptr);

		if (ImGui::IsItemHovered())
		{
			float MouseX = (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) / m_ImageZoom;
			float MouseY = (ImGui::GetMousePos().y - ImGui::GetItemRectMin().y) / m_ImageZoom;

			if (MouseX >= 0 && MouseX < m_LastKnownWidth && MouseY >= 0 && MouseY < m_LastKnownHeight)
			{
				ImGui::BeginTooltip();
				ImGui::Text("%d, %d", m_MousePixelX = uint16_t(MouseX), m_MousePixelY = uint16_t(MouseY));
				ImGui::EndTooltip();
			}
		}

		if (m_Texture && ImGui::BeginPopupContextItem("Pixel Data##ImageWindow"))
		{
			ImGui::Text("%d-bit pixel (x=%d, y=%d)", m_LastKnownBitsPerPixel, m_MousePixelX, m_MousePixelY);

			Sony_Texture_16bpp Color{};
			Sony_Texture_24bpp Color24{};

			if (m_LastKnownBitsPerPixel == 4)
			{
				Sony_Texture_4bpp Pixel = Texture().Get4bpp(m_MousePixelX, m_MousePixelY);
				if (!Texture().GetPalette().empty())
				{
					Color = Texture().GetPalette()[m_Palette][m_MousePixelX & 1 ? Pixel.Pix1 : Pixel.Pix0];
				}
				ImGui::Text("Palette Color (index=%d)", m_MousePixelX & 1 ? Pixel.Pix1 : Pixel.Pix0);
			}

			else if (m_LastKnownBitsPerPixel == 8)
			{
				Sony_Texture_8bpp Pixel = Texture().Get8bpp(m_MousePixelX, m_MousePixelY);
				if (!Texture().GetPalette().empty())
				{
					Color = Texture().GetPalette()[m_Palette][Pixel.Pix0];
				}
				ImGui::Text("Palette Color (index=%d)", Pixel.Pix0);
			}

			else if (m_LastKnownBitsPerPixel == 16)
			{
				Color = Texture().Get16bpp(m_MousePixelX, m_MousePixelY);
			}

			else if (m_LastKnownBitsPerPixel == 24)
			{
				Color24 = Texture().Get24bpp(m_MousePixelX, m_MousePixelY);
				ImGui::Text("R: %d, G: %d, B: %d", Color24.R0, Color24.G0, Color24.B0);
			}

			if (m_LastKnownBitsPerPixel < 24)
			{
				ImGui::Text("R: %d, G: %d, B: %d, STP: %d", Texture().Red(Color), Texture().Green(Color), Texture().Blue(Color), Texture().STP(Color));
			}

			if (ImGui::Button("Copy##ImageWindow_Pixel"))
			{
				if (m_LastKnownBitsPerPixel < 24)
				{
					ClipboardColor = (Texture().STP(Color) << 24) | (Texture().Blue(Color) << 16) | (Texture().Green(Color) << 8) | Texture().Red(Color);
				}
				else
				{
					ClipboardColor = (0xFF << 24) | (Color24.B0 << 16) | (Color24.G0 << 8) | Color24.R0;
				}
			}

			ImGui::EndPopup();
		}

		ImGui::EndChild();

		ImGui::End();
	}
}

void Global_Application::CreateModal(void)
{
	if (!ImGui::IsPopupOpen("Create##CreateModal"))
	{
		ImGui::OpenPopup("Create##CreateModal");
	}

	ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x - ImGui::GetWindowSize().x) / 2, (ImGui::GetIO().DisplaySize.y - ImGui::GetWindowSize().y) / 4));

	if (ImGui::BeginPopupModal("Create##CreateModal", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
	{
		float ItemWidth = ImGui::CalcTextSize("Width##CreateModal").x;

		{
			ImGui::BeginDisabled(b_RawExternalPixelsFromTIM);
			if (ImGui::RadioButton("4-bit##CreateModal", b_Raw4bpp))
			{
				b_Raw4bpp = true;
				b_Raw8bpp = false;
				b_Raw16bpp = false;
				b_Raw24bpp = false;
				AdjustWidthInput(m_RawWidth);
				AdjustHeightInput(m_RawHeight);
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("8-bit##CreateModal", b_Raw8bpp))
			{
				b_Raw4bpp = false;
				b_Raw8bpp = true;
				b_Raw16bpp = false;
				b_Raw24bpp = false;
				AdjustWidthInput(m_RawWidth);
				AdjustHeightInput(m_RawHeight);
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("16-bit##CreateModal", b_Raw16bpp))
			{
				b_Raw4bpp = false;
				b_Raw8bpp = false;
				b_Raw16bpp = true;
				b_Raw24bpp = false;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("24-bit##CreateModal", b_Raw24bpp))
			{
				b_Raw4bpp = false;
				b_Raw8bpp = false;
				b_Raw16bpp = false;
				b_Raw24bpp = true;
			}
			ImGui::EndDisabled();
		}

		ImGui::NewLine();

		{
			ImGui::BeginDisabled(b_RawExternalPixelsFromTIM);

			ImGui::SetNextItemWidth(ItemWidth);
			if (ImGui::InputScalar("Width##CreateModal", ImGuiDataType_U16, &m_RawWidth))
			{
				AdjustWidthInput(m_RawWidth);
			}
			if (ScrollOnHover(&m_RawWidth, ImGuiDataType_U16, 16, 1, 1024))
			{
				AdjustWidthInput(m_RawWidth);
			}
			TooltipOnHover("Value must be in decimal format\r\n\r\nMouse scrollwheel can be used while hovering");

			ImGui::SetNextItemWidth(ItemWidth);
			if (ImGui::InputScalar("Height##CreateModal", ImGuiDataType_U16, &m_RawHeight))
			{
				AdjustHeightInput(m_RawHeight);
			}
			if (ScrollOnHover(&m_RawHeight, ImGuiDataType_U16, 16, 1, 512))
			{
				AdjustHeightInput(m_RawHeight);
			}
			TooltipOnHover("Value must be in decimal format\r\n\r\nMouse scrollwheel can be used while hovering");

			ImGui::EndDisabled();
		}

		ImGui::NewLine();

		{
			bool b_PixelOffset = true;
			if (b_RawExternalPixels)
			{
				b_PixelOffset = false;
			}

			ImGui::BeginDisabled(!(b_RawExternalPixels || b_RawExternalPixelsFromTIM));
			ImGui::SetNextItemWidth(ItemWidth);
			String PointerStr = Str.FormatCStyle("%llx", m_RawPixelPtr);
			if (ImGui::InputTextWithHint("Pixel Offset##CreateModal", "hexadecimal", &PointerStr))
			{
				m_RawPixelPtr = std::stoull(PointerStr, nullptr, 16);
			}
			ScrollOnHover(&m_RawPixelPtr, ImGuiDataType_U64, 16, 0, -1);
			TooltipOnHover("Value must be in hexadecimal format");
			if (b_RawExternalPixels)
			{
				Tooltip(Str.FormatCStyle("Pointer to raw pixel data in \"%ws\"\r\n\r\nMouse scrollwheel can be used while hovering", m_RawPixelFilename.filename().wstring().c_str()));
			}
			else if (b_RawExternalPixelsFromTIM)
			{
				Tooltip(Str.FormatCStyle("Pointer to TIM file in \"%ws\"\r\n\r\nMouse scrollwheel can be used while hovering", m_RawPixelFilename.filename().wstring().c_str()));
			}
			else
			{
				Tooltip("...");
			}
			ImGui::EndDisabled();

			if (ImGui::Checkbox("Import RAW pixels##CreateModal", &b_RawExternalPixels))
			{
				b_RawExternalPixelsFromTIM = false;
			}
			Tooltip(Str.FormatCStyle("Import raw pixel data located at 0x%llx in \"%ws\"", m_RawPixelPtr, m_RawPixelFilename.filename().wstring().c_str()));

			if (ImGui::Checkbox("Import TIM Pixels##CreateModal", &b_RawExternalPixelsFromTIM))
			{
				b_RawExternalPixels = false;
			}
			Tooltip(Str.FormatCStyle("Import pixel data from TIM file located at 0x%llx in \"%ws\"", m_RawPixelPtr, m_RawPixelFilename.filename().wstring().c_str()));

			ImGui::BeginDisabled(!b_RawExternalPixels && !b_RawExternalPixelsFromTIM);
			String ButtonName = "";
			StringW ButtonDesc = L"";
			StringW ButtonFilter = L"";
			b_RawExternalPixels ? ButtonName = "Pixel File##CreateModal" : b_RawExternalPixelsFromTIM ? ButtonName = "Pixel TIM##CreateModal" : ButtonName = "...##CreateModalNoExtPixels";
			if (ImGui::Button(ButtonName.c_str(), ImVec2(ImGui::CalcTextSize("Width##CreateModal").x, 0)))
			{
				SetRawPixelFilename();
			}
			Tooltip("Filename of where pixel data stored");
			ImGui::Text("%ws", m_RawPixelFilename.filename().wstring().c_str());
			ImGui::EndDisabled();
		}

		ImGui::NewLine();

		{
			ImGui::BeginDisabled(b_RawExternalPaletteFromTIM);
			ImGui::SetNextItemWidth(ItemWidth);
			if (ImGui::InputScalar("CLUT Amount##CreateModal", ImGuiDataType_U16, &m_RawPaletteCount))
			{
				m_RawPaletteCount = std::clamp(m_RawPaletteCount, (uint16_t)0, (uint16_t)MAX_TIM_PALETTE);
			}
			ScrollOnHover(&m_RawPaletteCount, ImGuiDataType_U16, 1, 0, MAX_TIM_PALETTE);
			TooltipOnHover("Value must be in decimal format");
			Tooltip(Str.FormatCStyle("4bpp = 16 colors (0x20 bytes) per CLUT\r\n\r\n8bpp = 256 colors (0x200 bytes) per CLUT\r\n\r\nMouse scrollwheel can be used while hovering", m_RawPaletteFilename.filename().wstring().c_str()));
			ImGui::EndDisabled();

			ImGui::BeginDisabled(!(b_RawExternalPalette || b_RawExternalPaletteFromTIM));
			ImGui::SetNextItemWidth(ItemWidth);
			String PointerStr = Str.FormatCStyle("%llx", m_RawPalettePtr);
			if (ImGui::InputTextWithHint("CLUT Offset##CreateModal", "hexadecimal", &PointerStr))
			{
				m_RawPalettePtr = std::stoull(PointerStr, nullptr, 16);
			}
			ScrollOnHover(&m_RawPalettePtr, ImGuiDataType_U64, 16, 0, -1);
			TooltipOnHover("Value must be in hexadecimal format");
			if (b_RawExternalPalette)
			{
				Tooltip(Str.FormatCStyle("Pointer to raw palette data in \"%ws\"\r\n\r\nMouse scrollwheel can be used while hovering", m_RawPaletteFilename.filename().wstring().c_str()));
			}
			else if (b_RawExternalPaletteFromTIM)
			{
				Tooltip(Str.FormatCStyle("Pointer to TIM file in \"%ws\"\r\n\r\nMouse scrollwheel can be used while hovering", m_RawPaletteFilename.filename().wstring().c_str()));
			}
			else
			{
				Tooltip("...");
			}
			ImGui::EndDisabled();

			if (ImGui::Checkbox("Import RAW Palette##CreateModal", &b_RawExternalPalette))
			{
				b_RawExternalPaletteFromTIM = false;
			}
			Tooltip(Str.FormatCStyle("Import %d raw palette/s located at 0x%llx in \"%ws\"", m_RawPaletteCount, m_RawPalettePtr, m_RawPaletteFilename.filename().wstring().c_str()));

			if (ImGui::Checkbox("Import TIM Palette##CreateModal", &b_RawExternalPaletteFromTIM))
			{
				b_RawExternalPalette = false;
			}
			Tooltip(Str.FormatCStyle("Import all palettes from TIM file located at 0x%llx in \"%ws\"", m_RawPalettePtr, m_RawPaletteFilename.filename().wstring().c_str()));

			String ButtonName = "";
			
			ImGui::BeginDisabled(!b_RawExternalPalette && !b_RawExternalPaletteFromTIM);
			b_RawExternalPalette ? ButtonName = "Palette File##CreateModal" : b_RawExternalPaletteFromTIM ? ButtonName = "Palette TIM##CreateModal" : ButtonName = "...##CreateModalNoExtPalette";
			if (ImGui::Button(ButtonName.c_str(), ImVec2(ImGui::CalcTextSize("Width##CreateModal").x, 0)))
			{
				SetRawPaletteFilename();
			}
			Tooltip("Filename of where palette data is stored");
			ImGui::Text("%ws", m_RawPaletteFilename.filename().wstring().c_str());
			ImGui::EndDisabled();
		}

		ImGui::NewLine();

		{
			if (ImGui::Button("Create##CreateModal", ImVec2(ImGui::CalcTextSize("___##CreateModal").x, 0)))
			{
				if (Create(b_Raw4bpp ? 4 : b_Raw8bpp ? 8 : b_Raw16bpp ? 16 : b_Raw24bpp ? 24 : 16, m_RawWidth, m_RawHeight, m_RawPixelPtr, m_RawPalettePtr))
				{
					ImGui::CloseCurrentPopup();
					CreateModalFunc = []() {};
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel##CreateModal", ImVec2(ImGui::CalcTextSize("___##CreateModal").x, 0)))
			{
				ImGui::CloseCurrentPopup();
				CreateModalFunc = []() {};
			}
		}

		ImGui::SetWindowSize(ImVec2(ImGui::GetItemRectMax().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetItemRectMax().y + ImGui::GetStyle().WindowPadding.y));

		ImGui::EndPopup();
	}
}

void Global_Application::SearchModal(std::filesystem::path Filename)
{
	//bool b_ViewThisWindow = true;
	//ImGui::Begin("Search##TextureSearch", &b_ViewThisWindow, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);

	//float progress = 0.0f;
	//ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));

	//auto ProgressCallback = [&](float value) {
	//	progress = value;
	//	ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
	//	ImGui::Text("Searching for textures... %.0f%%", progress * 100.0f);
	//	};

	auto ProgressCallback = [&](float value) {
		};

	m_Files = std::make_unique<Sony_PlayStation_Texture>()->Search(Filename, 0, ProgressCallback);

	//ImGui::SetWindowSize(ImVec2(ImGui::GetItemRectMax().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetItemRectMax().y + ImGui::GetStyle().WindowPadding.y));

	//ImGui::End();
}