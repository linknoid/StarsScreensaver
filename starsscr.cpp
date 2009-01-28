
#include <windows.h>
//#include <windowsx.h>
//#include <scrnsave.h>
#include <time.h>
#include "consts.h"
#include "screensave.h"
#include "stars.h"
#include "GLRenderer.h"
#include "RegistrySettings.h"
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
Home (increase) and End (decrease) change the frame rate\n\
s switches the smooth but slow subpixel rendering on and off\n\
Tab switches keyboard control between multiple monitors"


BOOL Configure(HWND hDlg)
{
	MessageBox(hDlg, USAGE_STRING, "Instructions", MB_OK);
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
			static int LastDelay = DELAY;
			if (LastDelay != DELAY)
			{
				SendLogMessage(90, "Delay changed = %i, resetting timer", DELAY);
				KillTimer(hWnd, 1);
				SetTimer(hWnd, 1, (unsigned)(DELAY), NULL);
				LastDelay = DELAY;
			}
			TStars::DrawStarsAll();
			return 0;

		case WM_KEYDOWN:
		case WM_KEYUP:
			SendLogMessage(90, "WM_KEYDOWN OR KEYUP event, hWnd = %i", hWnd);
			if (handleKey(wParam, lParam))
			{
				if (message == WM_KEYDOWN)
					if (!lParam & (1 << 30))
						TStars::HandleKeyEventAll();
				return 0;
			}
			else
				PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;

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
				if (abs(CursorPos.x - OldCursorPos.x) >= 10 || abs(CursorPos.y - OldCursorPos.y) >= 10)
					PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
			return 0;

		case WM_CLOSE:
			SendLogMessage(100, "CLOSE event, hWnd = %i", hWnd);
			KillTimer(hWnd, 1);
			TStars::DestroyAll();
			DestroyWindow(hWnd);
			return 0;

	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}



bool handleKey(WPARAM wParam, LPARAM lParam)
{
	int newState;
	if (lParam & (1 << 31))
	{
		SendLogMessage("Key released %i", wParam);
		newState = 0;
	} else
	{
		SendLogMessage("Key pressed %i", wParam);
		newState = 1;
	}
	switch (wParam)
	{
		case 9: // Tab
			KBState.tab = newState;
			break;
		case 33: // Page Up
			KBState.pageup = newState;
			break;
		case 34: // Page Down
			KBState.pagedown = newState;
			break;
		case 35: // Home
			KBState.home = newState;
			break;
		case 36: // End
			KBState.end = newState;
			break;
		case 37: // Left Arrow
			KBState.leftarrow = newState;
			break;
		case 38: // Up Arrow
			KBState.uparrow = newState;
			break;
		case 39: // Right Arrow
			KBState.rightarrow = newState;
			break;
		case 40: // Down Arrow
			KBState.downarrow = newState;
			break;
		case 46: // Delete key
			KBState.del = newState;
			break;
		case 'S': // S key
			KBState.s = newState;
		case 107: // Keypad +
			KBState.plus = newState;
			break;
		case 109: // Keypad -
			KBState.minus = newState;
			break;
		default:
			return false;
	} 
	return true;
}


HWND CreateScreenSaveWnd(HWND hwndParent, RECT *rect)
{
	TraceMethod trace(99, "CreateScreenSaveWnd");
	TStars *Stars;
	HWND result;
	DWORD dwStyle = hwndParent ? WS_CHILD : WS_POPUP;

	DWORD dwStyleEx = WS_EX_TOPMOST;

	if(hwndParent)
		GetClientRect(hwndParent, rect);

	SendLogMessage("Creating window %i x %i", (int)(rect->right - rect->left), (int)(rect->bottom - rect->top));
	{
		TraceMethod trace("CreateWindowEx");
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
		TStarsRenderer *renderer = new TGLStars(result);
		Stars = new TStars(renderer);
		SetWindowPos(result, 0, rect->left, rect->top, 0, 0,
				SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_SHOWWINDOW);
	} else
		SendLogMessage("Bad window handle, creation failed");
	return result;
}


void InitScreenSaveClass(BOOL fPreview, HINSTANCE Instance)
{
	TraceMethod trace(99, "InitScreenSaveClass");
	TStars::settings = new TRegistrySettings();

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

