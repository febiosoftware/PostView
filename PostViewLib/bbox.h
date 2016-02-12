#pragma once
#include "math3d.h"

//-----------------------------------------------------------------------------
// Class that describes a bounding box structure
class BOUNDINGBOX
{
public:
	float x0, y0, z0;
	float x1, y1, z1;

public:
	// default constructor
	BOUNDINGBOX();

	// constructor from coordinates
	BOUNDINGBOX(float X0, float Y0, float Z0, float X1, float Y1, float Z1);

	// constructor from vectors
	BOUNDINGBOX(const vec3f& r0, const vec3f& r1);

	// size of box
	float Width () { return x1 - x0; }
	float Height() { return y1 - y0; }
	float Depth () { return z1 - z0; }

	// return largest dimension of box
	float GetMaxExtent();

	// return the center of the box
	vec3f Center();

	// return the radius of the box
	float Radius();

	// range of box
	void Range(vec3f& n, float& min, float& max);
	void Range(vec3f& n, vec3f& r0, vec3f& r1);

	// add a point to the box
	void operator += (vec3f r);

	// inflate the box
	void InflateTo(float fx, float fy, float fz);
};
