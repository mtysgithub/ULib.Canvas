#pragma once

#include "stdafx.h"

/************************************************************************/
/*
功能说明：绘制逻辑请实现该接口，外挂程序将在恰当的时机执行它
*/
/************************************************************************/
interface IExDrawLogic
{
public:
	HRESULT virtual Initialize(IN CONST CHAR *device, IN int canvasWidth, IN int canvasHeight) = 0;
	HRESULT virtual Release() = 0;
	HRESULT virtual RenderFrame() = 0;
};
