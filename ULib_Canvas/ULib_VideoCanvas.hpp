#pragma once
#include "stdafx.h"
#include "ULib_BaseCanvas.hpp"
#include "VideoMemory.h"
#include <ddraw.h>
#include <time.h>

#ifdef DIRECT3D_VERSION
#ifdef DIRECTDRAW_VERSION

#define USING_OWN_DD 0
#define USING_GITHUB 1

#if USING_OWN_DD
#define ULib_VideoCanvas_DDID 0x01
#define ULib_VideoCanvas_FPS 30
#endif

class ULib_VideoCanvas
	: public ULib_BaseCanvas
{
public:
	ULib_VideoCanvas(){}
	virtual ~ULib_VideoCanvas(){if(!m_releaseFlag) UnInit();}

	HRESULT virtual Init(IN CONST CHAR *device, IN CONST CHAR *wndName, IN int canvasWidth, IN int canvasHeight, IN int *pTargetTexturePixels, IN EnumWorkingMode mode = EnumWorkingMode::DataOutput, IN UINT dataFormat = GL_BGRA)
	{
		m_pTargetTexturePixels = pTargetTexturePixels;
		m_canvasWidth = canvasWidth;
		m_canvasHeight = canvasHeight;

		m_typWrkMod = mode;
		m_dataFormat = dataFormat;

		m_pDevice = new CHAR [strlen(device) + 1];
		strcpy_s(m_pDevice, strlen(device) + 1, device);

		//Create Win32Wnd
		//HRESULT vdrVal = IGutEx_Create_3DWnd(wndName, 0 + canvasWidth, 0 + canvasHeight, canvasWidth, canvasHeight, true, &m_pVdWnd, m_idxWwnd);
		//assert(S_OK == vdrVal);
		//if(S_OK != vdrVal) return E_FAIL;
		//m_hWnd = m_pVdWnd->m_hWnd;

		GutCreateWindow(0 + canvasWidth, 0 + canvasHeight, canvasWidth, canvasHeight, wndName);
		m_hWnd = GutGetWindowHandleWin32();

		m_pVideoOperator = new DirectDrawWrapper::VideoMemory(m_hWnd);
		m_pVideoOperator->SetColorFormat(DirectDrawWrapper::PixelSize::True);
		m_pVideoOperator->SetBackgroundColor(0x00000000U);
		m_pVideoOperator->CreateDevice();

#if USING_OWN_DD
		HRESULT ddrVal = DirectDrawCreate(NULL, &m_lpVideoDD, NULL);
		assert(DD_OK == ddrVal);
		if (DD_OK != ddrVal) return E_FAIL;
		
		//DDSCL_NORMAL模式只能使用块方式写表面
		ddrVal = m_lpVideoDD->SetCooperativeLevel(m_hWnd, DDSCL_NORMAL);
		assert(DD_OK == ddrVal);
		if(DD_OK != ddrVal) return E_FAIL;

		//Need to check display mode by IDirectDraw::EnumDisplayModes()
		ddrVal = m_lpVideoDD->SetDisplayMode(canvasWidth, canvasHeight, 32);
		assert(DD_OK == ddrVal);
		if(DD_OK != ddrVal) return E_FAIL;

		DDSURFACEDESC ddsFrontDesc;
		ddsFrontDesc.dwSize = sizeof(ddsFrontDesc);
		ddsFrontDesc.dwFlags = DDSD_CAPS;
		ddsFrontDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		ddrVal = m_lpVideoDD->CreateSurface(&ddsFrontDesc, &m_lpDDSPrimary, NULL);
		assert(DD_OK == ddrVal);
		if(DD_OK != ddrVal) return E_FAIL;

		/*
		事实上，只要有足够的显示内存，可以创建任意多个后台缓冲区，一般
		每1M 的显示内存只能用来创建一个后台缓冲区。表面的内存既可以是显示内存，也可以是系统内存。
		DirectDraw 在使用完了显示内存时（例如在仅有1M 的显示内存创建了2 个后台缓冲区）会自动使用系统
		内存。你可以通过将DDSCAPS 结构中的dwCaps 设定为DDSCAPS_SYSTEMMEMORY 或
		DDSCAPS_VIDEOMEMORY 来指定只使用系统内存或只使用显示内存。如果指定了
		DDSCAPS_VIDEOMEMORY 又没有足够的显示内存来创建表面，IDirectDraw::CreateSurface 将返回一
		个DDERR_OUTOFVIDEOMEMORY 错误。
		*/
		DDSURFACEDESC ddsBackDesc;
		ddsBackDesc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		ddsBackDesc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		ddsBackDesc.dwWidth = canvasWidth;
		ddsBackDesc.dwHeight = canvasHeight;
		ddrVal = m_lpVideoDD->CreateSurface(&ddsBackDesc, &m_lpDDSBack, NULL);
		assert(DD_OK == ddrVal);
		if(DD_OK != ddrVal) return E_FAIL;
#endif
		m_releaseFlag = false;
		return S_OK;
	}

	HRESULT virtual UnInit()
	{
		HRESULT ret = S_OK;

#if USING_OWN_DD
		//release DirectDraw
		if (m_lpVideoDD)
		{
			if (m_lpDDSPrimary)
			{
				m_lpDDSPrimary->Release();
				m_lpDDSPrimary = NULL;
			}
			m_lpVideoDD->Release();
			m_lpVideoDD = NULL;
		}
#endif

		//release wnd
		//if(m_pVdWnd->m_hWnd && IsWindow(m_pVdWnd->m_hWnd))
		//{
		//	ret |= IGutEx_Destroy_3DWnd(m_idxWwnd);
		//}

		if (m_hWnd && IsWindow(m_hWnd))
		{
			GutCloseWindow();
		}
		m_hWnd = NULL;
		m_releaseFlag = true;
		return ret;
	}

	HRESULT virtual Update()
	{
#if USING_OWN_DD
		static LPSTR str4Primary = _T("hello wworld !");
		static LPSTR str4Back = _T("hello guys !");
		static bool buffFlag = false;

		if (buffFlag)
		{
			HDC tmpDc;
			HRESULT ddrVal = m_lpDDSBack->GetDC(&tmpDc);
			assert(DD_OK == ddrVal);
			if(DD_OK != ddrVal) return E_FAIL;

			SetBkColor(tmpDc, RGB(0, 255, 0));
			SetTextColor(tmpDc, RGB(255, 255, 0));
			TextOut(tmpDc, 0, 0, str4Primary, lstrlen(str4Primary));

			m_lpDDSBack->ReleaseDC(tmpDc);
		}else
		{
			HDC tmpDc;
			HRESULT ddrVal = m_lpDDSBack->GetDC(&tmpDc);
			assert(DD_OK == ddrVal);
			if(DD_OK != ddrVal) return E_FAIL;

			SetBkColor(tmpDc, RGB(255, 0, 0));
			SetTextColor(tmpDc, RGB(255, 255, 0));
			TextOut(tmpDc, 0, 0, str4Back, lstrlen(str4Back));

			m_lpDDSBack->ReleaseDC(tmpDc);
		}

		//while (1)
		//{
		//	HRESULT ddrval;
		//	ddrval = m_lpDDSPrimary->Flip(NULL, 0);
		//	if (DD_OK == ddrval)
		//	{
		//		//return S_OK;
		//		break;
		//	}
		//	if (DDERR_SURFACELOST == ddrval)
		//	{
		//		if (DD_OK != (ddrval = m_lpDDSPrimary->Restore()))
		//		{
		//			//return S_FALSE;
		//			break;
		//		}
		//	}
		//	if (DDERR_WASSTILLDRAWING != ddrval)
		//	{
		//		//return E_FAIL;
		//		break;
		//	}
		//}

		buffFlag = !buffFlag;
#endif
		bool bIsReadyFrameData = m_pVideoOperator->ReadyFrame(true);
		memcpy(m_pVideoOperator->VideoMemoryPtr, m_pTargetTexturePixels, m_pVideoOperator->Width * m_pVideoOperator->Height * sizeof(DWORD));
		m_pVideoOperator->RenderFrame();
		return S_OK;
	}
protected:
	int m_idxWwnd;
	Gut3DWnd *m_pVdWnd;

#if USING_GITHUB
	DirectDrawWrapper::VideoMemory *m_pVideoOperator;
#endif

#if USING_OWN_DD
	LPDIRECTDRAWSURFACE m_lpDDSBack;
	LPDIRECTDRAWSURFACE m_lpDDSPrimary;	
	LPDIRECTDRAW m_lpVideoDD;
#endif

private:
	int m_releaseFlag;
};
#endif
#endif