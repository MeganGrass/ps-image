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
				TextureIO(ImageIO::All);
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Create##FileMenu", "CTRL+N"))
			{
				CreateModalFunc = [&]() { CreateModal(); };
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Save As##FileMenu", "CTRL+S", nullptr, (b_MenuPixelEnabled || b_MenuPaletteEnabled)))
			{
				TextureIO(ImageIO::SaveAsNew | ImageIO::WriteAll);
			}
			if (ImGui::BeginMenu("Save All As", m_Files.size() > 1 ? true : false))
			{
				if (ImGui::MenuItem("TIM##FileMenuSaveAs"))
				{
					TextureIO(ImageIO::SaveMultiNew | ImageIO::All, NULL, m_Palette, ImageType::TIM);
				}
				if (ImGui::MenuItem("CLT##FileMenuSaveAs"))
				{
					TextureIO(ImageIO::SaveMultiNew | ImageIO::All, NULL, m_Palette, ImageType::CLT);
				}
				if (ImGui::MenuItem("PXL##FileMenuSaveAs"))
				{
					TextureIO(ImageIO::SaveMultiNew | ImageIO::All, NULL, m_Palette, ImageType::PXL);
				}
				if (ImGui::MenuItem("BMP##FileMenuSaveAs"))
				{
					TextureIO(ImageIO::SaveMultiNew | ImageIO::All, NULL, m_Palette, ImageType::BMP);
				}
				if (ImGui::MenuItem("PAL##FileMenuSaveAs"))
				{
					TextureIO(ImageIO::SaveMultiNew | ImageIO::All, NULL, m_Palette, ImageType::PAL);
				}
				if (ImGui::MenuItem("PNG##FileMenuSaveAs"))
				{
					TextureIO(ImageIO::SaveMultiNew | ImageIO::All, NULL, m_Palette, ImageType::PNG);
				}
				if (ImGui::MenuItem("JPG##FileMenuSaveAs"))
				{
					TextureIO(ImageIO::SaveMultiNew | ImageIO::All, NULL, m_Palette, ImageType::JPG);
				}
				ImGui::EndMenu();
			}
			ImGui::Separator();
			if (ImGui::BeginMenu("Delete", m_Texture && m_Texture->IsValid()))
			{
				if (ImGui::MenuItem("Palettes##FileMenuDelete", NULL, nullptr, b_MenuPaletteEnabled))
				{
					DeletePalette(m_Palette, true);
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Pixels##FileMenuDelete", NULL, nullptr, b_MenuPixelEnabled))
				{
					DeletePixels();
				}

				ImGui::EndMenu();
			}
			ImGui::Separator();
			if (ImGui::BeginMenu("Export", (b_MenuPixelEnabled || b_MenuPaletteEnabled)))
			{
				if (ImGui::MenuItem("Palette (Current)##FileMenuExport", NULL, nullptr, b_MenuPaletteEnabled))
				{
					TextureIO(ImageIO::SaveAsNew | ImageIO::Palette | ImageIO::PaletteSingle, NULL, m_Palette);
				}
				if (ImGui::MenuItem("Palette (All)##FileMenuExport", NULL, nullptr, b_MenuPaletteEnabled))
				{
					TextureIO(ImageIO::SaveAsNew | ImageIO::Palette);
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Pixels##FileMenuExport", NULL, nullptr, b_MenuPixelEnabled))
				{
					TextureIO(ImageIO::SaveAsNew | ImageIO::Pixel);
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Import", m_Texture ? true : false))
			{
				if (ImGui::MenuItem("Palette (Current)##FileMenuImport", NULL, nullptr, m_Texture ? true : false))
				{
					TextureIO(ImageIO::ImportPalette | ImageIO::PaletteSingle, NULL, m_Palette);
				}
				if (ImGui::MenuItem("Palette (All)##FileMenuExport", NULL, nullptr, m_Texture ? true : false))
				{
					TextureIO(ImageIO::ImportPalette);
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Pixels##FileMenuImport", NULL, nullptr, m_Texture ? true : false))
				{
					TextureIO(ImageIO::ImportPixel);
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
			TextureIO(ImageIO::All);
		}
		TooltipOnHover("Open File");

		ImGui::SameLine();
		uv0 = ImVec2(64.0f / 320.0f, 0.0f);
		uv1 = ImVec2(128.0f / 320.0f, 64.0f / 64.0f);
		if (ImGui::ImageButton("New##ToolbarNew", (ImTextureID)(intptr_t)ToolbarIcons, ImVec2(64, 64), uv0, uv1))
		{
			CreateModalFunc = [this]() { CreateModal(); };
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
			TextureIO(ImageIO::SaveAsNew | ImageIO::WriteAll);
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
			if (b_Searching)
			{
				ImGui::Text("Searching for textures...");
			}
			else if (Render->NormalState() && DXTexture)
			{
				if (b_ImageOnDisk)
				{
					ImGui::Text(Str.FormatCStyle("%ws  |  %d x %d  |  %dbpp", m_Filename.filename().wstring().c_str(), m_LastKnownWidth, m_LastKnownHeight, m_LastKnownBitsPerPixel).c_str());
				}
				else
				{
					ImGui::Text(Str.FormatCStyle("*%ws  |  %d x %d  |  %dbpp", m_Filename.filename().wstring().c_str(), m_LastKnownWidth, m_LastKnownHeight, m_LastKnownBitsPerPixel).c_str());
				}

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
					else if (Texture().GetPixels().empty())
					{
						ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize("Warning: pixel data is empty").x - ImGui::GetFontSize() / 3);
						ImGui::Text("Warning: pixel data is empty");
					}
					else if (!Texture().GetPalette().empty())
					{
						ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize(Str.FormatCStyle("palette: %d / %d", m_Palette, Texture().GetPaletteCount()).c_str()).x - ImGui::GetFontSize() / 3);
						ImGui::Text(Str.FormatCStyle("palette: %d / %d", m_Palette, Texture().GetPaletteCount()).c_str());
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
				if (ImGui::Selectable(Str.FormatCStyle("0x%llX##FileOffset%d", m_Files.empty() ? 0 : File()[Row].first, Row).c_str(), m_SelectedFile == Row, ImGuiSelectableFlags_SpanAllColumns))
				{
					m_SelectedFile = m_PostSearchSelectedFile = Row;
				}

				if (ImGui::BeginPopupContextItem(Str.FormatCStyle("0x%llX##FileOffset%d", m_Files.empty() ? 0 : File()[Row].first, Row).c_str()))
				{
					if (m_Files.empty()) { ImGui::EndPopup(); ImGui::PopID(); break; }

					if (m_SelectedFile != Row) { SetTexture(m_SelectedFile = Row); }

					if (m_Texture)
					{
						ImVec2 ItemSize = ImVec2(ImGui::CalcTextSize("__FILE__##FileWindow").x, 0);

						ImGui::Text(Texture().About().c_str());

						std::uint16_t PaletteX = Texture().GetPaletteX();
						std::uint16_t PaletteY = Texture().GetPaletteY();
						std::uint16_t DataX = Texture().GetPixelX();
						std::uint16_t DataY = Texture().GetPixelY();

						ImGui::BeginDisabled(Texture().GetPalette().empty());
						{
							ImGui::SetNextItemWidth(ImGui::CalcTextSize("__FILE__").x);
							if (ImGui::InputScalar("CLUT X##FileWindow", ImGuiDataType_U16, &PaletteX))
							{
								Texture().SetPaletteX(std::clamp(PaletteX, std::uint16_t(0), std::uint16_t(1024 - Texture().GetPaletteWidth())));
							}
							ScrollOnHover(&PaletteX, ImGuiDataType_U16, 16, 0, std::uint16_t(1024 - Texture().GetPaletteWidth()));
							if (Texture().GetPaletteX() != PaletteX) { Texture().SetPaletteX(PaletteX); }
							TooltipOnHover("Value must be in decimal format");
							Tooltip(Str.FormatCStyle("Horizontal position of palette data in VRAM\r\n\r\nMouse scrollwheel can be used while hovering"));
						}
						ImGui::EndDisabled();

						ImGui::SameLine();

						ImGui::BeginDisabled(Texture().GetPixels().empty());
						{
							ItemSize.y = ImGui::GetCursorPosY();
							ItemSize.x = ImGui::GetCursorPosX() + ImGui::GetCursorPosX() / 3;
							ImGui::SetCursorPos(ItemSize);
							ItemSize = ImVec2(ImGui::CalcTextSize("__FILE__##FileWindow").x, 0);

							ImGui::SetNextItemWidth(ImGui::CalcTextSize("__FILE__").x);
							if (ImGui::InputScalar("Pixel X##FileWindow", ImGuiDataType_U16, &DataX))
							{
								Texture().SetPixelX(std::clamp(DataX, std::uint16_t(0), std::uint16_t(1024 - Texture().GetPaletteWidth())));
							}
							ScrollOnHover(&DataX, ImGuiDataType_U16, 16, 0, std::uint16_t(1024 - Texture().GetPaletteWidth()));
							if (Texture().GetPixelX() != DataX) { Texture().SetPixelX(DataX); }
							TooltipOnHover("Value must be in decimal format");
							Tooltip(Str.FormatCStyle("Horizontal position of pixel data in VRAM\r\n\r\nMouse scrollwheel can be used while hovering"));
						}
						ImGui::EndDisabled();

						ImGui::BeginDisabled(Texture().GetPalette().empty());
						{
							ImGui::SetNextItemWidth(ImGui::CalcTextSize("__FILE__").x);
							if (ImGui::InputScalar("CLUT Y##FileWindow", ImGuiDataType_U16, &PaletteY))
							{
								Texture().SetPaletteY(std::clamp(PaletteY, std::uint16_t(0), std::uint16_t(1024 - Texture().GetPaletteWidth())));
							}
							ScrollOnHover(&PaletteY, ImGuiDataType_U16, 16, 0, std::uint16_t(1024 - Texture().GetPaletteWidth()));
							if (Texture().GetPaletteY() != PaletteY) { Texture().SetPaletteY(PaletteY); }
							TooltipOnHover("Value must be in decimal format");
							Tooltip(Str.FormatCStyle("Vertical position of palette data in VRAM\r\n\r\nMouse scrollwheel can be used while hovering"));
						}
						ImGui::EndDisabled();

						ImGui::SameLine();

						ImGui::BeginDisabled(Texture().GetPixels().empty());
						{
							ItemSize.y = ImGui::GetCursorPosY();
							ItemSize.x = ImGui::GetCursorPosX() + ImGui::GetCursorPosX() / 3;
							ImGui::SetCursorPos(ItemSize);
							ItemSize = ImVec2(ImGui::CalcTextSize("__FILE__##FileWindow").x, 0);

							ImGui::SetNextItemWidth(ImGui::CalcTextSize("__FILE__").x);
							if (ImGui::InputScalar("Pixel Y##FileWindow", ImGuiDataType_U16, &DataY))
							{
								Texture().SetPixelY(std::clamp(DataY, std::uint16_t(0), std::uint16_t(1024 - Texture().GetPaletteWidth())));
							}
							ScrollOnHover(&DataY, ImGuiDataType_U16, 16, 0, std::uint16_t(1024 - Texture().GetPaletteWidth()));
							if (Texture().GetPixelY() != DataY) { Texture().SetPixelY(DataY); }
							TooltipOnHover("Value must be in decimal format");
							Tooltip(Str.FormatCStyle("Vertical position of pixel data in VRAM\r\n\r\nMouse scrollwheel can be used while hovering"));
						}
						ImGui::EndDisabled();

						if (ImGui::Button("Save As##FileWindow", ItemSize))
						{
							TextureIO(ImageIO::SaveAsNew | ImageIO::WriteAll);
						}
						TooltipOnHover("Export this PlayStation Texture Image as file type\r\n\r\nRaw data will be output when file type is unknown");

						ImGui::SameLine();

						if (ImGui::Button("Import##FileWindow", ItemSize))
						{
							TextureIO(ImageIO::IO | ImageIO::All);
							ImGui::EndPopup();
							ImGui::PopID();
							break;
						}
						TooltipOnHover(Str.FormatCStyle("Replace this PlayStation Texture Image (*.TIM)\r\n\r\nNew TIM data will be written to \"%ws\" at offset 0x%llx",
							m_Filename.filename().wstring().c_str(), File()[m_SelectedFile].first));
					}

					ImGui::EndPopup();
				}

				ImGui::TableSetColumnIndex(1);
				Row = File().empty() ? 0 : std::clamp(Row, (std::size_t)0, File().size() - 1);
				m_Files.empty() ? ImGui::Text(" ") : ImGui::Text(Str.FormatCStyle("0x%llX", File()[Row].second).c_str());

				ImGui::PopID();
			}
		}

		if (OldSelectedFile != m_SelectedFile)
		{
			if (!m_Filename.empty()) { SetTexture(m_SelectedFile); }
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

	ImVec2 ItemSize = ImVec2(ImGui::CalcTextSize("Width##CreateModal").x, 0);

	std::uint16_t nColor = Texture().GetPaletteColorMax();

	ImGui::Begin("Palette##PaletteWindow", &b_ViewPaletteWindow, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);

	ImGui::BeginDisabled(Texture().GetPalette().empty());
	{
		std::uint32_t OldPalette = std::clamp(m_Palette, (uint16_t)0, Texture().GetPaletteMaxIndex());

		float SliderIntWidth = ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("  ID##PaletteWindow").x / 3 - ImGui::GetStyle().ItemInnerSpacing.x * 2;

		ImGui::SetNextItemWidth(SliderIntWidth);

		uint16_t min = 0;
		uint16_t max = Texture().GetPaletteMaxIndex();
		if (ImGui::SliderScalar("  ID##PaletteWindow", ImGuiDataType_U16, &m_Palette, &min, &max))
		{
			m_Palette = std::clamp(m_Palette, (uint16_t)0, Texture().GetPaletteMaxIndex());
		}

		ScrollOnHover(&m_Palette, ImGuiDataType_U16, 1, 0, Texture().GetPaletteMaxIndex());
		Tooltip("Active Color Lookup Table\r\n\r\nMousewheel (while hovering) and Left/Right arrow keys can also be used");

		if (OldPalette != m_Palette)
		{
			SetPalette(m_Palette);
		}

		ImGui::BeginDisabled(!(Texture().GetPaletteCount() > 1));
		{
			ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, false);

			if (ImGui::ArrowButton("MovePrev##PaletteWindow", ImGuiDir_Left))
			{
				MovePalette(m_Palette, ImGuiDir_Left);
			}
			TooltipOnHover(Str.FormatCStyle("Move the currently selected Color Lookup Table (%d) to the previous index (%d)", m_Palette, std::clamp(uint16_t(m_Palette - 1), uint16_t(0), Texture().GetPaletteMaxIndex())));

			ImGui::SameLine();

			if (ImGui::ArrowButton("MoveNext##PaletteWindow", ImGuiDir_Right))
			{
				MovePalette(m_Palette, ImGuiDir_Right);
			}
			TooltipOnHover(Str.FormatCStyle("Move the currently selected Color Lookup Table (%d) to the next index (%d)", m_Palette, std::clamp(uint16_t(m_Palette + 1), uint16_t(0), Texture().GetPaletteMaxIndex())));

			ImGui::SameLine();
			ImGui::Text("Move");
			Tooltip("Reorganize the Color Lookup Table index using the left and right buttons");

			ImGui::PopItemFlag();
		}
		ImGui::EndDisabled();
	}
	ImGui::EndDisabled();

	if (m_Texture->GetPalette().empty())
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
	TooltipOnHover(Str.FormatCStyle("Append a new palette after the max (%d)", Texture().GetPaletteMaxIndex()));

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
		DeletePalette(m_Palette, false);
	}
	TooltipOnHover(Str.FormatCStyle("Delete the currently selected palette (%d)", m_Palette));
	ImGui::EndDisabled();

	ImGui::SameLine();

	if (ImGui::Button("Add File##PaletteWindow"))
	{
		TextureIO(ImageIO::ImportPalette | ImageIO::PaletteAdd);
	}
	TooltipOnHover(Str.FormatCStyle("Append an existing palette after the max (%d)", Texture().GetPaletteMaxIndex()));

	ImGui::BeginDisabled(Texture().GetPalette().empty());
	if (ImGui::Button("Save As##PaletteWindow", ItemSize))
	{
		TextureIO(ImageIO::SaveAsNew | ImageIO::Palette);
	}
	TooltipOnHover(Str.FormatCStyle("Export all palettes to file\r\n\r\nRaw data will be output when file type is unknown", m_Palette));
	ImGui::EndDisabled();

	ImGui::SameLine();

	if (ImGui::Button("Import##PaletteWindow", ItemSize))
	{
		TextureIO(ImageIO::ImportPalette);
	}
	TooltipOnHover(Str.FormatCStyle("Replace all palettes from file\r\n\r\nData will be interpreted as raw binary when file type is unknown", m_Palette));

	ImGui::BeginDisabled(Texture().GetPalette().empty());
	if (ImGui::Button("Export (Current)##PaletteWindow", ItemSize))
	{
		//TextureIO(ImageIO::WriteRawPalette | ImageIO::PaletteSingle | ImageIO::New, NULL, m_Palette);
		TextureIO(ImageIO::SaveAsNew | ImageIO::Palette | ImageIO::PaletteSingle, NULL, m_Palette);
	}
	TooltipOnHover(Str.FormatCStyle("Export palette (%d) to file\r\n\r\nRaw data will be output when file type is unknown", m_Palette));
	ImGui::EndDisabled();

	ImGui::SameLine();

	if (ImGui::Button("Import (Current)##PaletteWindow", ItemSize))
	{
		//TextureIO(ImageIO::ReadRawPalette | ImageIO::PaletteSingle, NULL, m_Palette);
		TextureIO(ImageIO::ImportPalette | ImageIO::PaletteSingle, NULL, m_Palette);
	}
	TooltipOnHover(Str.FormatCStyle("Replace palette (%d) from file\r\n\r\nData will be interpreted as raw binary when file type is unknown", m_Palette));

	/*ImGui::BeginDisabled(!Texture().GetCF());
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
	ImGui::EndDisabled();*/

	if (m_Texture && m_Texture->GetCF())
	{
		std::size_t pPalette = Texture().GetPalettePtr(m_Palette);

		for (auto i = 0; i < Texture().GetPaletteColorMax(); i++)
		{
			if (!m_Texture)
			{
				break;
			}

			if (i % 16) { ImGui::SameLine(); }

			Sony_Texture_16bpp Color0 = Texture().GetPalette().at(pPalette + i);

			ImVec4 ColorF = ImVec4(
				Texture().GetRed(Color0) / 255.0f,
				Texture().GetGreen(Color0) / 255.0f,
				Texture().GetBlue(Color0) / 255.0f,
				1.0f);

			ImGui::ColorEdit3(Str.FormatCStyle("Palette Index [%d]##PaletteWindowIndex%d", i, i).c_str(), (float*)&ColorF, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoOptions);

			Sony_Texture_16bpp Color1 = Texture().Create16bpp((uint8_t)(ColorF.x * 255.0f), (uint8_t)(ColorF.y * 255.0f), (uint8_t)(ColorF.z * 255.0f), Color0.STP);

			if (Color0 != Color1)
			{
				Texture().GetPalette().at(pPalette + i) = Color1;

				ResetTexture();
			}
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
			if (ImGui::Checkbox("Transparency##UtilityWindow", &Texture().TransparencyExternal()))
			{
				ResetTexture();
			}
			Tooltip("External color is semi/fully transparent");
		}
		ImGui::EndDisabled();

		ImGui::SameLine();

		ImGui::BeginDisabled((b_NoData && Texture().TransparencyExternal()) || Texture().TransparencySTP());
		{
			if (Texture().GetDepth() < 24)
			{
				Sony_Texture_16bpp Color0 = Texture().Create16bpp(Texture().TransparencyColor(), false);

				ImVec4 ColorF = ImVec4(
					Texture().GetRed(Color0) / 255.0f,
					Texture().GetGreen(Color0) / 255.0f,
					Texture().GetBlue(Color0) / 255.0f,
					1.0f);

				ImGui::ColorEdit3(Str.FormatCStyle("External Color##UtilityWindow").c_str(), (float*)&ColorF, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoOptions);
				Tooltip("External Color\r\n\r\nPreview only; effects are not written to TIM data");

				Sony_Texture_16bpp Color1 = Texture().Create16bpp((uint8_t)(ColorF.x * 255.0f), (uint8_t)(ColorF.y * 255.0f), (uint8_t)(ColorF.z * 255.0f), Color0.STP);

				if (Color0 != Color1)
				{
					Texture().TransparencyColor() = (0x00 << 24) | (Texture().GetBlue(Color1) << 16) | (Texture().GetGreen(Color1) << 8) | Texture().GetRed(Color1);

					ResetTexture();
				}
			}
			else
			{
				uint8_t Red = (Texture().TransparencyColor() & 0xFF);
				uint8_t Green = ((Texture().TransparencyColor() >> 8) & 0xFF);
				uint8_t Blue = ((Texture().TransparencyColor() >> 16) & 0xFF);
				uint8_t Alpha = ((Texture().TransparencyColor() >> 24) & 0xFF);

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
					Texture().TransparencyColor() = (NewAlpha << 24) | (NewBlue << 16) | (NewGreen << 8) | NewRed;

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
			if (Texture().GetDepth() == 24)
			{
				Tooltip("Solid black pixels (8:8:8) are fully transparent");
			}
			else if (Texture().GetDepth() == 16)
			{
				Tooltip("Solid black pixels (5:5:5:1) are fully transparent");
			}
			else
			{
				Tooltip("Solid black palette colors are fully transparent");
			}
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
					Color = Texture().GetPaletteColor(m_Palette, m_MousePixelX & 1 ? Pixel.Pix1 : Pixel.Pix0);
				}
				ImGui::Text("Palette Color (index=%d)", m_MousePixelX & 1 ? Pixel.Pix1 : Pixel.Pix0);
			}

			else if (m_LastKnownBitsPerPixel == 8)
			{
				Sony_Texture_8bpp Pixel = Texture().Get8bpp(m_MousePixelX, m_MousePixelY);
				if (!Texture().GetPalette().empty())
				{
					Color = Texture().GetPaletteColor(m_Palette, Pixel.Pix0);
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
				ImGui::Text("R: %d, G: %d, B: %d, STP: %d", Texture().GetRed(Color), Texture().GetGreen(Color), Texture().GetBlue(Color), Texture().GetSTP(Color));
			}

			if (ImGui::Button("Copy##ImageWindow_Pixel"))
			{
				if (m_LastKnownBitsPerPixel < 24)
				{
					ClipboardColor = (Texture().GetSTP(Color) << 24) | (Texture().GetBlue(Color) << 16) | (Texture().GetGreen(Color) << 8) | Texture().GetRed(Color);
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

	ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x - ImGui::GetWindowSize().x) / 2, (ImGui::GetIO().DisplaySize.y - ImGui::GetWindowSize().y) / 3));

	if (ImGui::BeginPopupModal("Create##CreateModal", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
	{
		float ItemWidth = ImGui::CalcTextSize("Width##CreateModal").x;

		const char* PaletteFileTypes[] = { "None", "RAW", "TIM", "CLT", "BMP", "PAL", "PNG" };
		const char* PixelFileTypes[] = { "None", "RAW", "TIM", "PXL", "BMP", "PNG", "JPG" };

		bool b_PaletteRaw = (std::to_underlying(m_CreateInfo.PaletteType) & (std::to_underlying(ImageType::RAW)));
		bool b_PaletteTIM = (std::to_underlying(m_CreateInfo.PaletteType) & (std::to_underlying(ImageType::TIM)));
		bool b_PaletteCLT = (std::to_underlying(m_CreateInfo.PaletteType) & (std::to_underlying(ImageType::CLT)));
		bool b_PalettePAL = (std::to_underlying(m_CreateInfo.PaletteType) & (std::to_underlying(ImageType::PAL)));
		bool b_PaletteNone = (b_PaletteRaw || b_PaletteTIM || b_PaletteCLT || b_PalettePAL) ? false : true;

		bool b_PixelsRaw = (std::to_underlying(m_CreateInfo.PixelType) & (std::to_underlying(ImageType::RAW)));
		bool b_PixelsTIM = (std::to_underlying(m_CreateInfo.PixelType) & (std::to_underlying(ImageType::TIM)));
		bool b_PixelsPXL = (std::to_underlying(m_CreateInfo.PixelType) & (std::to_underlying(ImageType::PXL)));
		bool b_PixelsBMP = (std::to_underlying(m_CreateInfo.PixelType) & (std::to_underlying(ImageType::BMP)));
		bool b_PixelsPNG = (std::to_underlying(m_CreateInfo.PixelType) & (std::to_underlying(ImageType::PNG)));
		bool b_PixelsJPG = (std::to_underlying(m_CreateInfo.PixelType) & (std::to_underlying(ImageType::JPG)));
		bool b_PixelsNone = (b_PixelsRaw || b_PixelsTIM || b_PixelsPXL || b_PixelsBMP || b_PixelsPNG || b_PixelsJPG) ? false : true;

		{
			ImGui::BeginDisabled(!b_PixelsNone && !b_PixelsRaw);
			if (ImGui::RadioButton("4-bit##CreateModal", (m_CreateInfo.Depth == 4)))
			{
				m_CreateInfo.Depth = 4;
				AdjustWidthInput(m_CreateInfo.Width);
				AdjustHeightInput(m_CreateInfo.Height);
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("8-bit##CreateModal", (m_CreateInfo.Depth == 8)))
			{
				m_CreateInfo.Depth = 8;
				AdjustWidthInput(m_CreateInfo.Width);
				AdjustHeightInput(m_CreateInfo.Height);
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("16-bit##CreateModal", (m_CreateInfo.Depth == 16)))
			{
				m_CreateInfo.Depth = 16;
				AdjustWidthInput(m_CreateInfo.Width);
				AdjustHeightInput(m_CreateInfo.Height);
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("24-bit##CreateModal", (m_CreateInfo.Depth == 24)))
			{
				m_CreateInfo.Depth = 24;
				AdjustWidthInput(m_CreateInfo.Width);
				AdjustHeightInput(m_CreateInfo.Height);
			}
			ImGui::EndDisabled();
		}

		uint16_t nMaxTIMPalette = m_CreateInfo.Depth == 4 ? ((1024 * 512) / 16) : ((1024 * 512) / 256);

		ImGui::NewLine();

		{
			ImGui::BeginDisabled(!b_PixelsNone && !b_PixelsRaw);

			ImGui::SetNextItemWidth(ItemWidth);
			if (ImGui::InputScalar("Width##CreateModal", ImGuiDataType_U16, &m_CreateInfo.Width))
			{
				AdjustWidthInput(m_CreateInfo.Width);
			}
			if (ScrollOnHover(&m_CreateInfo.Width, ImGuiDataType_U16, 16, 1, 1024))
			{
				AdjustWidthInput(m_CreateInfo.Width);
			}
			TooltipOnHover("Value must be in decimal format\r\n\r\nMouse scrollwheel can be used while hovering");

			ImGui::SetNextItemWidth(ItemWidth);
			if (ImGui::InputScalar("Height##CreateModal", ImGuiDataType_U16, &m_CreateInfo.Height))
			{
				AdjustHeightInput(m_CreateInfo.Height);
			}
			if (ScrollOnHover(&m_CreateInfo.Height, ImGuiDataType_U16, 16, 1, 512))
			{
				AdjustHeightInput(m_CreateInfo.Height);
			}
			TooltipOnHover("Value must be in decimal format\r\n\r\nMouse scrollwheel can be used while hovering");

			ImGui::EndDisabled();
		}

		ImGui::NewLine();

		{
			{
				ImGui::BeginDisabled(b_PixelsNone);
				ImGui::SetNextItemWidth(ItemWidth);
				String PointerStr = Str.FormatCStyle("%llx", m_CreateInfo.pPixel);
				if (ImGui::InputTextWithHint("Pixel Offset##CreateModal", "hexadecimal", &PointerStr))
				{
					m_CreateInfo.pPixel = std::stoull(PointerStr, nullptr, 16);
				}
				ScrollOnHover(&m_CreateInfo.pPixel, ImGuiDataType_U64, 16, 0, -1);
				TooltipOnHover("Value must be in hexadecimal format");
				if (!b_PixelsNone)
				{
					Tooltip(Str.FormatCStyle("Pointer to pixel file or raw pixels in \"%ws\"\r\n\r\nMouse scrollwheel can be used while hovering", m_CreateInfo.Pixel.filename().wstring().c_str()));
				}
				else
				{
					Tooltip("...");
				}
				ImGui::EndDisabled();
			}

			{
				uint32_t iFileType = b_PixelsNone ? 0 : b_PixelsRaw ? 1 : b_PixelsTIM ? 2 : b_PixelsPXL ? 3 : b_PixelsBMP ? 4 : b_PixelsPNG ? 5 : b_PixelsJPG ? 6 : 0;

				ImGui::SetNextItemWidth(ItemWidth);

				if (ImGui::BeginCombo("Pixel File Type##CreateModal", PixelFileTypes[iFileType]))
				{
					iFileType = std::clamp(iFileType, 0U, 6U);

					for (std::size_t i = 0; i < IM_ARRAYSIZE(PixelFileTypes); i++)
					{
						bool b_IsSelected = (iFileType == i);

						if (ImGui::Selectable(PixelFileTypes[i], b_IsSelected))
						{
							iFileType = (uint32_t)i;
							switch (iFileType)
							{
							case 0:
								m_CreateInfo.PixelType = ImageType::None;
								break;
							case 1:
								m_CreateInfo.PixelType = ImageType::RAW;
								break;
							case 2:
								m_CreateInfo.PixelType = ImageType::TIM;
								break;
							case 3:
								m_CreateInfo.PixelType = ImageType::PXL;
								break;
							case 4:
								m_CreateInfo.PixelType = ImageType::BMP;
								break;
							case 5:
								m_CreateInfo.PixelType = ImageType::PNG;
								break;
							case 6:
								m_CreateInfo.PixelType = ImageType::JPG;
								break;
							}
						}
						if (b_IsSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}

					ImGui::EndCombo();
				}

				switch (iFileType)
				{
				case 0:
					Tooltip(Str.FormatCStyle("Create %d x %d x %dbpp canvas", m_CreateInfo.Width, m_CreateInfo.Height, m_CreateInfo.Depth));
					break;
				case 1:
				{
					std::uint16_t Width = m_CreateInfo.Width;

					switch (m_CreateInfo.Depth)
					{
					case 4:
						Width = Width / 2;
						break;
					case 8:
						Width = Width;
						break;
					case 16:
						Width = Width * 2;
						break;
					case 24:
						Width = Width * 3;
						break;
					}

					Tooltip(Str.FormatCStyle("Read raw pixel data (0x%llx bytes) at 0x%llx in \"%ws\"", (Width * m_CreateInfo.Height), m_CreateInfo.pPixel, m_CreateInfo.Pixel.filename().wstring().c_str()));
				}
					break;
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
					Tooltip(Str.FormatCStyle("Read \"%s\" file type at 0x%llx in \"%ws\"", PixelFileTypes[iFileType], m_CreateInfo.pPixel, m_CreateInfo.Pixel.filename().wstring().c_str()));
					break;
				}
			}

			{
				ImGui::BeginDisabled(b_PixelsNone);
				String ButtonName = "";
				!b_PixelsNone ? ButtonName = "Pixel File##CreateModal" : ButtonName = "...##CreateModalNoExtPixels";
				if (ImGui::Button(ButtonName.c_str(), ImVec2(ItemWidth, 0.0f)))
				{
					SetExternalPixelFilename();
				}
				Tooltip("Select file name of where pixel data is stored");
				ImGui::Text("%ws", m_CreateInfo.Pixel.filename().wstring().c_str());
				ImGui::EndDisabled();
			}
		}

		ImGui::NewLine();

		{
			{
				ImGui::BeginDisabled(!b_PaletteNone && !b_PaletteRaw);
				ImGui::SetNextItemWidth(ItemWidth);
				if (ImGui::InputScalar("Palette Amount##CreateModal", ImGuiDataType_U16, &m_CreateInfo.nPalette))
				{
					m_CreateInfo.nPalette = std::clamp(m_CreateInfo.nPalette, (uint16_t)0, nMaxTIMPalette);
				}
				ScrollOnHover(&m_CreateInfo.nPalette, ImGuiDataType_U16, 1, 0, nMaxTIMPalette);
				TooltipOnHover("Value must be in decimal format");
				Tooltip(Str.FormatCStyle("4bpp = 16 colors (0x20 bytes) per palette\r\n\r\n8bpp = 256 colors (0x200 bytes) per palette\r\n\r\nMouse scrollwheel can be used while hovering", m_CreateInfo.Palette.filename().wstring().c_str()));
				ImGui::EndDisabled();
			}

			{
				ImGui::BeginDisabled(b_PaletteNone);
				ImGui::SetNextItemWidth(ItemWidth);
				String PointerStr = Str.FormatCStyle("%llx", m_CreateInfo.pPalette);
				if (ImGui::InputTextWithHint("Palette Offset##CreateModal", "hexadecimal", &PointerStr))
				{
					m_CreateInfo.pPalette = std::stoull(PointerStr, nullptr, 16);
				}
				ScrollOnHover(&m_CreateInfo.pPalette, ImGuiDataType_U64, 16, 0, -1);
				TooltipOnHover("Value must be in hexadecimal format");
				if (!b_PaletteNone)
				{
					Tooltip(Str.FormatCStyle("Pointer to palette file or raw palette(s) in \"%ws\"\r\n\r\nMouse scrollwheel can be used while hovering", m_CreateInfo.Pixel.filename().wstring().c_str()));
				}
				else
				{
					Tooltip("...");
				}
				ImGui::EndDisabled();
			}

			{
				uint32_t iFileType = b_PaletteNone ? 0 : b_PaletteRaw ? 1 : b_PaletteTIM ? 2 : b_PaletteCLT ? 3 : b_PalettePAL ? 4 : 0;

				ImGui::SetNextItemWidth(ItemWidth);

				if (ImGui::BeginCombo("Palette File Type##CreateModal", PaletteFileTypes[iFileType]))
				{
					iFileType = std::clamp(iFileType, 0U, 6U);

					for (std::size_t i = 0; i < IM_ARRAYSIZE(PaletteFileTypes); i++)
					{
						bool b_IsSelected = (iFileType == i);

						if (ImGui::Selectable(PaletteFileTypes[i], b_IsSelected))
						{
							iFileType = (uint32_t)i;
							switch (iFileType)
							{
							case 0:
								m_CreateInfo.PaletteType = ImageType::None;
								break;
							case 1:
								m_CreateInfo.PaletteType = ImageType::RAW;
								break;
							case 2:
								m_CreateInfo.PaletteType = ImageType::TIM;
								break;
							case 3:
								m_CreateInfo.PaletteType = ImageType::CLT;
								break;
							case 4:
								m_CreateInfo.PaletteType = ImageType::BMP;
								break;
							case 5:
								m_CreateInfo.PaletteType = ImageType::PAL;
								break;
							case 6:
								m_CreateInfo.PaletteType = ImageType::PNG;
								break;
							}
						}
						if (b_IsSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				switch (iFileType)
				{
				case 0:
					Tooltip(Str.FormatCStyle("Create %d palette(s)", m_CreateInfo.nPalette));
					break;
				case 1:
					Tooltip(Str.FormatCStyle("Read %d palette(s) (0x%llx bytes) at 0x%llx in \"%ws\"", m_CreateInfo.nPalette,
						m_CreateInfo.Depth == 4 ? (m_CreateInfo.nPalette * 0x20) : (m_CreateInfo.nPalette * 0x200),
						m_CreateInfo.pPalette, m_CreateInfo.Palette.filename().wstring().c_str()));
					break;
				case 2:
				case 3:
				case 4:
					Tooltip(Str.FormatCStyle("Read \"%s\" file type at %llx in \"%ws\"", PaletteFileTypes[iFileType], m_CreateInfo.pPalette, m_CreateInfo.Palette.filename().wstring().c_str()));
					break;
				}
			}
			
			{
				ImGui::BeginDisabled(b_PaletteNone);
				String ButtonName = "";
				!b_PaletteNone ? ButtonName = "Palette File##CreateModal" : ButtonName = "...##CreateModalNoExtPalette";
				if (ImGui::Button(ButtonName.c_str(), ImVec2(ImGui::CalcTextSize("Width##CreateModal").x, 0)))
				{
					SetExternalPaletteFilename();
				}
				Tooltip("Select file name of where palette data is stored");
				ImGui::Text("%ws", m_CreateInfo.Palette.filename().wstring().c_str());
				ImGui::EndDisabled();
			}
		}

		ImGui::NewLine();

		{
			if (ImGui::Button("Create##CreateModal", ImVec2(ImGui::CalcTextSize("___##CreateModal").x, 0)))
			{
				if (Create())
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

void Global_Application::SearchModal(void)
{
	if (!ImGui::IsPopupOpen("Search##SearchModal"))
	{
		ImGui::OpenPopup("Search##SearchModal");
	}

	ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x - ImGui::GetWindowSize().x) / 2, (ImGui::GetIO().DisplaySize.y - ImGui::GetWindowSize().y) / 1.5f));

	if (ImGui::BeginPopupModal("Search##SearchModal", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
	{
		if (m_ProgressBar >= 1.0f)
		{
			ImGui::CloseCurrentPopup();
			SearchModalFunc = []() {};
		}

		ImGui::ProgressBar(m_ProgressBar, ImVec2(0.0f, 0.0f));

		float WindowWidth = ImGui::GetWindowSize().x;
		float ButtonWidth = ImGui::CalcTextSize("Cancel##SearchModal").x + ImGui::GetStyle().FramePadding.x * 2.0f;
		float ButtonHeight = ImGui::GetFrameHeight() * 1.25f;
		ImGui::SetCursorPosX((WindowWidth - ButtonWidth) / 2.0f);

		if (ImGui::Button("Cancel##SearchModal", ImVec2(ButtonWidth, ButtonHeight)))
		{
			SearchModalCb(1.0f, b_Searching = false);
			ImGui::CloseCurrentPopup();
			SearchModalFunc = []() {};
		}

		ImGui::SetWindowSize(ImVec2(ImGui::GetItemRectMax().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetItemRectMax().y + ImGui::GetStyle().WindowPadding.y));

		ImGui::EndPopup();
	}
}

void Global_Application::SaveAllModal(void) const
{
	if (!ImGui::IsPopupOpen("Save As##SaveAllAsModal"))
	{
		ImGui::OpenPopup("Save As##SaveAllAsModal");
	}

	ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x - ImGui::GetWindowSize().x) / 2, (ImGui::GetIO().DisplaySize.y - ImGui::GetWindowSize().y) / 1.5f));

	if (ImGui::BeginPopupModal("Save As##SaveAllAsModal", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
	{
		if (m_ProgressBar >= 1.0f)
		{
			ImGui::CloseCurrentPopup();
			SaveAllFunc = []() {};
		}

		ImGui::ProgressBar(m_ProgressBar, ImVec2(0.0f, 0.0f));

		float WindowWidth = ImGui::GetWindowSize().x;
		float ButtonWidth = ImGui::CalcTextSize("Cancel##SaveAllAsModal").x + ImGui::GetStyle().FramePadding.x * 2.0f;
		float ButtonHeight = ImGui::GetFrameHeight() * 1.25f;
		ImGui::SetCursorPosX((WindowWidth - ButtonWidth) / 2.0f);

		if (ImGui::Button("Cancel##SaveAllAsModal", ImVec2(ButtonWidth, ButtonHeight)))
		{
			ImGui::CloseCurrentPopup();
			SaveAllFunc = []() {};
		}

		ImGui::SetWindowSize(ImVec2(ImGui::GetItemRectMax().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetItemRectMax().y + ImGui::GetStyle().WindowPadding.y));

		ImGui::EndPopup();
	}
}