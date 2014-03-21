#ifdef ULIB_OPENGLCANVAS_EXPORTS
#define ULIB_OPENGLCANVAS_API __declspec(dllexport)
#else
#define ULIB_OPENGLCANVAS_API __declspec(dllimport)
#endif

#include "stdafx.h"
#include "glew.h"
#include "Gut.h"
#include "GutWin32.h"
#include "GutInput.h"
#include "IExOpenGLCavas.h"
#include "DrawingLogic_EddyParticle.hpp"

#define MAX_SF_SIZE 1 << 8
#define  HIDE_CANVAS_WND !(_DEBUG)
#define  DEFAULT_FILLCOLOR 0xff808080

class ULIB_OPENGLCANVAS_API ULib_OpenGLCanvas : public IExOpenGLCanvas{
public: 
	static ULib_OpenGLCanvas *CreateInstance(int index);
	static ULib_OpenGLCanvas *Instance(int index);
	static HRESULT DestoryInstance(int index);

public:
	ULib_OpenGLCanvas(void)
	{

	}

	HRESULT virtual Init(IN int canvasWidth, IN int canvasHeight, IN int *pTargetTexturePixels)
	{
		m_canvasWidth = canvasWidth;
		m_canvasHeight = canvasHeight;

		m_pTargetTexturePixels = pTargetTexturePixels;
		if(!m_pTargetTexturePixels) 
		{
			//memset(m_pTargetTexturePixels, DEFAULT_FILLCOLOR, m_canvasWidth * m_canvasHeight * sizeof(m_pTargetTexturePixels[0]));
			return E_FAIL;
		}

		//Init OpenGL Content
		char *device = "opengl";
		GutCreateWindow(0, 0, m_canvasWidth, m_canvasHeight, "Particle");
		if (!GutInitGraphicsDevice(device))
		{
			exit(0);
		}

		m_hWnd = GutGetWindowHandleWin32();
#if HIDE_CANVAS_WND
		ShowWindow(m_hWnd, HIDE_WINDOW);
#else
		GutInputInit();
#endif

		//Init DrawLogic
		return (m_fnDrawingLogic = static_cast<IExDrawLogic *>(new EddyParticle()))->Init(canvasWidth, canvasHeight);
	}

	HRESULT virtual UnInit()
	{
		HRESULT ret = S_OK;

		if (m_fnDrawingLogic)
		{
			ret |= m_fnDrawingLogic->UnInit();
		}

		ret |= GutReleaseGraphicsDevice();

		if (m_hWnd && IsWindow(m_hWnd))
		{
			GutCloseWindow();
		}
		m_hWnd = NULL;

		m_pTargetTexturePixels = NULL;

		return ret;
	}

	HRESULT virtual Update()
	{
		if (GutProcessMessage())
		{
			HRESULT ret = S_OK;

			//TODO
			//Process Input Message

			if (m_fnDrawingLogic)
			{
				ret |= m_fnDrawingLogic->RenderFrame();

				//Refresh GLCanvas Buff
				GutSwapBuffersOpenGL();
			}

			//Copy gl buff data into targetTexture buff
			glPixelStorei(GL_PACK_ALIGNMENT, 1);
			glPixelStorei(GL_PACK_ROW_LENGTH, 0);
			glPixelStorei(GL_PACK_SKIP_ROWS, 0);
			glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

			GLenum iLastBuffer;
			glGetIntegerv(GL_READ_BUFFER, (GLint*)&iLastBuffer);
			glReadBuffer(GL_FRONT);
			glReadPixels(0, 0, m_canvasWidth, m_canvasHeight, GL_RGBA, GL_UNSIGNED_BYTE, m_pTargetTexturePixels);
			glReadBuffer(iLastBuffer);
			return ret;
		}else
		{
			//GL message loop has exited.
			return E_FAIL;
		}
	}

	int m_canvasWidth;
	int m_canvasHeight;
	int *m_pTargetTexturePixels;
	HWND m_hWnd;

	IExDrawLogic *m_fnDrawingLogic;

};

extern ULib_OpenGLCanvas **ppULib_OpenGLCanvas;