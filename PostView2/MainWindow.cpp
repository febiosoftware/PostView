#include "MainWindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QDesktopServices>
#include "Document.h"
#include "Document.h"
#include "GLModel.h"
#include <PostViewLib/xpltFileReader.h>
#include <PostViewLib/FEFEBioExport.h>
#include <PostViewLib/FELSDYNAExport.h>
#include <PostViewLib/FENikeExport.h>
#include <PostViewLib/FEVTKExport.h>
#include <PostViewLib/FELSDYNAPlot.h>
#include <PostViewLib/FEBioPlotExport.h>
#include <PostViewLib/FEBioImport.h>
#include <PostViewLib/FEU3DImport.h>
#include <PostViewLib/FELSDYNAimport.h>
#include <PostViewLib/FELSDYNAPlot.h>
#include <PostViewLib/FEVTKImport.h>
#include <PostViewLib/GMeshImport.h>
#include <PostViewLib/FENikeImport.h>
#include <PostViewLib/FEASCIIImport.h>
#include <PostViewLib/FESTLimport.h>
#include <PostViewLib/FERAWImageReader.h>
#include <PostViewLib/FEAsciiExport.h>
#include "GLPlaneCutPlot.h"
#include "GLIsoSurfacePlot.h"
#include "GLSlicePLot.h"
#include "GLVectorPlot.h"
#include "DlgViewSettings.h"
#include "DlgExportXPLT.h"
#include "DlgWidgetProps.h"
#include "DlgFind.h"
#include "DlgImportXPLT.h"
#include "DlgSelectRange.h"
#include "DlgTimeSettings.h"
#include "DlgFileInfo.h"
#include "DlgExportAscii.h"
#include "version.h"
#include "convert.h"
#include "ImgAnimation.h"
#include "AVIAnimation.h"
#include "MPEGAnimation.h"
#include <string>

CFileThread::CFileThread(CMainWindow* wnd, FEFileReader* file, const QString& fileName) : m_wnd(wnd), m_fileReader(file), m_fileName(fileName)
{
	QObject::connect(this, SIGNAL(resultReady(bool,const QString&)), wnd, SLOT(finishedReadingFile(bool, const QString&)));
	QObject::connect(this, SIGNAL(finished())   , this, SLOT(deleteLater()));
}

void CFileThread::run()
{
	if (m_fileReader)
	{
		std::string sfile = m_fileName.toStdString();
		CDocument& doc = *m_wnd->GetDocument();
		bool ret = doc.LoadFEModel(m_fileReader, sfile.c_str());
		std::string err = m_fileReader->GetErrorMessage();
		emit resultReady(ret, QString(err.c_str()));
	}
	else emit resultReady(false, "No file reader");
}

float CFileThread::getFileProgress() const
{
	if (m_fileReader) return m_fileReader->GetFileProgress();
	return 0.f;
}

CMainWindow::CMainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::CMainWindow)
{
	m_doc = new CDocument(this);
	ui->setupUi(this);
	m_fileThread = 0;

	// initialize color maps
	// (This must be done before we read the settings!)
	ColorMapManager::Initialize();

	// read settings from last session
	readSettings();

	// make sure the file viewer is visible
	ui->fileViewer->parentWidget()->raise();
}

CMainWindow::~CMainWindow()
{
}

void CMainWindow::SetStatusMessage(const QString& message)
{
	ui->statusBar->showMessage(message);
}

void CMainWindow::ClearStatusMessage()
{
	ui->statusBar->clearMessage();
}

CGLView* CMainWindow::GetGLView()
{
	return ui->glview;
}

void CMainWindow::UpdateUi(bool breset, QWidget* psender)
{
	// update the command panels
	if (psender != ui->modelViewer) ui->modelViewer->Update(breset);
	if (psender != ui->matPanel   ) ui->matPanel->Update(breset);
	if (psender != ui->dataPanel  ) ui->dataPanel->Update(breset);
	if (psender != ui->statePanel ) ui->statePanel->Update(breset);
	if (psender != ui->toolsPanel ) ui->toolsPanel->Update(breset);
	if (psender != ui->timePanel  ) ui->timePanel->Update(breset);

	// update all graph windows
	UpdateGraphs();

	// update the gl view
	ui->glview->GetCamera().Update(true);
	RedrawGL();
}

void CMainWindow::RedrawGL()
{
	ui->glview->repaint();
}

void CMainWindow::CheckUi()
{
	// check the color map state
	CDocument* doc = GetDocument();
	CGLModel* po = doc->GetGLModel();
	if (po)
	{
		bool bcheck = ui->actionColorMap->isChecked();
		CGLColorMap* col = po->GetColorMap();
		if (col)
		{
			if (bcheck != col->IsActive()) ui->actionColorMap->setChecked(col->IsActive());
		}
	}
}

void CMainWindow::UpdateView()
{
	ui->modelViewer->UpdateView();
}

void CMainWindow::UpdateTools()
{
	ui->toolsPanel->Update(false);
}

void CMainWindow::UpdateGraphs(bool breset, bool bfit)
{
	if (ui->graphList.isEmpty() == false)
	{
		QList<CGraphWindow*>::iterator it;
		for (it=ui->graphList.begin(); it != ui->graphList.end(); ++it)
			if ((*it)->isVisible()) (*it)->Update(breset, bfit);
	}

	if (ui->integrateWindow && ui->integrateWindow->isVisible()) 
		ui->integrateWindow->Update(false);

	if (ui->statsWindow && ui->statsWindow->isVisible()) 
		ui->statsWindow->Update(false);
}

QMenu* CMainWindow::BuildContextMenu()
{
	QMenu* menu = new QMenu(this);
	menu->addAction(ui->actionViewFront);
	menu->addAction(ui->actionViewBack);
	menu->addAction(ui->actionViewLeft);
	menu->addAction(ui->actionViewRight);
	menu->addAction(ui->actionViewTop);
	menu->addAction(ui->actionViewBottom);
	menu->addSeparator();
	menu->addAction(ui->actionViewProjection);
	menu->addSeparator();
	menu->addAction(ui->actionViewSettings);
	return menu;
}

void CMainWindow::SetCurrentFolder(const QString& folder)
{
	ui->currentPath = folder;
}

void CMainWindow::OpenFile(const QString& fileName, int nfilter)
{
	std::string sfile = fileName.toStdString();

	ui->actionColorMap->setDisabled(true);

	setWindowTitle(QString("PostView2"));

	// create a file reader
	FEFileReader* reader = 0;

	// If filter not specified, use extension
	if (nfilter == -1)
	{
		std::string sfile = fileName.toStdString();
		// conver to lower case
		std::transform(sfile.begin(), sfile.end(), sfile.begin(), ::tolower);
		
		int npos = sfile.find_last_of('.');
		if (npos == std::string::npos) nfilter = 2; // LSDYNA Database
		else
		{
			std::string ext = sfile.substr(npos+1);
			if      (ext.compare("xplt") == 0) nfilter = -1;
			else if (ext.compare("feb" ) == 0) nfilter =  1;
			else if (ext.compare("k"   ) == 0) nfilter =  3;
			else if (ext.compare("msh" ) == 0) nfilter =  4;
			else if (ext.compare("n  " ) == 0) nfilter =  5;
			else if (ext.compare("txt" ) == 0) nfilter =  6;
			else if (ext.compare("stl" ) == 0) nfilter =  7;
			else if (ext.compare("raw" ) == 0) nfilter =  8;
			else if (ext.compare("vtk" ) == 0) nfilter =  9;
			else if (ext.compare("u3d" ) == 0) nfilter = 10;
			else
			{
				return;
			}
		}
	}

	switch (nfilter)
	{
	case -1: reader = new xpltFileReader; break;
	case 0: 
		{
			xpltFileReader* xplt = new xpltFileReader;
			reader = xplt;

			CDlgImportXPLT dlg(this);
			if (dlg.exec())
			{
				xplt->SetReadStateFlag(dlg.m_nop);
				xplt->SetReadStatesList(dlg.m_item);
			}
			else return;
		}
		break;
	case  1: reader = new FEBioImport; break;
	case  2: reader = new FELSDYNAPlotImport; break;
	case  3: reader = new FELSDYNAimport; break;
	case  4: reader = new GMeshImport; break;
	case  5: reader = new FENikeImport; break;
	case  6: reader = new FEASCIIImport; break;
	case  7: reader = new FESTLimport; break;
	case  8: reader = new FERAWImageReader; break;
	case  9: reader = new FEVTKimport; break;
	case 10: reader = new FEU3DImport; break;
	default:
		QMessageBox::critical(this, "PostView2", "Don't know how to read this file");
		return;
	}

	// copy just the file title
	std::string stitle = sfile;
	int npos = sfile.rfind('/');
	if (npos == std::string::npos) npos = sfile.rfind('\\');
	if (npos != std::string::npos)
	{
		stitle = sfile.substr(npos+1);
	}

	// set the window title
	setWindowTitle(QString("PostView2 - %1").arg(QString(stitle.c_str())));

	// create the file reading thread and run it
	m_fileThread = new CFileThread(this, reader, fileName);
	m_fileThread->start();
	ui->statusBar->showMessage(QString("Reading file %1 ...").arg(fileName));

	int H = ui->statusBar->height() - 5;
	ui->fileProgress->setFixedHeight(H);
	ui->stopFileReading->setFixedHeight(H);

	ui->fileProgress->setValue(0);
	ui->statusBar->addPermanentWidget(ui->fileProgress);
	ui->statusBar->addPermanentWidget(ui->stopFileReading);
	ui->stopFileReading->setEnabled(true);
	ui->fileProgress->show();
	ui->stopFileReading->show();
	QObject::connect(ui->stopFileReading, SIGNAL(clicked()), this, SLOT(onCancelFileRead()));
	QTimer::singleShot(100, this, SLOT(checkFileProgress()));
}

void CMainWindow::onCancelFileRead()
{
	if (m_fileThread)
	{
		FEFileReader* fileReader = m_fileThread->GetFileReader();
		if (fileReader)
		{
			fileReader->Cancel();
			ui->stopFileReading->setDisabled(true);
		}
	}
}


void CMainWindow::checkFileProgress()
{
	if (m_fileThread)
	{
		float f = m_fileThread->getFileProgress();
		int n = (int) (100.f*f);
		ui->fileProgress->setValue(n);
		if (f < 1.0f) QTimer::singleShot(100, this, SLOT(checkFileProgress()));
	}
}

void CMainWindow::finishedReadingFile(bool success, const QString& errorString)
{
	m_fileThread = 0;
	ui->statusBar->clearMessage();
	ui->statusBar->removeWidget(ui->stopFileReading);
	ui->statusBar->removeWidget(ui->fileProgress);

	if (success == false)
	{
		QMessageBox::critical(this, "PostView2", QString("Failed reading file :\n%1").arg(errorString));
		return;
	}
	else if (errorString.isEmpty() == false)
	{
		QMessageBox::information(this, "PostView2", errorString);
	}
	ui->glview->UpdateWidgets();

	// update all Ui components
	UpdateUi(true);

	UpdateMainToolbar();

	// This is already done in UpdateMainToolbar so I can probably remove this
	FEModel* fem = m_doc->GetFEModel();
	if (fem && fem->GetStates() > 0)
	{
		ui->playToolBar->setEnabled(true);
	}
	else
	{
		ui->playToolBar->setDisabled(true);
	}
}

bool CMainWindow::SaveFile(const QString& fileName, int nfilter)
{
	if (fileName.isEmpty()) return false;
	string sfilename = fileName.toStdString();
	const char* szfilename = sfilename.c_str();

	if (m_doc->IsValid() == false) return true;

	FEModel& fem = *m_doc->GetFEModel();

	bool bret = false;
	QString error("(unknown)");
	switch (nfilter)
	{
	case 0:
		{
			CDlgExportXPLT dlg(this);
			if (dlg.exec() == QDialog::Accepted)
			{
				FEBioPlotExport ex;
				ex.SetCompression(dlg.m_bcompress);
				bret = ex.Save(fem, szfilename);
				error = ex.GetErrorMessage();
			}
		}
		break;
	case 1:
		{
			FEFEBioExport fr;
			bret = fr.Save(fem, szfilename);
		}
		break;
	case 2:
		{
			CDlgExportAscii dlg(this);
			if (dlg.exec() == QDialog::Accepted)
			{ 
				// decide which time steps to export
				int n0, n1;
				if (dlg.m_nstep == 0) n0 = n1 = m_doc->currentTime();
				else
				{
					n0 = 0;
					n1 = fem.GetStates() - 1;
				}

				// export the data
				FEASCIIExport out;
				out.m_bcoords = dlg.m_bcoords;
				out.m_bedata = dlg.m_bedata;
				out.m_belem = dlg.m_belem;
				out.m_bface = dlg.m_bface;
				out.m_bfnormals = dlg.m_bfnormals;
				out.m_bndata = dlg.m_bndata;
				out.m_bselonly = dlg.m_bsel;

				bret = out.Save(&fem, n0, n1, szfilename);
			}
		}
		break;
	case 3:
		{
			bret = m_doc->ExportVRML(szfilename);
		}
		break;
	case 4:
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
	case 5:
		{
			bret = m_doc->ExportBYU(szfilename);
		}
		break;
	case 6:
		{
			FENikeExport fr;
			bret = fr.Save(fem, szfilename);
		}
		break;
	case 7:
		{
			FEVTKExport w;
			bret = w.Save(fem, szfilename);
		}
		break;
	case 8:
		{
//			FELSDYNAPlotExport w;
//			bret = w.Save(fem, szfilename);
		}
		break;
	}

	if (bret == false)
	{
		QMessageBox b;
		b.setText(QString("Failed saving file.\nReason:%1").arg(error));
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
			<< "LSDYNA keyword (*.k)"
			<< "GMesh (*.msh)"
			<< "NIKE3D (*.n)"
			<< "ASCII data (*.txt)"
			<< "STL ASCII (*.stl)"
			<< "RAW image data (*.raw)"
			<< "VTK files (*.vtk)"
			<< "U3D files (*.u3d)";

	QFileDialog dlg(this, "Open");
	dlg.setNameFilters(filters);
	dlg.setFileMode(QFileDialog::ExistingFile);
	dlg.setDirectory(ui->currentPath);
	if (dlg.exec())
	{
		QDir dir = dlg.directory();
		SetCurrentFolder(dir.absolutePath());

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
		ui->selectData->BuildMenu(m_doc->GetFEModel(), DATA_SCALAR);

		// update the UI
		UpdatePlayToolbar(true);
		UpdateUi(true);
	}
}

void CMainWindow::on_actionFileInfo_triggered()
{
	CDlgFileInfo dlg;
	FEModel& fem = *GetDocument()->GetFEModel();
	MetaData& md = fem.GetMetaData();
	dlg.setSoftware(QString::fromStdString(md.software));
	dlg.exec();
}

void CMainWindow::on_actionSave_triggered()
{
	QStringList filters;
	filters << "FEBio xplt files (*.xplt)"
			<< "FEBio files (*.feb)"
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

void CMainWindow::on_actionOpenSession_triggered()
{
	QString filename = QFileDialog::getOpenFileName(this, "Open session file", 0, "PostView session (*.pvs)");
	if (filename.isEmpty() == false)
	{
		std::string sfile = filename.toStdString();
		const char* szfile = sfile.c_str();

		CDocument* pdoc = GetDocument();
		// try to open the session
		if (pdoc->OpenSession(szfile) == false)
		{
			QMessageBox::critical(this, "PostView", "Failed restoring session.");
//			ShowTimeController(false);
			ui->playToolBar->setDisabled(true);
		}
		else
		{
			int N = pdoc->GetFEModel()->GetStates();
//			ShowTimeController(N > 1);

			const char* ch = strrchr(szfile, '\\');
			if (ch == 0) 
			{
				ch = strrchr(szfile, '/'); 
				if (ch == 0) ch = szfile; else ch++;
			} else ch++;

			QString title; title = QString("%1 - PostView").arg(ch);
			setWindowTitle(title);
			
			ui->selectData->BuildMenu(m_doc->GetFEModel(), DATA_SCALAR);
			if (m_doc->GetFEModel()->GetStates() > 0) ui->playToolBar->setEnabled(true);
			else ui->playToolBar->setDisabled(true);

			// update all Ui components
			UpdateUi(true);
		}
	}
}

void CMainWindow::on_actionSaveSession_triggered()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Save session", 0, "PostView session (*.pvs)");
	if (fileName.isEmpty() == false)
	{
		std::string sfile = fileName.toStdString();
		const char* szfile = sfile.c_str();
		if (m_doc->SaveSession(szfile) == false)
		{
			QMessageBox::critical(this, "PostView", "Failed storing PostView session.");
		}
	}
}

void CMainWindow::on_actionQuit_triggered()
{
	writeSettings();
	QApplication::quit();
}

void CMainWindow::on_selectNodes_triggered()
{
	int oldMode = m_doc->GetSelectionMode();

	m_doc->ConvertSelection(oldMode, SELECT_NODES);

	m_doc->SetSelectionMode(SELECT_NODES);

	RedrawGL();
}

void CMainWindow::on_selectEdges_triggered()
{
	m_doc->SetSelectionMode(SELECT_EDGES);
	RedrawGL();
}

void CMainWindow::on_selectFaces_triggered()
{
	m_doc->SetSelectionMode(SELECT_FACES);
	RedrawGL();
}

void CMainWindow::on_selectElems_triggered()
{
	m_doc->SetSelectionMode(SELECT_ELEMS);
	RedrawGL();
}

void CMainWindow::on_actionSelectRect_triggered()
{
	GetDocument()->SetSelectionStyle(SELECT_RECT);
}

void CMainWindow::on_actionSelectCircle_triggered()
{
	GetDocument()->SetSelectionStyle(SELECT_CIRCLE);
}

void CMainWindow::on_actionSelectFree_triggered()
{
	GetDocument()->SetSelectionStyle(SELECT_FREE);
}

void CMainWindow::on_actionZoomSelected_triggered()
{
	ui->glview->OnZoomSelect();
}

void CMainWindow::on_actionZoomExtents_triggered()
{
	ui->glview->OnZoomExtents();
}

void CMainWindow::on_actionSelectItem_triggered()
{
	GetDocument()->GetViewSettings().m_bconn = false;
}

void CMainWindow::on_actionSelectConn_triggered()
{
	GetDocument()->GetViewSettings().m_bconn = true;
}

void CMainWindow::on_actionHideSelected_triggered()
{
	CDocument* pdoc = GetDocument();
	if (pdoc->IsValid() == false) return;

	CGLModel& mdl = *pdoc->GetGLModel();

	switch (pdoc->GetSelectionMode())
	{
	case SELECT_NODES: mdl.HideSelectedNodes(); break;
	case SELECT_EDGES: mdl.HideSelectedEdges(); break;
	case SELECT_FACES: mdl.HideSelectedFaces(); break;
	case SELECT_ELEMS: mdl.HideSelectedElements(); break;
	}

	pdoc->UpdateFEModel();
	RedrawGL();
}

void CMainWindow::on_actionHideUnselected_triggered()
{
	CDocument* pdoc = GetDocument();
	CGLModel& mdl = *pdoc->GetGLModel();

	mdl.HideUnselectedElements();

	pdoc->UpdateFEModel();
	RedrawGL();
}

void CMainWindow::on_actionInvertSelection_triggered()
{
	CDocument* pdoc = GetDocument();
	FEMeshBase* pfe = pdoc->GetActiveMesh();

	for (int i=0; i<pfe->Elements(); i++)
	{
		FEElement& e = pfe->Element(i);
		if (e.IsVisible())
			if (e.IsSelected()) e.Unselect(); else e.Select();
	}

	pdoc->UpdateFEModel();
	RedrawGL();
}

void CMainWindow::on_actionUnhideAll_triggered()
{
	CDocument* pdoc = GetDocument();
	if (pdoc->IsValid() == false) return;

	FEModel& fem = *pdoc->GetFEModel();
	FEMeshBase& mesh = *pdoc->GetActiveMesh();

	for (int i=0; i<mesh.Elements(); ++i) mesh.Element(i).Unhide();
	for (int i=0; i<mesh.Faces   (); ++i) mesh.Face   (i).Unhide();
	for (int i=0; i<mesh.Edges   (); ++i) mesh.Edge   (i).Unhide();
	for (int i=0; i<mesh.Nodes   (); ++i) mesh.Node   (i).Unhide();

	pdoc->UpdateFEModel();
	RedrawGL();
}

void CMainWindow::on_actionSelectAll_triggered()
{
	CDocument* pdoc = GetDocument();
	FEMeshBase* pfe = pdoc->GetActiveMesh();

	for (int i=0; i<pfe->Elements(); i++) 
	{
		FEElement& e = pfe->Element(i);
		if (e.IsVisible()) e.Select();
	}

	pdoc->UpdateFEModel();
	RedrawGL();
}

void CMainWindow::on_actionSelectRange_triggered()
{
	CDocument* pdoc = GetDocument();
	if (!pdoc->IsValid()) return;

	CGLColorMap* pcol = pdoc->GetGLModel()->GetColorMap();
	if (pcol == 0) return;

	float d[2];
	pcol->GetRange(d);

	CDlgSelectRange dlg(this);
	dlg.m_min = d[0];
	dlg.m_max = d[1];

	if (dlg.exec())
	{
		switch (pdoc->GetSelectionMode())
		{
		case SELECT_NODES: pdoc->SelectNodesInRange(dlg.m_min, dlg.m_max, dlg.m_brange); break;
		case SELECT_EDGES: pdoc->SelectEdgesInRange(dlg.m_min, dlg.m_max, dlg.m_brange); break;
		case SELECT_FACES: pdoc->SelectFacesInRange(dlg.m_min, dlg.m_max, dlg.m_brange); break;
		case SELECT_ELEMS: pdoc->SelectElemsInRange(dlg.m_min, dlg.m_max, dlg.m_brange); break;
		}
		
		pdoc->UpdateFEModel();
		UpdateUi(false);
	}
}

void CMainWindow::on_actionClearSelection_triggered()
{
	CDocument* pdoc = GetDocument();
	if (pdoc->IsValid())
	{
		CGLModel& mdl = *pdoc->GetGLModel();
		mdl.ClearSelection(); 
		pdoc->UpdateFEModel();
		RedrawGL();
	}
}

void CMainWindow::on_actionFind_triggered()
{
	CDocument& doc = *GetDocument();
	if (doc.IsValid() == false) return;

	int nview = doc.GetSelectionMode();
	int nsel = 0;
	if (nview == SELECT_NODES) nsel = 0;
	if (nview == SELECT_EDGES) nsel = 1;
	if (nview == SELECT_FACES) nsel = 2;
	if (nview == SELECT_ELEMS) nsel = 3;

	CDlgFind dlg(this, nsel);

	if (dlg.exec())
	{
		CGLModel* pm = doc.GetGLModel();

		if (dlg.m_bsel[0]) nview = SELECT_NODES;
		if (dlg.m_bsel[1]) nview = SELECT_EDGES;
		if (dlg.m_bsel[2]) nview = SELECT_FACES;
		if (dlg.m_bsel[3]) nview = SELECT_ELEMS;

		switch (nview)
		{
		case SELECT_NODES: on_selectNodes_triggered(); pm->SelectNodes   (dlg.m_item, dlg.m_bclear); break;
		case SELECT_EDGES: on_selectEdges_triggered(); pm->SelectEdges   (dlg.m_item, dlg.m_bclear); break;
		case SELECT_FACES: on_selectFaces_triggered(); pm->SelectFaces   (dlg.m_item, dlg.m_bclear); break;
		case SELECT_ELEMS: on_selectElems_triggered(); pm->SelectElements(dlg.m_item, dlg.m_bclear); break;
		}

		RedrawGL();
	}
}

void CMainWindow::on_actionDelete_triggered()
{
	GLWidget* pg = GLWidget::get_focus();
	if (pg && (pg->GetObject() == 0))
	{
		CGLWidgetManager* pwm = CGLWidgetManager::GetInstance();
		pwm->RemoveWidget(pg);
		RedrawGL();
	}
}

void CMainWindow::on_actionProperties_triggered()
{
	// get the selected widget
	GLWidget* pglw = GLWidget::get_focus();
	if (pglw == 0) return;

	// edit the properties
	if (dynamic_cast<GLBox*>(pglw))
	{
		CDlgBoxProps dlg(pglw, this);
		dlg.exec();
	}
	else if (dynamic_cast<GLLegendBar*>(pglw))
	{
		CDlgLegendProps dlg(pglw, this);
		dlg.exec();
	}
	else if (dynamic_cast<GLTriad*>(pglw))
	{
		CDlgTriadProps dlg(pglw, this);
		dlg.exec();
	}
	else if (dynamic_cast<GLSafeFrame*>(pglw))
	{
		CDlgCaptureFrameProps dlg(pglw, this);
		dlg.exec();
	}
	else
	{
		QMessageBox::information(this, "Properties", "No properties available");
	}

	UpdateFontToolbar();

	RedrawGL();
}

void CMainWindow::on_actionPlaneCut_triggered()
{
	CDocument* pdoc = GetDocument();
	CGLPlaneCutPlot* pp = new CGLPlaneCutPlot(pdoc->GetGLModel());
	pdoc->AddPlot(pp);

	ui->modelViewer->Update(true);
	ui->modelViewer->selectObject(pp);
	ui->modelViewer->parentWidget()->raise();
	RedrawGL();
}

void CMainWindow::on_actionVectorPlot_triggered()
{
	CDocument* pdoc = GetDocument();
	CGLVectorPlot* pp = new CGLVectorPlot(pdoc->GetGLModel());
	pdoc->AddPlot(pp);
	pdoc->UpdateFEModel();
	
	ui->modelViewer->Update(true);
	ui->modelViewer->selectObject(pp);
	ui->modelViewer->parentWidget()->raise();

	RedrawGL();
}

void CMainWindow::on_actionIsosurfacePlot_triggered()
{
	CDocument* pdoc = GetDocument();
	CGLIsoSurfacePlot* pp = new CGLIsoSurfacePlot(pdoc->GetGLModel());
	pdoc->AddPlot(pp);
	pdoc->UpdateFEModel();

	ui->modelViewer->Update(true);
	ui->modelViewer->selectObject(pp);
	ui->modelViewer->parentWidget()->raise();

	RedrawGL();
}

void CMainWindow::on_actionSlicePlot_triggered()
{
	CDocument* pdoc = GetDocument();
	CGLSlicePlot* pp = new CGLSlicePlot(pdoc->GetGLModel());
	pdoc->AddPlot(pp);
	pdoc->UpdateFEModel();

	ui->modelViewer->Update(true);
	ui->modelViewer->selectObject(pp);
	ui->modelViewer->parentWidget()->raise();

	RedrawGL();
}

void CMainWindow::on_actionDisplacementMap_triggered()
{
	CDocument* pdoc = GetDocument();
	CGLModel* pm = pdoc->GetGLModel();
	if (pm->GetDisplacementMap() == 0)
	{
		if (pm->AddDisplacementMap() == false)
		{
			QMessageBox::warning(this, "PostView", "You need at least one vector field before you can define a displacement map.");
		}
//		else m_pCmdWnd->Update();
	}
	else
	{
		QMessageBox::information(this, "PostView", "You need at least one vector field before you can define a displacement map.");
	}
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
		pg->setWindowTitle(QString("PostView2 : Graph%1").arg(ui->graphList.size()+1));
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

void CMainWindow::on_actionSummary_triggered()
{
	if (ui->summaryWindow == 0)
	{
		ui->summaryWindow = new CSummaryWindow(this);
	}
	ui->summaryWindow->Update(true);
	ui->summaryWindow->show();
	ui->summaryWindow->raise();
	ui->summaryWindow->activateWindow();
}

void CMainWindow::on_actionStats_triggered()
{
	if (ui->statsWindow == 0)
	{
		ui->statsWindow = new CStatsWindow(this);
	}
	ui->statsWindow->Update(true);
	ui->statsWindow->show();
	ui->statsWindow->raise();
	ui->statsWindow->activateWindow();
}

void CMainWindow::on_actionIntegrate_triggered()
{
	if (ui->integrateWindow == 0)
	{
		ui->integrateWindow = new CIntegrateWindow(this);
	}
	ui->integrateWindow->Update(true);
	ui->integrateWindow->show();
	ui->integrateWindow->raise();
	ui->integrateWindow->activateWindow();
}

void CMainWindow::on_actionColorMap_toggled(bool bchecked)
{
	CDocument* pdoc = GetDocument();
	CGLModel* po = pdoc->GetGLModel();
	po->GetColorMap()->Activate(bchecked);
	pdoc->UpdateFEModel();
	RedrawGL();
}

void CMainWindow::SetCurrentDataField(int nfield)
{
	ui->selectData->setCurrentValue(nfield);
}

void CMainWindow::on_selectData_currentIndexChanged(int index)
{
	if (index == -1)
		ui->actionColorMap->setDisabled(true);
	else
	{
		if (ui->actionColorMap->isEnabled() == false)
			ui->actionColorMap->setEnabled(true);

		int nfield = ui->selectData->currentData(Qt::UserRole).toInt();
		CDocument* pdoc = GetDocument();
		CGLModel* pm = pdoc->GetGLModel();
		pm->GetColorMap()->SetEvalField(nfield);

		pdoc->UpdateFEModel();

		ui->glview->UpdateWidgets(false);
//		RedrawGL();
	}

	if (ui->integrateWindow && ui->integrateWindow->isVisible()) 
		ui->integrateWindow->Update(true);

	if (ui->statsWindow && ui->statsWindow->isVisible()) 
		ui->statsWindow->Update(true);

	if (ui->modelViewer->isVisible()) ui->modelViewer->Update(false);
}

void CMainWindow::on_actionPlay_toggled(bool bchecked)
{
	TIMESETTINGS& time = GetDocument()->GetTimeSettings();
	double fps = time.m_fps;
	if (fps < 1.0) fps = 1.0;
	double msec_per_frame = 1000.0 / fps;

	if (bchecked)
	{
		m_timer.start(msec_per_frame, this);
	}
	else m_timer.stop();
}

void CMainWindow::SetCurrentTime(int n)
{
	ui->pspin->setValue(n + 1);
	ui->timePanel->SetCurrentTime(n);
	GetDocument()->SetCurrentTime(n);
	RedrawGL();
	UpdateGraphs(false);
}

void CMainWindow::StopAnimation()
{
	m_timer.stop();
	ui->actionPlay->setChecked(false);
}

void CMainWindow::timerEvent(QTimerEvent* ev)
{
	CDocument* pdoc = GetDocument();
	TIMESETTINGS& time = pdoc->GetTimeSettings();

	int N = pdoc->GetFEModel()->GetStates();
	int N0 = time.m_start;
	int N1 = time.m_end;

	int nstep = pdoc->currentTime();

	if (time.m_mode == MODE_FORWARD)
	{
		nstep++;
		if (nstep > N1)
		{
			if (time.m_bloop) nstep = N0;
			else { nstep = N1; StopAnimation(); }
		}
	}
	else if (time.m_mode == MODE_REVERSE)
	{
		nstep--;
		if (nstep < N0) 
		{
			if (time.m_bloop) nstep = N1;
			else { nstep = N0; StopAnimation(); }
		}
	}
	else if (time.m_mode == MODE_CYLCE)
	{
		nstep += time.m_inc;
		if (nstep > N1)
		{
			time.m_inc = -1;
			nstep = N1;
			if (time.m_bloop == false) StopAnimation();
		}
		else if (nstep < N0)
		{
			time.m_inc = 1;
			nstep = N0;
			if (time.m_bloop == false) StopAnimation();
		}
	}
	SetCurrentTime(nstep);
	RedrawGL();
}

void CMainWindow::on_actionFirst_triggered()
{
	CDocument* pdoc = GetDocument();
	TIMESETTINGS& time = pdoc->GetTimeSettings();
	SetCurrentTime(time.m_start);
}

void CMainWindow::on_actionPrev_triggered()
{
	CDocument* pdoc = GetDocument();
	TIMESETTINGS& time = pdoc->GetTimeSettings();
	int nstep = pdoc->currentTime();
	nstep--;
	if (nstep < time.m_start) nstep = time.m_start;
	SetCurrentTime(nstep);
}

void CMainWindow::on_actionNext_triggered()
{
	CDocument* pdoc = GetDocument();
	TIMESETTINGS& time = pdoc->GetTimeSettings();
	int nstep = pdoc->currentTime();
	nstep++;
	if (nstep > time.m_end) nstep = time.m_end;
	SetCurrentTime(nstep);
}

void CMainWindow::on_actionLast_triggered()
{
	CDocument* pdoc = GetDocument();
	TIMESETTINGS& time = pdoc->GetTimeSettings();
	SetCurrentTime(time.m_end);
}

void CMainWindow::on_actionTimeSettings_triggered()
{
	CDlgTimeSettings dlg(GetDocument(), this);
	if (dlg.exec())
	{
		TIMESETTINGS& time = m_doc->GetTimeSettings();
		ui->timePanel->SetRange(time.m_start, time.m_end);

		int ntime = m_doc->currentTime();
		if ((ntime < time.m_start) || (ntime > time.m_end))
		{
			if (ntime < time.m_start) ntime = time.m_start;
			if (ntime > time.m_end  ) ntime = time.m_end;
		}
		SetCurrentTime(ntime);
		RedrawGL();
	}
}

void CMainWindow::on_actionViewSettings_triggered()
{
	CDlgViewSettings dlg(this);
	dlg.exec();
}

void CMainWindow::on_actionViewMesh_toggled(bool bchecked)
{
	VIEWSETTINGS& view = GetDocument()->GetViewSettings();
	view.m_bmesh = bchecked;
	RedrawGL();
}

void CMainWindow::on_actionViewOutline_toggled(bool bchecked)
{
	VIEWSETTINGS& view = GetDocument()->GetViewSettings();
	view.m_boutline = bchecked;
	RedrawGL();
}

void CMainWindow::on_actionViewShowTags_toggled(bool bchecked)
{
	VIEWSETTINGS& view = GetDocument()->GetViewSettings();
	view.m_bTags = bchecked;
	RedrawGL();
}

void CMainWindow::on_actionViewSmooth_toggled(bool bchecked)
{
	CGLModel* po = GetDocument()->GetGLModel();
	if (po)
	{
		CGLColorMap* pcm = po->GetColorMap();
		if (pcm)
		{
			CColorTexture* pc = pcm->GetColorMap();
			if (pc)
			{
				bool b = pc->GetSmooth();
				pc->SetSmooth(!b);
				m_doc->UpdateFEModel();
				RedrawGL();
			}
		}
	}
}

void CMainWindow::on_actionViewCapture_toggled(bool bchecked)
{
	ui->glview->showSafeFrame(bchecked);
	RedrawGL();
}

void CMainWindow::on_actionViewProjection_toggled(bool bchecked)
{
	ui->glview->setPerspective(!bchecked);
}

void CMainWindow::on_actionViewFront_triggered()
{
	ui->glview->SetView(VIEW_FRONT);
}

void CMainWindow::on_actionViewBack_triggered()
{
	ui->glview->SetView(VIEW_BACK);
}

void CMainWindow::on_actionViewLeft_triggered()
{
	ui->glview->SetView(VIEW_LEFT);
}

void CMainWindow::on_actionViewRight_triggered()
{
	ui->glview->SetView(VIEW_RIGHT);
}

void CMainWindow::on_actionViewTop_triggered()
{
	ui->glview->SetView(VIEW_TOP);
}

void CMainWindow::on_actionViewBottom_triggered()
{
	ui->glview->SetView(VIEW_BOTTOM);
}

void CMainWindow::on_actionViewTrack_toggled(bool bchecked)
{
	ui->glview->TrackSelection(bchecked);
	RedrawGL();
}

void CMainWindow::on_actionViewVPSave_triggered()
{
	CGLCamera& cam = ui->glview->GetCamera();
	GLCameraTransform t;
	cam.GetTransform(t);

	CGView& view = *GetDocument()->GetView();
	static int n = 0; n++;
	char szname[64]={0};
	sprintf(szname, "key%02d", n);
	t.SetName(szname);
	view.AddCameraKey(t);
	ui->modelViewer->Update(true);
}

void CMainWindow::on_actionViewVPPrev_triggered()
{
	CGView& view = *GetDocument()->GetView();
	if (view.CameraKeys() > 0)
	{
		view.PrevKey();
		RedrawGL();
	}
}

void CMainWindow::on_actionViewVPNext_triggered()
{
	CGView& view = *GetDocument()->GetView();
	if (view.CameraKeys() > 0)
	{
		view.NextKey();
		RedrawGL();
	}
}

void CMainWindow::UpdateMainToolbar()
{
	FEModel* pfem = m_doc->GetFEModel();
	ui->selectData->BuildMenu(pfem, DATA_SCALAR);
	ui->checkColormap(false);
	ui->actionViewSmooth->setChecked(true);
	UpdatePlayToolbar(true);

	const VIEWSETTINGS& settings = GetDocument()->GetViewSettings();
	ui->actionViewProjection->setChecked(settings.m_nproj == 0);
	ui->actionViewOutline->setChecked(settings.m_boutline);
	ui->actionViewMesh->setChecked(settings.m_bmesh);
	ui->actionViewShowTags->setChecked(settings.m_bTags);

}

void CMainWindow::UpdatePlayToolbar(bool breset)
{
	FEModel* pfem = m_doc->GetFEModel();
	if (pfem == 0) ui->playToolBar->setDisabled(true);
	else
	{
		if (breset)
		{
			int states = pfem->GetStates();
			QString suff = QString("/%1").arg(states);
			ui->pspin->setSuffix(suff);
			ui->pspin->setRange(1, states);
		};
		int n = pfem->currentTime();
		ui->pspin->setValue(n + 1);
		ui->playToolBar->setEnabled(true);
	}
}

void CMainWindow::on_selectTime_valueChanged(int i)
{
	GetDocument()->SetCurrentTime(i - 1);
	RedrawGL();
	UpdateGraphs(false);
}

void CMainWindow::UpdateFontToolbar()
{
	GLWidget* pw = GLWidget::get_focus();
	if (pw)
	{
		QFont font = pw->get_font();
		ui->pFontStyle->setCurrentFont(font);
		ui->pFontSize->setValue(font.pointSize());
		ui->actionFontBold->setChecked(font.bold());
		ui->actionFontItalic->setChecked(font.italic());
		ui->pFontToolBar->setEnabled(true);
	}
	else ui->pFontToolBar->setDisabled(true);
}


void CMainWindow::on_fontStyle_currentFontChanged(const QFont& font)
{
	GLWidget* pw = GLWidget::get_focus();
	if (pw)
	{
		QFont old_font = pw->get_font();
		std::string s = font.family().toStdString();
		QFont new_font(font.family(), old_font.pointSize()); 
		new_font.setBold(old_font.bold());
		new_font.setItalic(old_font.italic());
		pw->set_font(new_font);
		RedrawGL();
	}
}

void CMainWindow::on_fontSize_valueChanged(int i)
{
	GLWidget* pw = GLWidget::get_focus();
	if (pw)
	{
		QFont font = pw->get_font();
		font.setPointSize(i);
		pw->set_font(font);
		RedrawGL();
	}
}

void CMainWindow::on_fontBold_toggled(bool checked)
{
	GLWidget* pw = GLWidget::get_focus();
	if (pw)
	{
		QFont font = pw->get_font();
		font.setBold(checked);
		pw->set_font(font);
		RedrawGL();
	}
}

void CMainWindow::on_fontItalic_toggled(bool bchecked)
{
	GLWidget* pw = GLWidget::get_focus();
	if (pw)
	{
		QFont font = pw->get_font();
		font.setItalic(bchecked);
		pw->set_font(font);
		RedrawGL();
	}
}

void CMainWindow::closeEvent(QCloseEvent* ev)
{
	writeSettings();
	ev->accept();
}

QByteArray colorToByteArray(GLCOLOR c)
{
	QByteArray a;
	a.resize(4);
	a[0] = c.r;
	a[1] = c.g;
	a[2] = c.b;
	a[3] = c.a;
	return a;
}

GLCOLOR byteArrayToColor(const QByteArray& a)
{
	GLCOLOR c;
	if (a.size() == 4)
	{
		c.r = a[0];
		c.g = a[1];
		c.b = a[2];
		c.a = a[3];
	}
	return c;
}

void CMainWindow::writeSettings()
{
	QSettings settings("MRLSoftware", "PostView");
	settings.beginGroup("MainWindow");
	settings.setValue("geometry", saveGeometry());
	settings.setValue("state", saveState());
	settings.endGroup();

	VIEWSETTINGS& view = GetDocument()->GetViewSettings();
	settings.beginGroup("ViewSettings");
	settings.setValue("bgcol1", colorToByteArray(view.bgcol1));
	settings.setValue("bgcol2", colorToByteArray(view.bgcol2));
	settings.setValue("fgcol" , colorToByteArray(view.fgcol));
	settings.setValue("bgstyle"           , view.bgstyle);
	settings.setValue("m_bShadows"        , view.m_bShadows);
	settings.setValue("m_shadow_intensity", view.m_shadow_intensity);
	settings.setValue("m_ambient"         , view.m_ambient);
	settings.setValue("m_diffuse"         , view.m_diffuse);
	settings.setValue("m_bTriad"          , view.m_bTriad);
	settings.setValue("m_bTags"           , view.m_bTags);
	settings.setValue("m_ntagInfo"        , view.m_ntagInfo);
	settings.setValue("m_bTitle"          , view.m_bTitle);
//	settings.setValue("m_bconn"           , view.m_bconn);
	settings.setValue("m_bext"            , view.m_bext);
	settings.setValue("m_bmesh"           , view.m_bmesh);
	settings.setValue("m_boutline"        , view.m_boutline);
	settings.setValue("m_bBox"            , view.m_bBox);
	settings.setValue("m_nproj"           , view.m_nproj);
	settings.setValue("m_bLighting"       , view.m_bLighting);
	settings.setValue("m_bcull"           , view.m_bignoreBackfacingItems);
	settings.setValue("m_blinesmooth"     , view.m_blinesmooth);
	settings.setValue("m_flinethick"      , view.m_flinethick);
	settings.setValue("m_fpointsize"      , view.m_fpointsize);
	settings.endGroup();

	settings.beginGroup("FolderSettings");
	settings.setValue("currentPath", ui->currentPath);
	settings.endGroup();

	// store the user color maps
	int maps = ColorMapManager::ColorMaps();
	if (maps > ColorMapManager::USER)
	{
		settings.beginWriteArray("colorMaps");
		{
			for (int i=ColorMapManager::USER; i < maps; ++i)
			{
				CColorMap& map = ColorMapManager::GetColorMap(i);
				settings.setArrayIndex(i - ColorMapManager::USER);

				string name = ColorMapManager::GetColorMapName(i);
				settings.setValue("name", QString(name.c_str()));

				// save colors
				settings.beginWriteArray("colors");
				{
					int ncol = map.Colors();
					for (int j=0; j<ncol; ++j)
					{
						settings.setArrayIndex(j);
						settings.setValue("colorPos", map.GetColorPos(j));
						settings.setValue("rgbColor", toQColor(map.GetColor(j)));
					}
				}
				settings.endArray();
			}
		}
		settings.endArray();
	}

	QStringList folders = ui->fileViewer->FolderList();
	if (folders.isEmpty() == false) settings.setValue("folders", folders);
}

void CMainWindow::readSettings()
{
	QSettings settings("MRLSoftware", "PostView");
	settings.beginGroup("MainWindow");
	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("state").toByteArray());
	settings.endGroup();

	VIEWSETTINGS& view = GetDocument()->GetViewSettings();
	view.Defaults();
	settings.beginGroup("ViewSettings");
	view.bgcol1 = byteArrayToColor(settings.value("bgcol1", colorToByteArray(view.bgcol1)).toByteArray());
	view.bgcol2 = byteArrayToColor(settings.value("bgcol2", colorToByteArray(view.bgcol2)).toByteArray());
	view.fgcol  = byteArrayToColor(settings.value("fgcol" , colorToByteArray(view.fgcol )).toByteArray());
	view.bgstyle            = settings.value("bgstyle"      , view.bgstyle   ).toInt();
	view.m_bShadows         = settings.value("m_bShadows"   , view.m_bShadows).toBool();
	view.m_shadow_intensity = settings.value("m_shadow_intensity", view.m_shadow_intensity).toFloat();
	view.m_ambient          = settings.value("m_ambient"    , view.m_ambient).toFloat();
	view.m_diffuse          = settings.value("m_diffuse"    , view.m_diffuse).toFloat();
	view.m_bTriad           = settings.value("m_bTriad"     , view.m_bTriad).toBool();
	view.m_bTags            = settings.value("m_bTags"      , view.m_bTags ).toBool();
	view.m_ntagInfo         = settings.value("m_ntagInfo"   , view.m_ntagInfo).toInt();
	view.m_bTitle           = settings.value("m_bTitle"     , view.m_bTitle).toBool();
//	view.m_bconn            = settings.value("m_bconn"      , view.m_bconn).toBool();
	view.m_bext             = settings.value("m_bext"       , view.m_bext).toBool();
//	view.m_bmesh            = settings.value("m_bmesh"      , view.m_bmesh).toBool();
	view.m_boutline         = settings.value("m_boutline"   , view.m_boutline).toBool();
	view.m_bBox             = settings.value("m_bBox"       , view.m_bBox).toBool();
	view.m_nproj            = settings.value("m_nproj"      , view.m_nproj).toInt();
	view.m_bLighting        = settings.value("m_bLighting"  , view.m_bLighting).toBool();
	view.m_bignoreBackfacingItems = settings.value("m_bcull", view.m_bignoreBackfacingItems).toBool();
	view.m_blinesmooth      = settings.value("m_blinesmooth", view.m_blinesmooth).toBool();
	view.m_flinethick       = settings.value("m_flinethick" , view.m_flinethick).toFloat();
	view.m_fpointsize       = settings.value("m_fpointsize" , view.m_fpointsize).toFloat();
	settings.endGroup();

	settings.beginGroup("FolderSettings");
	ui->currentPath = settings.value("currentPath", QDir::homePath()).toString();
	settings.endGroup();

	// store the user color maps
	int userMaps = settings.beginReadArray("colorMaps");
	{
		for (int i = 0; i < userMaps; ++i)
		{
			settings.setArrayIndex(i);

			QString name = settings.value("name").toString();
			string sname = name.toStdString();

			CColorMap map;

			// read colors
			int colors = settings.beginReadArray("colors");
			map.SetColors(colors);
			{
				for (int j = 0; j<colors; ++j)
				{
					settings.setArrayIndex(j);
					float colorPos = settings.value("colorPos").toFloat();
					QColor color = settings.value("rgbColor").value<QColor>();

					map.SetColorPos(j, colorPos);
					map.SetColor(j, toGLColor(color));
				}
			}
			settings.endArray();

			ColorMapManager::AddColormap(sname, map);
		}
	}
	settings.endArray();

	QStringList folders = settings.value("folders").toStringList();
	if (folders.isEmpty() == false)
	{
		ui->fileViewer->SetFolderList(folders);
	}

	ui->fileViewer->setCurrentPath(ui->currentPath);

	// update the menu and toolbar to reflect the correct settings
	UpdateMainToolbar();
}

void CMainWindow::on_actionHelp_triggered()
{
//	QDesktopServices::openUrl(QUrl("http://help.mrl.sci.utah.edu/help/index.jsp"));
	QDesktopServices::openUrl(QUrl("http://help.mrl.sci.utah.edu/help/topic/org.febio.help.postview_um/html/title_page.html?cp=3"));
}

void CMainWindow::on_actionAbout_triggered()
{
	QString txt = QString("<h1>PostView</h1><p>version %1.%2.%3</p><p>Musculoskeletal Research Laboratories, University of Utah</p><p> Copyright (c) 2005 - 2017, All rights reserved</p>").arg(VERSION).arg(SUBVERSION).arg(SUBSUBVERSION);
	QMessageBox::information(this, "About PostView2", txt, QMessageBox::Close);
}

void CMainWindow::on_actionRecordNew_triggered()
{
#ifdef WIN32
	int noff = 1;
	QStringList filters;
	filters << "Windows AVI files (*.avi)"
			<< "Bitmap files (*.bmp)"
			<< "JPG files (*.jpg)"
			<< "PNG files (*.png)"
			<< "MPG files (*.mpg)";
#else
	int noff = 0;
	QStringList filters;
	filters << "Bitmap files (*.bmp)"
			<< "JPG files (*.jpg)"
			<< "Tiff files (*.tiff)"
			<< "MPG files (*.mpg)";
#endif

	QFileDialog dlg(this, "Save");
	dlg.setNameFilters(filters);
	dlg.setFileMode(QFileDialog::AnyFile);
	dlg.setAcceptMode(QFileDialog::AcceptSave);
	if (dlg.exec())
	{
		QString fileName = dlg.selectedFiles().first();
		string sfile = fileName.toStdString();
		char szfilename[512] = {0};
		sprintf(szfilename, "%s", sfile.c_str());
		int l = sfile.length();
		char* ch = strrchr(szfilename, '.');

		int nfilter = filters.indexOf(dlg.selectedNameFilter());

		CAnimation* panim = 0;
#ifdef WIN32
		if (nfilter == 0)
		{
			panim = new CAVIAnimation;
			if (ch == 0) sprintf(szfilename + l, ".avi");
			ui->glview->NewAnimation(szfilename, panim, GL_BGR_EXT);
		}
		else if (nfilter == noff)
#else
		if (nfilter == noff)
#endif
		{
			panim = new CBmpAnimation;
			if (ch == 0) sprintf(szfilename + l, ".bmp");
			ui->glview->NewAnimation(szfilename, panim);
		}
		else if (nfilter == noff + 1)
		{
			panim = new CJpgAnimation;
			if (ch == 0) sprintf(szfilename + l, ".jpg");
			ui->glview->NewAnimation(szfilename, panim);
		}
		else if (nfilter == noff + 2)
		{
			panim = new CPNGAnimation;
			if (ch == 0) sprintf(szfilename + l, ".png");
			ui->glview->NewAnimation(szfilename, panim);
		}
		else if (nfilter == noff + 3)
		{
#ifdef FFMPEG
			panim = new CMPEGAnimation;
			if (ch == 0) sprintf(szfilename + l, ".mpg");
			m_pGLView->NewAnimation(szfilename, panim);
#else
			QMessageBox::critical(this, "PostView2", "This video format is not supported in this version");
#endif
		}
		RedrawGL();
	}
}

void CMainWindow::on_actionRecordStart_triggered()
{
	ui->glview->StartAnimation();
}

void CMainWindow::on_actionRecordPause_triggered()
{
	ui->glview->PauseAnimation();
}

void CMainWindow::on_actionRecordStop_triggered()
{
	ui->glview->StopAnimation();
}
