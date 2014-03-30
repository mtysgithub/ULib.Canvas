#pragma once

#include <cmath>
#include "stdafx.h"
#include "IExDrawLogic.h"
#include "Vector4.h"

#define PI 3.1415926535897932
#define  PARTICLE_LINE_NUMBER 40000
#define RANDOM (__makeRandom())

class EddyParticle : public IExDrawLogic
{
public:
	EddyParticle(){}
	~EddyParticle(){}

	HRESULT virtual Init(IN int canvasWidth, IN int canvasHeight)
	{
		m_iCanvasWidth = canvasWidth;
		m_iCanvasHeight = canvasHeight;
		m_fScreenRatio = m_iCanvasWidth / m_iCanvasHeight;

		__InitParticleDrawing();

		return S_OK;
	}

	HRESULT virtual UnInit()
	{
		//if(m_pEyePos) delete m_pEyePos;
		//if(m_pLookat) delete m_pLookat;
		//if(m_pUp) delete m_pUp;

		//if(m_pParticleVertices) delete m_pParticleVertices;
		//if(m_pVelocities) delete m_pVelocities;
		return S_OK;
	}

	HRESULT virtual RenderFrame()
	{
#if !HIDE_CANVAS_WND
		__GetUserInput();
#endif

#pragma loop(hint_parallel(4))
		for (int i = 0; i < PARTICLE_LINE_NUMBER; i+=2)
		{
			m_pParticleVertices[i].x = m_pParticleVertices[i + 1].x;
			m_pParticleVertices[i].y = m_pParticleVertices[i + 1].y;

			//¹ßÐÔ
			m_pVelocities[i].x *= m_pVelocities[i].z;
			m_pVelocities[i].y *= m_pVelocities[i].z;

			float tmpClacTargetVal;
			tmpClacTargetVal = m_pParticleVertices[i + 1].x;
			tmpClacTargetVal += m_pVelocities[i].x;
			if (tmpClacTargetVal < -m_fScreenRatio)
			{
				tmpClacTargetVal = -m_fScreenRatio;
				m_pVelocities[i].x = fabs(m_pVelocities[i].x);
			}else
			{
				if (tmpClacTargetVal > m_fScreenRatio)
				{
					tmpClacTargetVal = m_fScreenRatio;
					m_pVelocities[i].x = -fabs(m_pVelocities[i].x);
				}
			}
			m_pParticleVertices[i + 1].x = tmpClacTargetVal;

			//´¹Ö±
			tmpClacTargetVal = m_pParticleVertices[i + 1].y;
			tmpClacTargetVal += m_pVelocities[i].y;
			if (tmpClacTargetVal < -1)
			{
				tmpClacTargetVal = -1;
				m_pVelocities[i].y = fabs(m_pVelocities[i].y);
			}else
			{
				if (tmpClacTargetVal > 1)
				{
					tmpClacTargetVal = 1;
					m_pVelocities[i].y = -fabs(m_pVelocities[i].y);
				}
			}
			m_pParticleVertices[i + 1].y = tmpClacTargetVal;

			if (m_bIsTouch)
			{
				//touch
				float dx = m_fTouchX - m_pParticleVertices[i].x,
						dy = m_fTouchY - m_pParticleVertices[i].y,
						dist = sqrt(dx * dx + dy * dy);
				if (dist < 2)
				{
					if (dist < 0.03)
					{
						m_pParticleVertices[i + 1].x = (RANDOM * 2 - 1) * m_fScreenRatio;
						m_pParticleVertices[i + 1].y = RANDOM * 2 - 1;
						m_pVelocities[i].x = 0;
						m_pVelocities[i].y = 0;
					} 
					else
					{
						dx /= dist;
						dy /= dist;
						dist = (2 - dist) / 2;
						dist *= dist;
						m_pVelocities[i].x += dx * dist * 0.01;
						m_pVelocities[i].y += dy * dist * 0.01;
					}
				}
			}
		}

		//draw it
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(4, GL_FLOAT, sizeof(Vector4), m_pParticleVertices);
		glDrawArrays(GL_LINES, 0, PARTICLE_LINE_NUMBER);
		return S_OK;
	}
protected:

private:
	void __InitParticleDrawing()
	{
		srand(0);

		m_pParticleVertices = new Vector4[PARTICLE_LINE_NUMBER * 3];
		m_pVelocities = new Vector4[PARTICLE_LINE_NUMBER * 3];
		for (int i = 0; i < PARTICLE_LINE_NUMBER; ++i)
		{
			m_pParticleVertices[i].SetX(0);
			m_pParticleVertices[i].SetY(0);
			m_pParticleVertices[i].SetZ(-1.83);
			m_pParticleVertices[i].SetW(1);

			m_pVelocities[i].SetX((RANDOM * 2 - 1) * 0.05);
			m_pVelocities[i].SetY((RANDOM * 2 - 1) * 0.05);
			m_pVelocities[i].SetZ(0.93 + RANDOM * 0.02);
			m_pVelocities[i].SetW(1);
		}

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClearDepth(1.0);
		//glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//glEnable(GL_ALPHA_TEST);
		glEnable(GL_LINE_SMOOTH);
		glColor4f(0.24, 0.51, 0.18, 0.04);

		Matrix4x4 view_matrix = Matrix4x4::IdentityMatrix();

		//Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
		//Matrix4x4 perspective_matrix = GutMatrixPerspectiveRH_OpenGL(30.0f, g_ratio, 1.0f, 10000.0f);
		//Matrix4x4 perspective_matrix = Matrix4x4::IdentityMatrix();

		//float fieldOfView = 30.0;
		//float aspectRatio = g_ratio;
		//float nearPlane = 1.0;
		//float farPlane = 10000.0;
		//float top = nearPlane * tan(fieldOfView * PI / 360.0);
		//float bottom = -1 * top;
		//float right = top * aspectRatio;
		//float left = -1 * right;

		//float a = (right + left) / (right - left);
		//float b = (top + bottom) / (top - bottom);
		//float c = (farPlane + nearPlane) / (farPlane - nearPlane);
		//float d = (2 * farPlane * nearPlane) / (farPlane - nearPlane);
		//float x = (2 * nearPlane) / (right - left);
		//float y = (2 * nearPlane) / (top - bottom);

		//perspective_matrix[0][0] = x;
		//perspective_matrix[1][1] = y;
		//perspective_matrix[2][2] = c;
		//perspective_matrix[3][3] = 0;

		//perspective_matrix[2][0] = a;
		//perspective_matrix[2][1] = b;
		//perspective_matrix[3][2] = d;
		//perspective_matrix[2][3] = -1;

		Matrix4x4 perspective_matrix = GutMatrixOrthoRH_OpenGL(2, 2, 1, 10000);
		Matrix4x4 view_perspective_matrix = view_matrix * perspective_matrix;
		glLoadMatrixf((float *) &view_perspective_matrix);

		m_bIsTouch = false;
		m_fTouchX = 0;
		m_fTouchY = 0;
	}

	inline float __makeRandom()
	{
		float num = ((rand() * rand() * rand() * rand()) * 1.0f);
		num = fabs(num);
		while (num > 1.0f)
		{
			num /= 10.0f;
		}
		return num; 
	}

#if !HIDE_CANVAS_WND
	void __GetUserInput()
	{
		GutMouseInfo mouse;
		GutReadMouse(&mouse);
		if (mouse.button[0])
		{
			if (!m_bIsTouch)
			{
				__track_down(mouse);
			}else
			{
				__track_move(mouse);
			}
		}else
		{
			if (m_bIsTouch)
			{
				__track_up(mouse);
			}
		}
	}

	inline void __track_down(GutMouseInfo mouse)
	{
		if (!m_bIsTouch)
		{
			m_bIsTouch = true;
			__track_move(mouse);
		}
	}

	inline void __track_move(GutMouseInfo mouse)
	{
		int px, py, buttonState[3];
		GutGetMouseState(px, py, buttonState);
		m_fTouchX = (px / (m_iCanvasWidth / 2.0)) - 0.98;
		m_fTouchY = (-1 * (py / (m_iCanvasHeight / 2.0))) + 0.95;
	}

	inline void __track_up(GutMouseInfo mouse)
	{
		m_bIsTouch = false;
	}
#endif
	
private:
	int m_iCanvasWidth;
	int m_iCanvasHeight;
	float m_fScreenRatio;

	Vector4 *m_pEyePos;
	Vector4 *m_pLookat;
	Vector4 *m_pUp;

	float m_fTouchX;
	float m_fTouchY;
	bool m_bIsTouch;

	Vector4 *m_pParticleVertices;
	Vector4 *m_pVelocities;
};