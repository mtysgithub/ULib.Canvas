#include <stdio.h>
#include <math.h>
#include <iostream>

#include "Gut.h"
#include "GutWin32.hpp"
#include "GutInput.h"
#include "Vector4.h"
using namespace std;

#include "opencv2/opencv.hpp"
using namespace cv;

#define  USING_LIB 1

#if !USING_LIB
void RenderFrameOpenGL(void);
void InitParticleDrawing();

void GetUserInput();
void procMouse(int button, int state, int x, int y);
void track_down(GutMouseInfo mouse);
void track_move(GutMouseInfo mouse);
void track_up(GutMouseInfo mouse);

const int g_cw = 1366;
const int g_ch = 768;
float g_ratio = g_cw / g_ch;
float g_tw = g_cw / 2.0;
float g_th = g_ch / 2.0;
float g_tratio = g_tw / g_th;
#endif // !USING_LIB

int main(int argc, char *argv[])
{
#if USING_LIB
#else
	char *device = "opengl";
	GutCreateWindow(0, 0, g_cw, g_ch, "Particle");

	if (!GutInitGraphicsDevice(device))
	{
		exit(0);
	}

	InitParticleDrawing();
	GutInputInit();

	while (GutProcessMessage())
	{
		GetUserInput();
		RenderFrameOpenGL();
	}
	GutReleaseGraphicsDevice();
	GutCloseWindow();
#endif
	return 0;
}

#if !USING_LIB

#define PI 3.1415926535897932

Vector4 g_eye(0.0f,0.0f,0.0f); 
Vector4 g_lookat(0.0f, 0.0f, -1.0f); 
Vector4 g_up(0.0f, 1.0f, 0.0f); 

float touchX = 0;
float touchY = 0;
bool bIsTouch = false;

const int g_lineNumber = 40000;
Vector4 *p_g_vertices = NULL;
Vector4 *p_g_velocities = NULL;

#define RANDOM (gMakeRandom())

float gMakeRandom()
{
	float num = ((rand() * rand() * rand() * rand()) * 1.0f);
	num = fabs(num);
	while (num > 1.0f)
	{
		num /= 10.0f;
	}
	return num; 
}

void InitParticleDrawing()
{
	srand(0);
	p_g_vertices = new Vector4[g_lineNumber * 3];
	p_g_velocities = new Vector4[g_lineNumber * 3];
	for (int i = 0; i < g_lineNumber; ++i)
	{
		//printf("%.17f\n", RANDOM);
		p_g_vertices[i].SetX(0);
		p_g_vertices[i].SetY(0);
		p_g_vertices[i].SetZ(-1.83);
		p_g_vertices[i].SetW(1);

		p_g_velocities[i].SetX((RANDOM * 2 - 1) * 0.05);
		p_g_velocities[i].SetY((RANDOM * 2 - 1) * 0.05);
		p_g_velocities[i].SetZ(0.93 + RANDOM * 0.02);
		p_g_velocities[i].SetW(1);
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
	

	//Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	Matrix4x4 view_matrix = Matrix4x4::IdentityMatrix();
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
}

void RenderFrameOpenGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < g_lineNumber; i+=2)
	{
		p_g_vertices[i].x = p_g_vertices[i + 1].x;
		p_g_vertices[i].y = p_g_vertices[i + 1].y;

		//惯性
		p_g_velocities[i].x *= p_g_velocities[i].z;
		p_g_velocities[i].y *= p_g_velocities[i].z;

		float p;
		//水平
		p = p_g_vertices[i + 1].x;
		p += p_g_velocities[i].x;
		if (p < -g_ratio)
		{
			p = -g_ratio;
			p_g_velocities[i].x = fabs(p_g_velocities[i].x);
		} 
		else
		{
			if (p > g_ratio)
			{
				p = g_ratio;
				p_g_velocities[i].x = -fabs(p_g_velocities[i].x);
			}
		}
		p_g_vertices[i + 1].x = p;

		//垂直
		p = p_g_vertices[i + 1].y;
		p += p_g_velocities[i].y;
		if (p < -1)
		{
			p = -1;
			p_g_velocities[i].y = fabs(p_g_velocities[i].y);
		}
		else
		{
			if (p > 1)
			{
				p = 1;
				p_g_velocities[i].y = -fabs(p_g_velocities[i].y);
			}
		}
		p_g_vertices[i + 1].y = p;

		if (bIsTouch)
		{
			//touch 
			float dx = touchX - p_g_vertices[i].x,
					dy = touchY - p_g_vertices[i].y,
					d = sqrt(dx * dx + dy * dy);
			if (d < 2)
			{
				if (d < 0.03)
				{
					p_g_vertices[i + 1].x = (RANDOM * 2 - 1) * g_ratio;
					p_g_vertices[i + 1].y = RANDOM * 2 - 1;
					p_g_velocities[i].x = 0;
					p_g_velocities[i].y = 0;
				}
				else
				{
					dx /= d;
					dy /= d;
					d = (2 - d) / 2;
					d *= d;
					p_g_velocities[i].x += dx * d * 0.01;
					p_g_velocities[i].y += dy * d * 0.01;
				}
			} 
		}
	}

	//draw it	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(4, GL_FLOAT, sizeof(Vector4), p_g_vertices);
	glDrawArrays(GL_LINES, 0, g_lineNumber);
	GutSwapBuffersOpenGL();
}


void GetUserInput()
{
	GutMouseInfo mouse;
	GutReadMouse(&mouse);
	if (mouse.button[0])
	{
		if (!bIsTouch)
		{
			track_down(mouse);
		}else
		{
			track_move(mouse);
		}
	}else
	{
		if (bIsTouch)
		{
			track_up(mouse);
		}
	}
}

void track_down(GutMouseInfo mouse)
{
	if (!bIsTouch)
	{
		bIsTouch = true;
		track_move(mouse);
	}
}

void track_move(GutMouseInfo mouse)
{
	int px, py, buttonState[3];
	GutGetMouseState(px, py, buttonState);
	touchX = (px / g_tw) - 0.98;
	touchY = (-1 * (py / g_th)) + 0.95;
	//printf("%d, %d\n", px, py);
}

void track_up(GutMouseInfo mouse)
{
	bIsTouch = false;
}
#endif