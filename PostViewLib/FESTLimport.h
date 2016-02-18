#pragma once
#include "FEFileReader.h"
#include <vector>
#include <list>
#include "math3d.h"
using namespace std;

class FESTLimport : public FEFileReader
{
	struct FACET
	{
		vec3f	r[3];
		int		n[3];
	};

public:
	FESTLimport(void);
	virtual ~FESTLimport(void);

	bool Load(FEModel& fem, const char* szfile);

protected:
	bool read_line(char* szline, const char* sz);

	void build_mesh();
	int find_node(vec3f& r, const double eps = 1e-12);

protected:
	FEModel*		m_pfem;
	list<FACET>		m_Face;
	vector<vec3f>	m_Node;
	int				m_nline;	// line counter
};
