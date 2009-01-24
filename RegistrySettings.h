#ifndef _REGISTRYSETTINGS_H_
#define _REGISTRYSETTINGS_H_

#include <windows.h>
#include "StarsSettings.h"

class TRegistrySettings : public TStarsSettings
{
protected:
	char fKeyName[64];
	void GetRegistryValue(int ScreenID, HKEY key, const char *str, int &val);
	void SetRegistryValue(int ScreenID, HKEY key, const char *str, int val);
	void GetRegistryValue(int ScreenID, HKEY key, const char *str, float &val);
	void SetRegistryValue(int ScreenID, HKEY key, const char *str, float val);

public:
	TRegistrySettings();
	~TRegistrySettings();
	virtual bool LoadSettings(int ScreenID);
	virtual void SaveSettings(int ScreenID);
};

#endif // _REGISTRYSETTINGS_H_
