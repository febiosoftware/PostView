#include "MainWindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QDesktopServices>
#include <QtCore/QMimeData>
#include "DocManager.h"
#include "Document.h"
#include <PostGL/GLModel.h>
#include <XPLTLib/xpltFileReader.h>
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
#include <PostViewLib/VRMLExporter.h>
#include <PostGL/GLPlaneCutPlot.h>
#include <PostGL/GLMirrorPlane.h>
#include <PostGL/GLIsoSurfacePlot.h>
#include <PostGL/GLSlicePLot.h>
#include <PostGL/GLVectorPlot.h>
#include <PostGL/GLTensorPlot.h>
#include <PostGL/GLStreamLinePlot.h>
#include <PostGL/GLParticleFlowPlot.h>
#include "DlgViewSettings.h"
#include "DlgExportXPLT.h"
#include "DlgExportLSDYNA.h"
#include "DlgWidgetProps.h"
#include "DlgFind.h"
#include "DlgImportXPLT.h"
#include "DlgSelectRange.h"
#include "DlgTimeSettings.h"
#include "DlgFileInfo.h"
#include "DlgExportAscii.h"
#include "DlgExportVTK.h"
#include "version.h"
#include "convert.h"
#include <PostViewLib/ImgAnimation.h>
#include <PostViewLib/AVIAnimation.h>
#include <PostViewLib/MPEGAnimation.h>
#include "DocManager.h"
#include <PostViewLib/ImageModel.h>
#include <PostViewLib/ImageSlicer.h>
#include <PostViewLib/VolRender.h>
#include <PostViewLib/MarchingCubes.h>
#include <string>
#include <QStyleFactory>
using namespace Post;

// create a dark style theme (work in progress)
void darkStyle()
{
	qApp->setStyle(QStyleFactory::create("Fusion"));
	QPalette palette = qApp->palette();
	palette.setColor(QPalette::Window, QColor(53, 53, 53));
	palette.setColor(QPalette::WindowText, Qt::white);
	palette.setColor(QPalette::Base, QColor(30, 30, 30));
	palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
	palette.setColor(QPalette::ToolTipBase, Qt::white);
	palette.setColor(QPalette::ToolTipText, Qt::white);
	palette.setColor(QPalette::Text, Qt::white);
	palette.setColor(QPalette::Button, QColor(53, 53, 53));
	palette.setColor(QPalette::ButtonText, Qt::white);
	palette.setColor(QPalette::BrightText, Qt::red);
	palette.setColor(QPalette::Highlight, QColor(51, 153, 255));
	palette.setColor(QPalette::HighlightedText, Qt::white);
	palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
	palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
	qApp->setPalette(palette);
}

CMainWindow::CMainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::CMainWindow)
{
	m_DocManager = new CDocManager(this);
	ui->setupUi(this);
	m_fileThread = 0;
	m_activeDoc = nullptr;

	m_ops.Defaults();

	// initialize color maps
	// (This must be done before we read the settings!)
	ColorMapManager::Initialize();

	// read settings from last session
	readSettings();

	// activate dark style
	if (ui->m_theme == 1)
	{
		darkStyle();

		VIEWSETTINGS& view = GetViewSettings();
		view.bgcol1 = GLColor(83, 83, 83);
		view.bgcol2 = GLColor(0, 0, 0);
		view.bgstyle = BG_COLOR_1;

		GLWidget::set_base_color(GLColor(255,255,255));

		// adjust some toolbar buttons
		ui->actionFontBold->setIcon(QIcon(":/icons/font_bold_neg.png"));
		ui->actionFontItalic->setIcon(QIcon(":/icons/font_italic_neg.png"));
	}

	// make sure the file viewer is visible
	ui->fileViewer->parentWidget()->raise();

	setAcceptDrops(true);
}

CMainWindow::~CMainWindow()
{
}

CDocument*	CMainWindow::GetActiveDocument()
{ 
	return m_activeDoc;
}

void CMainWindow::dragEnterEvent(QDragEnterEvent* event)
{
	const QMimeData* mime = event->mimeData();
	if (mime->hasFormat("text/uri-list"))
	{
		event->accept();
	}
}

void CMainWindow::dropEvent(QDropEvent* event)
{
	QString fileName = event->mimeData()->text();

	const QMimeData* mimeData = event->mimeData();
	if (mimeData->hasUrls())
	{
		QList<QUrl> urlList = mimeData->urls();

		// extract the local paths of the files
		QString fileName;
		fileName = urlList.at(0).toLocalFile();

		// call a function to open the files
		OpenFile(fileName, -1);
	}
}

CDocument* CMainWindow::NewDocument(const std::string& docTitle)
{
	CDocument* doc = new CDocument(this);
	doc->SetTitle(docTitle);
	AddDocument(doc);
	MakeDocActive(doc);
	return doc;
}

// get the current them
int CMainWindow::currentTheme()
{
	return ui->m_theme;
}

// set current theme (must restart to take effect)
void CMainWindow::setCurrentTheme(int n)
{
	ui->m_theme = n;
}

void CMainWindow::SetStatusMessage(const QString& message)
{
	ui->statusBar->showMessage(message);
}

void CMainWindow::ClearStatusMessage()
{
	ui->statusBar->clearMessage();
}

void CMainWindow::UpdateStatusMessage()
{
	ClearStatusMessage();
	CDocument* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
		CGLModel& mdl = *doc->GetGLModel();
		FEMeshBase* mesh = doc->GetActiveMesh();
		if (mesh)
		{
			int selectionMode = mdl.GetSelectionMode();

			int N = 0;
			char sz[128] = {0};
			switch (selectionMode)
			{
			case SELECT_NODES:
				{
					// count nodal selection
					int nn = -1, i;
					for (i = 0; i<mesh->Nodes(); ++i)
					{
						if (mesh->Node(i).IsSelected())
						{
							N++;
							nn = i;
						}
					}

					if (N == 1)
					{
						FENode& n = mesh->Node(nn);
						vec3f r = n.m_rt;
						float f = mdl.currentState()->m_NODE[nn].m_val;
						sprintf(sz, "1 node selected: Id = %d, val = %g, pos = (%g, %g, %g)", nn + 1, f, r.x, r.y, r.z);
					}
					else if (N > 1)
					{
						sprintf(sz, "%d nodes selected", N);
					}
				}
				break;
			case SELECT_EDGES:
				{
					// count edge selection
					int nn = -1, i;
					for (i = 0; i<mesh->Edges(); ++i)
					{
						if (mesh->Edge(i).IsSelected())
						{
							N++;
							nn = i;
						}
					}

					if (N == 1)
					{
						FEEdge& n = mesh->Edge(nn);
						float f = mdl.currentState()->m_EDGE[nn].m_val;
						sprintf(sz, "1 edge selected");
					}
					else if (N > 1)
					{
						sprintf(sz, "%d edges selected", N);
					}
				}
				break;
			case SELECT_FACES:
				{
					// count selection
					int nn = -1, i;
					for (i = 0; i<mesh->Faces(); ++i)
					{
						if (mesh->Face(i).IsSelected())
						{
							N++;
							nn = i;
						}
					}

					if (N == 1)
					{
						sprintf(sz, "1 face selected: Id = %d", nn + 1);
					}
					else if (N > 1)
					{
						sprintf(sz, "%d faces selected", N);
					}
				}
				break;
			case SELECT_ELEMS:
				{
					int ne = -1, i;
					for (i = 0; i<mesh->Elements(); ++i)
					{
						if (mesh->Element(i).IsSelected())
						{
							N++;
							ne = i;
						}
					}
					if (N == 1)
					{
						sprintf(sz, "1 element selected: Id = %d", ne + 1);
					}
					else if (N > 1)
					{
						sprintf(sz, "%d elements selected", N);
					}
				}
				break;
			}

			if (N > 0) SetStatusMessage(sz);
			else ClearStatusMessage();
		}
	}
}

void CMainWindow::SetWindowTitle(const QString& t)
{
	QString title = "PostView2";
	if (t.isEmpty() == false)
	{
		if (t.length() > 50)
		{
			QString txt = t.right(50);
			title += QString(" - ...%1").arg(txt);
		}
		else title += QString(" - %1").arg(t);
	}

	setWindowTitle(title);
}

CGLView* CMainWindow::GetGLView()
{
	return ui->glview;
}

// update all the command panels
void CMainWindow::UpdateCommandPanels(bool breset, QWidget* psender)
{
	if (psender != ui->modelViewer) ui->modelViewer->Update(breset);
	if (psender != ui->matPanel   ) ui->matPanel->Update(breset);
	if (psender != ui->dataPanel  ) ui->dataPanel->Update(breset);
	if (psender != ui->statePanel ) ui->statePanel->Update(breset);
	if (psender != ui->toolsPanel ) ui->toolsPanel->Update(breset);
	if (psender != ui->timePanel  ) ui->timePanel->Update(breset);
}

void CMainWindow::UpdateUi(bool breset, QWidget* psender)
{
	// update the command panels
	UpdateCommandPanels(breset, psender);

	// update all graph windows
	UpdateGraphs(breset, breset);

	// update the gl view
	ui->glview->UpdateCamera(true);

	// redraw the Graphics View
	RedrawGL();
}

void CMainWindow::UpdateModelViewer(bool breset)
{
	ui->modelViewer->Update(breset);
}

void CMainWindow::RedrawGL()
{
	ui->glview->repaint();
}

void CMainWindow::CheckUi()
{
	// check the color map state
	CDocument* doc = GetActiveDocument();
	CGLModel* po = (doc ? doc->GetGLModel() : nullptr);
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

void CMainWindow::UpdateTools(bool reset)
{
	ui->toolsPanel->Update(reset);
}

// remove a graph from the list
void CMainWindow::RemoveGraph(CGraphWindow* graph)
{
	ui->graphList.removeOne(graph);
}

// Add a graph to the list of managed graph windows
void CMainWindow::AddGraph(CGraphWindow* graph)
{
	CDocument* doc = GetActiveDocument();
	QString title;
	if (doc) title = " - " + QString::fromStdString(doc->GetFileName());
	graph->setWindowTitle(QString("PostView2 : Graph%1%2").arg(ui->graphList.size() + 1).arg(title));
	ui->graphList.push_back(graph);
}

void CMainWindow::UpdateGraphs(bool breset, bool bfit)
{
	if (ui->graphList.isEmpty() == false)
	{
		QList<CGraphWindow*>::iterator it;
		for (it=ui->graphList.begin(); it != ui->graphList.end(); ++it)
			if ((*it)->isVisible()) (*it)->Update(breset, bfit);
	}
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
	// Stop the timer if it's running
	if (ui->m_isAnimating) ui->m_isAnimating = false;

	std::string sfile = fileName.toStdString();

	// create a file reader
	FEFileReader* reader = 0;

	// If filter not specified, use extension
	if (nfilter == -1)
	{
		std::string sfile = fileName.toStdString();
		// conver to lower case
		std::transform(sfile.begin(), sfile.end(), sfile.begin(), ::tolower);
		
		int npos = (int)sfile.find_last_of('.');
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
/*	int npos = sfile.rfind('/');
	if (npos == std::string::npos) npos = sfile.rfind('\\');
	if (npos != std::string::npos)
	{
		stitle = sfile.substr(npos+1);
	}
*/
	// Create a new document
	CDocument* doc = new CDocument(this);

	// create the file reading thread and run it
	m_fileThread = new CFileThread(this, doc, reader, fileName);
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
		}
		ui->stopFileReading->setDisabled(true);
		delete m_fileThread->GetDocument();
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

void CMainWindow::AddDocument(CDocument* doc)
{
	m_DocManager->AddDocument(doc);
	AddDocumentTab(doc);
}

void CMainWindow::AddDocumentTab(CDocument* doc)
{
	ui->addTab(QString::fromStdString(doc->GetFileName()), doc->GetFile());
}

void CMainWindow::finishedReadingFile(bool success, const QString& errorString)
{
	CDocument* doc = m_fileThread->GetDocument();
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

	// Add the document
	AddDocument(doc);

	// Make it the active document
	MakeDocActive(doc);

	// show the model viewer
	ui->modelViewer->parentWidget()->raise();

	// add file to recent list
	ui->addToRecentFiles(doc->GetFile());
}

bool CMainWindow::SaveFile(const QString& fileName, int nfilter)
{
	if (fileName.isEmpty()) return false;
	string sfilename = fileName.toStdString();
	const char* szfilename = sfilename.c_str();

	CDocument* doc = GetActiveDocument();
	if ((doc == nullptr) || (doc->IsValid() == false)) return true;

	FEModel& fem = *doc->GetFEModel();

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
				if (dlg.m_nstep == 0) n0 = n1 = doc->currentTime();
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
			VRMLExporter exporter;
			bret = exporter.Save(&fem, szfilename);
		}
		break;
	case 4:
		{
			CDlgExportLSDYNA dlg(this);
			if (dlg.exec())
			{
				FELSDYNAExport w;
				w.m_bsel = dlg.m_bsel;
				w.m_bsurf = dlg.m_bsurf;
				w.m_bnode = dlg.m_bnode;
				bret = w.Save(fem, doc->currentTime(), szfilename);
			}
		}
		break;
	case 5:
		{
			bret = doc->ExportBYU(szfilename);
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
			CDlgExportVTK dlg(this);
			if (dlg.exec())
			{
				FEVTKExport w;
				w.ExportAllStates(dlg.m_ops[0]);
				bret = w.Save(fem, szfilename);
				error = "Failed writing VTK file";
			}
		}
		break;
	case 8:
		{
			CDlgExportLSDYNAPlot dlg(&fem, this);
			if (dlg.exec())
			{
				FELSDYNAPlotExport ls;
				bret = ls.Save(fem, szfilename, dlg.m_flag, dlg.m_code);
				error = "Failed writing LSDYNA database file";
			}
		}
		break;
	default:
		assert(false);
		error = "Unknown file type";
		break;
	}

	if (bret == false)
	{
		QMessageBox b;
		b.setText(QString("Failed saving file.\nReason:%1").arg(error));
		b.setIcon(QMessageBox::Critical);
		b.exec();
	}
	else
	{
		QMessageBox::information(this, "PostView2", "Success saving file!");
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

	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;

	if (doc->LoadFEModel(0, doc->GetFile(), true) == false)
	{
		QMessageBox::critical(this, tr("PostView2"), "Failed updating the model");
	}
	else if (doc->IsValid())
	{
		int N = doc->GetFEModel()->GetStates();
		if (N > 1) ui->playToolBar->setEnabled(true);

		FEModel* fem = doc->GetFEModel();
		int nfield = doc->GetEvalField();

		// we need to update the model viewer before we rebuild the selection menu
		ui->modelViewer->Update(true);

		// now, we can rebuild the 
		ui->selectData->BuildMenu(doc->GetFEModel(), DATA_SCALAR);
		ui->selectData->setCurrentValue(nfield);
		ui->actionColorMap->setDisabled(false);

		// update the UI
		UpdateMainToolbar();
		UpdatePlayToolbar(true);
		UpdateCommandPanels(true);
		ui->glview->UpdateCamera(true);
		RedrawGL();
	}
}

void CMainWindow::on_actionFileInfo_triggered()
{
	CDlgFileInfo dlg;
	CDocument* doc = GetActiveDocument();

	FEModel* fem = (doc ? doc->GetFEModel() : nullptr);
	if (fem)
	{
		MetaData& md = fem->GetMetaData();
		dlg.setSoftware(QString::fromStdString(md.software));
		dlg.exec();
	}
	else
	{
		QMessageBox::information(this, "Info", "No model has been loaded");
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
		<< "LSDYNA database (*.d3plot)";

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
		OpenSession(filename);
	}
}

void CMainWindow::on_actionSaveSession_triggered()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Save session", 0, "PostView session (*.pvs)");
	if (fileName.isEmpty() == false)
	{
		std::string sfile = fileName.toStdString();
		const char* szfile = sfile.c_str();
		if (m_DocManager->SaveSession(szfile) == false)
		{
			QMessageBox::critical(this, "PostView", "Failed storing PostView session.");
		}
		else
		{
			// add file to recent list
			ui->addToRecentSessions(fileName);
		}
	}
}

void CMainWindow::OpenSession(const QString& fileName)
{
	std::string sfile = fileName.toStdString();
	const char* szfile = sfile.c_str();

	int docs = m_DocManager->Documents();

	// try to open the session
	if (m_DocManager->OpenSession(szfile) == false)
	{
		QMessageBox::critical(this, "PostView", "Failed restoring session.");
	}
	else
	{
		// add file to recent list
		ui->addToRecentSessions(fileName);
	}

	// add all the new docs
	for (int i = docs; i < m_DocManager->Documents(); ++i)
	{
		AddDocumentTab(m_DocManager->GetDocument(i));
	}

	if ((docs == 0) && (m_DocManager->Documents() > 0))
	{
		// Make it the active document
		MakeDocActive(m_DocManager->GetDocument(0));
	}

	ui->modelViewer->parentWidget()->raise();
	ui->modelViewer->parentWidget()->show();
}

void CMainWindow::on_actionQuit_triggered()
{
	writeSettings();
	QApplication::quit();
}

void CMainWindow::on_selectNodes_triggered()
{
	CDocument* doc = GetActiveDocument();
	if ((doc == nullptr) || (doc->IsValid() == false)) return;

	CGLModel* model = doc->GetGLModel(); assert(model);
	if (model == 0) return;
	
	int oldMode = model->GetSelectionMode();

	model->ConvertSelection(oldMode, SELECT_NODES);

	model->SetSelectionMode(SELECT_NODES);

	RedrawGL();
}

void CMainWindow::on_selectEdges_triggered()
{
	CDocument* doc = GetActiveDocument();
	if ((doc == nullptr) || (doc->IsValid() == false)) return;

	CGLModel* model = doc->GetGLModel(); assert(model);
	if (model == 0) return;

	model->SetSelectionMode(SELECT_EDGES);
	RedrawGL();
}

void CMainWindow::on_selectFaces_triggered()
{
	CDocument* doc = GetActiveDocument();
	if ((doc == nullptr) || (doc->IsValid() == false)) return;

	CGLModel* model = doc->GetGLModel(); assert(model);
	if (model == 0) return;

	model->SetSelectionMode(SELECT_FACES);
	RedrawGL();
}

void CMainWindow::on_selectElems_triggered()
{
	CDocument* doc = GetActiveDocument();
	if ((doc == nullptr) || (doc->IsValid() == false)) return;

	CGLModel* model = doc->GetGLModel(); assert(model);
	if (model == 0) return;

	model->SetSelectionMode(SELECT_ELEMS);
	RedrawGL();
}

void CMainWindow::on_actionSelectRect_triggered()
{
	CDocument* doc = GetActiveDocument();
	if ((doc == nullptr) || (doc->IsValid() == false)) return;

	CGLModel* model = doc->GetGLModel(); assert(model);
	if (model == 0) return;

	model->SetSelectionStyle(SELECT_RECT);
}

void CMainWindow::on_actionSelectCircle_triggered()
{
	CDocument* doc = GetActiveDocument();
	if ((doc == nullptr) || (doc->IsValid() == false)) return;

	CGLModel* model = doc->GetGLModel(); assert(model);
	if (model == 0) return;

	model->SetSelectionStyle(SELECT_CIRCLE);
}

void CMainWindow::on_actionSelectFree_triggered()
{
	CDocument* doc = GetActiveDocument();
	if ((doc == nullptr) || (doc->IsValid() == false)) return;

	CGLModel* model = doc->GetGLModel(); assert(model);
	if (model == 0) return;

	model->SetSelectionStyle(SELECT_FREE);
}

void CMainWindow::on_actionZoomSelected_triggered()
{
	ui->glview->OnZoomSelect();
}

void CMainWindow::on_actionZoomExtents_triggered()
{
	ui->glview->OnZoomExtents();
}

void CMainWindow::on_actionSelectConn_toggled(bool b)
{
	GetViewSettings().m_bconn = b;
}

void CMainWindow::on_actionHideSelected_triggered()
{
	CDocument* doc = GetActiveDocument();
	if ((doc == nullptr) || (doc->IsValid() == false)) return;

	CGLModel& mdl = *doc->GetGLModel();

	switch (mdl.GetSelectionMode())
	{
	case SELECT_NODES: mdl.HideSelectedNodes(); break;
	case SELECT_EDGES: mdl.HideSelectedEdges(); break;
	case SELECT_FACES: mdl.HideSelectedFaces(); break;
	case SELECT_ELEMS: mdl.HideSelectedElements(); break;
	}

	UpdateStatusMessage();
	doc->UpdateFEModel();
	RedrawGL();
}

void CMainWindow::on_actionHideUnselected_triggered()
{
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;

	CGLModel& mdl = *doc->GetGLModel();

	mdl.HideUnselectedElements();

	doc->UpdateFEModel();
	RedrawGL();
}

void CMainWindow::on_actionInvertSelection_triggered()
{
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;

	CGLModel& mdl = *doc->GetGLModel();

	int mode = mdl.GetSelectionMode();
	switch (mode)
	{
	case SELECT_NODES: mdl.InvertSelectedNodes(); break;
	case SELECT_EDGES: mdl.InvertSelectedEdges(); break;
	case SELECT_FACES: mdl.InvertSelectedFaces(); break;
	case SELECT_ELEMS: mdl.InvertSelectedElements(); break;
	}

	UpdateStatusMessage();
	doc->UpdateFEModel();
	RedrawGL();
}

void CMainWindow::on_actionUnhideAll_triggered()
{
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;
	if (doc->IsValid() == false) return;
	CGLModel& mdl = *doc->GetGLModel();
	mdl.UnhideAll();
	doc->UpdateFEModel();
	RedrawGL();
}

void CMainWindow::on_actionSelectAll_triggered()
{
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;
	CGLModel* m = doc->GetGLModel();

	int mode = m->GetSelectionMode();
	switch (mode)
	{
	case SELECT_NODES: m->SelectAllNodes(); break;
	case SELECT_EDGES: m->SelectAllEdges(); break;
	case SELECT_FACES: m->SelectAllFaces(); break;
	case SELECT_ELEMS: m->SelectAllElements(); break;
	}

	UpdateStatusMessage();
	doc->UpdateFEModel();
	RedrawGL();
}

void CMainWindow::on_actionSelectRange_triggered()
{
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;
	if (!doc->IsValid()) return;

	CGLModel* model = doc->GetGLModel(); assert(model);
	if (model == 0) return;

	CGLColorMap* pcol = doc->GetGLModel()->GetColorMap();
	if (pcol == 0) return;

	float d[2];
	pcol->GetRange(d);

	CDlgSelectRange dlg(this);
	dlg.m_min = d[0];
	dlg.m_max = d[1];

	if (dlg.exec())
	{
		switch (model->GetSelectionMode())
		{
		case SELECT_NODES: doc->SelectNodesInRange(dlg.m_min, dlg.m_max, dlg.m_brange); break;
		case SELECT_EDGES: doc->SelectEdgesInRange(dlg.m_min, dlg.m_max, dlg.m_brange); break;
		case SELECT_FACES: doc->SelectFacesInRange(dlg.m_min, dlg.m_max, dlg.m_brange); break;
		case SELECT_ELEMS: doc->SelectElemsInRange(dlg.m_min, dlg.m_max, dlg.m_brange); break;
		}
		
		CGLModel& mdl = *doc->GetGLModel();
		mdl.UpdateSelectionLists();
		UpdateStatusMessage();
		doc->UpdateFEModel();
		UpdateUi(false);
	}
}

void CMainWindow::on_actionClearSelection_triggered()
{
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;
	if (doc->IsValid())
	{
		CGLModel& mdl = *doc->GetGLModel();
		mdl.ClearSelection(); 
		UpdateStatusMessage();
		doc->UpdateFEModel();
		RedrawGL();
	}
}

void CMainWindow::on_actionFind_triggered()
{
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;
	if (doc->IsValid() == false) return;

	CGLModel* model = doc->GetGLModel(); assert(model);
	if (model == 0) return;

	int nview = model->GetSelectionMode();
	int nsel = 0;
	if (nview == SELECT_NODES) nsel = 0;
	if (nview == SELECT_EDGES) nsel = 1;
	if (nview == SELECT_FACES) nsel = 2;
	if (nview == SELECT_ELEMS) nsel = 3;

	CDlgFind dlg(this, nsel);

	if (dlg.exec())
	{
		CGLModel* pm = doc->GetGLModel();

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

		doc->GetGLModel()->UpdateSelectionLists();
		UpdateStatusMessage();
		RedrawGL();
	}
}

void CMainWindow::on_actionDelete_triggered()
{
	GLWidget* pg = GLWidget::get_focus();
	if (pg)
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
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;
	if (doc->IsValid() == false) return;

	CGLPlaneCutPlot* pp = new CGLPlaneCutPlot(doc->GetGLModel());
	doc->AddPlot(pp);

	ui->modelViewer->Update(true);
	ui->modelViewer->selectObject(pp);
	ui->modelViewer->parentWidget()->raise();
	RedrawGL();
}

void CMainWindow::on_actionMirrorPlane_triggered()
{
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;
	if (doc->IsValid() == false) return;

	CGLMirrorPlane* pp = new CGLMirrorPlane(doc->GetGLModel());
	doc->AddPlot(pp);

	ui->modelViewer->Update(true);
	ui->modelViewer->selectObject(pp);
	ui->modelViewer->parentWidget()->raise();
	RedrawGL();
}

void CMainWindow::on_actionVectorPlot_triggered()
{
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;
	if (doc->IsValid() == false) return;

	CGLVectorPlot* pp = new CGLVectorPlot(doc->GetGLModel());
	doc->AddPlot(pp);
	doc->UpdateFEModel();
	
	ui->modelViewer->Update(true);
	ui->modelViewer->selectObject(pp);
	ui->modelViewer->parentWidget()->raise();

	RedrawGL();
}

void CMainWindow::on_actionTensorPlot_triggered()
{
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;
	if (doc->IsValid() == false) return;

	GLTensorPlot* pp = new GLTensorPlot(doc->GetGLModel());
	doc->AddPlot(pp);
	doc->UpdateFEModel();

	ui->modelViewer->Update(true);
	ui->modelViewer->selectObject(pp);
	ui->modelViewer->parentWidget()->raise();

	RedrawGL();
}

void CMainWindow::on_actionStreamLinePlot_triggered()
{
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;
	if (doc->IsValid() == false) return;

	CGLStreamLinePlot* pp = new CGLStreamLinePlot(doc->GetGLModel());
	doc->AddPlot(pp);
	doc->UpdateFEModel();

	ui->modelViewer->Update(true);
	ui->modelViewer->selectObject(pp);
	ui->modelViewer->parentWidget()->raise();

	RedrawGL();
}

void CMainWindow::on_actionParticleFlowPlot_triggered()
{
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;
	if (doc->IsValid() == false) return;

	CGLParticleFlowPlot* pp = new CGLParticleFlowPlot(doc->GetGLModel());
	doc->AddPlot(pp);
	doc->UpdateFEModel();

	ui->modelViewer->Update(true);
	ui->modelViewer->selectObject(pp);
	ui->modelViewer->parentWidget()->raise();

	RedrawGL();
}

void CMainWindow::on_actionImageSlicer_triggered()
{
	CGLObject* po = ui->modelViewer->selectedObject();
	CImageModel* img = dynamic_cast<CImageModel*>(po);
	if (img == nullptr)
	{
		QMessageBox::critical(this, "PostView", "Please select an image data set first.");
	}
	else
	{
		CImageSlicer* slicer = new CImageSlicer(img);
		slicer->Create();
		img->AddImageRenderer(slicer);
		ui->modelViewer->Update(true);
		ui->modelViewer->selectObject(slicer);
		RedrawGL();
	}
}

void CMainWindow::on_actionVolumeRender_triggered()
{
	CGLObject* po = ui->modelViewer->selectedObject();
	CImageModel* img = dynamic_cast<CImageModel*>(po);
	if (img == nullptr)
	{
		QMessageBox::critical(this, "PostView", "Please select an image data set first.");
	}
	else
	{
		CVolRender* vr = new CVolRender(img);
		vr->Create();
		img->AddImageRenderer(vr);
		ui->modelViewer->Update(true);
		ui->modelViewer->selectObject(vr);
		RedrawGL();
	}
}

void CMainWindow::on_actionMarchingCubes_triggered()
{
	CGLObject* po = ui->modelViewer->selectedObject();
	CImageModel* img = dynamic_cast<CImageModel*>(po);
	if (img == nullptr)
	{
		QMessageBox::critical(this, "PostView", "Please select an image data set first.");
	}
	else
	{
		CMarchingCubes* mc = new CMarchingCubes(img);
		mc->Create();
		img->AddImageRenderer(mc);
		ui->modelViewer->Update(true);
		ui->modelViewer->selectObject(mc);
		RedrawGL();
	}
}

void CMainWindow::on_actionIsosurfacePlot_triggered()
{
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;
	if (doc->IsValid() == false) return;

	CGLIsoSurfacePlot* pp = new CGLIsoSurfacePlot(doc->GetGLModel());
	doc->AddPlot(pp);
	doc->UpdateFEModel();

	ui->modelViewer->Update(true);
	ui->modelViewer->selectObject(pp);
	ui->modelViewer->parentWidget()->raise();

	RedrawGL();
}

void CMainWindow::on_actionSlicePlot_triggered()
{
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;
	if (doc->IsValid() == false) return;

	CGLSlicePlot* pp = new CGLSlicePlot(doc->GetGLModel());
	doc->AddPlot(pp);
	doc->UpdateFEModel();

	ui->modelViewer->Update(true);
	ui->modelViewer->selectObject(pp);
	ui->modelViewer->parentWidget()->raise();

	RedrawGL();
}

void CMainWindow::on_actionDisplacementMap_triggered()
{
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;
	if (doc->IsValid() == false) return;

	CGLModel* pm = doc->GetGLModel();
	if (pm->GetDisplacementMap() == 0)
	{
		if (pm->AddDisplacementMap() == false)
		{
			QMessageBox::warning(this, "PostView", "You need at least one vector field before you can define a displacement map.");
		}
		else
		{
			doc->UpdateFEModel(true);
			ui->modelViewer->Update(true);
		}
	}
	else
	{
		QMessageBox::information(this, "PostView", "This model already has a displacement map.");
	}
}

void CMainWindow::on_actionGraph_triggered()
{
	CGraphWindow* pg = new CModelGraphWindow(this);
	AddGraph(pg);

	pg->show();
	pg->raise();
	pg->activateWindow();
	pg->Update();
}

void CMainWindow::on_actionSummary_triggered()
{
	CSummaryWindow* summaryWindow = new CSummaryWindow(this);

	summaryWindow->Update(true);
	summaryWindow->show();
	summaryWindow->raise();
	summaryWindow->activateWindow();

	AddGraph(summaryWindow);
}

void CMainWindow::on_actionStats_triggered()
{
	CStatsWindow* statsWindow = new CStatsWindow(this);
	statsWindow->Update(true);
	statsWindow->show();
	statsWindow->raise();
	statsWindow->activateWindow();

	AddGraph(statsWindow);
}

void CMainWindow::on_actionIntegrate_triggered()
{
	CIntegrateWindow* integrateWindow = new CIntegrateWindow(this);
	integrateWindow->Update(true);
	integrateWindow->show();
	integrateWindow->raise();
	integrateWindow->activateWindow();

	AddGraph(integrateWindow);
}

void CMainWindow::on_actionColorMap_toggled(bool bchecked)
{
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;
	CGLModel* po = doc->GetGLModel();
	po->GetColorMap()->Activate(bchecked);
	UpdateModelViewer(false);
	doc->UpdateFEModel();
	RedrawGL();
}

void CMainWindow::SetCurrentDataField(int nfield)
{
	ui->selectData->setCurrentValue(nfield);
}

void CMainWindow::on_selectData_currentValueChanged(int index)
{
	if (index == -1)
		ui->actionColorMap->setDisabled(true);
	else
	{
		if (ui->actionColorMap->isEnabled() == false)
			ui->actionColorMap->setEnabled(true);

		int nfield = ui->selectData->currentValue();
		CDocument* doc = GetActiveDocument();
		if (doc == nullptr) return;
		CGLModel* pm = doc->GetGLModel();
		pm->GetColorMap()->SetEvalField(nfield);

		// turn on the colormap
		if (ui->actionColorMap->isChecked() == false)
		{
			ui->actionColorMap->toggle();
		}
		else doc->UpdateFEModel();

		ui->glview->UpdateWidgets(false);
		RedrawGL();
	}

	UpdateGraphs(false);

	if (ui->modelViewer->isVisible()) ui->modelViewer->Update(false);
}

void CMainWindow::on_actionPlay_toggled(bool bchecked)
{
	CDocument* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
		if (bchecked)
		{
			TIMESETTINGS& time = doc->GetTimeSettings();
			double fps = time.m_fps;
			if (fps < 1.0) fps = 1.0;
			double msec_per_frame = 1000.0 / fps;

			ui->m_isAnimating = true;
			QTimer::singleShot(msec_per_frame, this, SLOT(onTimer()));
		}
		else ui->m_isAnimating = false;
	}
}

void CMainWindow::SetCurrentTime(int n)
{
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;

	doc->SetCurrentTime(n);

	// update the spinbox value
	// Changing the value will trigger a signal, which will call this function again
	// To avoid this recursive call, we set a flag
	ui->m_update_spin = false;
	ui->pspin->setValue(n + 1);
	ui->m_update_spin = true;

	// update the rest
	ui->timePanel->Update(false);
	UpdateTools(false);
	UpdateGraphs(false);
	RedrawGL();
}

void CMainWindow::SetCurrentTimeValue(float ftime)
{
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;

	int n0 = doc->currentTime();
	doc->SetCurrentTimeValue(ftime);
	int n1 = doc->currentTime();

	if (n0 != n1)
	{
		ui->m_update_spin = false;
		ui->pspin->setValue(n1 + 1);
		ui->m_update_spin = true;
	}

	// update the rest
	ui->timePanel->Update(false);
	UpdateTools(false);
	UpdateGraphs(false);
	RedrawGL();
}

void CMainWindow::StopAnimation()
{
	ui->m_isAnimating = false;
	ui->actionPlay->setChecked(false);
}

void CMainWindow::onTimer()
{
	if (ui->m_isAnimating == false) return;

	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;
	TIMESETTINGS& time = doc->GetTimeSettings();

	int N = doc->GetFEModel()->GetStates();
	int N0 = time.m_start;
	int N1 = time.m_end;

	float f0 = doc->GetTimeValue(N0);
	float f1 = doc->GetTimeValue(N1);

	int nstep = doc->currentTime();

	if (time.m_bfix)
	{
		float ftime = doc->GetTimeValue();
		
		if (time.m_mode == MODE_FORWARD)
		{
			ftime += time.m_dt;
			if (ftime > f1)
			{
				if (time.m_bloop) ftime = f0;
				else { ftime = f1; StopAnimation(); }
			}
		}
		else if (time.m_mode == MODE_REVERSE)
		{
			ftime -= time.m_dt;
			if (ftime < f0)
			{
				if (time.m_bloop) ftime = f1;
				else { ftime = f0; StopAnimation(); }
			}
		}
		else if (time.m_mode == MODE_CYLCE)
		{
			ftime += time.m_dt*time.m_inc;
			if (ftime > f1)
			{
				time.m_inc = -1;
				ftime = f1; 
				if (time.m_bloop == false) StopAnimation();
			}
			else if (ftime < f0)
			{
				time.m_inc = 1;
				ftime = f0;
				if (time.m_bloop == false) StopAnimation();
			}
		}

		SetCurrentTimeValue(ftime);
	}
	else
	{
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
	}

	RedrawGL();

	// TODO: Should I start the event before or after the view is redrawn?
	if (ui->m_isAnimating)
	{
		CDocument* doc = GetActiveDocument();
		if (doc == nullptr) return;
		if (doc->IsValid())
		{
			TIMESETTINGS& time = doc->GetTimeSettings();
			double fps = time.m_fps;
			if (fps < 1.0) fps = 1.0;
			double msec_per_frame = 1000.0 / fps;
			QTimer::singleShot(msec_per_frame, this, SLOT(onTimer()));
		}
	}
}

void CMainWindow::on_actionFirst_triggered()
{
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;
	TIMESETTINGS& time = doc->GetTimeSettings();
	SetCurrentTime(time.m_start);
}

void CMainWindow::on_actionPrev_triggered()
{
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;
	TIMESETTINGS& time = doc->GetTimeSettings();
	int nstep = doc->currentTime();
	nstep--;
	if (nstep < time.m_start) nstep = time.m_start;
	SetCurrentTime(nstep);
}

void CMainWindow::on_actionNext_triggered()
{
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;
	TIMESETTINGS& time = doc->GetTimeSettings();
	int nstep = doc->currentTime();
	nstep++;
	if (nstep > time.m_end) nstep = time.m_end;
	SetCurrentTime(nstep);
}

void CMainWindow::on_actionLast_triggered()
{
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;
	TIMESETTINGS& time = doc->GetTimeSettings();
	SetCurrentTime(time.m_end);
}

void CMainWindow::on_actionTimeSettings_triggered()
{
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;

	CDlgTimeSettings dlg(doc, this);
	if (dlg.exec())
	{
		TIMESETTINGS& time = doc->GetTimeSettings();
		ui->timePanel->SetRange(time.m_start, time.m_end);

		int ntime = doc->currentTime();
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
	VIEWSETTINGS& view = GetViewSettings();
	view.m_bmesh = bchecked;
	RedrawGL();
}

void CMainWindow::on_actionViewOutline_toggled(bool bchecked)
{
	VIEWSETTINGS& view = GetViewSettings();
	view.m_boutline = bchecked;
	RedrawGL();
}

void CMainWindow::on_actionViewShowTags_toggled(bool bchecked)
{
	VIEWSETTINGS& view = GetViewSettings();
	view.m_bTags = bchecked;
	RedrawGL();
}

void CMainWindow::on_actionViewSmooth_toggled(bool bchecked)
{
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;

	CGLModel* po = doc->GetGLModel();
	if (po)
	{
		CGLColorMap* pcm = po->GetColorMap();
		if (pcm)
		{
			pcm->SetColorSmooth(bchecked);
			RedrawGL();
		}
	}
}

void CMainWindow::on_actionViewCapture_toggled(bool bchecked)
{
	ui->glview->showSafeFrame(bchecked);
	RedrawGL();
}

void CMainWindow::on_actionViewWidgets_toggled(bool bchecked)
{
	ui->glview->showWidgets(bchecked);
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
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;

	CGLCamera& cam = ui->glview->GetCamera();
	GLCameraTransform t;
	cam.GetTransform(t);

	CGView& view = *doc->GetView();
	static int n = 0; n++;
	char szname[64]={0};
	sprintf(szname, "key%02d", n);
	t.SetName(szname);
	view.AddCameraKey(t);
	ui->modelViewer->Update(true);
}

void CMainWindow::on_actionViewVPPrev_triggered()
{
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;

	CGView& view = *doc->GetView();
	if (view.CameraKeys() > 0)
	{
		view.PrevKey();
		RedrawGL();
	}
}

void CMainWindow::on_actionViewVPNext_triggered()
{
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;

	CGView& view = *doc->GetView();
	if (view.CameraKeys() > 0)
	{
		view.NextKey();
		RedrawGL();
	}
}

// sync the views of all documents to the currently active one
void CMainWindow::on_actionSyncViews_triggered()
{
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;

	CGView& view = *doc->GetView();
	CGLCamera& cam = view.GetCamera();
	GLCameraTransform transform;
	cam.GetTransform(transform);
	for (int i = 0; i < m_DocManager->Documents(); ++i)
	{
		CDocument* doci = m_DocManager->GetDocument(i);
		if (doci != doc)
		{
			CGLCamera& cami = doci->GetView()->GetCamera();

			// copy the transforms
			cami.SetTransform(transform);
			cami.UpdatePosition(true);
		}
	}
}

void CMainWindow::UpdateMainToolbar(bool breset)
{
	CDocument* doc = GetActiveDocument();
	if ((doc == nullptr) || (doc->IsValid() == false)) return;

	FEModel* pfem = doc->GetFEModel();
	ui->selectData->BuildMenu(pfem, DATA_SCALAR);

	if (breset == false)
	{
		CGLColorMap* map = doc->GetGLModel()->GetColorMap();
		if (map)
		{
			int nfield = map->GetEvalField();
			SetCurrentDataField(nfield);
		}

		ui->checkColormap(map->IsActive());
		ui->actionViewSmooth->setChecked(map->GetColorSmooth());
	}
	else ui->actionViewSmooth->setChecked(true);

	UpdatePlayToolbar(true);

	CGLModel* m = doc->GetGLModel();
	if (m)
	{
		int mode = m->GetSelectionMode();
		if (mode == SELECT_NODES) ui->selectNodes->setChecked(true);
		if (mode == SELECT_EDGES) ui->selectEdges->setChecked(true);
		if (mode == SELECT_FACES) ui->selectFaces->setChecked(true);
		if (mode == SELECT_ELEMS) ui->selectElems->setChecked(true);
	}

	const VIEWSETTINGS& settings = GetViewSettings();
	ui->actionViewProjection->setChecked(settings.m_nproj == 0);
	ui->actionViewOutline->setChecked(settings.m_boutline);
	ui->actionViewMesh->setChecked(settings.m_bmesh);
	ui->actionViewShowTags->setChecked(settings.m_bTags);
	ui->pangle->setValue((int)settings.m_angleTol);
}

void CMainWindow::UpdatePlayToolbar(bool breset)
{
	CDocument* doc = GetActiveDocument();
	if (doc == nullptr) return;

	CGLModel* mdl = doc->GetGLModel();
	if (mdl == 0) ui->playToolBar->setDisabled(true);
	else
	{
		int ntime = mdl->currentTimeIndex() + 1;

		if (breset)
		{
			FEModel* fem = mdl->GetFEModel();
			int states = fem->GetStates();
			QString suff = QString("/%1").arg(states);
			ui->pspin->setSuffix(suff);

			ui->pspin->setRange(1, states);
		};
		ui->pspin->setValue(ntime);
		ui->playToolBar->setEnabled(true);
	}
}

void CMainWindow::on_selectTime_valueChanged(int i)
{
	if (ui->m_update_spin) SetCurrentTime(i - 1);
}

void CMainWindow::on_selectAngle_valueChanged(int i)
{
	VIEWSETTINGS& vs = GetViewSettings();
	vs.m_angleTol = (float)i;
}

void CMainWindow::on_tab_currentChanged(int i)
{
	CDocument* doc = m_DocManager->GetDocument(i);
	MakeDocActive(doc);
}

void CMainWindow::on_tab_tabCloseRequested(int i)
{
	m_activeDoc = nullptr;
	m_DocManager->RemoveDocument(i);
	ui->RemoveTab(i);
}

void CMainWindow::keyPressEvent(QKeyEvent* ev)
{
	switch (ev->key())
	{
	case Qt::Key_Backtab:
	case Qt::Key_Tab:
		if (ev->modifiers() & Qt::ControlModifier)
		{
			ev->accept();

			int docs = m_DocManager->Documents();
			if (docs > 1)
			{
				// activate the next document
				int i = ui->tab->currentIndex();

				// activate next or prev one 
				if (ev->key() == Qt::Key_Backtab)
				{
					i--;
					if (i < 0) i = docs - 1;
				}
				else
				{
					++i;
					if (i >= docs) i = 0;
				}
				ui->tab->setCurrentIndex(i);
			}
		}
		break;
	}
}

void CMainWindow::MakeDocActive(CDocument* doc)
{
	m_activeDoc = doc;

	if (doc)
	{
		// set the window title
		SetWindowTitle(QString(doc->GetFile()));

		CGLModel* m = doc->GetGLModel();
		if (m)
		{
			int layer = m->m_layer;
			CGLWidgetManager::GetInstance()->SetActiveLayer(layer);
			ui->glview->UpdateWidgets();
		}

		// update all command panels
		UpdateCommandPanels(true);

		// update the main toolbar
		UpdateMainToolbar(false);

		// This is already done in UpdateMainToolbar so I can probably remove this
		FEModel* fem = doc->GetFEModel();
		if (fem && fem->GetStates() > 0)
		{
			ui->playToolBar->setEnabled(true);
		}
		else
		{
			ui->playToolBar->setDisabled(true);
		}

		// redraw
		ui->glview->GetCamera().UpdatePosition(true);
		RedrawGL();
	}
	else
	{
		SetWindowTitle("");
		CGLWidgetManager::GetInstance()->SetActiveLayer(0);
		UpdateUi(true);
		UpdateMainToolbar();
		ui->playToolBar->setDisabled(true);
	}
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

QByteArray colorToByteArray(GLColor c)
{
	QByteArray a;
	a.resize(4);
	a[0] = c.r;
	a[1] = c.g;
	a[2] = c.b;
	a[3] = c.a;
	return a;
}

GLColor byteArrayToColor(const QByteArray& a)
{
	GLColor c;
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
	settings.setValue("theme", ui->m_theme);
	settings.endGroup();

	VIEWSETTINGS& view = GetViewSettings();
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
    settings.setValue("m_nconv"           , view.m_nconv);
	settings.setValue("m_bLighting"       , view.m_bLighting);
	settings.setValue("m_bcull"           , view.m_bignoreBackfacingItems);
	settings.setValue("m_blinesmooth"     , view.m_blinesmooth);
	settings.setValue("m_flinethick"      , view.m_flinethick);
	settings.setValue("m_fspringthick"    , view.m_fspringthick);
	settings.setValue("m_fpointsize"      , view.m_fpointsize);
	settings.setValue("colorMaps"         , ColorMapManager::UserColorMaps());
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

	if (ui->m_recentFiles.isEmpty() == false) settings.setValue("recentFiles", ui->m_recentFiles);
	if (ui->m_recentSessions.isEmpty() == false) settings.setValue("recentSessions", ui->m_recentSessions);
}

void CMainWindow::readSettings()
{
	QSettings settings("MRLSoftware", "PostView");
	settings.beginGroup("MainWindow");
	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("state").toByteArray());
	ui->m_theme = settings.value("theme", 0).toInt();
	settings.endGroup();

	int userColorMaps = -1;
	VIEWSETTINGS& view = GetViewSettings();
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
    view.m_nconv            = settings.value("m_nconv"      , view.m_nconv).toInt();
	view.m_bLighting        = settings.value("m_bLighting"  , view.m_bLighting).toBool();
	view.m_bignoreBackfacingItems = settings.value("m_bcull", view.m_bignoreBackfacingItems).toBool();
	view.m_blinesmooth      = settings.value("m_blinesmooth", view.m_blinesmooth).toBool();
	view.m_flinethick       = settings.value("m_flinethick" , view.m_flinethick).toFloat();
	view.m_fspringthick     = settings.value("m_fspringthick", view.m_fspringthick).toFloat();
	view.m_fpointsize       = settings.value("m_fpointsize" , view.m_fpointsize).toFloat();
	userColorMaps = settings.value("colorMaps", -1).toInt();
	settings.endGroup();

	settings.beginGroup("FolderSettings");
	ui->currentPath = settings.value("currentPath", QDir::homePath()).toString();
	settings.endGroup();

	// store the user color maps
	int userMaps = settings.beginReadArray("colorMaps");
	{
		if ((userColorMaps == -1) || (userColorMaps > userMaps)) userColorMaps = userMaps;
		for (int i = 0; i < userColorMaps; ++i)
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

	QStringList recentFiles = settings.value("recentFiles").toStringList();
	ui->setRecentFiles(recentFiles);

	QStringList recentSessions = settings.value("recentSessions").toStringList();
	ui->setRecentSessions(recentSessions);

	// update the menu and toolbar to reflect the correct settings
	UpdateMainToolbar();
}

void CMainWindow::on_actionHelp_triggered()
{
	QDesktopServices::openUrl(QUrl("https://help.febio.org/PostView/PostView_2_3/index.html"));
}

void CMainWindow::on_actionAbout_triggered()
{
	QString txt = QString("<h1>PostView</h1><p>Version %1.%2.%3</p><p>Musculoskeletal Research Laboratories, University of Utah</p><p> Copyright (c) 2005 - 2019, All rights reserved</p>").arg(VERSION).arg(SUBVERSION).arg(SUBSUBVERSION);

	QMessageBox about(this);
	about.setWindowTitle("About PostView");
	about.setText(txt);
	about.setIconPixmap(QPixmap(":/icons/postview_medium.png"));
	about.exec();
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
		int l = (int)sfile.length();
		char* ch = strrchr(szfilename, '.');

		int nfilter = filters.indexOf(dlg.selectedNameFilter());

		bool bret = false;
		CAnimation* panim = 0;
#ifdef WIN32
		if (nfilter == 0)
		{
			panim = new CAVIAnimation;
			if (ch == 0) sprintf(szfilename + l, ".avi");
			bret = ui->glview->NewAnimation(szfilename, panim, GL_BGR_EXT);
		}
		else if (nfilter == noff)
#else
		if (nfilter == noff)
#endif
		{
			panim = new CBmpAnimation;
			if (ch == 0) sprintf(szfilename + l, ".bmp");
			bret = ui->glview->NewAnimation(szfilename, panim);
		}
		else if (nfilter == noff + 1)
		{
			panim = new CJpgAnimation;
			if (ch == 0) sprintf(szfilename + l, ".jpg");
			bret = ui->glview->NewAnimation(szfilename, panim);
		}
		else if (nfilter == noff + 2)
		{
			panim = new CPNGAnimation;
			if (ch == 0) sprintf(szfilename + l, ".png");
			bret = ui->glview->NewAnimation(szfilename, panim);
		}
		else if (nfilter == noff + 3)
		{
#ifdef FFMPEG
			panim = new CMPEGAnimation;
			if (ch == 0) sprintf(szfilename + l, ".mpg");
			bret = ui->glview->NewAnimation(szfilename, panim);
#else
			QMessageBox::critical(this, "PostView2", "This video format is not supported in this version");
#endif
		}

		if (bret)
		{
			ui->m_old_title = windowTitle();
			setWindowTitle(ui->m_old_title + "   (RECORDING PAUSED)");
		}
		else bret = QMessageBox::critical(this, "PostView", "Failed creating animation stream.");

		RedrawGL();
	}
}

void CMainWindow::on_actionRecordStart_triggered()
{
	if (ui->glview->HasRecording())
	{
		if (ui->m_old_title.isEmpty()) ui->m_old_title = windowTitle();
			
		setWindowTitle(ui->m_old_title + "   (RECORDING)");

		ui->glview->StartAnimation();
	}
	else QMessageBox::information(this, "PostView", "You need to create a new video file before you can start recording");
}

void CMainWindow::on_actionRecordPause_triggered()
{
	if (ui->glview->HasRecording())
	{
		if (ui->glview->AnimationMode() == ANIM_RECORDING)
		{
			ui->glview->PauseAnimation();
			setWindowTitle(ui->m_old_title + "   (RECORDING PAUSED)");
		}
	}
	else QMessageBox::information(this, "PostView", "You need to create a new video file first.");
}

void CMainWindow::on_actionRecordStop_triggered()
{
	if (ui->glview->HasRecording())
	{
		if (ui->glview->AnimationMode() != ANIM_STOPPED)
		{
			ui->glview->StopAnimation();
			setWindowTitle(ui->m_old_title);
		}

		ui->m_old_title.clear();
	}
	else QMessageBox::information(this, "PostView", "You need to create a new video file first.");
}

void CMainWindow::on_recentFiles_triggered(QAction* action)
{
	QString fileName = action->text();
	OpenFile(fileName, 0);
}

void CMainWindow::on_recentSessions_triggered(QAction* action)
{
	QString fileName = action->text();
	OpenSession(fileName);
}

// show data in a graph window
void CMainWindow::ShowData(const std::vector<double>& data, const QString& label)
{
	CDataGraphWindow* graph = new CDataGraphWindow(this);
	AddGraph(graph);

	graph->SetData(data, label);
	graph->show();
	graph->raise();
	graph->activateWindow();
}
