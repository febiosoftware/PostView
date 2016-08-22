#include "stdafx.h"
#include "AddImage3DTool.h"
#include <QPushButton>
#include <QBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include "CIntInput.h"
#include "Document.h"
#include <PostViewLib/3DImage.h>
#include "MainWindow.h"

class CAddImage3DToolUI : public QWidget
{
public:
	CIntInput *pnx, *pny, *pnz;
	CFloatInput *pxmin, *pymin, *pzmin;
	CFloatInput *pxmax, *pymax, *pzmax;
	QLineEdit* pfile;

public:
	CAddImage3DToolUI(QObject* parent)
	{
		QVBoxLayout* pv = new QVBoxLayout;

		QHBoxLayout* phbox = new QHBoxLayout;
		QLabel* pl = new QLabel("File:");
		pfile = new QLineEdit;
		QPushButton* browse = new QPushButton("..."); browse->setFixedWidth(30);
		pl->setBuddy(browse);
		phbox->addWidget(pl);
		phbox->addWidget(pfile);
		phbox->addWidget(browse);
		pv->addLayout(phbox);

		QGridLayout* grid = new QGridLayout;
		grid->addWidget(pl = new QLabel("X-pixels"), 0, 0); grid->addWidget(pnx = new CIntInput, 0, 1); pl->setBuddy(pnx); pnx->setFixedWidth(70);
		grid->addWidget(pl = new QLabel("Y-pixels"), 1, 0); grid->addWidget(pny = new CIntInput, 1, 1); pl->setBuddy(pny); pny->setFixedWidth(70);
		grid->addWidget(pl = new QLabel("Z-pixels"), 2, 0); grid->addWidget(pnz = new CIntInput, 2, 1); pl->setBuddy(pnz); pnz->setFixedWidth(70);

		grid->addWidget(pl = new QLabel("x-min"), 3, 0); grid->addWidget(pxmin = new CFloatInput, 3, 1); pl->setBuddy(pxmin); pxmin->setFixedWidth(70);
		grid->addWidget(pl = new QLabel("y-min"), 4, 0); grid->addWidget(pymin = new CFloatInput, 4, 1); pl->setBuddy(pymin); pymin->setFixedWidth(70);
		grid->addWidget(pl = new QLabel("z-min"), 5, 0); grid->addWidget(pzmin = new CFloatInput, 5, 1); pl->setBuddy(pzmin); pzmin->setFixedWidth(70);

		grid->addWidget(pl = new QLabel("x-max"), 3, 2); grid->addWidget(pxmax = new CFloatInput, 3, 3); pl->setBuddy(pxmax); pxmax->setFixedWidth(70);
		grid->addWidget(pl = new QLabel("y-max"), 4, 2); grid->addWidget(pymax = new CFloatInput, 4, 3); pl->setBuddy(pymax); pymax->setFixedWidth(70);
		grid->addWidget(pl = new QLabel("z-max"), 5, 2); grid->addWidget(pzmax = new CFloatInput, 5, 3); pl->setBuddy(pzmax); pzmax->setFixedWidth(70);

		pv->addLayout(grid);

		QPushButton* apply = new QPushButton("Load");
		pv->addWidget(apply);
		pv->addStretch();

		setLayout(pv);

		QObject::connect(browse, SIGNAL(clicked()), parent, SLOT(OnBrowse()));
		QObject::connect(apply , SIGNAL(clicked()), parent, SLOT(OnApply ()));
	}
};

CAddImage3DTool::CAddImage3DTool(): CAbstractTool("Add 3D image")
{
	m_doc = 0;
	ui = 0;
};

// get the property list
QWidget* CAddImage3DTool::createUi()
{
	return ui = new CAddImage3DToolUI(this);
}

// activate the tool
void CAddImage3DTool::activate(CDocument* pdoc)
{
	m_doc = pdoc;
}

// deactive the tool
void CAddImage3DTool::deactivate()
{
	m_doc = 0;
}

void CAddImage3DTool::OnBrowse()
{
	QString filename = QFileDialog::getOpenFileName(0, "Open file", 0, "All files(*)");
	if (filename.isEmpty() == false)
	{
		ui->pfile->setText(filename);
	}
}

void CAddImage3DTool::OnApply()
{
	if (m_doc && m_doc->IsValid())
	{
		int nx = ui->pnx->value();
		int ny = ui->pny->value();
		int nz = ui->pnz->value();

		if ((nx <= 0) || (ny <= 0) || (nz <= 0))
		{
			QMessageBox::critical(0, "Add 3D Image tool", "Invalid image dimensions");
			return;
		}

		double xmin = ui->pxmin->value();
		double ymin = ui->pymin->value();
		double zmin = ui->pzmin->value();

		double xmax = ui->pxmax->value();
		double ymax = ui->pymax->value();
		double zmax = ui->pzmax->value();

		double w = xmax - xmin;
		double h = ymax - ymin;
		double d = zmax - zmin;

		if ((w <= 0.0) || (h <= 0.0) || (d <= 0.0))
		{
			QMessageBox::critical(0, "Add 3D Image tool", "Invalid image dimensions");
			return;
		}

		FEModel& fem = *m_doc->GetFEModel();
		C3DImage* pimg = new C3DImage;
		if (pimg->Create(nx, ny, nz) == false)
		{
			QMessageBox::critical(0, "Add 3D Image tool", "Failed creating image data.");
			delete pimg;
			return;
		}

		std::string sfile = ui->pfile->text().toStdString();
		if (pimg->LoadFromFile(sfile.c_str(), 8) == false)
		{
			QMessageBox::critical(0, "Add 3D Image tool", "Failed loading image data.");
			delete pimg;
			return;
		}

		m_doc->Add3DImage(pimg, xmin, ymin, zmin, xmax, ymax, zmax);
		CMainWindow* pwnd = m_doc->GetWindow();
		pwnd->UpdateUi(true);
	}
}
