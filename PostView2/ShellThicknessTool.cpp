#include "StatePanel.h"
#include "ShellThicknessTool.h"
#include "Document.h"
#include <PostViewLib/FEModel.h>
using namespace Post;

//-----------------------------------------------------------------------------
CShellThicknessTool::Props::Props(CShellThicknessTool* ptool) : m_ptool(ptool)
{
	addProperty("shell thickness", CProperty::Float);
}

//-----------------------------------------------------------------------------
QVariant CShellThicknessTool::Props::GetPropertyValue(int i)
{
	switch (i)
	{
	case 0: return m_ptool->m_h; break;
	}
	return QVariant();
}

//-----------------------------------------------------------------------------
void CShellThicknessTool::Props::SetPropertyValue(int i, const QVariant& v)
{
	if (i==0) m_ptool->m_h = v.toDouble();
}

//-----------------------------------------------------------------------------
CShellThicknessTool::CShellThicknessTool(CMainWindow* wnd) : CBasicTool("Shell Thickness", wnd, CBasicTool::HAS_APPLY_BUTTON)
{
	m_h = 0.0;
}

//-----------------------------------------------------------------------------
CPropertyList* CShellThicknessTool::getPropertyList()
{ 
	return new Props(this); 
}

//-----------------------------------------------------------------------------
void CShellThicknessTool::OnApply()
{
	CDocument* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
		FEModel& fem = *doc->GetFEModel();
		FEMeshBase& mesh = *fem.GetFEMesh(0);

		int NS = fem.GetStates();
		for (int n=0; n<NS; ++n)
		{
			FEState& state = *fem.GetState(n);
			for (int i=0; i<mesh.Elements(); ++i)
			{
				FEElement& elem = mesh.Element(i);
				if (elem.IsSelected() && elem.IsShell())
				{
					int ne = elem.Nodes();
					for (int k=0; k<ne; ++k) state.m_ELEM[i].m_h[k] = m_h;
				}
			}
		}
		doc->UpdateFEModel(true);
	}
	updateUi();
}
