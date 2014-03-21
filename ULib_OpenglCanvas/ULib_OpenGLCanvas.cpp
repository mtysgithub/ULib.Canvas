#include "stdafx.h"
#include "ULib_OpenGLCanvas.hpp"

ULib_OpenGLCanvas* ULib_OpenGLCanvas::CreateInstance(int index)
{
	if (!ppULib_OpenGLCanvas)
	{
		ppULib_OpenGLCanvas = new (ULib_OpenGLCanvas *[MAX_SF_SIZE]);
		for(int i = 0; i < MAX_SF_SIZE; ++i) ppULib_OpenGLCanvas[i] = NULL;
	}
	if (ppULib_OpenGLCanvas[index])
	{
		delete ppULib_OpenGLCanvas[index];
	}
	return ppULib_OpenGLCanvas[index] = new ULib_OpenGLCanvas ();
}

ULib_OpenGLCanvas* ULib_OpenGLCanvas::Instance(int index)
{
	if (ppULib_OpenGLCanvas && ppULib_OpenGLCanvas[index])
	{
		return ppULib_OpenGLCanvas[index];
	}else
	{
		return NULL;
	}
}

HRESULT ULib_OpenGLCanvas::DestoryInstance(int index)
{
	if (ppULib_OpenGLCanvas[index])
	{
		ppULib_OpenGLCanvas[index]->UnInit();
		delete ppULib_OpenGLCanvas[index];
		ppULib_OpenGLCanvas[index] = NULL;
		return S_OK;
	}else
	{
		return E_FAIL;
	}
}

ULib_OpenGLCanvas **ppULib_OpenGLCanvas = NULL;