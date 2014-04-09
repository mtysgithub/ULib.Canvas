#pragma once
#include "stdafx.h"
#include "ULib_BaseCanvas.hpp"

#ifdef DIRECT3D_VERSION
#if 0x0900 == DIRECT3D_VERSION
class ULib_DX9Canvas :
	public ULib_BaseCanvas
{
public:
	ULib_DX9Canvas(void){}
	virtual ~ULib_DX9Canvas(void){}
#pragma region Override IExCanvas
	HRESULT virtual Update()
	{
		if (S_OK == this->ULib_BaseCanvas::Update() /*此处做静态绑定 etc.*/)
		{
			switch (m_typWrkMod)
			{
			case DataOutput:
				{
					//TODO.
					return S_OK;
				}
			case DataInput:
				{
					LPDIRECT3DDEVICE9 pDx9Device = GutGetGraphicsDeviceDX9();
					if (pDx9Device)
					{
						//创建一个渲染表面 
						IDirect3DSurface9* pSurface; 
						//此处参数与OpenGL不同, 由于Pixle对象为一个DWORD,而非BYTE,故宏定义使用ARGB表达，底层字节序按照小端存放为B,G,R,A, 依然与OpenGL相同(Win32平台下)
						pDx9Device->CreateOffscreenPlainSurface(m_canvasWidth, m_canvasHeight, (D3DFORMAT)m_dataFormat, D3DPOOL_DEFAULT, &pSurface,0);

						//上写锁
						D3DLOCKED_RECT rc;  
						memset(&rc,0,sizeof(rc));  
						pSurface->LockRect(&rc,NULL,D3DLOCK_DISCARD); 

						DWORD* pBits = (DWORD*)rc.pBits;
						memcpy(pBits, m_pTargetTexturePixels, m_canvasWidth * m_canvasHeight * sizeof(DWORD));

						//释放写锁
						pSurface->UnlockRect();
						//获取D3D默认交换链中的后台缓存 
						IDirect3DSurface9* backBuffer = 0; 
						pDx9Device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer); 
						//使用自定义渲染表面填充后台缓存 
						pDx9Device->StretchRect(pSurface, 0, backBuffer, 0, D3DTEXF_NONE); 
						//GetBackBuffer所得的缓存需要被释放 
						backBuffer->Release(); 

						pDx9Device->Present(NULL, NULL, NULL, NULL);
						return S_OK;
					}else
					{
						return E_FAIL;
					}
				}
			default:
				{
					return E_FAIL;
				}
			}
		}else
		{
			return E_FAIL;
		}
	}
#pragma endregion Override IExCanvas
};
#endif
#endif
