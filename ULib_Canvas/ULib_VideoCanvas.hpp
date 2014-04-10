#pragma once
#include "stdafx.h"
#include "ULib_BaseCanvas.hpp"

#ifdef DIRECT3D_VERSION
#ifdef DIRECTDRAW_VERSION
class ULib_VideoCanvas
	: public ULib_BaseCanvas
{
public:

protected:

private:
};
#endif
#endif