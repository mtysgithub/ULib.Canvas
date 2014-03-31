#pragma once

#include "stdafx.h"

enum EnumWorkingMode
{
	DataOutput = 0,
	DataInput

};
interface IExCanvas
{
public:
	HRESULT virtual Init(IN CONST CHAR *device, IN CONST CHAR *wndName, IN int canvasWidth, IN int canvasHeight, IN int *pTargetTexturePixels, IN EnumWorkingMode mode = EnumWorkingMode::DataOutput, IN UINT dataFormat = GL_BGRA) = 0;
	HRESULT virtual UnInit() = 0;
	HRESULT virtual Update() = 0;
};