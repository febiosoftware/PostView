#include "stdafx.h"
#include "StrainMapTool.h"
#include <QCheckBox>
#include <QPushButton>
#include <QBoxLayout>
#include <vector>
#include "Document.h"
#include <PostViewLib/FEStrainMap.h>
using namespace std;
using namespace Post;

class CStrainMapToolUI : public QWidget
{
public:
	QPushButton* pf1;
	QPushButton* pb1;
	QPushButton* pf2;
	QPushButton* pb2;
	FEStrainMap m_map;

public:
	CStrainMapToolUI(CStrainMapTool* ptool)
	{
		QPushButton* apply;
		QVBoxLayout* pv = new QVBoxLayout;
		{
			pf1 = new QPushButton("Assign to front surface 1");
			pb1 = new QPushButton("Assign to back surface 1");
			pf2 = new QPushButton("Assign to front surface 2");
			pb2 = new QPushButton("Assign to back surface 2");
			apply = new QPushButton("Apply");

			pv->addWidget(pf1);
			pv->addWidget(pb1);
			pv->addWidget(pf2);
			pv->addWidget(pb2);
			pv->addWidget(apply);
			pv->addStretch();
		}
		setLayout(pv);
		QObject::connect(pf1, SIGNAL(clicked(bool)), ptool, SLOT(OnAssignFront1()));
		QObject::connect(pb1, SIGNAL(clicked(bool)), ptool, SLOT(OnAssignBack1()));
		QObject::connect(pf2, SIGNAL(clicked(bool)), ptool, SLOT(OnAssignFront2()));
		QObject::connect(pb2, SIGNAL(clicked(bool)), ptool, SLOT(OnAssignBack2()));
		QObject::connect(apply, SIGNAL(clicked(bool)), ptool, SLOT(OnApply()));
	}
};

// constructor
CStrainMapTool::CStrainMapTool(CMainWindow* wnd) : CAbstractTool("Strain Map", wnd)
{
	ui = 0;
}

// get the property list
QWidget* CStrainMapTool::createUi()
{
	return ui = new CStrainMapToolUI(this);
}

void CStrainMapTool::OnAssignFront1()
{
	CDocument* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
		vector<int> sel;
		doc->GetGLModel()->GetSelectionList(sel, SELECT_FACES);
		ui->m_map.SetFrontSurface1(sel);
		int n = (int)sel.size();
		ui->pf1->setText(QString("Assign to front surface 1 (%1 faces)").arg(n));
	}
}

void CStrainMapTool::OnAssignBack1()
{
	CDocument* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
		vector<int> sel;
		doc->GetGLModel()->GetSelectionList(sel, SELECT_FACES);
		ui->m_map.SetBackSurface1(sel);
		int n = (int)sel.size();
		ui->pb1->setText(QString("Assign to back surface 1 (%1 faces)").arg(n));
	}
}

void CStrainMapTool::OnAssignFront2()
{
	CDocument* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
		vector<int> sel;
		doc->GetGLModel()->GetSelectionList(sel, SELECT_FACES);
		ui->m_map.SetFrontSurface2(sel);
		int n = (int)sel.size();
		ui->pf2->setText(QString("Assign to front surface 2 (%1 faces)").arg(n));
	}
}

void CStrainMapTool::OnAssignBack2()
{
	CDocument* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
		vector<int> sel;
		doc->GetGLModel()->GetSelectionList(sel, SELECT_FACES);
		ui->m_map.SetBackSurface2(sel);
		int n = (int)sel.size();
		ui->pb2->setText(QString("Assign to back surface 2 (%1 faces)").arg(n));
	}
}


void CStrainMapTool::OnApply()
{
	CDocument* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
		FEStrainMap& map = ui->m_map;
		map.Apply(*doc->GetFEModel());
		updateUi();
	}
}
