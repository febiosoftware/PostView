#include "stdafx.h"
#include "ImportLinesTool.h"
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

class CImportLinesToolUI : public QWidget
{
public:
	QLineEdit*	name;
	QLineEdit*	fileName;
	int			m_ncount;

public:
	CImportLinesToolUI(CImportLinesTool* ptool)
	{
		m_ncount = 1;

		QPushButton* apply;
		QPushButton* browse;
		QVBoxLayout* pv = new QVBoxLayout;
		{
			QHBoxLayout* ph = new QHBoxLayout;
			name = new QLineEdit(QString("Name%1").arg(m_ncount));
			QLabel* pl = new QLabel("Name"); pl->setBuddy(name);
			ph->addWidget(pl);
			ph->addWidget(name);
			pv->addLayout(ph);

			ph = new QHBoxLayout;
			fileName = new QLineEdit;
			browse = new QPushButton("Browse...");
			ph->addWidget(fileName);
			ph->addWidget(browse);
			pv->addLayout(ph);

			apply = new QPushButton("Apply");
			pv->addWidget(apply);
			pv->addStretch();
		}
		setLayout(pv);
		QObject::connect(apply , SIGNAL(clicked(bool)), ptool, SLOT(OnApply()));
		QObject::connect(browse, SIGNAL(clicked(bool)), ptool, SLOT(OnBrowse()));
	}
};

// constructor
CImportLinesTool::CImportLinesTool() : CAbstractTool("Import lines")
{
	m_doc = 0;
	ui = 0;
}

// get the property list
QWidget* CImportLinesTool::createUi()
{
	return ui = new CImportLinesToolUI(this);
}

// activate the tool
void CImportLinesTool::activate(CDocument* pdoc)
{
	m_doc = pdoc;
}

// deactive the tool
void CImportLinesTool::deactivate()
{
	m_doc = 0;
}

void CImportLinesTool::OnApply()
{
	if (m_doc && m_doc->IsValid())
	{
		string fileName = ui->fileName->text().toStdString();
		string name = ui->name->text().toStdString();

		FILE* fp = fopen(fileName.c_str(), "rt");
		if (fp == 0)
		{
			QMessageBox::critical(0, "PostView2", "Failed opening data file.");
			return;
		}

		FEModel& fem = *m_doc->GetFEModel();

		char szline[256] = {0};
		while (!feof(fp))
		{
			if (fgets(szline, 255, fp))
			{
				int nstate;
				float x0, y0, z0, x1, y1, z1;
				int n = sscanf(szline, "%d%*g%g%g%g%g%g%g", &nstate, &x0, &y0, &z0, &x1, &y1, &z1);
				if (n == 7)
				{
					if ((nstate >= 0) && (nstate < fem.GetStates()))
					{
						FEState& s = *fem.GetState(nstate);
						s.AddLine(vec3f(x0, y0, z0), vec3f(x1, y1, z1));
					}
				}
			}
		}

		fclose(fp);

		// add a line plot
		CGLLinePlot* pgl = new CGLLinePlot(m_doc->GetGLModel());
		m_doc->AddPlot(pgl);
		pgl->SetName(name.c_str());

		ui->m_ncount++;
		ui->name->setText(QString("Lines%1").arg(ui->m_ncount));

		updateUi();
	}
}

void CImportLinesTool::OnBrowse()
{
	QString filename = QFileDialog::getOpenFileName(0, "Open file", 0, "All files(*)");
	if (filename.isEmpty() == false)
	{
		ui->fileName->setText(filename);
	}
}
