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
#include <PostViewLib/FELSDYNAPlot.h>
#include <string>

CMainWindow::CMainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::CMainWindow)
{
	m_doc = new CDocument(this);
	ui->setupUi(this);
}

CMainWindow::~CMainWindow()
{
}

void CMainWindow::UpdateUi()
{
	// update the command panels
	ui->modelViewer->Update();
	ui->matPanel->Update();
	ui->dataPanel->Update();
	ui->statePanel->Update();
	ui->toolsPanel->Update();

	// update all graph windows
	if (ui->graphList.isEmpty() == false)
	{
		QList<CGraphWindow*>::iterator it;
		for (it=ui->graphList.begin(); it != ui->graphList.end(); ++it)
			(*it)->Update();
	}

	// update the gl view
	ui->glview->GetCamera().Update(true);
	ui->glview->repaint();
}

bool CMainWindow::OpenFile(const QString& fileName, int nfilter)
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

	// update all Ui components
	UpdateUi();

	return true;
}

bool CMainWindow::SaveFile(const QString& fileName, int nfilter)
{
	if (fileName.isEmpty()) return false;
	const char* szfilename = fileName.toStdString().c_str();

	if (m_doc->IsValid() == false) return true;

	FEModel& fem = *m_doc->GetFEModel();

	bool bret = false;
	switch (nfilter)
	{
	case 0:
		{
			FEFEBioExport fr;
			bret = fr.Save(fem, szfilename);
		}
		break;
	case 1:
		{
			bret = m_doc->ExportAscii(szfilename);
		}
		break;
	case 2:
		{
			bret = m_doc->ExportVRML(szfilename);
		}
		break;
	case 3:
		{
//			CDlgExportLSDYNA dlg;
//			if (dlg.DoModal() == FLX_OK)
			{
				FELSDYNAExport w;
//				w.m_bsel = dlg.m_bsel;
//				w.m_bsurf = dlg.m_bsurf;
//				w.m_bnode = dlg.m_bnode;
				bret = w.Save(fem, m_doc->currentTime(), szfilename);
			}
		}
		break;
	case 4:
		{
			bret = m_doc->ExportBYU(szfilename);
		}
		break;
	case 5:
		{
			FENikeExport fr;
			bret = fr.Save(fem, szfilename);
		}
		break;
	case 6:
		{
			FEVTKExport w;
			bret = w.Save(fem, m_doc->currentTime(), szfilename);
		}
		break;
	case 7:
		{
//			FELSDYNAPlotExport w;
//			bret = w.Save(fem, szfilename);
		}
		break;
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
	// Make sure this list matches the one in CFileViewer
	// TODO: Can I somehow ensure that this is the case ?
	QStringList filters;
	filters << "FEBio plot files (*.xplt)"
			<< "FEBio files (*.feb)"
			<< "LSDYNA database (*)"
			<< "GMesh (*.msh)"
			<< "NIKE3D (*.n)"
			<< "ASCII data (*.txt)"
			<< "STL ASCII (*.stl)"
			<< "RAW image data (*.raw)"
			<< "VTK files (*.vtk)";

	QFileDialog dlg(this, "Open");
	dlg.setNameFilters(filters);
	dlg.setFileMode(QFileDialog::ExistingFile);
	if (dlg.exec())
	{
		QStringList files = dlg.selectedFiles();
		QString filter = dlg.selectedNameFilter();
		int nfilter = filters.indexOf(filter);
		OpenFile(files.first(), nfilter);
	}
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

		// update the UI
		UpdateUi();
	}
}

void CMainWindow::on_actionSave_triggered()
{
	QStringList filters;
	filters << "FEBio files (*.feb)"
			<< "ASCII files (*.*)"
			<< "VRML files (*.wrl)"
			<< "LSDYNA Keyword (*.k)"
			<< "BYU files(*.byu)"
			<< "NIKE3D files (*.n)"
			<< "VTK files (*.vtk)"
			<< "LSDYNA Database (*)";

	QFileDialog dlg(this, "Save");
	dlg.setFileMode(QFileDialog::AnyFile);
	dlg.setNameFilters(filters);
	dlg.setAcceptMode(QFileDialog::AcceptSave);
	if (dlg.exec())
	{
		QStringList files = dlg.selectedFiles();
		QString filter = dlg.selectedNameFilter();

		int nfilter = filters.indexOf(filter);
		SaveFile(files.first(), nfilter);
	}
}

void CMainWindow::on_actionSnapShot_triggered()
{
	QImage img = ui->glview->CaptureScreen();

	const uchar* bits = img.bits();
	
	QStringList filters;
	filters << "Bitmap files (*.bmp)"
			<< "PNG files (*.png)"
			<< "JPEG files (*.jpg)";

	QFileDialog dlg(this, "Save Image");
	dlg.setNameFilters(filters);
	dlg.setFileMode(QFileDialog::AnyFile);
	dlg.setAcceptMode(QFileDialog::AcceptSave);
	if (dlg.exec())
	{
		QString fileName = dlg.selectedFiles().first();
		int nfilter = filters.indexOf(dlg.selectedNameFilter());
		bool bret = false;
		switch (nfilter)
		{
		case 0: bret = img.save(fileName, "BMP"); break;
		case 1: bret = img.save(fileName, "PNG"); break;
		case 2: bret = img.save(fileName, "JPG"); break;
		}
		if (bret == false)
		{
			QMessageBox::critical(this, "PostView", "Failed saving image file.");
		}
	}
}

void CMainWindow::on_actionQuit_triggered()
{
	QApplication::quit();
}

void CMainWindow::on_actionGraph_triggered()
{
	// let's find an unused graph
	CGraphWindow* pg = 0;
	if (ui->graphList.isEmpty() == false)
	{
		QList<CGraphWindow*>::iterator it;
		for (it=ui->graphList.begin(); it != ui->graphList.end(); ++it)
		{
			if ((*it)->isVisible() == false)
			{
				pg = *it;
				break;
			}
		}
	}

	// couldn't find one so let's create a new one
	if (pg == 0)
	{
		pg = new CGraphWindow(this);
		pg->setWindowTitle(QString("Graph%1").arg(ui->graphList.size()+1));
		ui->graphList.push_back(pg);
		pg->Update();
	}

	if (pg)
	{
		pg->show();
		pg->raise();
		pg->activateWindow();
	}
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

	int nstep = pdoc->currentTime();
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
	int nstep = pdoc->currentTime();
	nstep--;
	if (nstep < 0) nstep = 0;
	pdoc->SetCurrentTime(nstep);
	ui->glview->repaint();
}

void CMainWindow::on_actionNext_triggered()
{
	CDocument* pdoc = GetDocument();
	int N = pdoc->GetFEModel()->GetStates();
	int nstep = pdoc->currentTime();
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
