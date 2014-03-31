#include "stdafx.h"
#include "glew.h"
#include "Gut.h"
#include "GutWin32.hpp"
#include "GutInput.h"
#include "IExCavas.h"
#include "DrawingLogic_EddyParticle.hpp"

#define MAX_SF_SIZE (1 << 8)
#define  HIDE_CANVAS_WND (0)
#define  DEFAULT_FILLCOLOR (0xff808080)

class ULib_BaseCanvas : public IExCanvas{
public: 
	static ULib_BaseCanvas *CreateInstance(CONST CHAR *device, int index);
	static ULib_BaseCanvas *Instance(int index);
	static HRESULT DestoryInstance(int index);

public:
	ULib_BaseCanvas(void)
	{

	}
#pragma region Override IExCanvas
	HRESULT virtual Init(IN CONST CHAR *device, IN CONST CHAR *wndName, IN int canvasWidth, IN int canvasHeight, IN int *pTargetTexturePixels, IN EnumWorkingMode mode = EnumWorkingMode::DataOutput, IN UINT dataFormat = GL_BGRA)
	{
		m_pDevice = new CHAR [strlen(device) + 1];
		strcpy_s(m_pDevice, strlen(device) + 1, device);

		m_typWrkMod = mode;
		m_dataFormat = dataFormat;

		m_canvasWidth = canvasWidth;
		m_canvasHeight = canvasHeight;

		m_pTargetTexturePixels = pTargetTexturePixels;
		if(!m_pTargetTexturePixels) 
		{
			return E_FAIL;
		}

		GutCreateWindow(0, 0, m_canvasWidth, m_canvasHeight, wndName);
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
		return (m_fnDrawingLogic = static_cast<IExDrawLogic *>(new EddyParticle()))->Init(device, canvasWidth, canvasHeight);
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
		/*
		*	Nothing to do.
		*/
		return S_OK;
	}
#pragma endregion Override IExCanvas

protected:
	int m_canvasWidth;
	int m_canvasHeight;
	int *m_pTargetTexturePixels;
	HWND m_hWnd;
	CHAR *m_pDevice;

	EnumWorkingMode m_typWrkMod;
	UINT m_dataFormat;
	IExDrawLogic *m_fnDrawingLogic;

};

extern ULib_BaseCanvas **ppULib_BaseCanvas;