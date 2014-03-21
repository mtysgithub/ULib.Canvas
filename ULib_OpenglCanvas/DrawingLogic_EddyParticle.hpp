#pragma once

#include "stdafx.h"
#include "IExDrawLogic.h"

class EddyParticle : public IExDrawLogic
{
public:
	EddyParticle(){}
	~EddyParticle(){}

	HRESULT virtual Init(IN int canvasWidth, IN int canvasHeight)
	{
		m_canvasWidth = canvasWidth;
		m_canvasHeight = canvasHeight;

		Matrix4x4 view_matrix = Matrix4x4::IdentityMatrix();
		Matrix4x4 perspective_matrix = GutMatrixOrthoRH_OpenGL(2, 2, 1, 10000);
		Matrix4x4 view_perspective_matrix = view_matrix * perspective_matrix;
		glLoadMatrixf((float *) &view_perspective_matrix);

		return S_OK;
	}

	HRESULT virtual UnInit()
	{
		return S_OK;
	}

	HRESULT virtual RenderFrame()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(1.0, 0.0, 0.0, 1);
		return S_OK;
	}
protected:
	
private:
	int m_canvasWidth;
	int m_canvasHeight;
};