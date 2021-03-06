/*This file is part of the PostView source code and is licensed under the MIT license
listed below.

See Copyright-PostView.txt for details.

Copyright (c) 2020 University of Utah, The Trustees of Columbia University in 
the City of New York, and others.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include "stdafx.h"
#include "PointCongruencyTool.h"
#include <QWidget>
#include <QBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QFormLayout>
#include <QLineEdit>
#include "Document.h"
#include <PostLib/FEPostModel.h>
#include <MeshTools/SphereFit.h>
#include "PropertyListView.h"
#include <PostLib/FEPointCongruency.h>
using namespace Post;

class CPointCongruencyToolUI : public QWidget
{
public:
	CIntInput*		node;
	CFloatInput*	smooth;
	CIntInput*		face;
	CFloatInput*	H1;
	CFloatInput*	G1;
	CFloatInput*	H2;
	CFloatInput*	G2;
	CFloatInput*	alpha;
	CFloatInput*	delta;
	CFloatInput*	Ke_min;
	CFloatInput*	Ke_max;
	CFloatInput*	cong;

public:
	CPointCongruencyToolUI(CPointCongruencyTool* ptool)
	{
		QPushButton* pb;
		QVBoxLayout* pv = new QVBoxLayout;
		{
			QFormLayout* pfi = new QFormLayout;
			pfi->addRow("Node:", node = new CIntInput);
			pfi->addRow("Smoothness:", smooth = new CFloatInput);
				
			pb = new QPushButton("Apply"); 

			QFormLayout* pfo = new QFormLayout;
			pfo->addRow("face" , face = new CIntInput  ); face ->setReadOnly(true);
			pfo->addRow("H1"   , H1   = new CFloatInput); H1   ->setReadOnly(true);
			pfo->addRow("G1"   , G1   = new CFloatInput); G1   ->setReadOnly(true);
			pfo->addRow("H2"   , H2   = new CFloatInput); H2   ->setReadOnly(true);
			pfo->addRow("G2"   , G2   = new CFloatInput); G2   ->setReadOnly(true);
			pfo->addRow("alpha" , alpha = new CFloatInput); alpha ->setReadOnly(true);
			pfo->addRow("delta" , delta = new CFloatInput); delta ->setReadOnly(true);
			pfo->addRow("Ke_min", Ke_min= new CFloatInput); Ke_min->setReadOnly(true);
			pfo->addRow("Ke_max", Ke_max= new CFloatInput); Ke_max->setReadOnly(true);
			pfo->addRow("Congruency", cong = new CFloatInput); cong->setReadOnly(true);

			pv->addLayout(pfi);
			pv->addWidget(pb);
			pv->addLayout(pfo);
		}
		setLayout(pv);
		QObject::connect(pb, SIGNAL(clicked(bool)), ptool, SLOT(OnApply()));
	}
};

// constructor
CPointCongruencyTool::CPointCongruencyTool(CMainWindow* wnd) : CAbstractTool("Pt. Congruency", wnd)
{
	ui = 0;
}

// get the property list
QWidget* CPointCongruencyTool::createUi()
{
	return ui = new CPointCongruencyToolUI(this);
}

void CPointCongruencyTool::OnApply()
{
	// get the nodal coordinates (surface only)
	CDocument* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
		int node = ui->node->value() - 1;
		FEPostModel& fem = *doc->GetFEModel();
		Post::FEPostMesh& mesh = *fem.GetFEMesh(0);
		if ((node >= 0)&&(node<mesh.Nodes()))
		{
			FEPointCongruency tool;
			tool.SetLevels(1);
			FEPointCongruency::CONGRUENCY_DATA d = tool.Congruency(&mesh, node);

			ui->face->setValue(d.nface + 1);
			ui->H1->setValue(d.H1);
			ui->G1->setValue(d.G1);
			ui->H2->setValue(d.H2);
			ui->G2->setValue(d.G2);
			ui->alpha->setValue(180*d.a/PI);
			ui->delta->setValue(d.D);
			ui->Ke_min->setValue(d.Kemin);
			ui->Ke_max->setValue(d.Kemax);
			ui->cong->setValue(d.Ke);
		}
	}
}
