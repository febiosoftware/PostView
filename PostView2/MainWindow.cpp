#include "MainWindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include "Document.h"
#include <PostViewLib/xpltReader.h>
#include "Document.h"
#include "GLModel.h"
#include <string>

CMainWindow::CMainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::CMainWindow)
{
	m_doc = new CDocument(this);
	ui->setupUi(this);
}

CMainWindow::~CMainWindow()
{
}

bool CMainWindow::OpenFile(const QString& fileName)
{
	std::string sfile = fileName.toStdString();

	ui->actionColorMap->setDisabled(true);

	XpltReader* reader = new XpltReader;
	if (m_doc->LoadFEModel(reader, sfile.c_str()) == false)
	{
		QMessageBox b;
		b.setText("Failed reading file.");
		b.setIcon(QMessageBox::Critical);
		b.exec();
	}

	ui->selectData->BuildMenu(m_doc->GetFEModel(), DATA_FLOAT);
	ui->actionColorMap->setEnabled(true);

	// update the command panels
	ui->modelViewer->Update();
	ui->matPanel->Update();
	ui->dataPanel->Update();
	ui->statePanel->Update();
	ui->toolsPanel->Update();

	// update the gl view
	ui->glview->GetCamera().Update(true);
	ui->glview->repaint();
	return true;
}

void CMainWindow::on_actionOpen_triggered()
{
	// build the file filter list
	QString filter;
	filter.append("FEBio plot files (*.xplt);;");
	filter.append("FEBio files (*.feb);;");
	filter.append("LSDYNA database (*);;");
	filter.append("GMesh (*.msh);;");
	filter.append("NIKE3D (*.n);;");
	filter.append("ASCII data (*.txt);;");
	filter.append("STL ASCII (*.stl);;");
	filter.append("RAW image data (*.raw);;");
	filter.append("VTK files (*.vtk);;");
	filter.append("All files (*)");

	QString selFilter;
	QString fileName = QFileDialog::getOpenFileName(this, "Open Files", 0, filter, &selFilter);

	if (fileName.isEmpty() == false) OpenFile(fileName);
}

void CMainWindow::on_actionQuit_triggered()
{
	QApplication::quit();
}

void CMainWindow::on_actionColorMap_toggled(bool bchecked)
{
	CDocument* pdoc = GetDocument();
	CGLModel* po = pdoc->GetGLModel();
	po->GetColorMap()->Activate(bchecked);
	pdoc->UpdateFEModel();
	ui->glview->repaint();
}

void CMainWindow::on_selectData_currentIndexChanged(int i)
{
	int nfield = ui->selectData->currentData(Qt::UserRole).toInt();

	CDocument* pdoc = GetDocument();
	CGLModel* pm = pdoc->GetGLModel();
	pm->GetColorMap()->SetEvalField(nfield);

	pdoc->SetFieldString(ui->selectData->currentText().toStdString().c_str());
	pdoc->UpdateFEModel();

	ui->glview->repaint();
}
