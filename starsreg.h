#ifndef _STARSREG_H_
#define _STARSREG_H_

#include <windows.h>
#include "glstars.h"

class TStarsReg : public TGLStars
{
protected:
	void reg_get_val(HKEY key, char *str, int *val);
	void reg_set_val(HKEY key, char *str, int val);
	void reg_get_val(HKEY key, char *str, float *val);
	void reg_set_val(HKEY key, char *str, float val);
public:
	TStarsReg(HWND hWnd, HDC &hDC, HGLRC &hRC)
		: TGLStars(hWnd, hDC, hRC)
   	{
	}

	~TStarsReg()
	{
	}
	
	virtual bool LoadSettings();
	virtual void SaveSettings();
};

#endif // _STARSREG_H_

