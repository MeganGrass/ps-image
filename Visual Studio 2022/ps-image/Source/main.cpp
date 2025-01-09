
#include "app.h"

#include "imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	HEAP_OPTIMIZE_RESOURCES_INFORMATION ResourceInfo = { HEAP_OPTIMIZE_RESOURCES_CURRENT_VERSION, 0 };
	HeapSetInformation(NULL, HeapOptimizeResources, &ResourceInfo, sizeof(HEAP_OPTIMIZE_RESOURCES_INFORMATION));

	return G->Main(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if ((message == WM_CLOSE) || (message == WM_DESTROY))
	{
		G->Shutdown();
	}

	if (message == WM_COMMAND)
	{
		if (LOWORD(wParam) == IDM_EXIT)
		{
			G->Shutdown();
		}
		if (LOWORD(wParam) == IDM_ABOUT)
		{
			G->About();
		}
	}

	if (G->Render && G->Render->NormalState())
	{
		if (message == WM_ACTIVATE)
		{
			if ((wParam & WA_ACTIVE) || (wParam & WA_CLICKACTIVE))
			{
				G->Render->Device()->Reset(G->Render->GetPresentParameters());
			}
		}

		if (message == WM_SETFOCUS)
		{
			G->Render->Device()->Reset(G->Render->GetPresentParameters());
		}

		if (message == WM_WINDOWPOSCHANGED)
		{
			hWnd = G->Render->Window()->Get();
			RECT Rect = G->Window->GetRect();
			WINDOWPOS WindowPos{};
			WindowPos.hwnd = hWnd;
			WindowPos.hwndInsertAfter = nullptr;
			WindowPos.x = Rect.left;
			WindowPos.y = Rect.top;
			WindowPos.cx = (Rect.right - Rect.left);
			WindowPos.cy = (Rect.bottom - Rect.top);
			WindowPos.flags = SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER | SWP_DEFERERASE | SWP_ASYNCWINDOWPOS;
			SendMessage(hWnd, WM_WINDOWPOSCHANGED, 0, (LPARAM)&WindowPos);
		}
	}

	if ((G->Render && G->Render->NormalState()) && (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam)))
	{
		return true;
	}

	if (message)
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

LRESULT CALLBACK RenderProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if ((G->Render && G->Render->NormalState()) && (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam)))
	{
		return true;
	}

	if (message)
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}