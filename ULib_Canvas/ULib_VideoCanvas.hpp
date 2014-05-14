#pragma once
#include "stdafx.h"
#include "ULib_BaseCanvas.hpp"
#include "VideoMemory.h"
#include <ddraw.h>
#include <time.h>

#ifdef DIRECT3D_VERSION
#ifdef DIRECTDRAW_VERSION

#define USING_GITHUB 1

class ULib_VideoCanvas
	: public ULib_BaseCanvas
{
public:
	ULib_VideoCanvas(){}
	virtual ~ULib_VideoCanvas(){if(!m_releaseFlag) Release();}

	HRESULT virtual Initialize(IN CONST CHAR *device, IN CONST CHAR *wndName, IN int canvasWidth, IN int canvasHeight, IN int *pTargetTexturePixels, IN EnumWorkingMode mode = EnumWorkingMode::DataOutput, IN UINT dataFormat = GL_BGRA)
	{
		m_pTargetTexturePixels = pTargetTexturePixels;
		m_canvasWidth = canvasWidth;
		m_canvasHeight = canvasHeight;

		m_typWrkMod = mode;
		m_dataFormat = dataFormat;

		m_pDevice = new CHAR [strlen(device) + 1];
		strcpy_s(m_pDevice, strlen(device) + 1, device);
#if USING_OO_WND
		//Create Win32Wnd
		HRESULT vdrVal = IGutEx_Create_3DWnd(wndName, 0 + canvasWidth, 0 + canvasHeight, canvasWidth, canvasHeight, true, &m_pVdWnd, m_idxWwnd);
		assert(S_OK == vdrVal);
		if(S_OK != vdrVal) return E_FAIL;
		m_hWnd = m_pVdWnd->m_hWnd;
#else
		GutCreateWindow(0 + canvasWidth, 0 + canvasHeight, canvasWidth, canvasHeight, wndName);
		m_hWnd = GutGetWindowHandleWin32();
#endif
		m_pVideoOperator = new DirectDrawWrapper::VideoMemory(m_hWnd);
		m_pVideoOperator->SetColorFormat(DirectDrawWrapper::PixelSize::True);
		m_pVideoOperator->SetBackgroundColor(0x00000000U);
		m_pVideoOperator->CreateDevice();

		m_releaseFlag = false;
		return S_OK;
	}

	HRESULT virtual Release()
	{
		HRESULT ret = S_OK;

		m_pVideoOperator->Release();
#if USING_OO_WND
		//release wnd
		if(m_pVdWnd->m_hWnd && IsWindow(m_pVdWnd->m_hWnd))
		{
			ret |= IGutEx_Destroy_3DWnd(m_idxWwnd);
		}
#else
		if (m_hWnd && IsWindow(m_hWnd))
		{
			GutCloseWindow();
		}
#endif
		m_hWnd = NULL;
		m_releaseFlag = true;
		return ret;
	}

	HRESULT virtual Update()
	{
#if USING_OO_WND
		if(m_pVdWnd->Active() && m_pVdWnd->OnIdle) m_pVdWnd->OnIdle();
#else
		if(GutProcessMessage()) return E_FAIL;
#endif
		bool bIsReadyFrameData = m_pVideoOperator->ReadyFrame(true);
		/*
		* 处理链接中所述的特殊情况，不直接使用memcpy();
		* http://hi.baidu.com/daoguchengshu/item/06aa4a09d3ca7810ebfe3815
		*/
		for (int j = 0; j < m_pVideoOperator->Height; ++j)
		{
			for (int i = 0; i < m_pVideoOperator->Width; ++i)
			{
				((UINT *)(m_pVideoOperator->VideoMemoryPtr))[j * m_pVideoOperator->Stride + i] = m_pTargetTexturePixels[j * m_canvasWidth + i];
			}
		}
		m_pVideoOperator->RenderFrame();
		return S_OK;
	}
protected:
	int m_idxWwnd;
	Gut3DWnd *m_pVdWnd;

	DirectDrawWrapper::VideoMemory *m_pVideoOperator;

private:
	int m_releaseFlag;
};
#endif
#endif