#ifndef _STARSREG_H_
#define _STARSREG_H_

#include <windows.h>
#include <stdio.h>

class TStarsReg
{
protected:
	static int InstanceCount;
	static TStarsReg *StarsList[32]; // maximum of 32 displays
	int fCurInstance;

	int fStarCount; 
	bool fAntialias;
	float fRadius, fSpeed, fAngle;

	char fKeyName[64];
	void GetRegistryValue(HKEY key, const char *str, int *val);
	void SetRegistryValue(HKEY key, const char *str, int val);
	void GetRegistryValue(HKEY key, const char *str, float *val);
	void SetRegistryValue(HKEY key, const char *str, float val);
public:
	TStarsReg();
	virtual ~TStarsReg();
	static void DestroyAll();

	bool LoadSettings();
	void SaveSettings();
};

#endif // _STARSREG_H_

