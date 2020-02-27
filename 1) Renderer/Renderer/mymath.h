#pragma once
#include <math.h>

#define PI 3.141592f
/*
	TVECTOR/TMATRIX Functions and unions created by myself (Cameron Marotto) 3/23/19 & Projection 10/12/19
*/
// Vector 4
typedef union TVECTOR
{
	// array access to vector elements
	float e[4];
	// individual component access
	struct
	{
		float x, y, z, w;
	};

}*LPTVECTOR;

// 4x4 Row Major Matrix
typedef union TMATRIX
{
	float e[16];
	// Single elements in row major format
	struct
	{
		float _e11, _e12, _e13, _e14;
		float _e21, _e22, _e23, _e24;
		float _e31, _e32, _e33, _e34;
		float _e41, _e42, _e43, _e44;
	};
}*LPTMATRIX;
struct VEC_2D
{
	float x, y;
};
struct VEC_3D
{
	float x, y, z;
};
// 3D Vertex 
struct VERTEX_3D
{
	TVECTOR xyzw;
	TVECTOR rgba;//unsigned int color;
	//VEC_2D uv;
};

// RETURN: Converts input to radian measure
float Degrees_To_Radians(float Deg)
{
	// NOTE: Do not modify.
	return Deg * PI / 180.0f;
}

// Multipy a matrix and a vector
TVECTOR Vector_Matrix_Multiply(TVECTOR v, TMATRIX m)
{
	TVECTOR answer;
	answer.x = (m._e11 * v.x + m._e21 * v.y + m._e31 * v.z + m._e41 * v.w);
	answer.y = (m._e12 * v.x + m._e22 * v.y + m._e32 * v.z + m._e42 * v.w);
	answer.z = (m._e13 * v.x + m._e23 * v.y + m._e33 * v.z + m._e43 * v.w);
	answer.w = (m._e14 * v.x + m._e24 * v.y + m._e34 * v.z + m._e44 * v.w);

	return answer;
}

// Multiply a matrix by a matrix
TMATRIX Matrix_Matrix_Multiply(TMATRIX m, TMATRIX n)
{
	TMATRIX answer;
	answer._e11 = m._e11 * n._e11 + m._e12 * n._e21 + m._e13 * n._e31 + m._e14 * n._e41;
	answer._e12 = m._e11 * n._e12 + m._e12 * n._e22 + m._e13 * n._e32 + m._e14 * n._e42;
	answer._e13 = m._e11 * n._e13 + m._e12 * n._e23 + m._e13 * n._e33 + m._e14 * n._e43;
	answer._e14 = m._e11 * n._e14 + m._e12 * n._e24 + m._e13 * n._e34 + m._e14 * n._e44;
	answer._e21 = m._e21 * n._e11 + m._e22 * n._e21 + m._e23 * n._e31 + m._e24 * n._e41;
	answer._e22 = m._e21 * n._e12 + m._e22 * n._e22 + m._e23 * n._e32 + m._e24 * n._e42;
	answer._e23 = m._e21 * n._e13 + m._e22 * n._e23 + m._e23 * n._e33 + m._e24 * n._e43;
	answer._e24 = m._e21 * n._e14 + m._e22 * n._e24 + m._e23 * n._e34 + m._e24 * n._e44;
	answer._e31 = m._e31 * n._e11 + m._e32 * n._e21 + m._e33 * n._e31 + m._e34 * n._e41;
	answer._e32 = m._e31 * n._e12 + m._e32 * n._e22 + m._e33 * n._e32 + m._e34 * n._e42;
	answer._e33 = m._e31 * n._e13 + m._e32 * n._e23 + m._e33 * n._e33 + m._e34 * n._e43;
	answer._e34 = m._e31 * n._e14 + m._e32 * n._e24 + m._e33 * n._e34 + m._e34 * n._e44;
	answer._e41 = m._e41 * n._e11 + m._e42 * n._e21 + m._e43 * n._e31 + m._e44 * n._e41;
	answer._e42 = m._e41 * n._e12 + m._e42 * n._e22 + m._e43 * n._e32 + m._e44 * n._e42;
	answer._e43 = m._e41 * n._e13 + m._e42 * n._e23 + m._e43 * n._e33 + m._e44 * n._e43;
	answer._e44 = m._e41 * n._e14 + m._e42 * n._e24 + m._e43 * n._e34 + m._e44 * n._e44;
	return answer;
}

// Get a Perspective Projection Matrix
TMATRIX Matrix_Create_Projection(float xScale, float yScale, float zNear, float zFar)
{
	TMATRIX m = { xScale, 0, 0, 0,
				  0, yScale, 0, 0,
				  0, 0, zFar / (zFar - zNear), 1.0f,
				  0, 0, -(zFar * zNear) / (zFar - zNear), 0
	};
	return m;
}

// Get a rotation matrix for rotation about the x-axis
TMATRIX Matrix_Create_Rotation_X(float Deg)
{
	float cosine = cosf(Deg * (PI / 180));
	float sine = sinf(Deg * (PI / 180));
	TMATRIX m = { 1, 0, 0, 0
				,0, cosine, -sine, 0
				,0, sine, cosine, 0
				,0, 0, 0, 1 };
	return m;
}

// Get a rotation matrix for rotation about the y-axis
TMATRIX Matrix_Create_Rotation_Y(float Deg)
{
	float cosine = cosf(Deg * (PI / 180));
	float sine = sinf(Deg * (PI / 180));
	TMATRIX m = { cosine, 0, sine, 0
				,0, 1, 0, 0
				,-sine,0, cosine, 0
				,0, 0, 0, 1 };
	return m;
}

// Get a translation matrix
TMATRIX Matrix_Create_Translation(float x, float y, float z)
{
	TMATRIX m = { 1, 0, 0, 0,
				  0, 1, 0, 0,
				  0, 0, 1, 0,
				  x, y, z, 1 };
	return m;
}

// Get a [I] matrix, returns A 4x4 Identity matrix
TMATRIX Matrix_Identity(void)
{
	TMATRIX m = { 1, 0, 0 ,0,
				  0, 1, 0, 0,
				  0, 0, 1, 0
				, 0, 0, 0, 1 };
	return m;
}

// Transpose a matrix
TMATRIX Matrix_Transpose(TMATRIX m)
{
	float e12 = m._e12;
	float e13 = m._e13;
	float e14 = m._e14;
	float e23 = m._e23;
	float e24 = m._e24;
	float e43 = m._e43;
	float e34 = m._e34;

	m._e12 = m._e21;
	m._e13 = m._e31;
	m._e14 = m._e41;
	m._e21 = e12;
	m._e31 = e13;
	m._e41 = e14;
	m._e23 = m._e32;
	m._e24 = m._e42;
	m._e34 = m._e43;
	m._e32 = e23;
	m._e42 = e24;
	m._e43 = e34;

	return m;
}

// Helper function that finds the Determinant of a 3x3 matrix
float Matrix_Determinant(float e_11, float e_12, float e_13,
	float e_21, float e_22, float e_23,
	float e_31, float e_32, float e_33)
{
	float answer = e_11 * (e_22 * e_33 - e_32 * e_23) - e_12 * (e_21 * e_33 - e_31 * e_23) + e_13 * (e_21 * e_32 - e_31 * e_22);
	return answer;
}

// Get the determinant of a matrix
float Matrix_Determinant(TMATRIX m)
{
	float first = m._e11 * Matrix_Determinant(m._e22, m._e23, m._e24, m._e32, m._e33, m._e34, m._e42, m._e43, m._e44);
	float second = -m._e12 * Matrix_Determinant(m._e21, m._e23, m._e24, m._e31, m._e33, m._e34, m._e41, m._e43, m._e44);
	float third = m._e13 * Matrix_Determinant(m._e21, m._e22, m._e24, m._e31, m._e32, m._e34, m._e41, m._e42, m._e44);
	float fourth = -m._e14 * Matrix_Determinant(m._e21, m._e22, m._e23, m._e31, m._e32, m._e33, m._e41, m._e42, m._e43);
	return first + second + third + fourth;
}

// Get the inverse of a matrix
TMATRIX Matrix_Inverse(TMATRIX m)
{
	TMATRIX answer;
	TMATRIX adj;
	adj._e11 = Matrix_Determinant(m._e22, m._e23, m._e24, m._e32, m._e33, m._e34, m._e42, m._e43, m._e44);
	adj._e12 = -Matrix_Determinant(m._e21, m._e23, m._e24, m._e31, m._e33, m._e34, m._e41, m._e43, m._e44);
	adj._e13 = Matrix_Determinant(m._e21, m._e22, m._e24, m._e31, m._e32, m._e34, m._e41, m._e42, m._e44);
	adj._e14 = -Matrix_Determinant(m._e21, m._e22, m._e23, m._e31, m._e32, m._e33, m._e41, m._e42, m._e43);
	adj._e21 = -Matrix_Determinant(m._e12, m._e13, m._e14, m._e32, m._e33, m._e34, m._e42, m._e43, m._e44);
	adj._e22 = Matrix_Determinant(m._e11, m._e13, m._e14, m._e31, m._e33, m._e34, m._e41, m._e43, m._e44);
	adj._e23 = -Matrix_Determinant(m._e11, m._e12, m._e14, m._e31, m._e32, m._e34, m._e41, m._e42, m._e44);
	adj._e24 = Matrix_Determinant(m._e11, m._e12, m._e13, m._e31, m._e32, m._e33, m._e41, m._e42, m._e43);
	adj._e31 = Matrix_Determinant(m._e12, m._e13, m._e14, m._e22, m._e23, m._e24, m._e42, m._e43, m._e44);
	adj._e32 = -Matrix_Determinant(m._e11, m._e13, m._e14, m._e21, m._e23, m._e24, m._e41, m._e43, m._e44);
	adj._e33 = Matrix_Determinant(m._e11, m._e12, m._e14, m._e21, m._e22, m._e24, m._e41, m._e42, m._e44);
	adj._e34 = -Matrix_Determinant(m._e11, m._e12, m._e13, m._e21, m._e22, m._e23, m._e41, m._e42, m._e43);
	adj._e41 = -Matrix_Determinant(m._e12, m._e13, m._e14, m._e22, m._e23, m._e24, m._e32, m._e33, m._e34);
	adj._e42 = Matrix_Determinant(m._e11, m._e13, m._e14, m._e21, m._e23, m._e24, m._e31, m._e33, m._e34);
	adj._e43 = -Matrix_Determinant(m._e11, m._e12, m._e14, m._e21, m._e22, m._e24, m._e31, m._e32, m._e34);
	adj._e44 = Matrix_Determinant(m._e11, m._e12, m._e13, m._e21, m._e22, m._e23, m._e31, m._e32, m._e33);

	adj = Matrix_Transpose(adj);
	float det = Matrix_Determinant(m);

	for (int i = 0; i < 16; i++)
	{
		answer.e[i] = adj.e[i] / det;
	}
	if (det != 0)
	{
		return answer;
	}
	else
		return m;
}

// Get Implicit Line Equation //takes 1 point and 1 line(2points)
float ImplicitLineEq(TVECTOR _a, TVECTOR _b, TVECTOR point)
{
	//(Y1-Y2)x + (X2-X1)y + X1Y2 - Y1X2
	return(_a.y - _b.y) * point.x + (_b.x - _a.x) * point.y + _a.x * _b.y - _a.y * _b.x;
}

// Find Barycentric coordinates
TVECTOR FindBarycentric(TVECTOR _a, TVECTOR _b, TVECTOR _c, TVECTOR point)
{
	float max_a = ImplicitLineEq(_b, _c, _a);
	float max_b = ImplicitLineEq(_a, _c, _b);
	float max_c = ImplicitLineEq(_b, _a, _c);

	float sub_a = ImplicitLineEq(_b, _c, point);
	float sub_b = ImplicitLineEq(_a, _c, point);
	float sub_c = ImplicitLineEq(_b, _a, point);

	return { sub_a / max_a, sub_b / max_b, sub_c / max_c };
}

unsigned int ColorLerp(unsigned int _a, unsigned int _b, float ratio)
{
	// Need to use ints instead of unsigned ints in this function in order to display properly
	//Shrub?

	int ar = (_a & 0xFF000000) >> 24;
	int ag = (_a & 0x00FF0000) >> 16;
	int ab = (_a & 0x0000FF00) >> 8;
	int aa = (_a & 0x000000FF);

	int br = (_b & 0xFF000000) >> 24;
	int bg = (_b & 0x00FF0000) >> 16;
	int bb = (_b & 0x0000FF00) >> 8;
	int ba = (_b & 0x000000FF);


	unsigned int ra, rr, rg, rb;

	rr = (unsigned int)((br - ar) * ratio) + ar;
	rg = (unsigned int)((bg - ag) * ratio) + ag;
	rb = (unsigned int)((bb - ab) * ratio) + ab;
	ra = (unsigned int)((ba - aa) * ratio) + aa;


	return (rr << 24) | (rg << 16) | (rb << 8) | (ra);
}

float LERP(float start, float end, float ratio)
{
	return ((end - start) * ratio) + start;
}