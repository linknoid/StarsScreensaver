
#include "glstars.h"
#include "math.h"
#include "logging/logging.h"


TGLStars::TGLStars(HWND hWnd)
{
	TraceMethod trace(100, "TGLStars::TGLStars()");
	RECT rect;
	GetClientRect(hWnd, &rect);
	SendLogMessage("Screen size %i x %i", (int)rect.right - rect.left, (int)rect.bottom - rect.top);
	SetScreenSize(rect.right - rect.left, rect.bottom - rect.top);

	fWnd = hWnd;

	InitGL(hWnd);
	SetWindowLong(fWnd, 0, (LONG)this);
}

TGLStars::~TGLStars()
{
	TraceMethod trace(100, "TGLStars::~TGLStars()");
	KillGL();
	SetWindowLong(fWnd, 0, 0);
}

void TGLStars::SetScreenSize(int width, int height)
{
	TraceMethod trace(90, "TGLStars::SetScreenSize()");
	TStars::SetScreenSize(width, height);
	fWidthFactor = 2 / (float)width;
}

void TGLStars::ShowActiveScreen()
{
	float distance = 1.0 - (ActiveScreenShowTime / 100.0);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2d(-distance, -distance);
	glVertex2d(distance, -distance);
	glVertex2d(distance, -distance);
	glVertex2d(distance, distance);
	glVertex2d(distance, distance);
	glVertex2d(-distance, distance);
	glVertex2d(-distance, distance);
	glVertex2d(-distance, -distance);
};

void TGLStars::DrawCircle(float x, float y, float radius)
{
//	TraceMethod trace(10, "TGLStars::DrawCircle()");
	static float color;
	static float i, j;
	static float r;
	if (radius < 1)
	{
		float vpos = y / (float)fHalfHeight - 1.0f;
		glColor4f(1.0f, 1.0f, 1.0f, radius*radius);
		glVertex2f(x * fWidthFactor - 1.0f, vpos);
		glVertex2f((x + 1.0f) * fWidthFactor - 1.0f, vpos);
	} else
	{
		r = (int)radius;
		for (j = -r - 1; j < r + 1; j++)
		{
			float vpos = (y - j) / (float)fHalfHeight - 1.0f;
			for (i = -r - 1; i < r + 1; i++)
			{
				float dist = sqrt(i * i + j * j);
				if (dist < r + 1)
				{
					float bright = sqrt(radius - dist);
					if (bright > 1.0f)
					{
						glColor3f(1.0f, 1.0f, 1.0f);
						glVertex2f((x + i) * fWidthFactor - 1.0f, vpos);
						glVertex2f((x - i + 1) * fWidthFactor - 1.0f, vpos);
						i = -i;

					} else
						if (bright > .001f)
						{
							glColor4f(1.0f, 1.0f, 1.0f, bright);
							glVertex2f((x - i) * fWidthFactor - 1.0f, vpos);
							glVertex2f((x - i + 1) * fWidthFactor - 1.0f, vpos);
						}
				}
			} 
		}
	} 
}


bool TGLStars::BeforeDraw()
{
	TraceMethod trace(40, "TGLStars::BeforeDraw()");
	wglMakeCurrent(fDC, fRC);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBegin(GL_LINES);
//	glBegin(GL_POINTS); // testing for optimization purposes
	return true;
}

bool TGLStars::AfterDraw()
{
	TraceMethod trace(40, "TGLStars::AfterDraw()");
	glEnd();
	glFinish();
	SwapBuffers(fDC);
	return true;
}


void TGLStars::InitGL(HWND hWnd)
{
	TraceMethod trace(90, "TGLStars::InitGL()");
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;

	SendLogMessage(85, "Getting device context"); 
	fDC = GetDC(hWnd);

	SendLogMessage(85, "Choosing Pixel Format"); 
	int i = ChoosePixelFormat(fDC, &pfd);
	SendLogMessage(85, "Setting Pixel Format"); 
	SetPixelFormat(fDC, i, &pfd);

	SendLogMessage(85, "Creating GL context"); 
	fRC = wglCreateContext(fDC);
	SendLogMessage(85, "Making context current"); 
	wglMakeCurrent(fDC, fRC);
	
	SendLogMessage(85, "Creating new quadric"); 
	quadric = gluNewQuadric();
	SendLogMessage(85, "Enabling GL blending"); 
	glEnable(GL_BLEND);
	SendLogMessage(85, "Setting GL Blend function"); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	SendLogMessage(85, "GL initialization complete"); 
}

void TGLStars::KillGL()
{
	TraceMethod trace(90, "TGLStars::KillGL()");
	gluDeleteQuadric(quadric);
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(fRC);
	ReleaseDC(fWnd, fDC);
}



