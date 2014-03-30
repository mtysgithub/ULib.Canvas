#pragma once

#include "stdafx.h"

enum EnumWorkingMode
{
	DataOutput = 0,
	DataInput

};
interface IExOpenGLCanvas
{
public:
	HRESULT virtual Init(IN int canvasWidth, IN int canvasHeight, IN int *pTargetTexturePixels, IN EnumWorkingMode mode) = 0;
	HRESULT virtual UnInit() = 0;
	HRESULT virtual Update() = 0;
};