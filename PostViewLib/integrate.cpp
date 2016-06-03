#include "stdafx.h"
#include "FEMesh.h"

//-----------------------------------------------------------------------------
// Integration rule for quads
// first index is integration point, second is node

// Shape functions:
const float H4[4][4] = {
	{ 0.62200847f, 0.16666667f, 0.04465820f, 0.16666667f},
	{ 0.16666667f, 0.62200847f, 0.16666667f, 0.04465820f},
	{ 0.04465820f, 0.16666667f, 0.62200847f, 0.16666667f},
	{ 0.16666667f, 0.04465820f, 0.16666667f, 0.62200847f}
};

// r-Derivatives of shape functions:
const float G4r[4][4] = {
	{-0.39433757f, 0.39433757f, 0.10566243f,-0.10566243f},
	{-0.39433757f, 0.39433757f, 0.10566243f,-0.10566243f},
	{-0.10566243f, 0.10566243f, 0.39433757f,-0.39433757f},
	{-0.10566243f, 0.10566243f, 0.39433757f,-0.39433757f},
};

// s-Derivatives of shape functions:
const float G4s[4][4] = {
	{-0.39433757f,-0.10566243f, 0.10566243f, 0.39433757f},
	{-0.10566243f,-0.39433757f, 0.39433757f, 0.10566243f},
	{-0.10566243f,-0.39433757f, 0.39433757f, 0.10566243f},
	{-0.39433757f,-0.10566243f, 0.10566243f, 0.39433757f}
};


//-----------------------------------------------------------------------------
// Integration rule for hexes
// first index is integration point, second is node

// Shape functions:
const float H8[8][8] = {
	{ 0.49056261f, 0.13144586f, 0.03522081f, 0.13144586f, 0.13144586f, 0.03522081f, 0.00943739f, 0.03522081f},
	{ 0.13144586f, 0.49056261f, 0.13144586f, 0.03522081f, 0.03522081f, 0.13144586f, 0.03522081f, 0.00943739f},
	{ 0.03522081f, 0.13144586f, 0.49056261f, 0.13144586f, 0.00943739f, 0.03522081f, 0.13144586f, 0.03522081f},
	{ 0.13144586f, 0.03522081f, 0.13144586f, 0.49056261f, 0.03522081f, 0.00943739f, 0.03522081f, 0.13144586f},
	{ 0.13144586f, 0.03522081f, 0.00943739f, 0.03522081f, 0.49056261f, 0.13144586f, 0.03522081f, 0.13144586f},
	{ 0.03522081f, 0.13144586f, 0.03522081f, 0.00943739f, 0.13144586f, 0.49056261f, 0.13144586f, 0.03522081f},
	{ 0.00943739f, 0.03522081f, 0.13144586f, 0.03522081f, 0.03522081f, 0.13144586f, 0.49056261f, 0.13144586f},
	{ 0.03522081f, 0.00943739f, 0.03522081f, 0.13144586f, 0.13144586f, 0.03522081f, 0.13144586f, 0.49056261f}
};

// r-Derivatives of shape functions:
const float G8r[8][8] = {
	{-0.31100423f, 0.31100423f, 0.08333333f,-0.08333333f,-0.08333333f, 0.08333333f, 0.02232910f,-0.02232910f},
	{-0.31100423f, 0.31100423f, 0.08333333f,-0.08333333f,-0.08333333f, 0.08333333f, 0.02232910f,-0.02232910f},
	{-0.08333333f, 0.08333333f, 0.31100423f,-0.31100423f,-0.02232910f, 0.02232910f, 0.08333333f,-0.08333333f},
	{-0.08333333f, 0.08333333f, 0.31100423f,-0.31100423f,-0.02232910f, 0.02232910f, 0.08333333f,-0.08333333f},
	{-0.08333333f, 0.08333333f, 0.02232910f,-0.02232910f,-0.31100423f, 0.31100423f, 0.08333333f,-0.08333333f},
	{-0.08333333f, 0.08333333f, 0.02232910f,-0.02232910f,-0.31100423f, 0.31100423f, 0.08333333f,-0.08333333f},
	{-0.02232910f, 0.02232910f, 0.08333333f,-0.08333333f,-0.08333333f, 0.08333333f, 0.31100423f,-0.31100423f},
	{-0.02232910f, 0.02232910f, 0.08333333f,-0.08333333f,-0.08333333f, 0.08333333f, 0.31100423f,-0.31100423f}
};

// s-Derivatives of shape functions:
const float G8s[8][8] = {
	{-0.31100423f,-0.08333333f, 0.08333333f, 0.31100423f,-0.08333333f,-0.02232910f, 0.02232910f, 0.08333333f},
	{-0.08333333f,-0.31100423f, 0.31100423f, 0.08333333f,-0.02232910f,-0.08333333f, 0.08333333f, 0.02232910f},
	{-0.08333333f,-0.31100423f, 0.31100423f, 0.08333333f,-0.02232910f,-0.08333333f, 0.08333333f, 0.02232910f},
	{-0.31100423f,-0.08333333f, 0.08333333f, 0.31100423f,-0.08333333f,-0.02232910f, 0.02232910f, 0.08333333f},
	{-0.08333333f,-0.02232910f, 0.02232910f, 0.08333333f,-0.31100423f,-0.08333333f, 0.08333333f, 0.31100423f},
	{-0.02232910f,-0.08333333f, 0.08333333f, 0.02232910f,-0.08333333f,-0.31100423f, 0.31100423f, 0.08333333f},
	{-0.02232910f,-0.08333333f, 0.08333333f, 0.02232910f,-0.08333333f,-0.31100423f, 0.31100423f, 0.08333333f},
	{-0.08333333f,-0.02232910f, 0.02232910f, 0.08333333f,-0.31100423f,-0.08333333f, 0.08333333f, 0.31100423f}
};

// t-Derivatives of shape functions:
const float G8t[8][8] = {
	{-0.31100423f,-0.08333333f,-0.02232910f,-0.08333333f, 0.31100423f, 0.08333333f, 0.02232910f, 0.08333333f},
	{-0.08333333f,-0.31100423f,-0.08333333f,-0.02232910f, 0.08333333f, 0.31100423f, 0.08333333f, 0.02232910f},
	{-0.02232910f,-0.08333333f,-0.31100423f,-0.08333333f, 0.02232910f, 0.08333333f, 0.31100423f, 0.08333333f},
	{-0.08333333f,-0.02232910f,-0.08333333f,-0.31100423f, 0.08333333f, 0.02232910f, 0.08333333f, 0.31100423f},
	{-0.31100423f,-0.08333333f,-0.02232910f,-0.08333333f, 0.31100423f, 0.08333333f, 0.02232910f, 0.08333333f},
	{-0.08333333f,-0.31100423f,-0.08333333f,-0.02232910f, 0.08333333f, 0.31100423f, 0.08333333f, 0.02232910f},
	{-0.02232910f,-0.08333333f,-0.31100423f,-0.08333333f, 0.02232910f, 0.08333333f, 0.31100423f, 0.08333333f},
	{-0.08333333f,-0.02232910f,-0.08333333f,-0.31100423f, 0.08333333f, 0.02232910f, 0.08333333f, 0.31100423f}
};

//-----------------------------------------------------------------------------
// Calculate integral over face
float FEMeshBase::IntegrateQuad(vec3f* r, float* v)
{
	int i, j;

	float I = 0.f;
	for (i=0; i<4; ++i)
	{
		// evaluate jacobian at integration point
		vec3f a1, a2;
		for (j=0; j<4; ++j)
		{
			a1 += r[j]*G4r[i][j];
			a2 += r[j]*G4s[i][j];
		}
		vec3f n = a1^a2;
		float J = n.Length();

		// evaluate function at integration point
		float vi = 0.f;
		for (j=0; j<4; ++j) vi += H4[i][j]*v[j];

		// sum up (integration weight = 1)
		I += J*vi;
	}

	return I;
}

//-----------------------------------------------------------------------------
// Calculate integral over element
float FEMeshBase::IntegrateHex(vec3f* r, float *v)
{
	int i, j;
	float I = 0.f;
	for (i=0; i<8; ++i)
	{
		// evaluate jacobian at integration point
		float J[3][3] = {0};
		for (j=0; j<8; ++j) 
		{
			J[0][0] += r[j].x*G8r[i][j]; J[0][1] += r[j].x*G8s[i][j]; J[0][2] += r[j].x*G8t[i][j];
			J[1][0] += r[j].y*G8r[i][j]; J[1][1] += r[j].y*G8s[i][j]; J[1][2] += r[j].y*G8t[i][j];
			J[2][0] += r[j].z*G8r[i][j]; J[2][1] += r[j].z*G8s[i][j]; J[2][2] += r[j].z*G8t[i][j];
		}

		float detJ = J[0][0]*J[1][1]*J[2][2] + J[0][1]*J[1][2]*J[2][0] + J[0][2]*J[1][0]*J[2][1] - \
					 J[1][1]*J[0][2]*J[2][0] - J[0][0]*J[1][2]*J[2][1] - J[2][2]*J[0][1]*J[1][0];
		
		// evaluate function at integration point
		float vi = 0.f;
		for (j=0; j<8; ++j) vi += H8[i][j]*v[j];

		// sum up (integration weight = 1)
		I += detJ*vi;
	}

	return I;
}
