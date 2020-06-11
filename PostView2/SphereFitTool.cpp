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
#include "SphereFitTool.h"
#include <QWidget>
#include <QBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QFormLayout>
#include <QLineEdit>
#include "Document.h"
#include <PostLib/FEPostModel.h>
#include <MeshTools/SphereFit.h>
using namespace Post;

class CSphereFitToolUI : public QWidget
{
public:
	QCheckBox*	pc;
	QLineEdit*	x;
	QLineEdit*	y;
	QLineEdit*	z;
	QLineEdit*	R;
	QLineEdit*	O;

public:
	CSphereFitToolUI(CSphereFitTool* ptool)
	{
		QPushButton* pb;
		QVBoxLayout* pv = new QVBoxLayout;
		{
			pc = new QCheckBox("Selection only");
			pb = new QPushButton("Fit");

			QFormLayout* pf = new QFormLayout;
			pf->addRow("x", x = new QLineEdit); x->setReadOnly(true);
			pf->addRow("y", y = new QLineEdit); y->setReadOnly(true);
			pf->addRow("z", z = new QLineEdit); z->setReadOnly(true);
			pf->addRow("R", R = new QLineEdit); R->setReadOnly(true);
			pf->addRow("Obj.", O = new QLineEdit); O->setReadOnly(true);

			pv->addWidget(pc);
			pv->addWidget(pb);
			pv->addLayout(pf);
		}
		setLayout(pv);
		QObject::connect(pb, SIGNAL(clicked(bool)), ptool, SLOT(OnFit()));
	}
};

// constructor
CSphereFitTool::CSphereFitTool(CMainWindow* wnd) : CAbstractTool("Sphere Fit", wnd)
{
	ui = 0;
}

// get the property list
QWidget* CSphereFitTool::createUi()
{
	return ui = new CSphereFitToolUI(this);
}

void CSphereFitTool::OnFit()
{
	// get the nodal coordinates (surface only)
	CDocument* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
		bool bsel = ui->pc->isChecked();

		FEPostModel& fem = *doc->GetFEModel();
		Post::FEPostMesh& mesh = *fem.GetFEMesh(0);
		int N = mesh.Nodes();
		int F = mesh.Faces();
		for (int i=0; i<N; ++i) mesh.Node(i).m_ntag = 0;
		for (int i=0; i<F; ++i)
		{
			FEFace& f = mesh.Face(i);
			if ((bsel == false) || (f.IsSelected()))
			{
				int nf = f.Nodes();
				for (int j=0; j<nf; ++j) mesh.Node(f.n[j]).m_ntag = 1;
			}
		}

		vector<vec3d> y;
		for (int i=0; i<N; ++i)
		{
			if (mesh.Node(i).m_ntag == 1)
			{
				vec3d r = mesh.Node(i).r;
				y.push_back(r);
			}
		}

		// find the best fit sphere
		SphereFit fit;
		fit.Fit(y, 50);
		vec3d sc = fit.m_rc;
		double R = fit.m_R;

		// calculate the objective function
		double objs = fit.ObjFunc(y);

		// update GUI
		ui->x->setText(QString("%1").arg(sc.x));
		ui->y->setText(QString("%1").arg(sc.y));
		ui->z->setText(QString("%1").arg(sc.z));
		ui->R->setText(QString("%1").arg(R));
		ui->O->setText(QString("%1").arg(objs));
	}
}
