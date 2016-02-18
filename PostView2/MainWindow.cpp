#include "MainWindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include "Document.h"
#include <PostViewLib/xpltReader.h>
#include "Document.h"
#include "GLModel.h"
#include <PostViewLib/FEFEBioExport.h>
#include <PostViewLib/FELSDYNAExport.h>
#include <PostViewLib/FENikeExport.h>
#include <PostViewLib/FEVTKExport.h>
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
	if (m_doc->GetFEModel()->GetStates() > 0)
	{
		ui->playToolBar->setEnabled(true);
	}
	else
	{
		ui->playToolBar->setDisabled(true);
	}

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

bool CMainWindow::SaveFile(const QString& fileName, const QString& flt)
{
	if (fileName.isEmpty()) return false;
	const char* szfilename = flt.toStdString().c_str();

	if (m_doc->IsValid() == false) return true;

	FEModel& fem = *m_doc->GetFEModel();

	bool bret = false;
	if (flt=="feb")
	{
		FEFEBioExport fr;
		bret = fr.Save(fem, szfilename);
	}
	else if (flt=="txt") // ASCII Scene
	{
		bret = m_doc->ExportAscii(szfilename);
	}
	else if (flt=="vrml")
	{
		bret = m_doc->ExportVRML(szfilename);
	}
	else if (flt=="k")
	{
//		CDlgExportLSDYNA dlg;
//		if (dlg.DoModal() == FLX_OK)
		{
			FELSDYNAExport w;
//			w.m_bsel = dlg.m_bsel;
//			w.m_bsurf = dlg.m_bsurf;
//			w.m_bnode = dlg.m_bnode;
			bret = w.Save(fem, m_doc->GetCurrentTime(), szfilename);
		}
	}
	else if (flt=="byu")
	{
		bret = m_doc->ExportBYU(szfilename);
	}
	else if (flt == "k")
	{
		FENikeExport fr;
		bret = fr.Save(fem, szfilename);
	}
	else if (flt == "vtk")
	{
		FEVTKExport w;
		bret = w.Save(fem, m_doc->GetCurrentTime(), szfilename);
	}

	if (bret == false)
	{
		QMessageBox b;
		b.setText("Failed saving file.");
		b.setIcon(QMessageBox::Critical);
		b.exec();
	}
	return bret;
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

void CMainWindow::on_actionUpdate_triggered()
{
	ui->actionColorMap->setDisabled(true);
	ui->playToolBar->setDisabled(true);

	if (m_doc->LoadFEModel(0, m_doc->GetFile(), true) == false)
	{
		QMessageBox::critical(this, tr("PostView2"), "Failed updating the model");
	}
	else if (m_doc->IsValid())
	{
		int N = m_doc->GetFEModel()->GetStates();
		if (N > 1) ui->playToolBar->setEnabled(true);
		ui->selectData->BuildMenu(m_doc->GetFEModel(), DATA_FLOAT);

		// update the command panels
		ui->modelViewer->Update();
		ui->matPanel->Update();
		ui->dataPanel->Update();
		ui->statePanel->Update();
		ui->toolsPanel->Update();

		// update the gl view
		ui->glview->GetCamera().Update(true);
		ui->glview->repaint();
	}
}

void CMainWindow::on_actionSave_triggered()
{
	QString filter;
	filter.append("FEBio files (*.feb);;");
	filter.append("ASCII files (*.*);;");
	filter.append("VRML files (*.wrl);;");
	filter.append("LSDYNA Keyword (*.k);;");
	filter.append("BYU files(*.byu);;");
	filter.append("NIKE3D files (*.n);;");
	filter.append("VTK files (*.vtk);;");
	filter.append("LSDYNA Database (*)");

	QString selFilter;
	QString fileName = QFileDialog::getSaveFileName(this, "Save", 0, filter, &selFilter);

	if (fileName.isEmpty() == false) SaveFile(fileName, selFilter);
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
	if (i == -1)
		ui->actionColorMap->setDisabled(true);
	else
	{
		if (ui->actionColorMap->isEnabled() == false)
			ui->actionColorMap->setEnabled(true);

		int nfield = ui->selectData->currentData(Qt::UserRole).toInt();
		CDocument* pdoc = GetDocument();
		CGLModel* pm = pdoc->GetGLModel();
		pm->GetColorMap()->SetEvalField(nfield);

		pdoc->SetFieldString(ui->selectData->currentText().toStdString().c_str());
		pdoc->UpdateFEModel();

		ui->glview->repaint();
	}
}

void CMainWindow::on_actionPlay_toggled(bool bchecked)
{
	if (bchecked)
	{
		m_timer.start(50, this);
	}
	else m_timer.stop();
}

void CMainWindow::timerEvent(QTimerEvent* ev)
{
	CDocument* pdoc = GetDocument();
	int N = pdoc->GetFEModel()->GetStates();

	int nstep = pdoc->GetCurrentTime();
	nstep++;
	if (nstep >= N) nstep = 0;
	pdoc->SetCurrentTime(nstep);

	ui->glview->repaint();
}

void CMainWindow::on_actionFirst_triggered()
{
	CDocument* pdoc = GetDocument();
	pdoc->SetCurrentTime(0);
	ui->glview->repaint();
}

void CMainWindow::on_actionPrev_triggered()
{
	CDocument* pdoc = GetDocument();
	int N = pdoc->GetFEModel()->GetStates();
	int nstep = pdoc->GetCurrentTime();
	nstep--;
	if (nstep < 0) nstep = 0;
	pdoc->SetCurrentTime(nstep);
	ui->glview->repaint();
}

void CMainWindow::on_actionNext_triggered()
{
	CDocument* pdoc = GetDocument();
	int N = pdoc->GetFEModel()->GetStates();
	int nstep = pdoc->GetCurrentTime();
	nstep++;
	if (nstep >= N) nstep = N-1;
	pdoc->SetCurrentTime(nstep);
	ui->glview->repaint();
}

void CMainWindow::on_actionLast_triggered()
{
	CDocument* pdoc = GetDocument();
	int N = pdoc->GetFEModel()->GetStates();
	pdoc->SetCurrentTime(N-1);
	ui->glview->repaint();
}
