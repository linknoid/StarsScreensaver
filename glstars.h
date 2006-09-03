#ifndef _GLSTARS_H_
#define _GLSTARS_H_

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "stars.h"


class TGLStars : public TStars
{
private:
	GLUquadricObj *quadric;

	HDC fDC;	// Private GDI Device Context
	HGLRC fRC;	// Permanent Rendering Context
	HWND fWnd;	// Holds Our Window Handle

protected:
	void InitGL(HWND hWnd, HDC &hDC, HGLRC &hRC);
	void KillGL(HWND hWnd, HDC &hDC, HGLRC &hRC);
public:

	TGLStars(HWND hWnd, HDC &hDC, HGLRC &hRC);
	virtual ~TGLStars();
	virtual void DrawCircle(int x, int y, float radius, int c);
	virtual bool BeforeDraw();
	virtual bool AfterDraw();

};

#endif // _GLSTARS_H_
