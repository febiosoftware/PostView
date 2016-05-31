#include "MainWindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QtCore/QSettings>
#include "Document.h"
#include <PostViewLib/xpltReader.h>
#include "Document.h"
#include "GLModel.h"
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
#include <string>

CFileThread::CFileThread(CMainWindow* wnd, FEFileReader* file, const QString& fileName) : m_wnd(wnd), m_fileReader(file), m_fileName(fileName)
{
	QObject::connect(this, SIGNAL(resultReady(bool,const QString&)), wnd, SLOT(finishedReadingFile(bool, const QString&)));
	QObject::connect(this, SIGNAL(finished)   , this, SLOT(deleteLater));
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

CMainWindow::CMainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::CMainWindow)
{
	m_doc = new CDocument(this);
	ui->setupUi(this);
	readSettings();
}

CMainWindow::~CMainWindow()
{
}

CGLView* CMainWindow::GetGLView()
{
	return ui->glview;
}

void CMainWindow::UpdateUi(bool breset)
{
	// update the command panels
	ui->modelViewer->Update(breset);
	ui->matPanel->Update(breset);
	ui->dataPanel->Update(breset);
	ui->statePanel->Update(breset);
	ui->toolsPanel->Update(breset);

	// update all graph windows
	UpdateGraphs();

	// update the gl view
	ui->glview->GetCamera().Update(true);
	ui->glview->repaint();
}

void CMainWindow::UpdateView()
{
	ui->modelViewer->UpdateView();
}

void CMainWindow::UpdateTools()
{
	ui->toolsPanel->Update(false);
}

void CMainWindow::UpdateGraphs(bool breset)
{
	if (ui->graphList.isEmpty() == false)
	{
		QList<CGraphWindow*>::iterator it;
		for (it=ui->graphList.begin(); it != ui->graphList.end(); ++it)
			(*it)->Update(breset);
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
	case -1: reader = new XpltReader; break;
	case 0: 
		{
			XpltReader* xplt = new XpltReader;
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
	CFileThread* fileThread = new CFileThread(this, reader, fileName);
	fileThread->start();
	ui->statusBar->showMessage(QString("Reading file %1 ...").arg(fileName));
}

void CMainWindow::finishedReadingFile(bool success, const QString& errorString)
{
	ui->statusBar->clearMessage();

	if (success == false)
	{
		QMessageBox::critical(this, "PostView2", "Failed reading file");
		return;
	}

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

	// update all Ui components
	UpdateUi(true);
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
			bret = m_doc->ExportAscii(szfilename);
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
			bret = w.Save(fem, m_doc->currentTime(), szfilename);
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
		ui->selectData->BuildMenu(m_doc->GetFEModel(), DATA_SCALAR);

		// update the UI
		UpdateUi(true);
	}
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
	char szfile[1024] = {0};
	QString filename = QFileDialog::getOpenFileName(this, "Open session file", 0, "PostView session (*.pvs)");
	if (filename.isEmpty() == false)
	{
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

			char* ch = strrchr(szfile, '\\');
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
		const char* szfile = fileName.toStdString().c_str();
		if (m_doc->SaveSession(szfile) == false)
		{
			QMessageBox::critical(this, "PostView", "Failed storing PostView session.");
		}
	}
}

void CMainWindow::on_actionQuit_triggered()
{
	QApplication::quit();
}

void CMainWindow::on_selectNodes_triggered()
{
	m_doc->SetSelectionMode(SELECT_NODES);
	ui->glview->repaint();
}

void CMainWindow::on_selectEdges_triggered()
{
	m_doc->SetSelectionMode(SELECT_EDGES);
	ui->glview->repaint();
}

void CMainWindow::on_selectFaces_triggered()
{
	m_doc->SetSelectionMode(SELECT_FACES);
	ui->glview->repaint();
}

void CMainWindow::on_selectElems_triggered()
{
	m_doc->SetSelectionMode(SELECT_ELEMS);
	ui->glview->repaint();
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

void CMainWindow::on_actionHideSelected_triggered()
{
	CDocument* pdoc = GetDocument();
	if (pdoc->IsValid() == false) return;

	FEMesh* pm = pdoc->GetFEModel()->GetMesh();

	switch (pdoc->GetSelectionMode())
	{
	case SELECT_NODES: pm->HideSelectedNodes(); break;
	case SELECT_EDGES: pm->HideSelectedEdges(); break;
	case SELECT_FACES: pm->HideSelectedFaces(); break;
	case SELECT_ELEMS: pm->HideSelectedElements(); break;
	}

	pdoc->UpdateFEModel();
	ui->glview->repaint();
}

void CMainWindow::on_actionHideUnselected_triggered()
{
	CDocument* pdoc = GetDocument();
	FEMesh* pm = pdoc->GetFEModel()->GetMesh();

	pm->HideUnselectedElements();

	pdoc->UpdateFEModel();
	ui->glview->repaint();
}

void CMainWindow::on_actionInvertSelection_triggered()
{
	CDocument* pdoc = GetDocument();
	FEMesh* pfe = pdoc->GetFEModel()->GetMesh();

	for (int i=0; i<pfe->Elements(); i++)
	{
		FEElement& e = pfe->Element(i);
		if (e.IsVisible())
			if (e.IsSelected()) e.Unselect(); else e.Select();
	}

	pdoc->UpdateFEModel();
	ui->glview->repaint();
}

void CMainWindow::on_actionUnhideAll_triggered()
{
	CDocument* pdoc = GetDocument();
	if (pdoc->IsValid() == false) return;

	FEModel& fem = *pdoc->GetFEModel();
	FEMesh& mesh = *fem.GetMesh();

	for (int i=0; i<mesh.Elements(); ++i) mesh.Element(i).Unhide();
	for (int i=0; i<mesh.Faces   (); ++i) mesh.Face   (i).Unhide();
	for (int i=0; i<mesh.Edges   (); ++i) mesh.Edge   (i).Unhide();
	for (int i=0; i<mesh.Nodes   (); ++i) mesh.Node   (i).Unhide();

	pdoc->UpdateFEModel();
	ui->glview->repaint();
}

void CMainWindow::on_actionSelectAll_triggered()
{
	CDocument* pdoc = GetDocument();
	FEMesh* pfe = pdoc->GetFEModel()->GetMesh();

	for (int i=0; i<pfe->Elements(); i++) 
	{
		FEElement& e = pfe->Element(i);
		if (e.IsVisible()) e.Select();
	}

	pdoc->UpdateFEModel();
	ui->glview->repaint();
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
		pdoc->GetFEModel()->GetMesh()->ClearSelection(); 
		pdoc->UpdateFEModel();
		ui->glview->repaint();
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
		FEMesh* pm = doc.GetFEModel()->GetMesh();

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

		repaint();
	}
}

void CMainWindow::on_actionDelete_triggered()
{
	GLWidget* pg = GLWidget::get_focus();
	if (pg && (pg->GetObject() == 0))
	{
		CGLWidgetManager* pwm = CGLWidgetManager::GetInstance();
		pwm->RemoveWidget(pg);
		repaint();
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

	repaint();
}

void CMainWindow::on_actionPlaneCut_triggered()
{
	CDocument* pdoc = GetDocument();
	CGLPlaneCutPlot* pp = new CGLPlaneCutPlot(pdoc->GetGLModel());
	pdoc->AddPlot(pp);

	ui->modelViewer->Update(true);
	ui->modelViewer->selectObject(pp);
	ui->modelViewer->parentWidget()->raise();
	ui->glview->repaint();
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

	ui->glview->repaint();
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

	ui->glview->repaint();
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

	ui->glview->repaint();
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
	ui->glview->repaint();
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

		ui->glview->repaint();
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

	int nstep = pdoc->currentTime();

	if (time.m_mode == MODE_FORWARD)
	{
		nstep++;
		if (nstep >= N)
		{
			if (time.m_bloop) nstep = 0;
			else { nstep = N - 1; StopAnimation(); }
		}
	}
	else if (time.m_mode == MODE_REVERSE)
	{
		nstep--;
		if (nstep < 0) 
		{
			if (time.m_bloop) nstep = N - 1;
			else { nstep = 0; StopAnimation(); }
		}
	}
	else if (time.m_mode == MODE_CYLCE)
	{
		nstep += time.m_inc;
		if (nstep >= N)
		{
			time.m_inc = -1;
			nstep = N - 1;
			if (time.m_bloop == false) StopAnimation();
		}
		else if (nstep < 0)
		{
			time.m_inc = 1;
			nstep = 0;
			if (time.m_bloop == false) StopAnimation();
		}
	}
	SetCurrentTime(nstep);
}

void CMainWindow::on_actionFirst_triggered()
{
	SetCurrentTime(0);
}

void CMainWindow::on_actionPrev_triggered()
{
	CDocument* pdoc = GetDocument();
	int N = pdoc->GetFEModel()->GetStates();
	int nstep = pdoc->currentTime();
	nstep--;
	if (nstep < 0) nstep = 0;
	SetCurrentTime(nstep);
}

void CMainWindow::on_actionNext_triggered()
{
	CDocument* pdoc = GetDocument();
	int N = pdoc->GetFEModel()->GetStates();
	int nstep = pdoc->currentTime();
	nstep++;
	if (nstep >= N) nstep = N-1;
	SetCurrentTime(nstep);
}

void CMainWindow::on_actionLast_triggered()
{
	CDocument* pdoc = GetDocument();
	int N = pdoc->GetFEModel()->GetStates();
	SetCurrentTime(N-1);
}

void CMainWindow::on_actionTimeSettings_triggered()
{
	CDlgTimeSettings dlg(GetDocument(), this);
	if (dlg.exec())
	{
		repaint();
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
	ui->glview->repaint();
}

void CMainWindow::on_actionViewOutline_toggled(bool bchecked)
{
	CGLModel* po = GetDocument()->GetGLModel();
	po->m_boutline = !po->m_boutline;
	ui->glview->repaint();
}

void CMainWindow::on_actionViewShowTags_toggled(bool bchecked)
{
	VIEWSETTINGS& view = GetDocument()->GetViewSettings();
	view.m_bTags = bchecked;
	ui->glview->repaint();
}

void CMainWindow::on_actionViewSmooth_toggled(bool bchecked)
{
	CGLModel* po = GetDocument()->GetGLModel();
	CGLColorMap* pcm = po->GetColorMap();
	if (pcm)
	{
		CColorMap* pc = pcm->GetColorMap();
		if (pc)
		{
			bool b = pc->Smooth();
			pc->Smooth(!b);
			m_doc->UpdateFEModel();
			ui->glview->repaint();
		}
	}
}

void CMainWindow::on_actionViewCapture_toggled(bool bchecked)
{
	ui->glview->showSafeFrame(bchecked);
	ui->glview->repaint();
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
	ui->glview->repaint();
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
		ui->glview->repaint();
	}
}

void CMainWindow::on_actionViewVPNext_triggered()
{
	CGView& view = *GetDocument()->GetView();
	if (view.CameraKeys() > 0)
	{
		view.NextKey();
		ui->glview->repaint();
	}
}

void CMainWindow::UpdateMainToolbar()
{
	FEModel* pfem = m_doc->GetFEModel();
	ui->selectData->BuildMenu(pfem, DATA_SCALAR);
	UpdatePlayToolbar(true);
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
		ui->pspin->setValue(n);
	}
}

void CMainWindow::on_selectTime_valueChanged(int i)
{
	GetDocument()->SetCurrentTime(i - 1);
	repaint();
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
		repaint();
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
		repaint();
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
		repaint();
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
		repaint();
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
	settings.setValue("m_bconn"           , view.m_bconn);
	settings.setValue("m_bext"            , view.m_bext);
	settings.setValue("m_bmesh"           , view.m_bmesh);
	settings.setValue("m_bBox"            , view.m_bBox);
	settings.setValue("m_nproj"           , view.m_nproj);
	settings.setValue("m_bLighting"       , view.m_bLighting);
	settings.setValue("m_bcull"           , view.m_bcull);
	settings.setValue("m_blinesmooth"     , view.m_blinesmooth);
	settings.setValue("m_flinethick"      , view.m_flinethick);
	settings.setValue("m_fpointsize"      , view.m_fpointsize);
	settings.endGroup();
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
	view.m_bconn            = settings.value("m_bconn"      , view.m_bconn).toBool();
	view.m_bext             = settings.value("m_bext"       , view.m_bext).toBool();
	view.m_bmesh            = settings.value("m_bmesh"      , view.m_bmesh).toBool();
	view.m_bBox             = settings.value("m_bBox"       , view.m_bBox).toBool();
	view.m_nproj            = settings.value("m_nproj"      , view.m_nproj).toInt();
	view.m_bLighting        = settings.value("m_bLighting"  , view.m_bLighting).toBool();
	view.m_bcull            = settings.value("m_bcull"      , view.m_bcull).toBool();
	view.m_blinesmooth      = settings.value("m_blinesmooth", view.m_blinesmooth).toBool();
	view.m_flinethick       = settings.value("m_flinethick" , view.m_flinethick).toFloat();
	view.m_fpointsize       = settings.value("m_fpointsize" , view.m_fpointsize).toFloat();
	settings.endGroup();
}
