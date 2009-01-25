
#include <math.h>
#include "GLRenderer.h"
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
	ScreenWidth = width;
	ScreenHeight = height;
	fHeight = height;
	fWidth = width;
	fHalfHeight = height / 2;
	fHalfWidth = width / 2;

	fWidthFactor = 2 / (float)width;
}

void TGLStars::ShowActiveScreen(int TimeLeft)
{
	float distance = 1.0 - (TimeLeft / 100.0);
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

// This variable should be in the class private section, but
// it runs slightly faster if I put it here as a global
float fVerticalScreenLine;

void TGLStars::DrawPoint(float x, float y, float brightness)
{
	if (x < 0 || y < 0 || x > fWidth || y > fHeight)
		return;
	glColor4f(1.0f, 1.0f, 1.0f, brightness);
	x = floor(x);
	glVertex2f(x * fWidthFactor - 1.0f, fVerticalScreenLine);
	glVertex2f((x + 1.0f) * fWidthFactor - 1.0f, fVerticalScreenLine);
}

void TGLStars::DrawLine(float Fromx, float Tox, float y)
{
	if (y < 0.0f || y > fHeight || Fromx > fWidth || Tox < 0.0f)
		return;
	if (Fromx < 0.0f)
		Fromx = 0.0f;
	if (Tox > fWidth)
		Tox = fWidth;

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(Fromx * fWidthFactor - 1.0f, fVerticalScreenLine);
	glVertex2f(Tox * fWidthFactor - 1.0f, fVerticalScreenLine);
}

void TGLStars::DrawCircleExact(float x, float y, float radius)
{
	if (radius < 1)
	{
		float Brightness = radius * radius;
		float RightXMult = x - floor(x);
		float LeftXMult = 1.0f - RightXMult;
		float BottomYMult = y - floor(y);
		float TopYMult = 1.0f - BottomYMult;
		RightXMult = sqrt(RightXMult);
		LeftXMult = sqrt(LeftXMult);
		BottomYMult = sqrt(BottomYMult);
		TopYMult = sqrt(TopYMult);

		fVerticalScreenLine = floor(y) / fHalfHeight - 1.0f;
		DrawPoint(x, y, Brightness * (LeftXMult * TopYMult));
		DrawPoint(x + 1.0f, y, Brightness * (RightXMult * TopYMult));
		fVerticalScreenLine = floor(y + 1.0f) / fHalfHeight - 1.0f;
		DrawPoint(x, y + 1.0f, Brightness * (LeftXMult * BottomYMult));
		DrawPoint(x + 1.0f, y + 1.0f, Brightness * (RightXMult * BottomYMult));
	}
	else
	{
		float xfraction = x - floor(x);
		float yfraction = y - floor(y);

		float r = ceil(radius);

		float bottom = -r - 1.0f;
		float top = r + 1.0f;
		if (y + bottom < 0)
			bottom = -floor(y);
		if (y + top > fHeight)
			top = ceil(y);

		float minleft = -x;
		float maxright = fWidth - x;
		for (float j = bottom; j < top; j++)
		{
			fVerticalScreenLine = floor(y + j) / fHalfHeight - 1.0f;
			float ydist = j - yfraction;

			float left = -r - 1.0f;
			float right = r + 1.0f;
			if (left < minleft) // left is a negative number, if we're past the left edge
				left = -floor(x);
			if (right > maxright) // right is a positive number, if we're past the right edge
				right = ceil(x);

			for (float i = left; i < right; i++)
			{
				float xdist = i - xfraction;

				float dist = sqrt(xdist*xdist + ydist*ydist);
				if (dist > radius - 1.0f)
				{
					if (dist < radius)
					{
						float bright = (radius - dist);
						bright = sqrt(bright);
						if (bright > .001f)
						{
							DrawPoint(x + i, y + j, bright);
						}
					}
				}
				else
				{
					if (r > 5.0f && i <= -1.0f)
					{
						DrawLine(floor(x) + i, floor(x) - i, y + j);
						i = -i - 1.0f;
					}
					else
					{
						DrawPoint(x + i, y + j, 1.0f);
					}
				}
			} 
		}
	}
}

void TGLStars::DrawCircle(float x, float y, float radius)
{
//	TraceMethod trace(10, "TGLStars::DrawCircle()");
	if (radius < 1)
	{
		fVerticalScreenLine = floor(y) / fHalfHeight - 1.0f;
		DrawPoint(x, y, radius * radius);
	}
	else
	{
		float r = floor(radius);
		for (float j = -r - 1; j < r + 1; j++)
		{
			float vpos = (y - j) / fHalfHeight - 1.0f;
			for (float i = -r - 1; i < r + 1; i++)
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
					}
					else if (bright > .001f)
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
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
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



