#include "stdafx.h"
#include "DistanceMapTool.h"
#include <QWidget>
#include <QBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QFormLayout>
#include <QLineEdit>
#include "Document.h"
#include <PostViewLib/FEModel.h>
#include <PostViewLib/SphereFit.h>
#include "PropertyListView.h"
#include <PostViewLib/FEPointCongruency.h>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include "GLLinePlot.h"
#include <PostViewLib/FEDistanceMap.h>
#include <PostViewLib/FECurvatureMap.h>

class CDistanceMapToolUI : public QWidget
{
public:
	QCheckBox*	check;
	QPushButton* p1;
	QPushButton* p2;
	FEDistanceMap	m_map;

public:
	CDistanceMapToolUI(CDistanceMapTool* ptool)
	{
		QPushButton* apply;
		QVBoxLayout* pv = new QVBoxLayout;
		{
			p1 = new QPushButton("Assign to surface 1");
			p2 = new QPushButton("Assign to surface 2");
			check = new QCheckBox("Signed distance");
			apply = new QPushButton("Apply");

			pv->addWidget(p1);
			pv->addWidget(p2);
			pv->addWidget(check);
			pv->addWidget(apply);
			pv->addStretch();
		}
		setLayout(pv);
		QObject::connect(p1, SIGNAL(clicked(bool)), ptool, SLOT(OnAssign1()));
		QObject::connect(p2, SIGNAL(clicked(bool)), ptool, SLOT(OnAssign2()));
		QObject::connect(apply, SIGNAL(clicked(bool)), ptool, SLOT(OnApply()));
	}
};

// constructor
CDistanceMapTool::CDistanceMapTool() : CAbstractTool("Distance Map")
{
	m_doc = 0;
	ui = 0;
}

// get the property list
QWidget* CDistanceMapTool::createUi()
{
	return ui = new CDistanceMapToolUI(this);
}

// activate the tool
void CDistanceMapTool::activate(CDocument* pdoc)
{
	m_doc = pdoc;
}

// deactive the tool
void CDistanceMapTool::deactivate()
{
	m_doc = 0;
}

void CDistanceMapTool::OnAssign1()
{
	vector<int> sel;
	m_doc->GetSelectionList(sel, SELECT_FACES);
	ui->m_map.SetSelection1(sel);
	int n = (int) sel.size();
	ui->p1->setText(QString("Assign to surface 1 (%1 faces)").arg(n));
}

void CDistanceMapTool::OnAssign2()
{
	vector<int> sel;
	m_doc->GetSelectionList(sel, SELECT_FACES);
	ui->m_map.SetSelection2(sel);
	int n = (int) sel.size();
	ui->p2->setText(QString("Assign to surface 2 (%1 faces)").arg(n));
}

void CDistanceMapTool::OnApply()
{
	if (m_doc && m_doc->IsValid())
	{
		bool bcheck = ui->check->isChecked();
		FEDistanceMap& map = ui->m_map;
		map.m_bsigned = bcheck;
		map.Apply(*m_doc->GetFEModel());
		updateUi();
	}
}


class CCurvatureMapToolUI : public QWidget
{
public:
	QPushButton* p1;
	QPushButton* p2;
	FECongruencyMap	m_map;

public:
	CCurvatureMapToolUI(CCurvatureMapTool* ptool)
	{
		QPushButton* apply;
		QVBoxLayout* pv = new QVBoxLayout;
		{
			p1 = new QPushButton("Assign to surface 1");
			p2 = new QPushButton("Assign to surface 2");
			apply = new QPushButton("Apply");

			pv->addWidget(p1);
			pv->addWidget(p2);
			pv->addWidget(apply);
			pv->addStretch();
		}
		setLayout(pv);
		QObject::connect(p1, SIGNAL(clicked(bool)), ptool, SLOT(OnAssign1()));
		QObject::connect(p2, SIGNAL(clicked(bool)), ptool, SLOT(OnAssign2()));
		QObject::connect(apply, SIGNAL(clicked(bool)), ptool, SLOT(OnApply()));
	}
};

// constructor
CCurvatureMapTool::CCurvatureMapTool() : CAbstractTool("Curvature Map")
{
	m_doc = 0;
	ui = 0;
}

// get the property list
QWidget* CCurvatureMapTool::createUi()
{
	return ui = new CCurvatureMapToolUI(this);
}

// activate the tool
void CCurvatureMapTool::activate(CDocument* pdoc)
{
	m_doc = pdoc;
}

// deactive the tool
void CCurvatureMapTool::deactivate()
{
	m_doc = 0;
}

void CCurvatureMapTool::OnAssign1()
{
	vector<int> sel;
	m_doc->GetSelectionList(sel, SELECT_FACES);
	ui->m_map.SetSelection1(sel);
	int n = (int) sel.size();
	ui->p1->setText(QString("Assign to surface 1 (%1 faces)").arg(n));
}

void CCurvatureMapTool::OnAssign2()
{
	vector<int> sel;
	m_doc->GetSelectionList(sel, SELECT_FACES);
	ui->m_map.SetSelection2(sel);
	int n = (int) sel.size();
	ui->p2->setText(QString("Assign to surface 2 (%1 faces)").arg(n));
}

void CCurvatureMapTool::OnApply()
{
	if (m_doc && m_doc->IsValid())
	{
		FECongruencyMap& map = ui->m_map;
		map.Apply(*m_doc->GetFEModel());
		updateUi();
	}
}
