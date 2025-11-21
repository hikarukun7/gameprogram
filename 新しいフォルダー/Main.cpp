#include <Windows.h>
#include <memory>
#include "Game.h"

//関数のプロトタイプ宣言
//ウィンドウプロシージャ
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
//ゲームループ関数のプロトタイプ宣言
//bool loop(HWND hwnd);
//クライアント領域のクリア関数のプロトタイプ宣言
//void clear(HDC hdc, COLORREF color);


//グローバル変数
namespace
{
	//ウィンドウのクラス名とタイトル
	const wchar_t* WinClassName = L"DXProj";
	const wchar_t* WinGameName = L"DirectX Game";
	//ウィンドウの幅と高さ
	constexpr UINT WinWidth = 640;
	constexpr UINT WinHeight = 480;

	//ゲームデータ
	std::unique_ptr<Game> game;

}
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine, _In_ int nCmdshow)
{
	//ウィンドウの登録と生成
	//ウィンドウクラス構造体の設定
	WNDCLASSEXW wc = {};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIconW(nullptr, L"IDI_ICON");
	wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	//wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hbrBackground = (HBRUSH)(GetStockObject(BLACK_BRUSH));
	wc.lpszClassName = WinClassName;
	wc.hIconSm = LoadIconW(nullptr, L"IDI_ICON");

	//ウィンドウクラスの登録
	if (!RegisterClassExW(&wc)) return 1;

	//ウィンドウの生成と表示
	RECT rc = {};
	rc.right = (LONG)WinWidth;
	rc.bottom = (LONG)WinHeight;

	HWND hwnd;
	DWORD style = WS_SYSMENU | WS_DLGFRAME | WS_MINIMIZEBOX;
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW | style, FALSE);

	hwnd = CreateWindowExW(0, WinClassName, WinGameName, WS_OVERLAPPED | style,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
		nullptr, nullptr, hInstance, nullptr);
	if (!hwnd) return 1;

	ShowWindow(hwnd, SW_SHOWNORMAL);
	//メッセージループ
	{
		//バックバッファの初期化
		game = std::make_unique<Game>();
		game->initialize(hwnd, WinWidth, WinHeight);
	}

	MSG msg = {};
	while (true)
	{
		if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		if (msg.message == WM_QUIT) break;

		//ゲームループ
		if (!game->loop())
		{
			DestroyWindow(hwnd);
		}

	}

	//データ開放
	

	//ウィンドウの登録を解除
	UnregisterClassW(wc.lpszClassName, wc.hInstance);

	return 0;
}

//関数の定義

LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_KEYDOWN:
		switch (wparam)
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			return 0;
		}
		break;
	}
	return DefWindowProcW(hwnd, msg, wparam, lparam);
}

//関数の定義