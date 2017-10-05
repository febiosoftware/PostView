#include "stdafx.h"
#include "SphereFitTool.h"
#include <QWidget>
#include <QBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QFormLayout>
#include <QLineEdit>
#include "Document.h"
#include <PostViewLib/FEModel.h>
#include <PostViewLib/SphereFit.h>

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
CSphereFitTool::CSphereFitTool(CDocument* doc) : CAbstractTool("Sphere Fit", doc)
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
	if (m_doc && m_doc->IsValid())
	{
		bool bsel = ui->pc->isChecked();

		FEModel& fem = *m_doc->GetFEModel();
		FEMeshBase& mesh = *fem.GetFEMesh(0);
		int N = mesh.Nodes();
		int F = mesh.Faces();
		for (int i=0; i<N; ++i) mesh.Node(i).m_ntag = 0;
		for (int i=0; i<F; ++i)
		{
			FEFace& f = mesh.Face(i);
			if ((bsel == false) || (f.IsSelected()))
			{
				int nf = f.Nodes();
				for (int j=0; j<nf; ++j) mesh.Node(f.node[j]).m_ntag = 1;
			}
		}

		vector<vec3f> y;
		for (int i=0; i<N; ++i)
		{
			if (mesh.Node(i).m_ntag == 1) y.push_back(mesh.Node(i).m_rt);
		}

		// find the best fit sphere
		SphereFit fit;
		fit.Fit(y, 50);
		vec3f sc = fit.m_rc;
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
