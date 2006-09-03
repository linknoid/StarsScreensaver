
#include "starsreg.h"
#include "logging/logging.h"

#define MY_HKEY "Software\\Linknoid\\Stars"



	
bool TStarsReg::LoadSettings()
{
	MethodTrace trace(90, "TStarsReg::LoadSettings()");
	int StarCount = fStarCount;
	float Radius = fRadius;
	float Speed = fSpeed;
	float Angle = fAngle;
	int Antialias = fAntialias;
	HKEY key;
	if (RegOpenKeyEx( HKEY_CURRENT_USER, MY_HKEY, 0, KEY_QUERY_VALUE, &key)
			== ERROR_SUCCESS)
	{
		reg_get_val(key, "starcount", &StarCount);
		reg_get_val(key, "radius", &Radius);
		reg_get_val(key, "speed", &Speed);
		reg_get_val(key, "angle", &Angle);
		reg_get_val(key, "antialias", &Antialias);
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
	MethodTrace trace(90, "TStarsReg::SaveSettings()");
	HKEY key;
	DWORD lpdw;

	if (RegCreateKeyEx(HKEY_CURRENT_USER, MY_HKEY, 0, "", 
				REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, &lpdw)
			== ERROR_SUCCESS)
	{
		reg_set_val(key, "starcount", fStarCount);
		reg_set_val(key, "radius", fRadius);
		reg_set_val(key, "speed", fSpeed);
		reg_set_val(key, "angle", fAngle * 720);
		reg_set_val(key, "antialias", fAntialias);
	}
}


void TStarsReg::reg_get_val(HKEY key, char *str, int *val)
{
	MethodTrace trace(90, "TStarsReg::reg_get_val(int)");
    DWORD dsize = sizeof(int);
    DWORD dwtype = 0;
    int tmp;

    if (RegQueryValueEx(key, str, 0, &dwtype, (BYTE*)&tmp, &dsize)==0)
	*val = tmp;
}

void TStarsReg::reg_set_val(HKEY key, char *str, int val)
{
	MethodTrace trace(90, "TStarsReg::reg_set_val(int)");
    RegSetValueEx(key, str, 0, REG_DWORD, (BYTE*)&val, sizeof(val));
}

void TStarsReg::reg_get_val(HKEY key, char *str, float *val)
{
	MethodTrace trace(90, "TStarsReg::reg_set_val(float)");
    DWORD dsize = sizeof(int);
    DWORD dwtype = 0;
    int tmp;

    if (RegQueryValueEx(key, str, 0, &dwtype, (BYTE*)&tmp, &dsize)==0)
	*val = (double)tmp;
}

void TStarsReg::reg_set_val(HKEY key, char *str, float val)
{
	MethodTrace trace(90, "TStarsReg::reg_set_val(float)");
    int tmp = (int)val;
    RegSetValueEx(key, str, 0, REG_DWORD, (BYTE*)&tmp, sizeof(tmp));
}

