#include "stdafx.h"
#include "ULib_OpenGLCanvas.hpp"

ULIB_OPENGLCANVAS_API HRESULT ULib_GLCanvas_Init(IN BYTE index, IN int canvasWidth, IN int canvasHeight, IN int *pTargetTexturePixels, IN EnumWorkingMode mode = EnumWorkingMode::DataOutput, IN UINT dataFormat = GL_BGRA)
{
	return (ULib_OpenGLCanvas::CreateInstance(index))->Init(canvasWidth, canvasHeight, pTargetTexturePixels, mode, dataFormat);
}

ULIB_OPENGLCANVAS_API HRESULT ULib_GLCanvas_UnInit(BYTE index)
{
	return ULib_OpenGLCanvas::DestoryInstance(index);
}

ULIB_OPENGLCANVAS_API HRESULT ULib_GLCanvas_Update(BYTE index)
{
	return (ULib_OpenGLCanvas::Instance(index))->Update();
}