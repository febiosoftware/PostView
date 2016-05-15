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
#include <PostViewLib/FEBioPlotExport.h>
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
#include <string>

CMainWindow::CMainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::CMainWindow)
{
	m_doc = new CDocument(this);
	ui->setupUi(this);
}

CMainWindow::~CMainWindow()
{
}

CGLView* CMainWindow::GetGLView()
{
	return ui->glview;
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

void CMainWindow::UpdateView()
{
	ui->modelViewer->UpdateView();
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

bool CMainWindow::OpenFile(const QString& fileName, int nfilter)
{
	std::string sfile = fileName.toStdString();

	ui->actionColorMap->setDisabled(true);

	setWindowTitle(QString("PostView2"));

	XpltReader* reader = new XpltReader;

	if (nfilter == 0)
	{
		CDlgImportXPLT dlg(this);
		if (dlg.exec())
		{
			reader->SetReadStateFlag(dlg.m_nop);
			reader->SetReadStatesList(dlg.m_item);
		}
		else return false;
	}

	if (m_doc->LoadFEModel(reader, sfile.c_str()) == false)
	{
		QMessageBox::critical(this, "PostView2", "Failed reading file");
	}
	else
	{
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
	}

	ui->selectData->BuildMenu(m_doc->GetFEModel(), DATA_SCALAR);
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
		ui->selectData->BuildMenu(m_doc->GetFEModel(), DATA_SCALAR);

		// update the UI
		UpdateUi();
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
			UpdateUi();
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

void CMainWindow::on_actionHideSelected_triggered()
{
	CDocument* pdoc = GetDocument();
	if (pdoc->IsValid() == false) return;

	FEMesh* pm = pdoc->GetFEModel()->GetMesh();

	switch (pdoc->GetSelectionMode())
	{
	case SELECT_NODES: pm->HideSelectedNodes(); break;
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
	FEMesh* pfe = pdoc->GetFEModel()->GetMesh();
	FEModel* ps = pdoc->GetFEModel();

	// only unhide the visible materials
	for (int i=0; i<ps->Materials(); i++) 
	{
		FEMaterial* pm = ps->GetMaterial(i);
		if (pm->bvisible) pfe->ShowElements(i);
	}

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
		case SELECT_FACES: pdoc->SelectFacesInRange(dlg.m_min, dlg.m_max, dlg.m_brange); break;
		case SELECT_ELEMS: pdoc->SelectElemsInRange(dlg.m_min, dlg.m_max, dlg.m_brange); break;
		}
		
		pdoc->UpdateFEModel();
		UpdateUi();
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
	if (nview == SELECT_FACES) nsel = 1;
	if (nview == SELECT_ELEMS) nsel = 2;

	CDlgFind dlg(this, nsel);

	if (dlg.exec())
	{
		FEMesh* pm = doc.GetFEModel()->GetMesh();

		if (dlg.m_bsel[0]) nview = SELECT_NODES;
		if (dlg.m_bsel[1]) nview = SELECT_FACES;
		if (dlg.m_bsel[2]) nview = SELECT_ELEMS;

		switch (nview)
		{
		case SELECT_NODES: on_selectNodes_triggered(); pm->SelectNodes   (dlg.m_item, dlg.m_bclear); break;
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

	repaint();
}

void CMainWindow::on_actionPlaneCut_triggered()
{
	CDocument* pdoc = GetDocument();
	CGLPlaneCutPlot* pp = new CGLPlaneCutPlot(pdoc->GetGLModel());
	pdoc->AddPlot(pp);

	ui->modelViewer->Update();
	ui->modelViewer->selectObject(pp);
	ui->glview->repaint();
}

void CMainWindow::on_actionVectorPlot_triggered()
{
	CDocument* pdoc = GetDocument();
	CGLVectorPlot* pp = new CGLVectorPlot(pdoc->GetGLModel());
	pdoc->AddPlot(pp);
	pdoc->UpdateFEModel();
	
	ui->modelViewer->Update();
	ui->modelViewer->selectObject(pp);

	ui->glview->repaint();
}

void CMainWindow::on_actionIsosurfacePlot_triggered()
{
	CDocument* pdoc = GetDocument();
	CGLIsoSurfacePlot* pp = new CGLIsoSurfacePlot(pdoc->GetGLModel());
	pdoc->AddPlot(pp);
	pdoc->UpdateFEModel();

	ui->modelViewer->Update();
	ui->modelViewer->selectObject(pp);

	ui->glview->repaint();
}

void CMainWindow::on_actionSlicePlot_triggered()
{
	CDocument* pdoc = GetDocument();
	CGLSlicePlot* pp = new CGLSlicePlot(pdoc->GetGLModel());
	pdoc->AddPlot(pp);
	pdoc->UpdateFEModel();

	ui->modelViewer->Update();
	ui->modelViewer->selectObject(pp);

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

void CMainWindow::on_actionSummary_triggered()
{
}

void CMainWindow::on_actionStats_triggered()
{
}

void CMainWindow::on_actionIntegrate_triggered()
{
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
	ui->modelViewer->Update();
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
