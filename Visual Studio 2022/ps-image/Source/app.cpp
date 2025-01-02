/*
*
*	Megan Grass
*	December 14, 2024
* 
*	TODO:
* 
*	 - use first color in CLUT for transparency
*	 - crtl+alt+q freezes app instead of closing it
*	 - Render->CreateTexture/UpdateTexture overload for Sony_Texture
*	 - resize texture
*	 - VRAM Window
*	 - ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
*
*/

#include "app.h"

std::unique_ptr<Global_Application> G = std::make_unique<Global_Application>();

std::function<void()> CreateModalFunc = []() {};

void Global_Application::About(void) const
{
	Standard_String Str;

	String AboutStr = Str.FormatCStyle("%ws - %ws", VER_INTERNAL_NAME_STR, VER_FILE_DESCRIPTION_STR);
    AboutStr += Str.FormatCStyle("\r\nv%ws %s %s Megan Grass", VER_PRODUCT_VERSION_STR, __DATE__, __TIME__);
    AboutStr += Str.FormatCStyle("\r\n\r\n<a href=\"https://github.com/MeganGrass/%ws\">https://github.com/megangrass/%ws</a>", VER_INTERNAL_NAME_STR, VER_INTERNAL_NAME_STR);
	AboutStr += Str.FormatCStyle("\r\n\r\nConfiguration:\r\n<a href=\"%ws\">%ws</a>", GetConfigFilename().wstring().c_str(), GetConfigFilename().wstring().c_str());
    AboutStr += Str.FormatCStyle("\r\n\r\nImGui:\r\n<a href=\"https://github.com/ocornut/imgui\">https://github.com/ocornut/imgui</a>");
    AboutStr += Str.FormatCStyle("\r\n\r\nBitstream (*.bs) Decoder:\r\n<a href=\"https://github.com/XProger/OpenResident\">https://github.com/XProger/OpenResident</a>");
    AboutStr += Str.FormatCStyle("\r\n<a href=\"https://psx-spx.consoledev.net/macroblockdecodermdec\">https://psx-spx.consoledev.net/macroblockdecodermdec</a>");
    AboutStr += Str.FormatCStyle("\r\n<a href=\"https://github.com/grumpycoders/pcsx-redux\">https://github.com/grumpycoders/pcsx-redux</a>");
    AboutStr += Str.FormatCStyle("\r\n<a href=\"http://jpsxdec.blogspot.com/2011/06/decoding-mpeg-like-bitstreams.html\">http://jpsxdec.blogspot.com/2011/06/decoding-mpeg-like-bitstreams.html</a>");
    AboutStr += Str.FormatCStyle("\r\n\r\nDither Matrix:\r\n<a href=\"https://psx-spx.consoledev.net/graphicsprocessingunitgpu/#24bit-rgb-to-15bit-rgb-dithering-enabled-in-texpage-attribute\">https://psx-spx.consoledev.net/graphicsprocessingunitgpu/#24bit-rgb-to-15bit-rgb-dithering-enabled-in-texpage-attribute</a>");
    AboutStr += Str.FormatCStyle("\r\n\r\nPSX.dev:\r\n<a href=\"https://discord.com/invite/psx-dev-642647820683444236\">https://discord.com/invite/psx-dev-642647820683444236</a>");

    std::wstring WideStr = std::wstring(AboutStr.begin(), AboutStr.end());

    G->Window->MessageModal(L"About", L"", WideStr);
}

void Global_Application::Draw(void)
{
    if (b_FontChangeRequested)
    {
        b_FontChangeRequested = false;

        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->Clear();
        io.Fonts->AddFontFromFileTTF(Window->FontList()[Window->GetFontIndex()].string().c_str(), Window->FontSize());
        io.FontDefault = io.Fonts->Fonts.back();
        io.Fonts->Build();

        ImGui_ImplDX9_InvalidateDeviceObjects();
        ImGui_ImplDX9_CreateDeviceObjects();
    }

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();

    ImGui::NewFrame();
    {
        //bool show_demo_window = true;
        //ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 7.4f);

        MainMenu();

        Toolbar();

        Statusbar();

        Options();

        File();

        Palette();

		BitstreamSettings();

        ImageSettings();

        ImageWindow();

        ImGui::PopStyleVar();
    }
    ImGui::EndFrame();

    Render->Device()->SetRenderState(D3DRS_ZENABLE, FALSE);
    Render->Device()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    Render->Device()->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

    Render->Device()->Clear(0, NULL,
        D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
        D3DCOLOR_XRGB(GetRValue(Window->GetColor()), GetGValue(Window->GetColor()), GetBValue(Window->GetColor())),
        1.0f, 0);
    Render->Device()->BeginScene();

    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

    Render->Device()->EndScene();
    Render->Device()->PresentEx(NULL, NULL, NULL, NULL, NULL);
}

void Global_Application::Update(void)
{
    if (Window->Device()->GetKeyDown(VK_F11))
    {
        Window->AutoFullscreen();
    }

    if (Window->Device()->GetKeyDown(VK_DOWN))
    {
        SetTexture(++m_SelectedFile);
    }

    if (Window->Device()->GetKeyDown(VK_UP))
    {
        SetTexture(--m_SelectedFile);
    }

    if (Window->Device()->GetKeyDown(VK_LEFT))
    {
        SetPalette(--m_Palette);
    }

    if (Window->Device()->GetKeyDown(VK_RIGHT))
    {
        SetPalette(++m_Palette);
    }

    if (Window->Device()->GetKeyDown(VK_ESCAPE))
    {
        PostMessage(Window->Get(), WM_CLOSE, 0, 0);
    }

    ImGuiIO& io = ImGui::GetIO();
    if (io.KeyCtrl)
    {
        // CTRL + O
        if (Window->Device()->GetKeyDown(0x4F))
        {
            if (auto Filename = Window->GetOpenFilename({ L"Sony Texture Image", L"Sony Bitstream", L"All files" }, { L"*.tim", L"*.bs", L"*.*" }); Filename.has_value())
            {
                Open(Filename.value());
            }
        }

        // CTRL + R
        if (Window->Device()->GetKeyDown(0x52))
        {
            CreateModalFunc = [this]()
                {
                    b_RawCreateNew = false;
                    CreateModal();
                };
        }

        // CTRL + N
        if (Window->Device()->GetKeyDown(0x4E))
        {
            CreateModalFunc = [this]()
                {
                    b_RawCreateNew = true;
                    CreateModal();
                };
        }

        // CTRL + S
        if (Window->Device()->GetKeyDown(0x53))
        {
            if (Texture)
            {
                if (auto Filename = Window->GetSaveFilename({ L"Sony Texture Image" }, { L"*.tim" }); Filename.has_value())
                {
                    Texture->Save(Filename.value());
                }
            }
        }

        // CTRL + MOUSEWHEEL
        if (Window->Device()->GetMouseDeltaZ() != 0.0f)
        {
            m_ImageZoom += Window->Device()->GetMouseDeltaZ() * 0.25f;
            m_ImageZoom = std::clamp(m_ImageZoom, m_ImageZoomMin, m_ImageZoomMax);
        }
    }
}

void Global_Application::DragAndDrop(StrVecW Files)
{
    /*Standard_String Str;

    Standard_FileSystem FS;

    for (std::size_t i = 0; i < Files.size(); i++)
    {
        StringW FileExtension = FS.GetFileExtension(Files[i]).wstring();

        Str.ToUpper(FileExtension);

        if ((FileExtension == L".TIM") || (FileExtension == L".BS"))
        {
            Open(Files[i]);
        }
    }*/

    if (!Files.empty()) { Open(Files[NULL]); }

    Window->ClearDroppedFiles();
}

int Global_Application::Main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	{
		Standard_FileSystem FS;

		if (FS.Exists(GetConfigFilename()))
		{
			OpenConfig();
		}
		else
		{
			SaveConfig();
		}
	}
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGuiIO& io = ImGui::GetIO();
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.Fonts->Clear();
        io.Fonts->AddFontFromFileTTF(Window->GetFont().string().c_str(), Window->FontSize());
        io.FontDefault = io.Fonts->Fonts.back();
    }
    {
		int DefaultWidth = 1024 + static_cast<int>(ImGui::GetFrameHeightWithSpacing()) * 2;
		int DefaultHeight = 512 + static_cast<int>(ImGui::GetFrameHeightWithSpacing()) * 4 + 160;

		if (b_BootMaximized)
		{
			m_BootWidth = DefaultWidth;
			m_BootHeight = DefaultHeight;
		}

		if (b_BootFullscreen)
		{
			b_BootMaximized = false;
			m_BootWidth = DefaultWidth;
			m_BootHeight = DefaultHeight;
		}

        int Width = m_BootWidth;
        int Height = m_BootHeight;

		if ((!m_BootWidth) || (m_BootWidth < DefaultWidth) || (!m_BootHeight) || (m_BootHeight < DefaultHeight))
		{
			Width = DefaultWidth;
			Height = DefaultHeight;
		}

        Window->PresetStyle(WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
        Window->PresetStyleEx(WS_EX_ACCEPTFILES | WS_EX_APPWINDOW);
        Window->SetCaptionName(VER_PRODUCT_NAME_STR);
        Window->PresetClassName(VER_INTERNAL_NAME_STR);
        Window->SetIcon(hInstance, IDI_WINDOW);
        Window->SetIconSmall(hInstance, IDI_WINDOW);
        Window->SetAcceleratorTable(hInstance, IDR_WINDOW);
		Window->Create(Width, Height, hInstance, SW_HIDE, WindowProc);

        Render->Initialize(Window->CreateChild(0, 0, Width, Height, hInstance, SW_SHOW, RenderProc, NULL, NULL), Width, Height, false);

		ShowWindow(Window->Get(), SW_SHOWDEFAULT);

		if (b_BootMaximized)
		{
			ShowWindow(Window->Get(), SW_SHOWMAXIMIZED);
		}

		if (b_BootFullscreen)
		{
			Window->AutoFullscreen();
		}

        while (Render->DeviceState() != D3DDEVICE_STATE::NORMAL) { Window->SleepTimer(); }

		Window->SetTimer(30);
    }
	{
		Standard_String Str;
		std::filesystem::path Filename = Str.FormatCStyle(L"%s\\%s", Window->GetCurrentWorkingDir().c_str(), L"icons.png");

		D3DXIMAGE_INFO ImageInfo;
		if (D3DXCreateTextureFromFileExW(Render->Device(),
			Filename.wstring().c_str(),
			320, 64,
			D3DX_DEFAULT,
			D3DUSAGE_DYNAMIC,
			D3DFMT_A8R8G8B8,
			D3DPOOL_DEFAULT,
			D3DX_FILTER_NONE,
			D3DX_DEFAULT,
			D3DCOLOR_XRGB(255, 255, 255), &ImageInfo, NULL, &ToolbarIcons))
		{
			Window->GetErrorMessage();
		}
	}
    {
        ImGui_ImplWin32_Init(Render->Window()->Get());
        ImGui_ImplDX9_Init(Render->Device());
    }

	if ((b_OpenLastFileOnBoot) && (!m_Filename.empty()))
	{
		Open(m_Filename);
	}

    HACCEL hAccTable = Window->GetAcceleratorTable();

    MSG msg{};
    msg.message = NULL;
    msg.hwnd = Window->Get();

	bool b_Active = true;
    
    while (b_Active)
    {
        while (::TranslateAccelerator(msg.hwnd, hAccTable, &msg))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }

        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
            {
				b_Active = false;
				break;
            }
        }

        if (Window->DroppedFiles()) { DragAndDrop(Window->GetDroppedFiles()); }

        Update();

        Draw();

        Window->SleepTimer();
    }

	if (ToolbarIcons) { ToolbarIcons->Release(); }

	Close();

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();

	return (int)msg.wParam;
}