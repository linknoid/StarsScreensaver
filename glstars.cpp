
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

void TGLStars::DrawCircle(int x, int y, float radius, int c)
{
//	TraceMethod trace(10, "TGLStars::DrawCircle()");
	static float color;
	static int i, j, r;
	r = (int) radius;
	if (radius < 1)
	{
		glColor4f(1.0f, 1.0f, 1.0f, radius*radius);
		glVertex2d((float)(x * 2 - fWidth) / (float)fWidth , (float)(y * 2 - fHeight) / (float)fHeight);
		glVertex2d((float)(x * 2 - fWidth + 2) / (float)fWidth, (float)(y * 2 - fHeight) / (float)fHeight);
		return;
	} else
	{
		glColor3f(1.0f, 1.0f, 1.0f);


		if (fAntialias)
		{
			for (j = -r - 1; j < r + 1; j++)
				for (i = -r - 1; i < r + 1; i++)
				{
					float dist = sqrt((i + .5) * (i + .5) + (j + .5) * (j + .5));
					if (dist < r + 1)
					{
						float bright = sqrt(radius - dist);// * (radius - dist);
						if (bright > 1.0f)
						{
							glColor3f(1.0f, 1.0f, 1.0f);
							glVertex2d((float)((x + i) * 2 - fWidth) / (float)fWidth, (float)((y - j) * 2 - fHeight) / (float)fHeight);
							glVertex2d((float)((x - i) * 2 - fWidth + 2) / (float)fWidth , (float)((y - j) * 2 - fHeight) / (float)fHeight);
							i = -i;

						} else
							if (bright > .001f)
							{
								glColor4f(1.0f, 1.0f, 1.0f, bright);
								glVertex2d((float)((x - i) * 2 - fWidth) / (float)fWidth , (float)((y - j) * 2 - fHeight) / (float)fHeight);
								glVertex2d((float)((x - i) * 2 - fWidth + 2) / (float)fWidth, (float)((y - j) * 2 - fHeight) / (float)fHeight);
							}
					}
				}

		} else
		{ // Not Antialiased
			for (i = 0; i < 2 * r; i++)
			{
				// vertical clipping: (top and bottom)
				if ((y - r + i) >= 0 && (y - r + i) < fHeight)
				{
//				int len = (int)sqrt((float)(r * r - (r - i) * (r - i))) * 2;
					float flen = sqrt((float)(2 * r - i) * i) * 2;
					int len = (int) flen;
					int xofs = x - len / 2;

					// left border
					if (xofs < 0)
					{
						len += xofs;
						xofs = 0;
					}

					// right border
					if (xofs + len >= fWidth)
					{
						len -= (xofs + len) - fWidth;
					}
					int ofs = (y - r + i);

					// note that len may be 0 at this point, 
					// and no pixels get drawn!
					glVertex2d((float)((xofs) * 2 - fWidth) / (float)fWidth , (float)(ofs * 2 - fHeight) / (float)fHeight);
					glVertex2d((float)((len + xofs) * 2 - fWidth) / (float)fWidth , (float)(ofs * 2 - fHeight) / (float)fHeight);

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

	fDC = GetDC(hWnd);

	int i = ChoosePixelFormat(fDC, &pfd);
	SetPixelFormat(fDC, i, &pfd);

	fRC = wglCreateContext(fDC);
	wglMakeCurrent(fDC, fRC);
	
	quadric = gluNewQuadric();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void TGLStars::KillGL()
{
	TraceMethod trace(90, "TGLStars::KillGL()");
	gluDeleteQuadric(quadric);
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(fRC);
	ReleaseDC(fWnd, fDC);
}



