//
//	screensave.cpp
//
//	Entrypoint for a win32 screensaver 
//
//	NOTES: Screen savers don't like being UPX'd (compressed). Don't
//         know why, it just is. C-runtime has been stripped with
//         "libctiny.lib". 
//	
//	v1.0 1/12/2003 J Brown
//
//	Copyright (C) 2006 J Brown
//	www.catch22.net
//
//	Freeware
//
//

#include <ctype.h>
#include "consts.h"
#include "screensave.h"
#include "logging/logging.h"


//#pragma comment(lib, "libctiny.lib")
#pragma comment(linker, "/OPT:NOWIN98")

//
//	Added: Multimonitor support!!
//
typedef HMONITOR (WINAPI * TfnEnumDisplayMonitors)(HDC, LPCRECT, MONITORENUMPROC, LPARAM);
TfnEnumDisplayMonitors pfnEnumDisplayMonitors;

//
//	Callback function for EnumDisplayMonitors API. Use this function
//  to kickstart a screen-saver window for each monitor in the system
//
BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, RECT *rcMonitor, LPARAM Param)
{
	TraceMethod trace("MonitorEnumProc");
	HWND hwnd;

	// Create a screensaver on this monitor!
	hwnd = CreateScreenSaveWnd((HWND)Param, rcMonitor);

	// For some reason windows always places this window at 0,0...
	// position it ourselves
	SetWindowPos(hwnd, 0, rcMonitor->left, rcMonitor->top, 0, 0, 
		SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_SHOWWINDOW);

	return TRUE;
}

//
//	Start screen saver!
//
BOOL ScreenSaver(HWND hwndParent, HINSTANCE hInst)
{
	HMODULE		hUser32;
	UINT		nPreviousState;
	MSG			msg;

//	TraceMethod trace("ScreenSaver");
	InitScreenSaveClass(hwndParent != NULL, hInst);
	
	SystemParametersInfo(SPI_SETSCREENSAVERRUNNING, TRUE, &nPreviousState, 0);
	
	// Dynamically locate API call from USER32 - not present in all versions
	hUser32					= GetModuleHandle("USER32.DLL");
	pfnEnumDisplayMonitors	= (TfnEnumDisplayMonitors)GetProcAddress(hUser32, "EnumDisplayMonitors");

	// If we're running Win2k+ then the API is available...so call it!
	if(pfnEnumDisplayMonitors && hwndParent == 0)
	{
		pfnEnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0);
	}
	// Otherwise simulate by calling enum-proc manually
	else 
	{
		RECT rect;

		rect.left   = 0;
		rect.right  = GetSystemMetrics(SM_CXSCREEN);
		rect.top    = 0;
		rect.bottom = GetSystemMetrics(SM_CYSCREEN);

		MonitorEnumProc(NULL, NULL, &rect, (LPARAM)hwndParent);
	}

	// run message loop to handle all screen-saver windows
	while(GetMessage(&msg, NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	SystemParametersInfo(SPI_SETSCREENSAVERRUNNING, FALSE, &nPreviousState, 0);

	return TRUE;
}

//
//	Look for any options Windows has passed to us:
//
//	-a <hwnd>		(set password)
//  -s				(screensave)
//  -p <hwnd>		(preview)
//  -c <hwnd>		(configure)
//
BOOL ParseCommandLine(PSTR szCmdLine, int *chOption, HWND *hwndParent)
{
	TraceMethod trace(80, "ParseCommandLine");
	int ch = *szCmdLine++;

	if(ch == '-' || ch == '/')
		ch = *szCmdLine++;

	if(ch >= 'A' && ch <= 'Z')
		ch += 'a' - 'A';		//convert to lower case

	*chOption = ch;
	ch = *szCmdLine++;
	
	if(ch == ':') 
		ch = *szCmdLine++;

	while(ch == ' ' || ch == '\t')
		ch = *szCmdLine++;

	if(isdigit(ch))
	{
		unsigned int i = atoi(szCmdLine - 1);
		*hwndParent = (HWND)i;
	}
	else
		*hwndParent = NULL;

	return TRUE;
}

//
//	Entrypoint for screen-saver: it's just a normal win32 app!
//
int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int iCmdShow)
{
	LoggingThreshold = THRESHOLDFORLOGGING;
	StartLogFile(LOGFILENAME, false);
	TraceMethod trace("WinMain");

	HWND	hwndParent;
	int		chOption;

	// Make sure that only 1 instance runs at a time - 
	// Win98 seems to want us to restart every 5 seconds!!
	if(FindWindowEx(NULL, NULL, APPNAME, APPNAME))
	{
		return 0;
	}

/*
	LoadSettings();
*/

	ParseCommandLine(lpCmdLine, &chOption, &hwndParent);
	
	switch(chOption)
	{
	case 's':
		SendLogMessage("Starting screen saver");
		return ScreenSaver(NULL, hInst);	// screen save
	case 'p':	return ScreenSaver(hwndParent, hInst);	// preview in small window
//	case 'a':	return ChangePassword(hwndParent);	// ask for password
	case 'c':	return Configure(hwndParent);		// configuration dialog
	default:	return Configure(hwndParent);		// configuration dialog
	}
	SendLogMessage("Ending screen saver");
	EndLogFile();

	return 0;	
}

