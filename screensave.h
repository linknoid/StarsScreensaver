#ifndef _SCREENSAVE_H_
#define _SCREENSAVE_H_

#include <windows.h>

extern HWND CreateScreenSaveWnd(HWND hwndParent, RECT *rect);
extern void InitScreenSaveClass(BOOL fPreview, HINSTANCE Instance);
extern BOOL Configure(HWND hwndParent);

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, RECT *rcMonitor, LPARAM Param);

#endif // _SCREENSAVE_H_

