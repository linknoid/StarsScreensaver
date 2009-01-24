
#include <stdio.h>
#include "RegistrySettings.h"
#include "logging/logging.h"
#include "consts.h"

#define MY_HKEY "Software\\Linknoid\\Stars"

TRegistrySettings::TRegistrySettings()
{
	TraceMethod trace(100, "TRegistrySettings::TRegistrySettings()");
	ResetDefaults();
}

TRegistrySettings::~TRegistrySettings()
{
}

bool TRegistrySettings::LoadSettings(int ScreenID)
{
	TraceMethod trace(90, "TStarsReg::LoadSettings()");
	Angle *= 7200.0;
	HKEY key;
	if (RegOpenKeyEx( HKEY_CURRENT_USER, MY_HKEY, 0, KEY_QUERY_VALUE, &key)
			== ERROR_SUCCESS)
	{
		GetRegistryValue(ScreenID, key, "starcount", StarCount);
		GetRegistryValue(ScreenID, key, "radius", Radius);
		GetRegistryValue(ScreenID, key, "speed", Speed);
		GetRegistryValue(ScreenID, key, "angle", Angle);
		GetRegistryValue(ScreenID, key, "delay", DELAY);
	}
	Angle /= 7200.0;
}

void TRegistrySettings::SaveSettings(int ScreenID)
{
	TraceMethod trace(90, "TStarsReg::SaveSettings()");
	HKEY key;
	DWORD lpdw;

	if (RegCreateKeyEx(HKEY_CURRENT_USER, MY_HKEY, 0, "", 
				REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, &lpdw)
			== ERROR_SUCCESS)
	{
		SetRegistryValue(ScreenID, key, "starcount", StarCount);
		SetRegistryValue(ScreenID, key, "radius", Radius);
		SetRegistryValue(ScreenID, key, "speed", Speed);
		SetRegistryValue(ScreenID, key, "angle", Angle * 7200);
		SetRegistryValue(ScreenID, key, "delay", DELAY);
	}
}

void TRegistrySettings::GetRegistryValue(int ScreenID, HKEY key, const char *str, int &val)
{
	TraceMethod trace(90, "TRegistrySettings::GetRegistryValue(int)");
	sprintf(fKeyName, "%s%i", str, ScreenID);
    DWORD dsize = sizeof(int);
    DWORD dwtype = 0;
    int tmp;

    if (RegQueryValueEx(key, fKeyName, 0, &dwtype, (BYTE*)&tmp, &dsize)==0)
		val = tmp;
}

void TRegistrySettings::SetRegistryValue(int ScreenID, HKEY key, const char *str, int val)
{
	TraceMethod trace(90, "TRegistrySettings::SetRegistryValue(int)");
	sprintf(fKeyName, "%s%i", str, ScreenID);
    RegSetValueEx(key, fKeyName, 0, REG_DWORD, (BYTE*)&val, sizeof(val));
}

void TRegistrySettings::GetRegistryValue(int ScreenID, HKEY key, const char *str, float &val)
{
	TraceMethod trace(90, "TRegistrySettings::GetRegistryValue(float)");
	sprintf(fKeyName, "%s%i", str, ScreenID);
    DWORD dsize = sizeof(int);
    DWORD dwtype = 0;
    int tmp;

    if (RegQueryValueEx(key, fKeyName, 0, &dwtype, (BYTE*)&tmp, &dsize)==0)
		val = (double)tmp;
}

void TRegistrySettings::SetRegistryValue(int ScreenID, HKEY key, const char *str, float val)
{
	TraceMethod trace(90, "TRegistrySettings::SetRegistryValue(float)");
	sprintf(fKeyName, "%s%i", str, ScreenID);
    int tmp = (int)val;
    RegSetValueEx(key, fKeyName, 0, REG_DWORD, (BYTE*)&tmp, sizeof(tmp));
}

