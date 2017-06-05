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

		const char* szfile = fileName.c_str();
		const char* szext = strrchr(szfile, '.');
		if (strcmp(szext, ".ang2") == 0)
		{
			// Read AngioFE2 format
			if (ReadAng2Format(szfile) == false)
			{
				QMessageBox::critical(0, "PostView2", "Failed reading ang2 file.");
				return;
			}
		}
		else
		{
			// read old format (this assumes this is a text file)
			if (ReadOldFormat(szfile) == false)
			{
				QMessageBox::critical(0, "PostView2", "Failed reading line data file.");
				return;
			}
		}

		// add a line plot for visualizing the line data
		CGLLinePlot* pgl = new CGLLinePlot(m_doc->GetGLModel());
		m_doc->AddPlot(pgl);
		pgl->SetName(fileName);

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

bool CImportLinesTool::ReadOldFormat(const char* szfile)
{
	FILE* fp = fopen(szfile, "rt");
	if (fp == 0) return false;

	FEModel& fem = *m_doc->GetFEModel();

	char szline[256] = { 0 };
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

	return true;
}

bool CImportLinesTool::ReadAng2Format(const char* szfile)
{
	FILE* fp = fopen(szfile, "rb");
	if (fp == 0) return false;

	FEModel& fem = *m_doc->GetFEModel();

	// read the magic number
	unsigned int magic = 0;
	if (fread(&magic, sizeof(unsigned int), 1, fp) != 1) { fclose(fp); return false; };
	if (magic != 0xfdb97531) { fclose(fp); return false; }

	// read the version number
	unsigned int version = 0;
	if (fread(&version, sizeof(unsigned int), 1, fp) != 1) { fclose(fp); return false; }
	if (version != 0) { fclose(fp); return false; }

	int nstate = 0;
	while (!feof(fp) && !ferror(fp))
	{
		if (nstate >= fem.GetStates()) break;
		FEState& s = *fem.GetState(nstate);

		// this file format only stores incremental changes to the network
		// so we need to copy all the data from the previous state as well
		if (nstate > 0)
		{
			// get previous state
			FEState& sp = *fem.GetState(nstate - 1);

			// copy line data
			int nlines = sp.Lines();
			for (int i=0; i<nlines; ++i)
			{
				LINEDATA& lineData = sp.Line(i);
				s.AddLine(lineData.m_r0, lineData.m_r1);
			}
		}

		// read number of segments 
		unsigned int segs = 0;
		if (fread(&segs, sizeof(unsigned int), 1, fp) != 1) { fclose(fp); return false; }

		// read time stamp (is not used right now)
		float ftime = 0.0f;
		if (fread(&ftime, sizeof(float), 1, fp) != 1) { fclose(fp); return false; }

		// read the segments
		for (int i=0; i<segs; ++i)
		{
			float d[6] = {0.0f};
			if (fread(d, sizeof(float), 6, fp) != 6) { fclose(fp); return false; }

			// the coordinates are in reference frame so convert them to global coordinates
			vec3f r0 = fem.NodePosition(vec3f(d[0], d[1], d[2]), nstate);
			vec3f r1 = fem.NodePosition(vec3f(d[3], d[4], d[5]), nstate);

			// add the line data
			s.AddLine(r0, r1);
		}

		// next state
		nstate++;
	}

	fclose(fp);

	return true;
}


//=============================================================================

class CImportPointsToolUI : public QWidget
{
public:
	QLineEdit*	name;
	QLineEdit*	fileName;
	int			m_ncount;

public:
	CImportPointsToolUI(CImportPointsTool* ptool)
	{
		m_ncount = 1;

		QPushButton* apply;
		QPushButton* browse;
		QVBoxLayout* pv = new QVBoxLayout;
		{
			QGridLayout* pgrid = new QGridLayout;
			name = new QLineEdit(QString("Points%1").arg(m_ncount));
			QLabel* pl = new QLabel("Name:"); pl->setBuddy(name);
			pgrid->addWidget(pl, 0, 0);
			pgrid->addWidget(name, 0, 1);

			fileName = new QLineEdit;
			pl = new QLabel("File:"); pl->setBuddy(fileName);
			browse = new QPushButton("..."); browse->setFixedWidth(30);
			pgrid->addWidget(pl, 1, 0);
			pgrid->addWidget(fileName, 1, 1);
			pgrid->addWidget(browse, 1, 2);
			
			pv->addLayout(pgrid);

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
CImportPointsTool::CImportPointsTool() : CAbstractTool("Import points")
{
	m_doc = 0;
	ui = 0;
}

// get the property list
QWidget* CImportPointsTool::createUi()
{
	return ui = new CImportPointsToolUI(this);
}

// activate the tool
void CImportPointsTool::activate(CDocument* pdoc)
{
	m_doc = pdoc;
}

// deactive the tool
void CImportPointsTool::deactivate()
{
	m_doc = 0;
}

void CImportPointsTool::OnBrowse()
{
	QString filename = QFileDialog::getOpenFileName(0, "Open file", 0, "All files(*)");
	if (filename.isEmpty() == false)
	{
		ui->fileName->setText(filename);
	}
}

void CImportPointsTool::OnApply()
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
				int nstate, id;
				float x, y, z;
				int n = sscanf(szline, "%d%d%g%g%g", &nstate, &id, &x, &y, &z);
				if (n == 5)
				{
					if ((nstate >= 0) && (nstate < fem.GetStates()))
					{
						FEState& s = *fem.GetState(nstate);
						s.AddPoint(vec3f(x, y, z), id);
					}
				}
			}
		}
		fclose(fp);

		// add a line plot
		CGLPointPlot* pgl = new CGLPointPlot(m_doc->GetGLModel());
		m_doc->AddPlot(pgl);
		pgl->SetName(name.c_str());

		ui->m_ncount++;
		ui->name->setText(QString("Points%1").arg(ui->m_ncount));

		updateUi();
	}
}
