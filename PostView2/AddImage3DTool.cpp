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

class CAddImage3DToolUI : public QWidget
{
public:
	CIntInput *px, *py, *pz;
	CFloatInput *pw, *ph, *pd;
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
		grid->addWidget(pl = new QLabel("X-pixels"), 0, 0); grid->addWidget(px = new CIntInput, 0, 1); pl->setBuddy(px); px->setFixedWidth(70);
		grid->addWidget(pl = new QLabel("Y-pixels"), 1, 0); grid->addWidget(py = new CIntInput, 1, 1); pl->setBuddy(py); py->setFixedWidth(70);
		grid->addWidget(pl = new QLabel("Z-pixels"), 2, 0); grid->addWidget(pz = new CIntInput, 2, 1); pl->setBuddy(pz); pz->setFixedWidth(70);

		grid->addWidget(pl = new QLabel("width" ), 0, 2); grid->addWidget(pw = new CFloatInput, 0, 3); pl->setBuddy(pw); pw->setFixedWidth(70);
		grid->addWidget(pl = new QLabel("height"), 1, 2); grid->addWidget(ph = new CFloatInput, 1, 3); pl->setBuddy(ph); ph->setFixedWidth(70);
		grid->addWidget(pl = new QLabel("depth" ), 2, 2); grid->addWidget(pd = new CFloatInput, 2, 3); pl->setBuddy(pd); pd->setFixedWidth(70);
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
		int nx = ui->px->value();
		int ny = ui->py->value();
		int nz = ui->pz->value();

		if ((nx <= 0) || (ny <= 0) || (nz <= 0))
		{
			QMessageBox::critical(0, "Add 3D Image tool", "Invalid image dimensions");
			return;
		}

		double w = ui->pw->value();
		double h = ui->ph->value();
		double d = ui->pd->value();

		if ((w <= 0.0) || (h <= 0.0) || (d <= 0.0))
		{
			QMessageBox::critical(0, "Add 3D Image tool", "Invalid image size");
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

		m_doc->Add3DImage(pimg, w, h, d);
		updateUi();
	}
}
