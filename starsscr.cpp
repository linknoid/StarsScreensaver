
#include <windows.h>
#include <scrnsave.h>
#include <time.h>
#include "starsreg.h"
#include "logging/logging.h"


TStarsReg *Stars = NULL;

bool handleKey(WPARAM wParam, LPARAM lParam);
BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL WINAPI RegisterDialogClasses(HANDLE hInst);
LRESULT WINAPI ScreenSaverProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#define USAGE_STRING "Settings can only be changed while this screen saver is active.\n\n\
Use the following keys while it is active:\n\n\
Left and right arrows change directions.\n\
Up and down arrows change speed.\n\
PageUp and PageDown changes number of stars.\n\
Keypad + and - change star size.\n\
Delete resets everything to default.\n\
A key toggles antialiasing.\n"


BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
		MessageBox(hDlg, USAGE_STRING, "Instructions", MB_OK);
		EndDialog(hDlg, TRUE);
		return TRUE;
	}

	return FALSE;
}

BOOL WINAPI RegisterDialogClasses(HANDLE hInst)
{
    return TRUE;
}


extern int ThresholdForLogging;

// main loop for screensaver
LRESULT WINAPI ScreenSaverProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch ( message ) 
	{
		case WM_CREATE:
			LoggingThreshold = ThresholdForLogging;
			StartLogFile("C:\\cygwin\\home\\brycew\\stars\\stars\\mylog.txt", false);

			srand(time(0));
			// Change this type to change the behavior
			Stars = new TStarsReg(hWnd);
			SetTimer(hWnd, 1, (unsigned)(DELAY), NULL);
			return 0;

		case WM_DESTROY:
			KillTimer(hWnd, 1);
			delete Stars;
			EndLogFile();
			return 0;

		case WM_TIMER:
			Stars->DrawStars();
			return 0;

		case WM_KEYDOWN:
		case WM_KEYUP:
			if (handleKey(wParam, lParam))
				return 0;
	}

	return DefScreenSaverProc(hWnd, message, wParam, lParam);
}



bool handleKey(WPARAM wParam, LPARAM lParam)
{
	if (lParam & (1 << 31))
	{ // key released
		SendLogMessage("Key released %i", wParam);
		switch (wParam)
		{
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
				Stars->OnKeyDel();
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
			case 109: // Keypad -
				KBState.minus = 1;
			case 46: // Delete key
				break;
			default:
				return false;
		} 
	}
	return true;
}

