
#include "starsreg.h"
#include "consts.h"
#include "logging/logging.h"

#define MY_HKEY "Software\\Linknoid\\Stars"

int TStarsReg::InstanceCount = 0;
TStarsReg *TStarsReg::StarsList[32]; // maximum of 31 displays (0 reserved for all)

TStarsReg::TStarsReg()
{
	TraceMethod trace(100, "TStarsReg::TStarsReg()");

	fRadius = 8000;
	fSpeed = 5 * DELAY;
	fAngle = 0;
	fStarCount = 10000;
	fAntialias = true;

	fCurInstance = ++InstanceCount;
	if (fCurInstance < 32)
		StarsList[fCurInstance] = this;
	SendLogMessage(90, "Instance #%i", fCurInstance);
	LoadSettings();
}

TStarsReg::~TStarsReg()
{
	TraceMethod trace(100, "TStarsReg::~TStarsReg()");
	SaveSettings();
	if (fCurInstance < 32)
		StarsList[fCurInstance] = NULL;
}

void TStarsReg::DestroyAll()
{
	TraceMethod trace(100, "static TStarsReg::DestroyAll()");
	for (int i = 1; i <= InstanceCount; i++)
		if (StarsList[i])
			delete StarsList[i];
}
	
bool TStarsReg::LoadSettings()
{
	TraceMethod trace(90, "TStarsReg::LoadSettings()");
	int StarCount = fStarCount;
	float Radius = fRadius;
	float Speed = fSpeed;
	float Angle = fAngle;
	int Antialias = fAntialias;
	HKEY key;
	if (RegOpenKeyEx( HKEY_CURRENT_USER, MY_HKEY, 0, KEY_QUERY_VALUE, &key)
			== ERROR_SUCCESS)
	{
		GetRegistryValue(key, "starcount", &StarCount);
		GetRegistryValue(key, "radius", &Radius);
		GetRegistryValue(key, "speed", &Speed);
		GetRegistryValue(key, "angle", &Angle);
		GetRegistryValue(key, "antialias", &Antialias);
		GetRegistryValue(key, "delay", &DELAY);
	}
	fStarCount = StarCount;
	fRadius = Radius;
	fSpeed = Speed;
	fAngle = Angle / 720.0;
	if (Antialias)
		fAntialias = true;
	else
		fAntialias = false;

}
void TStarsReg::SaveSettings()
{
	TraceMethod trace(90, "TStarsReg::SaveSettings()");
	HKEY key;
	DWORD lpdw;

	if (RegCreateKeyEx(HKEY_CURRENT_USER, MY_HKEY, 0, "", 
				REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, &lpdw)
			== ERROR_SUCCESS)
	{
		SetRegistryValue(key, "starcount", fStarCount);
		SetRegistryValue(key, "radius", fRadius);
		SetRegistryValue(key, "speed", fSpeed);
		SetRegistryValue(key, "angle", fAngle * 720);
		SetRegistryValue(key, "antialias", fAntialias);
		SetRegistryValue(key, "delay", DELAY);
	}
}

void TStarsReg::GetRegistryValue(HKEY key, const char *str, int *val)
{
	TraceMethod trace(90, "TStarsReg::GetRegistryValue(int)");
	sprintf(fKeyName, "%s%i", str, fCurInstance);
    DWORD dsize = sizeof(int);
    DWORD dwtype = 0;
    int tmp;

    if (RegQueryValueEx(key, fKeyName, 0, &dwtype, (BYTE*)&tmp, &dsize)==0)
	*val = tmp;
}

void TStarsReg::SetRegistryValue(HKEY key, const char *str, int val)
{
	TraceMethod trace(90, "TStarsReg::SetRegistryValue(int)");
	sprintf(fKeyName, "%s%i", str, fCurInstance);
    RegSetValueEx(key, fKeyName, 0, REG_DWORD, (BYTE*)&val, sizeof(val));
}

void TStarsReg::GetRegistryValue(HKEY key, const char *str, float *val)
{
	TraceMethod trace(90, "TStarsReg::GetRegistryValue(float)");
	sprintf(fKeyName, "%s%i", str, fCurInstance);
    DWORD dsize = sizeof(int);
    DWORD dwtype = 0;
    int tmp;

    if (RegQueryValueEx(key, fKeyName, 0, &dwtype, (BYTE*)&tmp, &dsize)==0)
	*val = (double)tmp;
}

void TStarsReg::SetRegistryValue(HKEY key, const char *str, float val)
{
	TraceMethod trace(90, "TStarsReg::SetRegistryValue(float)");
	sprintf(fKeyName, "%s%i", str, fCurInstance);
    int tmp = (int)val;
    RegSetValueEx(key, fKeyName, 0, REG_DWORD, (BYTE*)&tmp, sizeof(tmp));
}

void TStarsReg::SetSpeed(float NewSpeed)
{
	fSpeed = NewSpeed;
}

void TStarsReg::SetDelay(int NewDelay)
{
	SetSpeed(fSpeed * NewDelay / DELAY);
}

void TStarsReg::SetAllDelay(int NewDelay)
{
	if (NewDelay < 5)
		NewDelay = 5;
	if (NewDelay > 50)
		NewDelay = 50;
	for (int i = 1; i <= InstanceCount; i++)
		if (StarsList[i])
			StarsList[i]->SetDelay(NewDelay);
	DELAY = NewDelay;
}

