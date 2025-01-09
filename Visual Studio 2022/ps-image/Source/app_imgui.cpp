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
	bool b_MenuExportPixelEnabled = false;
	if ((Texture) && (!Texture->GetPixels().empty()))
	{
		b_MenuExportPixelEnabled = true;
	}

	bool b_MenuExportPaletteEnabled = false;
	if ((Texture) && (!Texture->GetPalette().empty()))
	{
		b_MenuExportPaletteEnabled = true;
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
			if (ImGui::MenuItem("Save As##FileMenu", "CTRL+S", nullptr, (b_MenuExportPixelEnabled || b_MenuExportPaletteEnabled)))
			{
				SaveAs();
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
			if (ImGui::BeginMenu("Export", (b_MenuExportPixelEnabled || b_MenuExportPaletteEnabled)))
			{
				if (ImGui::MenuItem("Palette (Current)##FileMenuExport", NULL, nullptr, b_MenuExportPaletteEnabled))
				{
					ExportPalette(m_Palette);
				}
				if (ImGui::MenuItem("Palette (All)##FileMenuExport", NULL, nullptr, b_MenuExportPaletteEnabled))
				{
					ExportPalette();
				}
				if (ImGui::MenuItem("Palette As PAL##FileMenuExport", NULL, nullptr, b_MenuExportPaletteEnabled))
				{
					ExportPaletteToPAL();
				}
				if (ImGui::MenuItem("Palette As TIM##FileMenuExport", NULL, nullptr, b_MenuExportPaletteEnabled))
				{
					ExportPaletteToTIM();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Pixels##FileMenuExport", NULL, nullptr, b_MenuExportPixelEnabled))
				{
					ExportPixels();
				}
				if (ImGui::MenuItem("Pixels As TIM##FileMenuExport", NULL, nullptr, b_MenuExportPixelEnabled))
				{
					ExportPixelsToTIM();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Import", Texture ? true : false))
			{
				if (ImGui::MenuItem("Palette (Current)##FileMenuImport", NULL, nullptr, Texture ? true : false))
				{
					ImportPalette(m_Palette);
				}
				if (ImGui::MenuItem("Palette (All)##FileMenuExport", NULL, nullptr, Texture ? true : false))
				{
					ImportPalette();
				}
				if (ImGui::MenuItem("Palette From PAL##FileMenuExport", NULL, nullptr, Texture ? true : false))
				{
					ImportPaletteFromPAL();
				}
				if (ImGui::MenuItem("Palette From TIM##FileMenuImport", NULL, nullptr, Texture ? true : false))
				{
					ImportPaletteFromTIM();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Pixels##FileMenuImport", NULL, nullptr, (b_MenuExportPixelEnabled || b_MenuExportPaletteEnabled)))
				{
					ImportPixels();
				}
				if (ImGui::MenuItem("Pixels From TIM##FileMenuImport", NULL, nullptr, Texture ? true : false))
				{
					ImportPixelsFromTIM();
				}
				ImGui::EndMenu();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Close##FileMenu", NULL, nullptr, Image ? true : false))
			{
				Close();
				b_TransparencySuperimposed = b_Transparency = false;
				Texture->GetTransparentColor() = NULL;
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
			ImGui::MenuItem("Palette Editor##ViewMenu", NULL, &b_ViewPaletteWindow, b_MenuExportPaletteEnabled);
			ImGui::MenuItem("File Properties##ViewMenu", NULL, &b_ViewFileWindow, m_Files.empty() ? false : true);
			ImGui::Separator();
			ImGui::MenuItem("Bitstream Options##ViewMenu", NULL, &b_ViewBitstreamOptions);
			ImGui::Separator();
			if (ImGui::MenuItem("Next Palette##ViewMenu", "RIGHT", nullptr, b_MenuExportPaletteEnabled))
			{
				SetPalette(++m_Palette);
			}
			if (ImGui::MenuItem("Prev Palette##ViewMenu", "LEFT", nullptr, b_MenuExportPaletteEnabled))
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
				String ControlsStr = Str.FormatCStyle("\r\nCTRL+O -- Open (any file type)");
				ControlsStr += Str.FormatCStyle("\r\nCTRL+N -- New Sony PlayStation Texture Image (*.TIM)");
				ControlsStr += Str.FormatCStyle("\r\nCTRL+S -- Save Sony PlayStation Texture Image (*.TIM)");
				ControlsStr += Str.FormatCStyle("\r\nUP/DOWN -- View previous/next texture (when file count > 1)");
				ControlsStr += Str.FormatCStyle("\r\nLEFT/RIGHT -- View previous/next color lookup table (palette)");\
				ControlsStr += Str.FormatCStyle("\r\nCTRL+MOUSEWHEEL -- Adjust image zoom");
				ControlsStr += Str.FormatCStyle("\r\nF11 -- Enter/Exit fullscreen mode");
				ControlsStr += Str.FormatCStyle("\r\nCTRL+? -- About this application");
				ControlsStr += Str.FormatCStyle("\r\nESC -- Exit Application");

				Window->MessageModal(L"Keyboard and Mouse Controls", L"", Str.GetWide(ControlsStr));
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
			b_TransparencySuperimposed = b_Transparency = false;
			Texture->GetTransparentColor() = NULL;
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
			if (Image)
			{
				ImGui::Text(m_Filename.filename().string().c_str());
				ImGui::Indent(ImGui::CalcTextSize(m_Filename.filename().string().c_str()).x);

				ImGui::Text(Str.FormatCStyle("  |  %d x %d  |  %dbpp", Image->GetWidth(), Image->GetHeight(), Image->GetDepth()).c_str());

				if (Texture)
				{
					if (!Texture->GetPalette().empty())
					{
						ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize(Str.FormatCStyle("palette: %d / %d ", m_Palette, Texture->GetClutSize() - 1).c_str()).x);
						ImGui::Text(Str.FormatCStyle("palette: %d / %d", m_Palette, Texture->GetClutSize() - 1).c_str());
					}
					else if (Texture->GetPalette().empty() && Texture->GetPixels().empty())
					{
						ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize("Warning: palette and pixel data is empty ").x);
						ImGui::Text("Warning: palette and pixel data is empty ");
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
	}

	ImGui::BeginDisabled(!b_ToolbarIconsOnBoot);
	ImGui::Checkbox("Tool Bar##OptionsWindow", &b_ViewToolbar);
	ImGui::EndDisabled();

	ImGui::Checkbox("Status Bar##OptionsWindow", &b_ViewStatusbar);

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

					if (Texture)
					{
						ImGui::Text(TextureInfo.c_str());

						{
							ImGui::SetNextItemWidth(ImGui::CalcTextSize("VRAM X (Pixels)##FileWindow").x);
							String PointerStr = Str.FormatCStyle("%d", Texture->GetClutX());
							if (ImGui::InputTextWithHint("VRAM X (CLUT)##FileWindow", "decimal", &PointerStr))
							{
								Texture->GetClutX() = std::stoi(PointerStr, nullptr, 10);
								Texture->GetClutX() = std::clamp(Texture->GetClutX(), static_cast<std::int16_t>(0), static_cast<std::int16_t>(1024));
								TextureInfo = Texture->About();
							}
							if (ImGui::IsItemHovered())
							{
								ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelX);
								ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);

								if (Window->Device()->GetMouseDeltaZ() != 0.0f)
								{
									if (Window->Device()->GetMouseDeltaZ() < 0.0f)
									{
										Texture->GetClutX() -= 16;
									}
									else
									{
										Texture->GetClutX() += 16;
									}
									Texture->GetClutX() = std::clamp(Texture->GetClutX(), static_cast<std::int16_t>(0), static_cast<std::int16_t>(1024));
									TextureInfo = Texture->About();
								}
							}
							TooltipOnHover("Value must be in decimal format");
							Tooltip(Str.FormatCStyle("Horizontal position of palette data in VRAM\r\n\r\nMouse scrollwheel can be used while hovering"));

							ImGui::SetNextItemWidth(ImGui::CalcTextSize("VRAM Y (Pixels)##FileWindow").x);
							PointerStr = Str.FormatCStyle("%d", Texture->GetClutY());
							if (ImGui::InputTextWithHint("VRAM Y (CLUT)##FileWindow", "decimal", &PointerStr))
							{
								Texture->GetClutY() = std::stoi(PointerStr, nullptr, 10);
								Texture->GetClutY() = std::clamp(Texture->GetClutY(), static_cast<std::int16_t>(0), static_cast<std::int16_t>(512));
								TextureInfo = Texture->About();
							}
							if (ImGui::IsItemHovered())
							{
								ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelX);
								ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);

								if (Window->Device()->GetMouseDeltaZ() != 0.0f)
								{
									if (Window->Device()->GetMouseDeltaZ() < 0.0f)
									{
										Texture->GetClutY() -= 16;
									}
									else
									{
										Texture->GetClutY() += 16;
									}
									Texture->GetClutY() = std::clamp(Texture->GetClutY(), static_cast<std::int16_t>(0), static_cast<std::int16_t>(512));
									TextureInfo = Texture->About();
								}
							}
							TooltipOnHover("Value must be in decimal format");
							Tooltip(Str.FormatCStyle("Vertical position of palette data in VRAM\r\n\r\nMouse scrollwheel can be used while hovering"));
						}

						{
							ImGui::SetNextItemWidth(ImGui::CalcTextSize("VRAM X (Pixels)##FileWindow").x);
							String PointerStr = Str.FormatCStyle("%d", Texture->GetDataX());
							if (ImGui::InputTextWithHint("VRAM X (Data)##FileWindow", "decimal", &PointerStr))
							{
								Texture->GetDataX() = std::stoi(PointerStr, nullptr, 10);
								Texture->GetDataX() = std::clamp(Texture->GetDataX(), static_cast<std::int16_t>(0), static_cast<std::int16_t>(1024));
								TextureInfo = Texture->About();
							}
							if (ImGui::IsItemHovered())
							{
								ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelX);
								ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);

								if (Window->Device()->GetMouseDeltaZ() != 0.0f)
								{
									if (Window->Device()->GetMouseDeltaZ() < 0.0f)
									{
										Texture->GetDataX() -= 16;
									}
									else
									{
										Texture->GetDataX() += 16;
									}
									Texture->GetDataX() = std::clamp(Texture->GetDataX(), static_cast<std::int16_t>(0), static_cast<std::int16_t>(1024));
									TextureInfo = Texture->About();
								}
							}
							TooltipOnHover("Value must be in decimal format");
							Tooltip(Str.FormatCStyle("Horizontal position of pixel data in VRAM\r\n\r\nMouse scrollwheel can be used while hovering"));

							ImGui::SetNextItemWidth(ImGui::CalcTextSize("VRAM Y (Pixels)##FileWindow").x);
							PointerStr = Str.FormatCStyle("%d", Texture->GetDataY());
							if (ImGui::InputTextWithHint("VRAM Y (Data)##FileWindow", "decimal", &PointerStr))
							{
								Texture->GetDataY() = std::stoi(PointerStr, nullptr, 10);
								Texture->GetDataY() = std::clamp(Texture->GetDataY(), static_cast<std::int16_t>(0), static_cast<std::int16_t>(512));
								TextureInfo = Texture->About();
							}
							if (ImGui::IsItemHovered())
							{
								ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelX);
								ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);

								if (Window->Device()->GetMouseDeltaZ() != 0.0f)
								{
									if (Window->Device()->GetMouseDeltaZ() < 0.0f)
									{
										Texture->GetDataY() -= 16;
									}
									else
									{
										Texture->GetDataY() += 16;
									}
									Texture->GetDataY() = std::clamp(Texture->GetDataY(), static_cast<std::int16_t>(0), static_cast<std::int16_t>(512));
									TextureInfo = Texture->About();
								}
							}
							TooltipOnHover("Value must be in decimal format");
							Tooltip(Str.FormatCStyle("Vertical position of pixel data in VRAM\r\n\r\nMouse scrollwheel can be used while hovering"));
						}

						ImGui::BeginDisabled(Texture->GetPixels().empty() && Texture->GetPalette().empty());
						if (ImGui::Button("Export##FileWindow", ImVec2(ImGui::CalcTextSize("Extract##FileWindow").x, 0)))
						{
							Export(m_SelectedFile);
						}
						TooltipOnHover("Export this PlayStation Texture Image");
						ImGui::EndDisabled();

						ImGui::SameLine();

						if (ImGui::Button("Import##FileWindow", ImVec2(ImGui::CalcTextSize("Replace##FileWindow").x, 0)))
						{
							Import(m_SelectedFile);
						}
						TooltipOnHover("Replace this PlayStation Texture Image");

						ImGui::BeginDisabled(Texture->GetPixels().empty());
						if (ImGui::Button("Export RAW##FileWindow", ImVec2(ImGui::CalcTextSize("Extract##FileWindow").x, 0)))
						{
							ExportPixels();
						}
						TooltipOnHover("Export pixel data to raw binary file");
						ImGui::EndDisabled();

						ImGui::SameLine();

						ImGui::BeginDisabled(Texture->GetPixels().empty());
						if (ImGui::Button("Import RAW##FileWindow", ImVec2(ImGui::CalcTextSize("Replace##FileWindow").x, 0)))
						{
							ImportPixels();
						}
						TooltipOnHover("Import pixel data from raw binary file");
						ImGui::EndDisabled();

						ImGui::BeginDisabled(Texture->GetPixels().empty());
						if (ImGui::Button("Export TIM##FileWindow", ImVec2(ImGui::CalcTextSize("Extract##FileWindow").x, 0)))
						{
							ExportPixelsToTIM();
						}
						TooltipOnHover("Export pixel data to PlayStation Texture Image");
						ImGui::EndDisabled();

						ImGui::SameLine();

						if (ImGui::Button("Import TIM##FileWindow", ImVec2(ImGui::CalcTextSize("Replace##FileWindow").x, 0)))
						{
							ImportPixelsFromTIM();
						}
						TooltipOnHover("Import pixel data from PlayStation Texture Image");

						ImGui::BeginDisabled(Texture->GetPixels().empty());
						if (ImGui::Button("Save As Bitmap##FileWindow", ImVec2(ImGui::CalcTextSize("Extract##FileWindow").x, 0)))
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

	if (!Texture) { return; }

	std::uint16_t nColor = Texture->GetDepth() == 4 ? 16 : 256;
	std::uint32_t OldPalette = std::clamp(m_Palette, 0U, Texture->GetClutSize() - 1);

	ImGui::Begin("Palette##PaletteWindow", &b_ViewPaletteWindow, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);

	ImGui::BeginDisabled(Texture->GetPalette().empty());

	float SliderIntWidth = ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("  ID##PaletteWindow").x / 3 - ImGui::GetStyle().ItemInnerSpacing.x * 2;
	ImGui::SetNextItemWidth(SliderIntWidth);
	ImGui::SliderInt("  ID##PaletteWindow", (int*)&m_Palette, 0, Texture->GetPalette().empty() ? 0 : Texture->GetClutSize() - 1);
	m_Palette = std::clamp(m_Palette, 0U, Texture->GetClutSize() - 1);
	if (ImGui::IsItemHovered())
	{
		ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelX);
		ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);

		if (Window->Device()->GetMouseDeltaZ() != 0.0f)
		{
			if (Window->Device()->GetMouseDeltaZ() < 0.0f)
			{
				m_Palette = std::clamp(m_Palette - 1, 0U, Texture->GetClutSize() - 1);
			}
			else
			{
				m_Palette = std::clamp(m_Palette + 1, 0U, Texture->GetClutSize() - 1);
			}
		}
	}
	Tooltip("Active Color Lookup Table\r\n\r\nMousewheel (while hovering) and Left/Right arrow keys can also be used");
	if (OldPalette != m_Palette)
	{
		SetPalette(m_Palette);
	}

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

	ImGui::EndDisabled();

	if (Texture->GetPalette().empty())
	{
		m_Palette = 0;
	}

	ImGui::BeginDisabled(Texture->GetPalette().empty());
	if (ImGui::Button("Copy##PaletteWindow"))
	{
		CopyPalette();
	}
	TooltipOnHover(Str.FormatCStyle("Copy the currently selected palette (%d) to the clipboard", m_Palette));
	ImGui::EndDisabled();

	ImGui::SameLine();

	ImGui::BeginDisabled(Texture->GetPalette().empty());
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
	TooltipOnHover(Str.FormatCStyle("Append a new palette after the max (%d)", Texture->GetClutSize() - 1));

	ImGui::SameLine();

	if (ImGui::Button("Insert##PaletteWindow"))
	{
		InsertPalette();
	}
	TooltipOnHover(Str.FormatCStyle("Insert a new palette after the currently selected (%d)", m_Palette));

	ImGui::SameLine();

	ImGui::BeginDisabled(Texture->GetPalette().empty());
	if (ImGui::Button("Delete##PaletteWindow"))
	{
		DeletePalette();
	}
	TooltipOnHover(Str.FormatCStyle("Delete the currently selected palette (%d)", m_Palette));
	ImGui::EndDisabled();

	ImGui::BeginDisabled(Texture->GetPalette().empty());
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

	ImGui::BeginDisabled(Texture->GetPalette().empty());
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
	TooltipOnHover(Str.FormatCStyle("Import all palettes from raw file data"));

	ImGui::BeginDisabled(Texture->GetPalette().empty());
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
	TooltipOnHover(Str.FormatCStyle("Import all palettes from Sony Texture Image", m_Palette));

	{
		ImGui::SetNextItemWidth(ImGui::CalcTextSize("VRAM X##PaletteWindow").x);
		String PointerStr = Str.FormatCStyle("%d", Texture->GetClutX());
		if (ImGui::InputTextWithHint("VRAM X##PaletteWindow", "decimal", &PointerStr))
		{
			Texture->GetClutX() = std::stoi(PointerStr, nullptr, 10);
			Texture->GetClutX() = std::clamp(Texture->GetClutX(), static_cast<std::int16_t>(0), static_cast<std::int16_t>(1024));
			TextureInfo = Texture->About();
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelX);
			ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);

			if (Window->Device()->GetMouseDeltaZ() != 0.0f)
			{
				if (Window->Device()->GetMouseDeltaZ() < 0.0f)
				{
					Texture->GetClutX() -= 16;
				}
				else
				{
					Texture->GetClutX() += 16;
				}
				Texture->GetClutX() = std::clamp(Texture->GetClutX(), static_cast<std::int16_t>(0), static_cast<std::int16_t>(1024));
				TextureInfo = Texture->About();
			}
		}
		TooltipOnHover("Value must be in decimal format");
		Tooltip(Str.FormatCStyle("Horizontal position of palette in VRAM\r\n\r\nMouse scrollwheel can be used while hovering"));

		ImGui::SetNextItemWidth(ImGui::CalcTextSize("VRAM Y##PaletteWindow").x);
		PointerStr = Str.FormatCStyle("%d", Texture->GetClutY());
		if (ImGui::InputTextWithHint("VRAM Y##PaletteWindow", "decimal", &PointerStr))
		{
			Texture->GetClutY() = std::stoi(PointerStr, nullptr, 10);
			Texture->GetClutY() = std::clamp(Texture->GetClutY(), static_cast<std::int16_t>(0), static_cast<std::int16_t>(512));
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelX);
			ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);

			if (Window->Device()->GetMouseDeltaZ() != 0.0f)
			{
				if (Window->Device()->GetMouseDeltaZ() < 0.0f)
				{
					Texture->GetClutY() -= 16;
				}
				else
				{
					Texture->GetClutY() += 16;
				}
				Texture->GetClutY() = std::clamp(Texture->GetClutY(), static_cast<std::int16_t>(0), static_cast<std::int16_t>(512));
				TextureInfo = Texture->About();
			}
		}
		TooltipOnHover("Value must be in decimal format");
		Tooltip(Str.FormatCStyle("Vertical position of palette in VRAM\r\n\r\nMouse scrollwheel can be used while hovering"));
	}

	if (Texture->GetPalette().empty())
	{
		nColor = 0;
	}
	for (std::uint16_t x = 0; x < nColor; x++)
	{
		if (x % 16) { ImGui::SameLine(); }

		ImVec4 ColorF = ImVec4(
			Texture->Red(Texture->GetPalette()[m_Palette][x]) / 255.0f,
			Texture->Green(Texture->GetPalette()[m_Palette][x]) / 255.0f,
			Texture->Blue(Texture->GetPalette()[m_Palette][x]) / 255.0f,
			1.0f);

		ImGui::ColorEdit3(Str.FormatCStyle("Palette Index [%d]##PaletteWindowIndex%d", x, x).c_str(), (float*)&ColorF, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoOptions);

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
			ResetTexture(b_Transparency);
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
	if (ImGui::InputScalar("Width##BitstreamSettings", ImGuiDataType_S32, &m_BistreamWidth))
	{
		m_BistreamWidth = std::clamp(m_BistreamWidth, 1, 640);
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelX);
		ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);

		if (Window->Device()->GetMouseDeltaZ() != 0.0f)
		{
			if (Window->Device()->GetMouseDeltaZ() < 0.0f)
			{
				m_BistreamWidth -= 16;
			}
			else
			{
				m_BistreamWidth += 16;
			}
			m_BistreamWidth = std::clamp(m_BistreamWidth, 0, 640);
		}
	}
	TooltipOnHover("Width of Sony Bitstream (*.bs) image");

	ImGui::SetNextItemWidth(ImGui::CalcTextSize("Width##BitstreamSettings").x);
	if (ImGui::InputScalar("Height##BitstreamSettings", ImGuiDataType_S32, &m_BistreamHeight))
	{
		m_BistreamHeight = std::clamp(m_BistreamHeight, 1, 480);
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelX);
		ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);

		if (Window->Device()->GetMouseDeltaZ() != 0.0f)
		{
			if (Window->Device()->GetMouseDeltaZ() < 0.0f)
			{
				m_BistreamHeight -= 16;
			}
			else
			{
				m_BistreamHeight += 16;
			}
			m_BistreamHeight = std::clamp(m_BistreamHeight, 0, 640);
		}
	}
	TooltipOnHover("Height of Sony Bitstream (*.bs) image");

	Tooltip("Width and Height of Sony Bitstream (*.bs) image\r\n\r\nValues are used when opening a Bitstream image");

	ImGui::SetWindowSize(ImVec2(ImGui::GetItemRectMax().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetItemRectMax().y + ImGui::GetStyle().WindowPadding.y));

	ImGui::End();
}

void Global_Application::ImageSettings(void)
{
	if (!b_ViewImageOptions) { return; }

	const char* TextureFilterTypes[] = { "None", "Point", "Linear", "Anisotropic" };

	ImGui::Begin("Image Options##UtilityWindow", &b_ViewImageOptions, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);

	ImGui::Checkbox("Dithering##UtilityWindow", &b_Dithering);
	Tooltip("Sony PlayStation (1994) Dithering Pixel Shader\r\n\r\nPreview only; effects are not written to TIM data");

	ImGui::SliderFloat("  Zoom##UtilityWindow", (float*)&m_ImageZoom, m_ImageZoomMin, m_ImageZoomMax);
	if (ImGui::IsItemHovered())
	{
		ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelX);
		ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);

		if (Window->Device()->GetMouseDeltaZ() != 0.0f)
		{
			if (Window->Device()->GetMouseDeltaZ() < 0.0f)
			{
				m_ImageZoom = std::clamp(m_ImageZoom - 0.25f, m_ImageZoomMin, m_ImageZoomMax);
			}
			else
			{
				m_ImageZoom = std::clamp(m_ImageZoom + 0.25f, m_ImageZoomMin, m_ImageZoomMax);
			}
		}
	}
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

	ImGui::Checkbox("Caption##UtilityWindow", &b_ImageWindowTitleBar);
	Tooltip("Enable/Disable the image window title bar");

	ImGui::Checkbox("Background##UtilityWindow", &b_ImageWindowBackground);
	Tooltip("Enable/Disable the image window background\r\n\r\nWhen disabled, main window background can be used for transparency");

	{
		bool b_TransparencyButton = false;
		bool b_DisableTransparencyButton = false;
		bool b_DisableTransparencyColor = false;
		bool b_DisableSuperimposedButton = false;

		if ((!Texture) ||
			(Texture->GetPixels().empty() && !Texture->GetPalette().empty()) ||
			(Texture->GetDepth() <= 8 && Texture->GetPalette().empty()))
		{
			b_TransparencyButton = true;
			b_DisableTransparencyButton = true;
			b_DisableTransparencyColor = true;
			b_DisableSuperimposedButton = true;
		}
		else
		{
			if (!b_Transparency || b_TransparencySuperimposed)
			{
				b_DisableTransparencyColor = true;
			}
			else if (Texture->GetPalette().empty())
			{
				b_TransparencyButton = !b_Transparency;
			}
		}

		if (!b_DisableSuperimposedButton)
		{
			b_DisableSuperimposedButton = (Texture && Texture->GetPalette().empty());
		}

		ImGui::BeginDisabled(b_TransparencyButton || b_DisableTransparencyButton);
		if (ImGui::Checkbox("Transparency##UtilityWindow", &b_Transparency))
		{
			ResetTexture(b_Transparency);
		}
		Tooltip("Enable/Disable Transparency\r\n\r\nPreview only; effects are not written to TIM data");
		ImGui::EndDisabled();

		ImGui::BeginDisabled(b_TransparencyButton || b_DisableTransparencyColor);
		{
			Sony_Texture_16bpp TransparentColor = Texture->Create16bpp(Texture->GetTransparentColor(), false);

			auto OldRed = GetRValue(Texture->GetTransparentColor());
			auto OldGreen = GetGValue(Texture->GetTransparentColor());
			auto OldBlue = GetBValue(Texture->GetTransparentColor());

			ImVec4 ColorF = ImVec4(
				Texture->Red(TransparentColor) / 255.0f,
				Texture->Green(TransparentColor) / 255.0f,
				Texture->Blue(TransparentColor) / 255.0f,
				1.0f);

			ImGui::SameLine();
			ImGui::ColorEdit3(Str.FormatCStyle("Transparent Color##UtilityWindow").c_str(), (float*)&ColorF, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoOptions);
			ImGui::SameLine();
			Tooltip("Transparent Color\r\n\r\nRepresents solid black pixels when \"Superimposed\"\r\nis disabled and/or palette data is unavailable\r\n\r\nPreview only; effects are not written to TIM data");

			COLORREF Color = RGB(
				static_cast<BYTE>(ColorF.x * 255.0f),
				static_cast<BYTE>(ColorF.y * 255.0f),
				static_cast<BYTE>(ColorF.z * 255.0f));

			auto Red = GetRValue(Color);
			auto Green = GetGValue(Color);
			auto Blue = GetBValue(Color);

			if ((Red != OldRed) || (Green != OldGreen) || (Blue != OldBlue))
			{
				Texture->GetTransparentColor() = RGB(Red, Green, Blue);

				ResetTexture(b_Transparency);
			}
		}
		ImGui::EndDisabled();

		ImGui::SameLine();

		ImGui::BeginDisabled(b_DisableSuperimposedButton);
		if (ImGui::Checkbox("Superimposed##UtilityWindow", &b_TransparencySuperimposed))
		{
			if (!b_Transparency)
			{
				b_Transparency = true;
			}
			ResetTexture(b_Transparency);
		}
		Tooltip("Transparent Color is first color in Palette index");
		ImGui::EndDisabled();
	}

	//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::SetWindowSize(ImVec2(ImGui::GetItemRectMax().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetItemRectMax().y + ImGui::GetStyle().WindowPadding.y));

	ImGui::End();
}

void Global_Application::ImageWindow(void)
{
	if (Image)
	{
		RECT Rect = Window->GetRect();

		ImGuiWindowFlags b_TitleBar = b_ImageWindowTitleBar ? ImGuiWindowFlags_None : ImGuiWindowFlags_NoTitleBar;
		ImGuiWindowFlags b_Background = b_ImageWindowBackground ? ImGuiWindowFlags_None : ImGuiWindowFlags_NoBackground;

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

		ImGui::SetNextWindowSize(ImVec2(ContentSize.x + ImGui::GetStyle().WindowPadding.x * 2, ContentSize.y + ImGui::GetStyle().WindowPadding.y * 3));

		ImGui::SetNextWindowPos(b_ViewToolbar ? ImVec2(2, ImGui::GetFrameHeightWithSpacing() + GetToolbarHeight()) : ImVec2(2, ImGui::GetFrameHeightWithSpacing()));

		if (m_Filename.empty())
		{
			m_Filename = L"untitled.tim";
		}

		ImGui::Begin(m_Filename.filename().string().c_str(), nullptr,
			b_TitleBar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize | 
			b_Background |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoBringToFrontOnFocus);

		ContentSize = ImVec2(ImGui::GetWindowWidth() - ImGui::GetStyle().WindowPadding.x * 3, ImGui::GetWindowHeight() - ImGui::GetStyle().WindowPadding.x * 3);
		ImGui::BeginChild("Image##ImageWindowChild", ContentSize, false, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_HorizontalScrollbar);

		D3DSURFACE_DESC m_TextureDesc{};
		DXTexture->GetLevelDesc(0, &m_TextureDesc);

		m_TextureWidth = static_cast<float>(m_TextureDesc.Width);
		m_TextureHeight = static_cast<float>(m_TextureDesc.Height);

		ImGui::GetWindowDrawList()->AddCallback([](const ImDrawList* parent_list, const ImDrawCmd* cmd)
			{
				G->Render->Device()->SetPixelShaderConstantF(0, &G->m_TextureWidth, 1);
				G->Render->Device()->SetPixelShaderConstantF(1, &G->m_TextureHeight, 1);

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

		ImVec2 uv0 = ImVec2(0.0f, static_cast<float>(Image->GetHeight()) / m_TextureHeight);
		ImVec2 uv1 = ImVec2(static_cast<float>(Image->GetWidth()) / m_TextureWidth, 0.0f);

		ImGui::Image((ImTextureID)(intptr_t)DXTexture, ImVec2(Image->GetWidth()* m_ImageZoom, Image->GetHeight()* m_ImageZoom), uv0, uv1);

		if (ImGui::IsItemHovered())
		{
			float MouseX = (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) / m_ImageZoom;
			float MouseY = (ImGui::GetMousePos().y - ImGui::GetItemRectMin().y) / m_ImageZoom;

			if (MouseX >= 0 && MouseX < Image->GetWidth() && MouseY >= 0 && MouseY < Image->GetHeight())
			{
				ImGui::BeginTooltip();
				ImGui::Text("%d, %d", m_MousePixelX = static_cast<std::int32_t>(MouseX), m_MousePixelY = static_cast<std::int32_t>(MouseY));
				ImGui::EndTooltip();
			}
		}

		ImGui::GetWindowDrawList()->AddCallback(ImDrawCallback_ResetRenderState, nullptr);

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

		float ItemWidth = ImGui::CalcTextSize("Width##CreateModal").x;

		ImGui::BeginDisabled(b_RawExternalPixelsFromTIM);

		// mousewheel on hover needed here
		ImGui::SetNextItemWidth(ItemWidth);
		if (ImGui::InputScalar("Width##CreateModal", ImGuiDataType_S32, &m_RawWidth))
		{
			m_RawWidth = std::clamp(m_RawWidth, 1, 1024);
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelX);
			ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);

			if (Window->Device()->GetMouseDeltaZ() != 0.0f)
			{
				if (Window->Device()->GetMouseDeltaZ() < 0.0f)
				{
					m_RawWidth -= 16;
				}
				else
				{
					m_RawWidth += 16;
				}
				m_RawWidth = std::clamp(m_RawWidth, 0, 1024);
			}
		}
		TooltipOnHover("Value must be in decimal format\r\n\r\nMouse scrollwheel can be used while hovering");

		ImGui::SetNextItemWidth(ItemWidth);
		if (ImGui::InputScalar("Height##CreateModal", ImGuiDataType_S32, &m_RawHeight))
		{
			m_RawHeight = std::clamp(m_RawHeight, 1, 512);
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelX);
			ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);

			if (Window->Device()->GetMouseDeltaZ() != 0.0f)
			{
				if (Window->Device()->GetMouseDeltaZ() < 0.0f)
				{
					m_RawHeight -= 16;
				}
				else
				{
					m_RawHeight += 16;
				}
				m_RawHeight = std::clamp(m_RawHeight, 0, 512);
			}
		}
		TooltipOnHover("Value must be in decimal format\r\n\r\nMouse scrollwheel can be used while hovering");

		ImGui::EndDisabled();

		{
			ImGui::NewLine();

			String PointerStr = Str.FormatCStyle("%llx", m_RawPixelPtr);

			bool b_PixelOffset = true;
			if (b_RawExternalPixels)
			{
				b_PixelOffset = false;
			}
			ImGui::BeginDisabled(b_PixelOffset);
			ImGui::SetNextItemWidth(ItemWidth);
			if (ImGui::InputTextWithHint("Pixel Offset##CreateModal", "hexadecimal", &PointerStr))
			{
				m_RawPixelPtr = std::stoull(PointerStr, nullptr, 16);
				m_RawPixelPtr = std::clamp(m_RawPixelPtr, 0ULL, 0xFFFFFFFFFFFFFFFFULL);
			}
			if (ImGui::IsItemHovered())
			{
				ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelX);
				ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);

				if (Window->Device()->GetMouseDeltaZ() != 0.0f)
				{
					if (Window->Device()->GetMouseDeltaZ() < 0.0f)
					{
						m_RawPixelPtr -= 16;
					}
					else
					{
						m_RawPixelPtr += 16;
					}
				}
				m_RawPixelPtr = std::clamp(m_RawPixelPtr, 0ULL, 0xFFFFFFFFFFFFFFFFULL);
			}
			ImGui::EndDisabled();

			TooltipOnHover("Value must be in hexadecimal format");
			Tooltip("Pointer to the pixel data in the file\r\n\r\n0x prefix not required\r\n\r\nMouse scrollwheel can be used while hovering");

			if (ImGui::Checkbox("Import RAW pixels##CreateModal", &b_RawExternalPixels))
			{
				b_RawExternalPixelsFromTIM = false;
			}
			Tooltip(Str.FormatCStyle("(Optional) Pixel data is stored in \"%ws\"", m_RawPixelFilename.filename().wstring().c_str()));

			if (ImGui::Checkbox("Import TIM Pixels##CreateModal", &b_RawExternalPixelsFromTIM))
			{
				b_RawExternalPixels = false;
			}
			Tooltip(Str.FormatCStyle("(Optional) Import pixel data from TIM file: \"%ws\"", m_RawPixelFilename.filename().wstring().c_str()));

			ImGui::BeginDisabled(!b_RawExternalPixels && !b_RawExternalPixelsFromTIM);
			String ButtonName = "";
			StringW ButtonDesc = L"";
			StringW ButtonFilter = L"";
			b_RawExternalPixels ? ButtonName = "Pixel File##CreateModal" : b_RawExternalPixelsFromTIM ? ButtonName = "Pixel TIM##CreateModal" : ButtonName = "...##CreateModalNoExtPixels";
			b_RawExternalPixels ? ButtonDesc = L"All files" : b_RawExternalPixelsFromTIM ? ButtonDesc = L"Sony Texture Image" : ButtonDesc = L"All files";
			b_RawExternalPixels ? ButtonFilter = L"*.*" : b_RawExternalPixelsFromTIM ? ButtonFilter = L"*.tim" : ButtonFilter = L"*.*";
			if (ImGui::Button(ButtonName.c_str(), ImVec2(ImGui::CalcTextSize("Width##CreateModal").x, 0)))
			{
				SetRawPixelFilename();
			}
			Tooltip("Select filename of where pixel data stored");
			ImGui::Text("%ws", m_RawPixelFilename.filename().wstring().c_str());
			ImGui::EndDisabled();
		}

		ImGui::NewLine();

		{
			ImGui::BeginDisabled(!b_RawExternalPalette && (b_RawExternalPaletteFromTIM && b_RawImportAllPalettesFromTIM));
			ImGui::SetNextItemWidth(ItemWidth);
			ImGui::InputScalar("CLUT Amount##CreateModal", ImGuiDataType_U64, &m_RawPaletteCount);
			if (ImGui::IsItemHovered())
			{
				ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelX);
				ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);

				if (Window->Device()->GetMouseDeltaZ() != 0.0f)
				{
					if (Window->Device()->GetMouseDeltaZ() < 0.0f)
					{
						--m_RawPaletteCount;
					}
					else
					{
						++m_RawPaletteCount;
					}
				}
			}
			TooltipOnHover("Value must be in decimal format");
			m_RawPaletteCount = std::clamp(m_RawPaletteCount, 0ULL, static_cast<std::uintmax_t>(MAX_TIM_PALETTE));
			Tooltip(Str.FormatCStyle("Total count of palettes in \"%ws\"\r\n\r\n4bpp = 16 colors (0x20 bytes) per CLUT\r\n\r\n8bpp = 256 colors (0x200 bytes) per CLUT\r\n\r\nMouse scrollwheel can be used while hovering", m_RawPaletteFilename.filename().wstring().c_str()));
			ImGui::EndDisabled();

			ImGui::BeginDisabled(!b_RawExternalPalette);
			ImGui::SetNextItemWidth(ItemWidth);
			String PointerStr = Str.FormatCStyle("%llx", m_RawPalettePtr);
			if (ImGui::InputTextWithHint("CLUT Offset##CreateModal", "hexadecimal", &PointerStr))
			{
				m_RawPalettePtr = std::stoull(PointerStr, nullptr, 16);
			}
			if (ImGui::IsItemHovered())
			{
				ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelX);
				ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);

				if (Window->Device()->GetMouseDeltaZ() != 0.0f)
				{
					if (Window->Device()->GetMouseDeltaZ() < 0.0f)
					{
						m_RawPalettePtr -= 16;
					}
					else
					{
						m_RawPalettePtr += 16;
					}
				}
			}
			TooltipOnHover("Value must be in hexadecimal format");
			Tooltip(Str.FormatCStyle("(Optional) \"CLUT Amount\" (%d) raw palettes are stored in \"%ws\" at \"CLUT Offset\" (%llx)\r\n\r\n0x prefix not required\r\n\r\nMouse scrollwheel can be used while hovering", m_RawPaletteCount, m_RawPaletteFilename.filename().wstring().c_str(), m_RawPalettePtr));
			ImGui::EndDisabled();

			if (ImGui::Checkbox("Import RAW Palette##CreateModal", &b_RawExternalPalette))
			{
				b_RawExternalPaletteFromTIM = false;
			}
			Tooltip(Str.FormatCStyle("(Optional) \"CLUT Amount\" (%d) palettes are stored in \"%ws\"", m_RawPaletteCount, m_RawPaletteFilename.filename().wstring().c_str()));

			if (ImGui::Checkbox("Import TIM Palette##CreateModal", &b_RawExternalPaletteFromTIM))
			{
				b_RawExternalPalette = false;
			}
			Tooltip(Str.FormatCStyle("(Optional) Import palettes from TIM file: \"%ws\"", m_RawPaletteFilename.filename().wstring().c_str()));

			ImGui::BeginDisabled(!b_RawExternalPaletteFromTIM);
			ImGui::Checkbox("Import all CLUTs from TIM##CreateModal", &b_RawImportAllPalettesFromTIM);
			Tooltip("(Optional) Import all palettes from TIM file");
			ImGui::BeginDisabled(b_RawImportAllPalettesFromTIM);
			ImGui::SetNextItemWidth(ItemWidth);
			ImGui::InputScalar("CLUT ID##BitstreamSettings", ImGuiDataType_U64, &m_RawPaletteID);
			Tooltip("Palette index ID of TIM file to import\r\n\r\nWhen not importing all and if \"CLUT Amount\" is greater than one (1), this value is starting index value");
			m_RawPaletteID = std::clamp(m_RawPaletteID, 0ULL, 0xFFFFULL);
			ImGui::EndDisabled();
			ImGui::EndDisabled();

			String ButtonName = "";
			
			ImGui::BeginDisabled(!b_RawExternalPalette && !b_RawExternalPaletteFromTIM);
			b_RawExternalPalette ? ButtonName = "Palette File##CreateModal" : b_RawExternalPaletteFromTIM ? ButtonName = "Palette TIM##CreateModal" : ButtonName = "...##CreateModalNoExtPalette";
			if (ImGui::Button(ButtonName.c_str(), ImVec2(ImGui::CalcTextSize("Width##CreateModal").x, 0)))
			{
				SetRawPaletteFilename();
			}
			Tooltip("Filename of where palettes are stored");
			ImGui::Text("%ws", m_RawPaletteFilename.filename().wstring().c_str());
			ImGui::EndDisabled();
		}

		ImGui::NewLine();

		if (ImGui::Button("Create##CreateModal", ImVec2(ImGui::CalcTextSize("Create##CreateModal").x, 0)))
		{
			if (Create(b_Raw4bpp ? 4 : b_Raw8bpp ? 8 : b_Raw16bpp ? 16 : 16, m_RawWidth, m_RawHeight, m_RawPixelPtr, m_RawPalettePtr))
			{
				ImGui::CloseCurrentPopup();
				CreateModalFunc = []() {};
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