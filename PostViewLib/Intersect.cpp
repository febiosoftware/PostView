#include "stdafx.h"
#include "Intersect.h"

//-----------------------------------------------------------------------------
// Find intersection of a ray with a triangle
bool IntersectTriangle(const Ray& ray, const Triangle& tri, Intersection& intersection)
{
	const double tol = 0.01;

	vec3f n1 = tri.r0;
	vec3f n2 = tri.r1;
	vec3f n3 = tri.r2;

	vec3f r0 = ray.origin;
	vec3f nn = ray.direction;

	// calculate the triangle normal
	vec3f fn = (n2 - n1)^(n3 - n1);
	fn.Normalize();

	// find the intersection of the point with the plane
	if (fn*nn == 0.f) return false;
	double l = fn*(n1 - r0) / (fn*nn);

	//	if (l <= 0) return false;
	vec3f q = r0 + nn*l;

	// find  the natural coordinates
	vec3f e1 = n2 - n1;
	vec3f e2 = n3 - n1;

	double A[2][2] = { { e1*e1, e1*e2 }, { e2*e1, e2*e2 } };
	double D = A[0][0] * A[1][1] - A[0][1] * A[1][0];
	double Ai[2][2];
	Ai[0][0] = (A[1][1]) / D;
	Ai[1][1] = (A[0][0]) / D;
	Ai[0][1] = -A[0][1] / D;
	Ai[1][0] = -A[1][0] / D;

	vec3f E1 = e1*Ai[0][0] + e2*Ai[0][1];
	vec3f E2 = e1*Ai[1][0] + e2*Ai[1][1];

	double r = (q - n1)*E1;
	double s = (q - n1)*E2;

	intersection.point = q;
	intersection.r[0] = r;
	intersection.r[1] = s;

	return ((r >= -tol) && (s >= -tol) && (r + s <= 1.0 + tol));
}

//-----------------------------------------------------------------------------
// Find intersection of a ray with a quad
bool IntersectQuad(const Ray& ray, const Quad& quad, Intersection& intersect)
{
	const double tol = 0.01;

	vec3f nr = ray.origin;
	vec3f nn = ray.direction;

	vec3f y[4];
	y[0] = quad.r0 - nr;
	y[1] = quad.r1 - nr;
	y[2] = quad.r2 - nr;
	y[3] = quad.r3 - nr;

	double r = 0, s = 0, l = 0, normu;
	int niter = 0;
	const int NMAX = 10;
	do
	{
		// evaluate shape functions
		double H[4], Hr[4], Hs[4];
		H[0] = 0.25*(1 - r)*(1 - s);
		H[1] = 0.25*(1 + r)*(1 - s);
		H[2] = 0.25*(1 + r)*(1 + s);
		H[3] = 0.25*(1 - r)*(1 + s);

		Hr[0] = -0.25*(1 - s);
		Hr[1] = 0.25*(1 - s);
		Hr[2] = 0.25*(1 + s);
		Hr[3] = -0.25*(1 + s);

		Hs[0] = -0.25*(1 - r);
		Hs[1] = -0.25*(1 + r);
		Hs[2] = 0.25*(1 + r);
		Hs[3] = 0.25*(1 - r);

		// evaluate residual
		vec3f R = nn*l - y[0] * H[0] - y[1] * H[1] - y[2] * H[2] - y[3] * H[3];

		mat3d K;
		K[0][0] = nn.x;
		K[0][1] = -y[0].x*Hr[0] - y[1].x*Hr[1] - y[2].x*Hr[2] - y[3].x*Hr[3];
		K[0][2] = -y[0].x*Hs[0] - y[1].x*Hs[1] - y[2].x*Hs[2] - y[3].x*Hs[3];

		K[1][0] = nn.y;
		K[1][1] = -y[0].y*Hr[0] - y[1].y*Hr[1] - y[2].y*Hr[2] - y[3].y*Hr[3];
		K[1][2] = -y[0].y*Hs[0] - y[1].y*Hs[1] - y[2].y*Hs[2] - y[3].y*Hs[3];

		K[2][0] = nn.z;
		K[2][1] = -y[0].z*Hr[0] - y[1].z*Hr[1] - y[2].z*Hr[2] - y[3].z*Hr[3];
		K[2][2] = -y[0].z*Hs[0] - y[1].z*Hs[1] - y[2].z*Hs[2] - y[3].z*Hs[3];

		K.Invert();

		vec3f du = K*R;
		l -= du.x;
		r -= du.y;
		s -= du.z;

		normu = du.y*du.y + du.z*du.z;
		niter++;
	} 
	while ((normu > 1e-6) && (niter < NMAX));

	intersect.point = nr + nn*l;
	intersect.r[0] = r;
	intersect.r[1] = s;

	return ((r + tol >= -1) && (r - tol <= 1) && (s + tol >= -1) && (s - tol <= 1));
}

//-----------------------------------------------------------------------------
bool FindFaceIntersection(const Ray& ray, const FEMeshBase& mesh, Intersection& q)
{
	vec3f rn[10];

	int faces = mesh.Faces();
	vec3f r, rmin;
	double gmin = 1e99;
	bool b = false;

	q.m_index = -1;
	Intersection tmp;
	for (int i=0; i<faces; ++i)
	{
		const FEFace& face = mesh.Face(i);
		if (face.IsVisible())
		{
			mesh.FaceNodePosition(face, rn);

			bool bfound = false;
			switch (face.m_ntype)
			{
			case FACE_TRI3:
			case FACE_TRI6:
			case FACE_TRI7:
			case FACE_TRI10:
			{
				Triangle tri = {rn[0], rn[1], rn[2]};
				bfound = IntersectTriangle(ray, tri, tmp);
			}
			break;
			case FACE_QUAD4:
			case FACE_QUAD8:
			case FACE_QUAD9:
			{
				Quad quad = { rn[0], rn[1], rn[2], rn[3] };
				bfound = IntersectQuad(ray, quad, tmp);
			}
			break;
			}

			if (bfound)
			{
				// signed distance
				float distance = ray.direction*(tmp.point - ray.origin);

				if ((distance > 0.f) && (distance < gmin))
				{
					gmin = distance;
					rmin = q.point;
					b = true;
					q.m_index = i;
					q.point = tmp.point;
					q.r[0] = tmp.r[0];
					q.r[1] = tmp.r[1];
				}
			}
		}
	}

	return b;
}

//-----------------------------------------------------------------------------
bool FindElementIntersection(const Ray& ray, const FEMeshBase& mesh, Intersection& q)
{
	vec3f rn[10];

	int elems = mesh.Elements();
	vec3f r, rmin;
	double gmin = 1e99;
	bool b = false;

	FEFace face;
	q.m_index = -1;
	Intersection tmp;
	for (int i = 0; i<elems; ++i)
	{
		const FEElement& elem = mesh.Element(i);
		if (elem.IsVisible())
		{
			// solid elements
			int NF = elem.Faces();
			for (int j = 0; j<NF; ++j)
			{
				bool bfound = false;
				elem.GetFace(j, face);
				switch (face.m_ntype)
				{
				case FACE_QUAD4:
				case FACE_QUAD8:
				case FACE_QUAD9:
				{
					rn[0] = mesh.Node(face.node[0]).m_rt;
					rn[1] = mesh.Node(face.node[1]).m_rt;
					rn[2] = mesh.Node(face.node[2]).m_rt;
					rn[3] = mesh.Node(face.node[3]).m_rt;

					Quad quad = { rn[0], rn[1], rn[2], rn[3] };
					bfound = IntersectQuad(ray, quad, tmp);
				}
				break;
				case FACE_TRI3:
				case FACE_TRI6:
				case FACE_TRI7:
				case FACE_TRI10:
				{
					rn[0] = mesh.Node(face.node[0]).m_rt;
					rn[1] = mesh.Node(face.node[1]).m_rt;
					rn[2] = mesh.Node(face.node[2]).m_rt;

					Triangle tri = { rn[0], rn[1], rn[2] };
					bfound = IntersectTriangle(ray, tri, tmp);
				}
				break;
				default:
					assert(false);
				}

				if (bfound)
				{
					// signed distance
					float distance = ray.direction*(tmp.point - ray.origin);

					if ((distance > 0.f) && (distance < gmin))
					{
						gmin = distance;
						rmin = q.point;
						b = true;
						q.m_index = i;
						q.point = tmp.point;
						q.r[0] = tmp.r[0];
						q.r[1] = tmp.r[1];
					}
				}
			}

			// shell elements
			int NE = elem.Edges();
			if (NE > 0)
			{
				bool bfound = false;
				if (elem.Nodes() == 4)
				{
					rn[0] = mesh.Node(elem.m_node[0]).m_rt;
					rn[1] = mesh.Node(elem.m_node[1]).m_rt;
					rn[2] = mesh.Node(elem.m_node[2]).m_rt;
					rn[3] = mesh.Node(elem.m_node[3]).m_rt;

					Quad quad = { rn[0], rn[1], rn[2], rn[3] };
					bfound = IntersectQuad(ray, quad, tmp);
				}
				else
				{
					rn[0] = mesh.Node(elem.m_node[0]).m_rt;
					rn[1] = mesh.Node(elem.m_node[1]).m_rt;
					rn[2] = mesh.Node(elem.m_node[2]).m_rt;

					Triangle tri = { rn[0], rn[1], rn[2] };
					bfound = IntersectTriangle(ray, tri, tmp);
				}

				if (bfound)
				{
					// signed distance
					float distance = ray.direction*(tmp.point - ray.origin);

					if ((distance > 0.f) && (distance < gmin))
					{
						gmin = distance;
						rmin = q.point;
						b = true;
						q.m_index = i;
						q.point = tmp.point;
						q.r[0] = tmp.r[0];
						q.r[1] = tmp.r[1];
					}
				}
			}
		}
	}

	return b;
}
