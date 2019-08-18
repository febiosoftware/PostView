#include "stdafx.h"
#include "MeasureVolumeTool.h"
#include "Document.h"
#include <PostLib/FEModel.h>
using namespace Post;

//-----------------------------------------------------------------------------
CMeasureVolumeTool::Props::Props(CMeasureVolumeTool* ptool) : m_ptool(ptool)
{
	addProperty("selected faces", CProperty::Int)->setFlags(CProperty::Visible);
	addProperty("volume", CProperty::Float)->setFlags(CProperty::Visible);
	addProperty("symmetry", CProperty::Enum)->setEnumValues(QStringList() << "(None)" << "X" << "Y" << "Z");
}

//-----------------------------------------------------------------------------
QVariant CMeasureVolumeTool::Props::GetPropertyValue(int i)
{
	switch (i)
	{
	case 0: return m_ptool->m_nsel; break;
	case 1: return m_ptool->m_vol; break;
	case 2: return m_ptool->m_nformula; break;
	}
	return QVariant();
}

//-----------------------------------------------------------------------------
void CMeasureVolumeTool::Props::SetPropertyValue(int i, const QVariant& v)
{
	if (i == 2) m_ptool->m_nformula = v.toInt();
}

//-----------------------------------------------------------------------------
CMeasureVolumeTool::CMeasureVolumeTool(CMainWindow* wnd) : CBasicTool("Measure Volume", wnd, CBasicTool::HAS_APPLY_BUTTON)
{
	m_nsel = 0;
	m_vol = 0.0;
	m_nformula = 0;
}

//-----------------------------------------------------------------------------
CPropertyList* CMeasureVolumeTool::getPropertyList()
{
	return new Props(this);
}

//-----------------------------------------------------------------------------
void CMeasureVolumeTool::OnApply()
{
	m_nsel = 0;
	m_vol = 0.0;
	CDocument* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
		FEModel& fem = *doc->GetFEModel();
		int ntime = fem.currentTime();
		FEMeshBase& mesh = *fem.GetFEMesh(0);
		const vector<FEFace*> selectedFaces = doc->GetGLModel()->GetFaceSelection();
		int N = (int)selectedFaces.size();
		for (int i = 0; i<N; ++i)
		{
			FEFace& f = *selectedFaces[i];

			// get the average position, area and normal
			vec3f r = mesh.FaceCenter(f);
			float area = mesh.FaceArea(f);
			vec3f N = f.m_fn;

			switch (m_nformula)
			{
			case 0: m_vol += area*(N*r) / 3.f; break;
			case 1: m_vol += 2.f*area*(r.x*N.x); break;
			case 2: m_vol += 2.f*area*(r.y*N.y); break;
			case 3: m_vol += 2.f*area*(r.z*N.z); break;
			}
			++m_nsel;
		}

		m_vol = fabs(m_vol);
	}
	updateUi();
}
