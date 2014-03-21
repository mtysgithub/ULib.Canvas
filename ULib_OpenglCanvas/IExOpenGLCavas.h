#pragma once

#include "stdafx.h"

interface IExOpenGLCanvas
{
public:
	HRESULT virtual Init(IN int canvasWidth, IN int canvasHeight, IN int *pTargetTexturePixels) = 0;
	HRESULT virtual UnInit() = 0;
	HRESULT virtual Update() = 0;
};