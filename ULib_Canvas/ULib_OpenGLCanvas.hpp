#pragma once
#include "ULib_BaseCanvas.hpp"
class ULib_OpenGLCanvas :
	public ULib_BaseCanvas
{
public:

	ULib_OpenGLCanvas(void)
	{
	}

	virtual ~ULib_OpenGLCanvas(void)
	{
	}

#pragma region Override IExCanvas
	HRESULT virtual Init(IN CONST CHAR *device, IN CONST CHAR *wndName, IN int canvasWidth, IN int canvasHeight, IN int *pTargetTexturePixels, IN EnumWorkingMode mode = EnumWorkingMode::DataOutput, IN UINT dataFormat = GL_BGRA)
	{
		HRESULT ret = this->ULib_BaseCanvas::Init(device, wndName, canvasWidth, canvasHeight, pTargetTexturePixels, mode, dataFormat);
		if (S_OK == ret)
		{
			//Init DrawLogic
			return ret |= (m_fnDrawingLogic = static_cast<IExDrawLogic *>(new EddyParticle()))->Init(device, canvasWidth, canvasHeight);
		}
		return ret;
	}

	HRESULT virtual Update()
	{
		if (S_OK == this->ULib_BaseCanvas::Update())
		{
			switch (m_typWrkMod)
			{
			case DataOutput:
				//Output data to GameEngine

				if (GutProcessMessage())
				{
					HRESULT ret = S_OK;

					//TODO
					//Process Input Message

					if (m_fnDrawingLogic)
					{
						glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
#if WIN32
					glReadPixels(0, 0, m_canvasWidth, m_canvasHeight, m_dataFormat, GL_UNSIGNED_BYTE, m_pTargetTexturePixels);
#endif
					glReadBuffer(iLastBuffer);
					return ret;
				}else
				{
					//GL message loop has exited.
					return E_FAIL;
				}

			case DataInput:
				//Receive data from GameEngine

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glDrawPixels(m_canvasWidth, m_canvasHeight, m_dataFormat, GL_UNSIGNED_BYTE, m_pTargetTexturePixels);
				GutSwapBuffersOpenGL();

				return S_OK;

			default:
				return E_FAIL;
			}
		}
	}

	HRESULT virtual UnInit()
	{
		HRESULT ret = S_OK;
		if (m_fnDrawingLogic)
		{
			ret |= m_fnDrawingLogic->UnInit();
		}
		if (S_OK == ret)
		{
			return ret |= this->ULib_BaseCanvas::UnInit();
		}
		return ret;
	}
#pragma endregion Override IExCanvas
};

