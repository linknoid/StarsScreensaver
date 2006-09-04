
#include <windows.h>
//#include <windowsx.h>
//#include <scrnsave.h>
#include <time.h>
#include "consts.h"
#include "screensave.h"
#include "glstars.h"
#include "logging/logging.h"



bool handleKey(WPARAM wParam, LPARAM lParam);
BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL WINAPI RegisterDialogClasses(HANDLE hInst);
LRESULT WINAPI ScreenSaverProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void InitScreenSaveClass(BOOL fPreview);

#define USAGE_STRING "Settings can only be changed while this screen saver is active.\n\n\
Use the following keys while it is active:\n\n\
Left and right arrows change directions.\n\
Up and down arrows change speed.\n\
PageUp and PageDown changes number of stars.\n\
Keypad + and - change star size.\n\
Delete resets everything to default.\n\
'A' key toggles antialiasing.\n\
Tab switches keyboard control between multiple monitors"


BOOL Configure(HWND hDlg)
{
	MessageBox(hDlg, USAGE_STRING, "Instructions", MB_OK);
	EndDialog(hDlg, TRUE);
	return TRUE;
}

// main loop for screensaver
LRESULT WINAPI ScreenSaverProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

//	TraceMethod trace("ScreenSaverProc");
	switch ( message ) 
	{
		case WM_NCCREATE:
			{
				SendLogMessage(80, "WM_NCCREATE event, hWnd = %i", hWnd);
				SendLogMessage(75, "Stars handle for window = %i, window handle = %i", GetWindowLong(hWnd, 0), hWnd);
				SetTimer(hWnd, 1, (unsigned)(DELAY), NULL);
				return TRUE;
			}

		case WM_NCDESTROY:
			SendLogMessage(80, "WM_NCDESTROY event, hWnd = %i", hWnd);
			PostQuitMessage(0);
			return 0;

		case WM_TIMER:
			SendLogMessage(49, "WM_TIMER event, hWnd = %i", hWnd);
			SendLogMessage(49, "Stars handle for window = %i", GetWindowLong(hWnd, 0));
			((TGLStars *)GetWindowLong(hWnd, 0))->DrawStars();
			return 0;

		case WM_KEYDOWN:
		case WM_KEYUP:
			SendLogMessage(90, "WM_KEYDOWN OR KEYUP event, hWnd = %i", hWnd);
			if (handleKey(wParam, lParam))
				return 0;
			else
				PostMessage(hWnd, WM_CLOSE, 0, 0);

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MOUSEMOVE:
			SendLogMessage(90, "MOUSE event, hWnd = %i", hWnd);
        
			// If we've got a parent then we must be a preview
			if(!GetParent(hWnd))
			{
				static POINT CursorPos, OldCursorPos;
				static bool first = true;
				if (first)
					GetCursorPos(&OldCursorPos);
				first = false;
				GetCursorPos(&CursorPos);
				if(abs(CursorPos.x - OldCursorPos.x) >= 3 || abs(CursorPos.y - OldCursorPos.y) >= 3)
					PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
			return 0;

		case WM_CLOSE:
			SendLogMessage(100, "CLOSE event, hWnd = %i", hWnd);
			KillTimer(hWnd, 1);
			TGLStars::DestroyAll();
			DestroyWindow(hWnd);
			return 0;

	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}



bool handleKey(WPARAM wParam, LPARAM lParam)
{
	if (lParam & (1 << 31))
	{ // key released
		SendLogMessage("Key released %i", wParam);
		switch (wParam)
		{
			case 9: // Tab
				KBState.tab = 0;
				break;
			case 33: // Page Up
				KBState.pageup = 0;
				break;
			case 34: // Page Down
				KBState.pagedown = 0;
				break;
			case 37: // Left Arrow
				KBState.leftarrow = 0;
				break;
			case 38: // Up Arrow
				KBState.uparrow = 0;
				break;
			case 39: // Right Arrow
				KBState.rightarrow = 0;
				break;
			case 40: // Down Arrow
				KBState.downarrow = 0;
				break;
			case 46: // Delete key
				KBState.del = 0;
				break;
			case 65: // 'a' key
				KBState.a = 0;
				break;
			case 107: // Keypad +
				KBState.plus = 0;
				break;
			case 109: // Keypad -
				KBState.minus = 0;
				break;
			default:
				return false;
		} 
	} else
	{ // key pressed
		SendLogMessage("Key pressed %i", wParam);
		switch (wParam)
		{
			case 9: // Tab
				KBState.tab = 1;
				break;
			case 33: // Page Up
				KBState.pageup = 1;
				break;
			case 34: // Page Down
				KBState.pagedown = 1;
				break;
			case 37: // Left Arrow
				KBState.leftarrow = 1;
				break;
			case 38: // Up Arrow
				KBState.uparrow = 1;
				break;
			case 39: // Right Arrow
				KBState.rightarrow = 1;
				break;
			case 40: // Down Arrow
				KBState.downarrow = 1;
				break;
			case 65: // 'a' key
				KBState.a = 1;
				break;
			case 107: // Keypad +
				KBState.plus = 1;
				break;
			case 109: // Keypad -
				KBState.minus = 1;
				break;
			case 46: // Delete key
				KBState.del = 1;
				break;
			default:
				return false;
		} 
	}
	return true;
}


HWND CreateScreenSaveWnd(HWND hwndParent, RECT *rect)
{
	TraceMethod trace(99, "CreateScreenSaveWnd");
	TGLStars *Stars;
	HWND result;
	DWORD dwStyle = hwndParent ? WS_CHILD : WS_POPUP;

	DWORD dwStyleEx = WS_EX_TOPMOST;

	if(hwndParent)
		GetClientRect(hwndParent, rect);

	SendLogMessage("Creating window %i x %i", (int)(rect->right - rect->left), (int)(rect->bottom - rect->top));
	{
		TraceMethod("CreateWindowEx");
		result = CreateWindowEx(	dwStyleEx, 
							APPNAME, 
							NULL, 
							WS_VISIBLE | dwStyle, 
							rect->left, 
							rect->top,
							rect->right - rect->left,
							rect->bottom - rect->top,
							hwndParent, 
							0,
							GetModuleHandle(0),
							0
						);
		SendLogMessage("New Window handle = %i", result);
	}
	if (result)
	{
		SendLogMessage("Setting window position to %i, %i", rect->left, rect->top);
		Stars = new TGLStars(result);
		SetWindowPos(result, 0, rect->left, rect->top, 0, 0,
				SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_SHOWWINDOW);
	} else
		SendLogMessage("Bad window handle, creation failed");
	return result;
}


void InitScreenSaveClass(BOOL fPreview, HINSTANCE Instance)
{
	TraceMethod trace(99, "InitScreenSaveClass");
	WNDCLASSEX	wcx;

	wcx.cbSize			= sizeof(WNDCLASSEX);
	wcx.style			= CS_OWNDC;
	wcx.lpfnWndProc		= ScreenSaverProc;
	wcx.cbClsExtra		= 0;
	wcx.cbWndExtra		= sizeof(TGLStars *);
	wcx.hInstance		= Instance;
	wcx.hIcon			= 0;
	wcx.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcx.lpszMenuName	= 0;
	wcx.lpszClassName	= APPNAME;
	wcx.hIconSm			= 0;	

	if(fPreview)
		wcx.hCursor			= LoadCursor(0, IDC_ARROW);
	else
		wcx.hCursor			= LoadCursor(wcx.hInstance, MAKEINTRESOURCE(IDC_BLANKCURSOR));

	RegisterClassEx(&wcx);

	srand(time(0));
}

