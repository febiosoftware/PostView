#include "StatePanel.h"
#include "ShellThicknessTool.h"
#include "Document.h"
#include <PostViewLib/FEModel.h>

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
CShellThicknessTool::CShellThicknessTool() : CBasicTool("Shell Thickness", CBasicTool::HAS_APPLY_BUTTON)
{
	m_doc = 0;
	m_h = 0.0;
}

//-----------------------------------------------------------------------------
CPropertyList* CShellThicknessTool::getPropertyList()
{ 
	return new Props(this); 
}

//-----------------------------------------------------------------------------
void CShellThicknessTool::activate(CDocument* pdoc)
{
	m_doc = pdoc;
}

//-----------------------------------------------------------------------------
void CShellThicknessTool::deactivate()
{
}

//-----------------------------------------------------------------------------
void CShellThicknessTool::OnApply()
{
	if (m_doc && m_doc->IsValid())
	{
		FEModel& fem = *m_doc->GetFEModel();
		FEMesh& mesh = *fem.GetMesh();

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
		m_doc->UpdateFEModel(true);
	}
	updateUi();
}
