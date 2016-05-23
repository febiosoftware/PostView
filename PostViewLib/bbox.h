#pragma once
#include "math3d.h"

//-----------------------------------------------------------------------------
// Class that describes a bounding box structure
class BOUNDINGBOX
{
public:
	float x0, y0, z0;
	float x1, y1, z1;
	bool	bvalid;

public:
	// default constructor
	BOUNDINGBOX();

	// constructor from coordinates
	BOUNDINGBOX(float X0, float Y0, float Z0, float X1, float Y1, float Z1);

	// constructor from vectors
	BOUNDINGBOX(const vec3f& r0, const vec3f& r1);

	// is the bounding box valid
	bool IsValid() const { return bvalid; }

	// size of box
	float Width () const { return x1 - x0; }
	float Height() const { return y1 - y0; }
	float Depth () const { return z1 - z0; }

	// return largest dimension of box
	float GetMaxExtent() const ;

	// return the center of the box
	vec3f Center() const ;

	// return the radius of the box
	float Radius() const ;

	// range of box
	void Range(vec3f& n, float& min, float& max);
	void Range(vec3f& n, vec3f& r0, vec3f& r1);

	// add a point to the box
	void operator += (vec3f r);

	// inflate the box
	void InflateTo(float fx, float fy, float fz);
};
