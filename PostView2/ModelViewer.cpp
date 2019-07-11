#include "ModelViewer.h"
#include <QBoxLayout>
#include <QTreeWidget>
#include <QTableWidget>
#include <QTabWidget>
#include <QMessageBox>
#include <QHeaderView>
#include <QCheckBox>
#include <QSplitter>
#include <QLabel>
#include <QToolButton>
#include "MainWindow.h"
#include "Document.h"
#include "PropertyListView.h"
#include "GLModel.h"
#include <PostViewLib/FEModel.h>
#include "GLPlot.h"
#include "GLPlaneCutPlot.h"
#include "GLVectorPlot.h"
#include "GLSlicePLot.h"
#include "GLIsoSurfacePlot.h"
#include "GLLinePlot.h"
#include "GLStreamLinePlot.h"
#include "GLParticleFlowPlot.h"
#include "GLTensorPlot.h"
#include <PostViewLib/3DImage.h>
#include <PostViewLib/VolRender.h>
#include <PostViewLib/ImageSlicer.h>
#include <PostViewLib/ImageModel.h>
#include <PostViewLib/GLImageRenderer.h>
#include <PostViewLib/MarchingCubes.h>
#include "GLMirrorPlane.h"
#include "ImageViewer.h"
#include "Histogram.h"

//-----------------------------------------------------------------------------
class CModelProps : public CPropertyList
{
public:
	CModelProps(CGLModel* fem) : m_fem(fem) 
	{
		addProperty("Element subdivions"       , CProperty::Int)->setIntRange(0, 100).setAutoValue(true);
		addProperty("Render mode"              , CProperty::Enum, "Render mode")->setEnumValues(QStringList() << "default" << "wireframe" << "solid");
		addProperty("Render undeformed outline", CProperty::Bool);
		addProperty("Outline color"            , CProperty::Color);
		addProperty("Node color"               , CProperty::Color);
		addProperty("Selection color"          , CProperty::Color);
		addProperty("Render smooth"            , CProperty::Bool);
		addProperty("Shells as hexes"          , CProperty::Bool);
		addProperty("Shell reference surface"  , CProperty::Enum, "set the shell reference surface")->setEnumValues(QStringList() << "Mid surface" << "bottom surface" << "top surface");
		addProperty("Smoothing angle"          , CProperty::Float);
	}

	QVariant GetPropertyValue(int i)
	{
		QVariant v;
		switch (i)
		{
		case 0: v = m_fem->m_nDivs; break;
		case 1: v = m_fem->m_nrender; break;
		case 2: v = m_fem->m_bghost; break;
		case 3: v = toQColor(m_fem->m_line_col); break;
		case 4: v = toQColor(m_fem->m_node_col); break;
		case 5: v = toQColor(m_fem->m_sel_col); break;
		case 6: v = m_fem->m_bsmooth; break;
		case 7: v = m_fem->m_bShell2Hex; break;
		case 8: v = m_fem->m_nshellref; break;
		case 9: v = m_fem->GetSmoothingAngle(); break;
		}
		return v;
	}

	void SetPropertyValue(int i, const QVariant& v)
	{
		switch (i)
		{
		case 0: m_fem->m_nDivs    = v.toInt(); break;
		case 1: m_fem->m_nrender  = v.toInt(); break;
		case 2: m_fem->m_bghost   = v.toBool(); break;
		case 3: m_fem->m_line_col = toGLColor(v.value<QColor>());
		case 4: m_fem->m_node_col = toGLColor(v.value<QColor>());
		case 5: m_fem->m_sel_col  = toGLColor(v.value<QColor>());
		case 6: m_fem->m_bsmooth  = v.toBool(); break;
		case 7: m_fem->m_bShell2Hex = v.toBool(); break;
		case 8: m_fem->m_nshellref = v.toInt(); break;
		case 9: 
			m_fem->SetSmoothingAngle(v.toDouble()); 
			break;
		}
	}
	
private:
	CGLModel*	m_fem;
};

//-----------------------------------------------------------------------------
class CMeshProps : public CPropertyList
{
public:
	CMeshProps(FEModel* fem) : m_fem(fem) 
	{
		FEMeshBase& mesh = *fem->GetFEMesh(0);
		addProperty("Nodes"         , CProperty::Int, "Number of nodes"         )->setFlags(CProperty::Visible);
		addProperty("Faces"         , CProperty::Int, "Number of faces"         )->setFlags(CProperty::Visible);
		addProperty("Solid Elements", CProperty::Int, "Number of solid elements")->setFlags(CProperty::Visible);
		addProperty("Shell Elements", CProperty::Int, "Number of shell elemetns")->setFlags(CProperty::Visible);
	}

	QVariant GetPropertyValue(int i)
	{
		QVariant v;
		if (m_fem)
		{
			FEMeshBase& mesh = *m_fem->GetFEMesh(0);
			switch (i)
			{
			case 0: v = mesh.Nodes(); break;
			case 1: v = mesh.Faces(); break;
			case 2: v = mesh.SolidElements(); break;
			case 3: v = mesh.ShellElements(); break;
			}
		}
		return v;
	}

	void SetPropertyValue(int i, const QVariant& v) { }

private:
	FEModel*	m_fem;
};

//-----------------------------------------------------------------------------
class CDisplacementMapProps : public CPropertyList
{
public:
	CDisplacementMapProps(CMainWindow* wnd, CGLDisplacementMap* map) : m_wnd(wnd), m_map(map)
	{
		addProperty("Data field"  , CProperty::DataVec3);
		addProperty("Scale factor", CProperty::Float   );
	}

	QVariant GetPropertyValue(int i)
	{
		if (m_map)
		{
			if (i==0)
			{
				FEModel* pfem = m_map->GetModel()->GetFEModel();
				return pfem->GetDisplacementField();
			}
			if (i==1) return m_map->m_scl;
		}
		return QVariant();
	}

	void SetPropertyValue(int i, const QVariant& v)
	{
		if (i==0)
		{
			FEModel* pfem = m_map->GetModel()->GetFEModel();
			pfem->SetDisplacementField(v.toInt());
		}
		if (i==1) m_map->m_scl = v.toFloat();
		m_wnd->GetActiveDocument()->UpdateFEModel();
	}

private:
	CGLDisplacementMap*	m_map;
	CMainWindow*	m_wnd;
};

//-----------------------------------------------------------------------------
class CColorMapProps : public CPropertyList
{
public:
	CColorMapProps(CMainWindow* wnd, CGLColorMap* map) : m_wnd(wnd), m_map(map)
	{
		QStringList cols;

		for (int i=0; i<ColorMapManager::ColorMaps(); ++i)
		{
			string name = ColorMapManager::GetColorMapName(i);
			cols << name.c_str();
		}

		addProperty("Data field"        , CProperty::DataScalar);
		addProperty("Gradient smoothing", CProperty::Bool);
		addProperty("Color map"         , CProperty::Enum)->setEnumValues(cols);
		addProperty("Nodal Values"      , CProperty::Bool);
		addProperty("Range type"        , CProperty::Enum)->setEnumValues(QStringList() << "dynamic" << "static" << "user");
		addProperty("Range divisions"   , CProperty::Int)->setIntRange(1, 100);
		addProperty("Show Legend"       , CProperty::Bool);
		addProperty("Legend orientation", CProperty::Enum)->setEnumValues(QStringList() << "Horizontal" << "Vertical");
		addProperty("User max"          , CProperty::Float);
		addProperty("User min"          , CProperty::Float);
	}

	QVariant GetPropertyValue(int i)
	{
		if (m_map)
		{
			float rng[2];
			m_map->GetRange(rng);
			switch (i)
			{
			case 0: return m_map->GetEvalField(); break;
			case 1: return m_map->GetColorSmooth(); break;
			case 2: return m_map->GetColorMap()->GetColorMap();
			case 3: return m_map->m_bDispNodeVals; break;
			case 4: return m_map->GetRangeType(); break;
			case 5: return m_map->GetColorMap()->GetDivisions(); break;
			case 6: return m_map->ShowLegend(); break;
			case 7: return m_map->m_pbar->Orientation(); break;
			case 8: return rng[1]; break;
			case 9: return rng[0]; break;
			}
		}
		return QVariant();
	}

	void SetPropertyValue(int i, const QVariant& v)
	{
		if (m_map == 0) return;
		float rng[2];
		m_map->GetRange(rng);

		switch (i)
		{
		case 0: m_wnd->SetCurrentDataField(v.toInt()); break;
		case 1: m_map->SetColorSmooth(v.toBool()); break;
		case 2: m_map->GetColorMap()->SetColorMap(v.toInt()); break;
		case 3: m_map->m_bDispNodeVals = v.toBool(); break;
		case 4: m_map->SetRangeType(v.toInt()); break;
		case 5: m_map->GetColorMap()->SetDivisions(v.toInt()); break;
		case 6: m_map->ShowLegend(v.toBool()); break;
		case 7: m_map->m_pbar->SetOrientation(v.toInt()); break;
		case 8: rng[1] = v.toFloat(); m_map->SetRange(rng); break;
		case 9: rng[0] = v.toFloat(); m_map->SetRange(rng); break;
		}
		m_wnd->GetActiveDocument()->UpdateFEModel();
	}

private:
	CMainWindow*	m_wnd;
	CGLColorMap*	m_map;
};

//-----------------------------------------------------------------------------
class CViewProps : public CPropertyList
{
public:
	CViewProps(CGView& view) : m_view(view)
	{
		addProperty("X-angle", CProperty::Float);
		addProperty("Y-angle", CProperty::Float);
		addProperty("Z-angle", CProperty::Float);
		addProperty("X-target", CProperty::Float);
		addProperty("Y-target", CProperty::Float);
		addProperty("Z-target", CProperty::Float);
		addProperty("Target distance", CProperty::Float);
	}

	QVariant GetPropertyValue(int i)
	{
		CGLCamera& cam = m_view.GetCamera();
		quat4f q = cam.GetOrientation();
		float w = q.GetAngle()*180.f/PI;
		vec3f v = q.GetVector()*w;

		vec3f r = cam.GetPosition();
		float d = cam.GetTargetDistance();

		switch (i)
		{
		case 0: return v.x; break;
		case 1: return v.y; break;
		case 2: return v.z; break;
		case 3: return r.x; break;
		case 4: return r.y; break;
		case 5: return r.z; break;
		case 6: return d; break;
		}

		return QVariant();
	}

	void SetPropertyValue(int i, const QVariant& val)
	{
		CGLCamera& cam = m_view.GetCamera();
		quat4f q = cam.GetOrientation();
		float w = q.GetAngle()*180.f/PI;
		vec3f v = q.GetVector()*w;

		vec3f r = cam.GetPosition();
		float d = cam.GetTargetDistance();

		switch (i)
		{
		case 0: v.x = val.toFloat(); break;
		case 1: v.y = val.toFloat(); break;
		case 2: v.z = val.toFloat(); break;
		case 3: r.x = val.toFloat(); break;
		case 4: r.y = val.toFloat(); break;
		case 5: r.z = val.toFloat(); break;
		case 6: d = val.toFloat(); break;
		}

		w = PI*v.Length()/180.f; v.Normalize();
		q = quat4f(w, v);
		cam.SetOrientation(q);

		cam.SetTarget(r);
		cam.SetTargetDistance(d);

		cam.UpdatePosition(true);
	}

private:
	CGView&	m_view;
};

//-----------------------------------------------------------------------------
class CCameraTransformProps : public CPropertyList
{
public:
	CCameraTransformProps(GLCameraTransform& cam) : m_cam(cam)
	{
		addProperty("X-angle", CProperty::Float);
		addProperty("Y-angle", CProperty::Float);
		addProperty("Z-angle", CProperty::Float);
		addProperty("X-target", CProperty::Float);
		addProperty("Y-target", CProperty::Float);
		addProperty("Z-target", CProperty::Float);
		addProperty("Target distance", CProperty::Float);
	}

	QVariant GetPropertyValue(int i)
	{
		quat4f q = m_cam.rot;
		float w = q.GetAngle()*180.f/PI;
		vec3f v = q.GetVector()*w;

		vec3f r = m_cam.pos;
		float d = m_cam.trg.z;

		switch (i)
		{
		case 0: return v.x; break;
		case 1: return v.y; break;
		case 2: return v.z; break;
		case 3: return r.x; break;
		case 4: return r.y; break;
		case 5: return r.z; break;
		case 6: return d; break;
		}

		return QVariant();
	}

	void SetPropertyValue(int i, const QVariant& val)
	{
		quat4f q = m_cam.rot;
		float w = q.GetAngle()*180.f/PI;
		vec3f v = q.GetVector()*w;

		vec3f r = m_cam.pos;
		float d = m_cam.trg.z;

		switch (i)
		{
		case 0: v.x = val.toFloat(); break;
		case 1: v.y = val.toFloat(); break;
		case 2: v.z = val.toFloat(); break;
		case 3: r.x = val.toFloat(); break;
		case 4: r.y = val.toFloat(); break;
		case 5: r.z = val.toFloat(); break;
		case 6: d = val.toFloat(); break;
		}

		w = PI*v.Length()/180.f; v.Normalize();
		q = quat4f(w, v);
		m_cam.rot = q;

		m_cam.pos = r;
		m_cam.trg.z = d;
	}

private:
	GLCameraTransform&	m_cam;
};

//-----------------------------------------------------------------------------
class CImageModelProps : public CPropertyList
{
public:
	CImageModelProps(CImageModel* img)
	{
		m_img = img;

		addProperty("show box", CProperty::Bool);
		addProperty("x-min", CProperty::Float);
		addProperty("y-min", CProperty::Float);
		addProperty("z-min", CProperty::Float);
		addProperty("x-max", CProperty::Float);
		addProperty("y-max", CProperty::Float);
		addProperty("z-max", CProperty::Float);
	}

	QVariant GetPropertyValue(int i)
	{
		BOUNDINGBOX box = m_img->GetBoundingBox();
		switch (i)
		{
		case 0: return m_img->ShowBox(); break;
		case 1: return box.x0; break;
		case 2: return box.y0; break;
		case 3: return box.z0; break;
		case 4: return box.x1; break;
		case 5: return box.y1; break;
		case 6: return box.z1; break;
		}
		return QVariant();
	}

	void SetPropertyValue(int i, const QVariant& val)
	{
		BOUNDINGBOX& box = m_img->GetBoundingBox();
		switch (i)
		{
		case 0: m_img->ShowBox(val.toBool()); break;
		case 1: box.x0 = val.toFloat(); break;
		case 2: box.y0 = val.toFloat(); break;
		case 3: box.z0 = val.toFloat(); break;
		case 4: box.x1 = val.toFloat(); break;
		case 5: box.y1 = val.toFloat(); break;
		case 6: box.z1 = val.toFloat(); break;
		}
	}

private:
	CImageModel*	m_img;
};


//-----------------------------------------------------------------------------
class CVolRenderProps : public CPropertyList
{
public:
	CVolRenderProps(CMainWindow* wnd, CVolRender* vr) : m_wnd(wnd), m_vr(vr)
	{
		QStringList cols;

		for (int i = 0; i<ColorMapManager::ColorMaps(); ++i)
		{
			string name = ColorMapManager::GetColorMapName(i);
			cols << name.c_str();
		}

		addProperty("alpha scale", CProperty::Float)->setFloatRange(0.0, 1.0);
		addProperty("min intensity", CProperty::Int)->setIntRange(0, 255);
		addProperty("max intensity", CProperty::Int)->setIntRange(0, 255);
		addProperty("min alpha", CProperty::Int)->setIntRange(0, 255);
		addProperty("max alpha", CProperty::Int)->setIntRange(0, 255);
		addProperty("Amin", CProperty::Int)->setIntRange(0, 255);
		addProperty("Amax", CProperty::Int)->setIntRange(0, 255);
		addProperty("Color map", CProperty::Enum)->setEnumValues(cols);
		addProperty("Lighting effect", CProperty::Bool);
		addProperty("Lighting strength", CProperty::Float);
		addProperty("Ambient color", CProperty::Color);
		addProperty("Specular color", CProperty::Color);
		//		addProperty("Light direction", CProperty::DataVec3);
	}

	QVariant GetPropertyValue(int i)
	{
		switch (i)
		{
		case  0: return m_vr->m_alpha; break;
		case  1: return m_vr->m_I0; break;
		case  2: return m_vr->m_I1; break;
		case  3: return m_vr->m_A0; break;
		case  4: return m_vr->m_A1; break;
		case  5: return m_vr->m_Amin; break;
		case  6: return m_vr->m_Amax; break;
		case  7: return m_vr->GetColorMap(); break;
		case  8: return m_vr->m_blight; break;
		case  9: return m_vr->m_shadeStrength; break;
		case 10: return toQColor(m_vr->m_amb); break;
		case 11: return toQColor(m_vr->m_spc); break;
//		case 10: return m_vr->GetLightPosition(); break;
		}
		return QVariant();
	}

	void SetPropertyValue(int i, const QVariant& val)
	{
		switch (i)
		{
		case  0: m_vr->m_alpha = val.toFloat(); break;
		case  1: m_vr->m_I0 = val.toInt(); break;
		case  2: m_vr->m_I1 = val.toInt(); break;
		case  3: m_vr->m_A0 = val.toInt(); break;
		case  4: m_vr->m_A1 = val.toInt(); break;
		case  5: m_vr->m_Amin = val.toInt(); break;
		case  6: m_vr->m_Amax = val.toInt(); break;
		case  7: m_vr->SetColorMap(val.value<int>()); break;
		case  8: m_vr->m_blight = val.toBool(); break;
		case  9: m_vr->m_shadeStrength = val.toFloat(); break;
		case 10: m_vr->m_amb = toGLColor(val.value<QColor>()); break;
		case 11: m_vr->m_spc = toGLColor(val.value<QColor>()); break;
//		case 10: break;
		}

		m_vr->Update();
		m_wnd->RedrawGL();
	}

private:
	CMainWindow* m_wnd;
	CVolRender*	m_vr;
};

//-----------------------------------------------------------------------------
class CImageSlicerProps : public CPropertyList
{
public:
	CImageSlicerProps(CMainWindow* wnd, CImageSlicer* is) : m_wnd(wnd), m_is(is)
	{
		QStringList cols;

		for (int i = 0; i<ColorMapManager::ColorMaps(); ++i)
		{
			string name = ColorMapManager::GetColorMapName(i);
			cols << name.c_str();
		}

		QStringList ops;
		ops << "X" << "Y" << "Z";
		addProperty("Image orientation", CProperty::Enum)->setEnumValues(ops);
		addProperty("Image offset"     , CProperty::Float)->setFloatRange(0.0, 1.0);
		addProperty("Color map"        , CProperty::Enum)->setEnumValues(cols);
	}

	QVariant GetPropertyValue(int i)
	{
		switch (i)
		{
		case  0: return m_is->GetOrientation(); break;
		case  1: return m_is->GetOffset(); break;
		case  2: return m_is->GetColorMap(); break;
		}
		return QVariant();
	}

	void SetPropertyValue(int i, const QVariant& val)
	{
		switch (i)
		{
		case  0: m_is->SetOrientation(val.toInt()); break;
		case  1: m_is->SetOffset(val.toDouble()); break;
		case  2: m_is->SetColorMap(val.value<int>()); break;
		}
		m_is->Update();
		m_wnd->RedrawGL();
	}

private:
	CMainWindow*	m_wnd;
	CImageSlicer*	m_is;
};

//-----------------------------------------------------------------------------
class CMarchingCubesProps : public CPropertyList
{
public:
	CMarchingCubesProps(CMarchingCubes* mc) : m_mc(mc)
	{
		addProperty("isosurface value", CProperty::Float)->setFloatRange(0.0, 1.0);
		addProperty("smooth surface", CProperty::Bool);
		addProperty("surface color", CProperty::Color);
		addProperty("close surface", CProperty::Bool);
		addProperty("invert space", CProperty::Bool);
		addProperty("allow clipping", CProperty::Bool);
	}

	QVariant GetPropertyValue(int i)
	{
		switch (i)
		{
		case 0: return m_mc->GetIsoValue(); break;
		case 1: return m_mc->GetSmooth(); break;
		case 2: return toQColor(m_mc->GetColor()); break;
		case 3: return m_mc->GetCloseSurface(); break;
		case 4: return m_mc->GetInvertSpace(); break;
		case 5: return m_mc->AllowClipping(); break;
		}
		return QVariant();
	}

	void SetPropertyValue(int i, const QVariant& val)
	{
		switch (i)
		{
		case 0: m_mc->SetIsoValue(val.toFloat()); break;
		case 1: m_mc->SetSmooth(val.toBool()); break;
		case 2: m_mc->SetColor(toGLColor(val.value<QColor>())); break;
		case 3: m_mc->SetCloseSurface(val.toBool()); break;
		case 4: m_mc->SetInvertSpace(val.toBool()); break;
		case 5: m_mc->AllowClipping(val.toBool()); break;
		}
	}

private:
	CMarchingCubes*	m_mc;
};


//-----------------------------------------------------------------------------
class CModelTreeItem : public QTreeWidgetItem
{
public:
	CModelTreeItem(CGLObject* po, QTreeWidget* tree) : QTreeWidgetItem(tree), m_po(po) {}
	CModelTreeItem(CGLObject* po, QTreeWidgetItem* item) : QTreeWidgetItem(item), m_po(po) {}

	CGLObject* Object() { return m_po; }

	void SetObject(CGLObject* po) { m_po = po; }

private:
	CGLObject* m_po;
};

//-----------------------------------------------------------------------------
class Ui::CModelViewer
{
public:
	QTreeWidget*			m_tree;
	::CPropertyListView*	m_props;
	QVector<CPropertyList*>	m_list;

	CImageViewer*	m_imgView;

	CHistogramViewer* m_histo;

	QTabWidget*	m_tab;


	QLineEdit* name;
	QCheckBox* enabled;

	QPushButton* autoUpdate;
	QPushButton* apply;

public:
	void setupUi(::CModelViewer* parent)
	{
		QVBoxLayout* pg = new QVBoxLayout(parent);
		
		QSplitter* psplitter = new QSplitter;
		psplitter->setOrientation(Qt::Vertical);
		pg->addWidget(psplitter);

		m_tree = new QTreeWidget;
		m_tree->setObjectName(QStringLiteral("modelTree"));
		m_tree->setColumnCount(1);
		m_tree->setHeaderHidden(true);

		QWidget* w = new QWidget;
		QVBoxLayout* pvl = new QVBoxLayout;
		pvl->setMargin(0);
		
		QHBoxLayout* phl = new QHBoxLayout;
		enabled = new QCheckBox; enabled->setObjectName("enabled");

		phl->addWidget(enabled);
		phl->addWidget(name = new QLineEdit); name->setObjectName("nameEdit");

		QPushButton* del = new QPushButton("Delete"); del->setObjectName("deleteButton");
		phl->addWidget(del);
		phl->addStretch();

/*		autoUpdate = new QPushButton; autoUpdate->setObjectName("autoUpdate"); autoUpdate->setToolTip("Auto-update");
		autoUpdate->setIcon(QIcon(":/icons/auto.png")); autoUpdate->setFixedWidth(20); 
		autoUpdate->setCheckable(true);

		apply = new QPushButton; apply->setObjectName("applyButton"); autoUpdate->setToolTip("Update");
		apply->setIcon(QIcon(":/icons/apply.png"));

		QHBoxLayout* bl = new QHBoxLayout;
		bl->setSpacing(0);
		bl->addStretch();
		bl->addWidget(autoUpdate);
		bl->addWidget(apply);

		phl->addLayout(bl);
*/

		pvl->addLayout(phl);

		m_tab = new QTabWidget;
		m_props = new ::CPropertyListView;
		m_props->setObjectName("props");
		m_tab->addTab(m_props, "Properties");
//		tab->setTabPosition(QTabWidget::West);

		m_imgView = new CImageViewer;
//		m_tab->addTab(m_imgView, "Image Viewer");

		m_histo = new CHistogramViewer;

		m_imgView->hide();
		m_histo->hide();

		pvl->addWidget(m_tab);
		w->setLayout(pvl);

		psplitter->addWidget(m_tree);
		psplitter->addWidget(w);

		QMetaObject::connectSlotsByName(parent);
	}

	CGLObject* currentObject()
	{
		QTreeWidgetItem* current = m_tree->currentItem();
		CModelTreeItem* item = dynamic_cast<CModelTreeItem*>(current);
		if (item == 0) return 0;

		CGLObject* po = item->Object();
		return po;
	}

	void ShowImageViewer(CImageModel* img)
	{
		if (m_tab->count() == 1)
		{
			m_tab->addTab(m_imgView, "Image Viewer");
			m_tab->addTab(m_histo, "Histogram");
		}
		m_imgView->SetImageModel(img);
		m_histo->SetImageModel(img);
	}

	void HideImageViewer()
	{
		if (m_tab->count() != 1)
		{
			m_tab->removeTab(2);
			m_tab->removeTab(1);
		}
		m_imgView->SetImageModel(nullptr);
		m_histo->SetImageModel(nullptr);
	}
};

CModelViewer::CModelViewer(CMainWindow* pwnd, QWidget* parent) : CCommandPanel(pwnd, parent), ui(new Ui::CModelViewer)
{
	ui->setupUi(this);
}

void CModelViewer::selectObject(CGLObject* po)
{
	if (po == 0) ui->m_tree->clearSelection();
	else
	{
		string name = po->GetName();
		QString s(name.c_str());
		QTreeWidgetItemIterator it(ui->m_tree);
		while (*it)
		{
			QString t = (*it)->text(0);
			string st = t.toStdString();
			if ((*it)->text(0) == s)
			{
				(*it)->setSelected(true);
				ui->m_tree->setCurrentItem(*it);
//				on_modelTree_currentItemChanged(*it, 0);
				break;
			}
			++it;
		}
	}
}

CGLObject* CModelViewer::selectedObject()
{
	CModelTreeItem* item = dynamic_cast<CModelTreeItem*>(ui->m_tree->currentItem());
	if (item == nullptr) return nullptr;
	return item->Object();
}

void CModelViewer::UpdateView()
{
	QTreeWidgetItem* psel = ui->m_tree->currentItem();
	if (psel && psel->text(0) == QString("View"))
	{
		on_modelTree_currentItemChanged(psel, psel);
	}
}

void CModelViewer::Update(bool breset)
{
	if (breset)
	{
//		CModelTreeItem* item = dynamic_cast<CModelTreeItem*>(ui->m_tree->currentItem());
/*		CGLObject* po = 0;
		if (item)
		{
			po = item->Object();
		}
*/
		// clear all property lists
		if (ui->m_list.isEmpty() == false)
		{
			QVector<CPropertyList*>::iterator it;
			for (it=ui->m_list.begin(); it != ui->m_list.end(); ++it) delete (*it);
			ui->m_list.clear();
		}

		// clear object list
		m_obj.clear();

		// hide the image viewer
		ui->HideImageViewer();

		ui->name->clear();

		// rebuild the tree
		CDocument* pdoc = GetActiveDocument();
		ui->m_props->Update(0);
		ui->m_tree->clear();
		if (pdoc && pdoc->IsValid())
		{
			FEModel* fem = pdoc->GetFEModel();
			CGLModel* mdl = pdoc->GetGLModel();

			CModelTreeItem* pi1 = new CModelTreeItem(0, ui->m_tree);
			pi1->setText(0, QString::fromStdString(fem->GetName()));
			pi1->setIcon(0, QIcon(QString(":/icons/postview_small.png")));
			ui->m_list.push_back(new CModelProps(mdl));
			pi1->setData(0, Qt::UserRole, (int) (ui->m_list.size()-1));
			m_obj.push_back(0);
			pi1->setExpanded(true);

			// add the mesh
			CModelTreeItem* pi2 = new CModelTreeItem(0, pi1);
			pi2->setText(0, "Mesh");
			pi2->setIcon(0, QIcon(QString(":/icons/mesh.png")));
			ui->m_list.push_back(new CMeshProps(fem));
			pi2->setData(0, Qt::UserRole, (int) (ui->m_list.size()-1));
			m_obj.push_back(0);
		
			CGLDisplacementMap* map = mdl->GetDisplacementMap();
			if (map)
			{
				pi2 = new CModelTreeItem(map, pi1);
				pi2->setText(0, QString::fromStdString(map->GetName()));
//				pi2->setTextColor(0, map && map->IsActive() ? Qt::black : Qt::gray);
				pi2->setIcon(0, QIcon(QString(":/icons/distort.png")));
				ui->m_list.push_back(new CDisplacementMapProps(m_wnd, map));
				pi2->setData(0, Qt::UserRole, (int) (ui->m_list.size()-1));
				m_obj.push_back(map);
			}

			CGLColorMap* col = mdl->GetColorMap();
			pi2 = new CModelTreeItem(col, pi1);
			pi2->setText(0, QString::fromStdString(col->GetName()));
//			pi2->setTextColor(0, col->IsActive() ? Qt::black : Qt::gray);
			pi2->setIcon(0, QIcon(QString(":/icons/colormap.png")));
			ui->m_list.push_back(new CColorMapProps(m_wnd, col));
			pi2->setData(0, Qt::UserRole, (int) (ui->m_list.size()-1));
			m_obj.push_back(0);

			GPlotList& pl = pdoc->GetPlotList();
			GPlotList::iterator it;
			for (it = pl.begin(); it != pl.end(); ++it)
			{
				CGLPlot& plot = *(*it);
				CModelTreeItem* pi1 = new CModelTreeItem(&plot, ui->m_tree);

				if      (dynamic_cast<CGLPlaneCutPlot    *>(&plot)) pi1->setIcon(0, QIcon(QString(":/icons/cut.png")));
				else if (dynamic_cast<CGLVectorPlot      *>(&plot)) pi1->setIcon(0, QIcon(QString(":/icons/vectors.png")));
				else if (dynamic_cast<CGLSlicePlot       *>(&plot)) pi1->setIcon(0, QIcon(QString(":/icons/slice.png")));
				else if (dynamic_cast<CGLIsoSurfacePlot  *>(&plot)) pi1->setIcon(0, QIcon(QString(":/icons/isosurface.png")));
				else if (dynamic_cast<CGLStreamLinePlot  *>(&plot)) pi1->setIcon(0, QIcon(QString(":/icons/streamlines.png")));
				else if (dynamic_cast<CGLParticleFlowPlot*>(&plot)) pi1->setIcon(0, QIcon(QString(":/icons/particle.png")));
				else if (dynamic_cast<GLTensorPlot*       >(&plot)) pi1->setIcon(0, QIcon(QString(":/icons/tensor.png")));
				else if (dynamic_cast<CGLMirrorPlane*	  >(&plot)) pi1->setIcon(0, QIcon(QString(":/icons/mirror.png")));

				string name = plot.GetName();

				pi1->setText(0, name.c_str());
//				pi1->setTextColor(0, plot.IsActive() ? Qt::black : Qt::gray);
				ui->m_list.push_back(plot.propertyList());
				pi1->setData(0, Qt::UserRole, (int) (ui->m_list.size()-1));
				m_obj.push_back(&plot);
			}

			for (int i = 0; i < pdoc->ImageModels(); ++i)
			{
				CImageModel* img = pdoc->GetImageModel(i);

				CModelTreeItem* pi1 = new CModelTreeItem(img, ui->m_tree);
				pi1->setText(0, QString::fromStdString(img->GetName()));
				pi1->setIcon(0, QIcon(QString(":/icons/image.png")));
				ui->m_list.push_back(new CImageModelProps(img));
				pi1->setData(0, Qt::UserRole, (int)(ui->m_list.size() - 1));
				m_obj.push_back(img);
				pi1->setExpanded(true);

				for (int j = 0; j < img->ImageRenderers(); ++j)
				{
					CGLImageRenderer* render = img->GetImageRenderer(j);

					CVolRender* volRender = dynamic_cast<CVolRender*>(render);
					if (volRender)
					{
						CModelTreeItem* pi = new CModelTreeItem(volRender, pi1);
						pi->setText(0, QString::fromStdString(render->GetName()));
						//				pi->setTextColor(0, volRender->IsActive() ? Qt::black : Qt::gray);
						pi->setIcon(0, QIcon(QString(":/icons/volrender.png")));
						ui->m_list.push_back(new CVolRenderProps(m_wnd, volRender));
						pi->setData(0, Qt::UserRole, (int)(ui->m_list.size() - 1));
						m_obj.push_back(volRender);
					}

					CImageSlicer* imgSlice = dynamic_cast<CImageSlicer*>(render);
					if (imgSlice)
					{
						CModelTreeItem* pi = new CModelTreeItem(imgSlice, pi1);
						pi->setText(0, QString::fromStdString(render->GetName()));
						//				pi->setTextColor(0, imgSlice->IsActive() ? Qt::black : Qt::gray);
						pi->setIcon(0, QIcon(QString(":/icons/imageslice.png")));
						ui->m_list.push_back(new CImageSlicerProps(m_wnd, imgSlice));
						pi->setData(0, Qt::UserRole, (int)(ui->m_list.size() - 1));
						m_obj.push_back(imgSlice);
					}

					CMarchingCubes* marchCube = dynamic_cast<CMarchingCubes*>(render);
					if (marchCube)
					{
						CModelTreeItem* pi = new CModelTreeItem(marchCube, pi1);
						pi->setText(0, QString::fromStdString(render->GetName()));
						//				pi->setTextColor(0, imgSlice->IsActive() ? Qt::black : Qt::gray);
						pi->setIcon(0, QIcon(QString(":/icons/marching_cubes.png")));
						ui->m_list.push_back(new CMarchingCubesProps(marchCube));
						pi->setData(0, Qt::UserRole, (int)(ui->m_list.size() - 1));
						m_obj.push_back(marchCube);
					}
				}
			}
	
			CGView& view = *pdoc->GetView();
			pi1 = new CModelTreeItem(&view, ui->m_tree);
			pi1->setText(0, "View");
			pi1->setIcon(0, QIcon(QString(":/icons/view.png")));
			ui->m_list.push_back(new CViewProps(view));
			pi1->setData(0, Qt::UserRole, (int) (ui->m_list.size() - 1));
			pi1->setExpanded(true);
			m_obj.push_back(0);

			for (int i=0; i<view.CameraKeys(); ++i)
			{
				GLCameraTransform& key = view.GetKey(i);
				pi2 = new CModelTreeItem(&key, pi1);

				string name = key.GetName();
				pi2->setText(0, name.c_str());

				pi2->setIcon(0, QIcon(QString(":/icons/view.png")));
				ui->m_list.push_back(new CCameraTransformProps(key));
				pi2->setData(0, Qt::UserRole, (int) (ui->m_list.size() - 1));
				m_obj.push_back(&key);
			}
		}

		// This can crash PostView if po no longer exists (e.g. after new file is read)
//		if (po) selectObject(po);
	}
	else
	{
		on_modelTree_currentItemChanged(ui->m_tree->currentItem(), 0);
	}
}

void CModelViewer::on_modelTree_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* prev)
{
	if (current)
	{
		CModelTreeItem* item = dynamic_cast<CModelTreeItem*>(current);
		if (item)
		{
			CGLObject* po = item->Object();
			if (po)
			{
				ui->enabled->setEnabled(true);
				ui->enabled->setChecked(po->IsActive());

				if (dynamic_cast<CImageModel*>(po))
				{
					CImageModel* img = dynamic_cast<CImageModel*>(po);
					ui->ShowImageViewer(img);
				}
				else ui->HideImageViewer();
			}
			else 
			{
				ui->HideImageViewer();
				ui->enabled->setEnabled(false);
				ui->enabled->setChecked(true);
			}
		}
		else
		{
			ui->HideImageViewer();
			ui->enabled->setEnabled(false);
			ui->enabled->setChecked(false);
		}

		ui->name->setText(current->text(0));
		QVariant v = current->data(0, Qt::UserRole);
		ui->m_props->Update(ui->m_list[v.toInt()]);
	}
	else
	{
		ui->HideImageViewer();
		ui->m_props->Update(0);
	}
}

void CModelViewer::on_modelTree_itemDoubleClicked(QTreeWidgetItem* item, int column)
{
	int n = item->data(0, Qt::UserRole).toInt();
	GLCameraTransform* pkey = dynamic_cast<GLCameraTransform*>(m_obj[n]);
	if (pkey)
	{
		CGView* view = GetActiveDocument()->GetView();
		view->SetCurrentKey(pkey);
		m_wnd->RedrawGL();
	}
}

void CModelViewer::on_nameEdit_editingFinished()
{
	QString name = ui->name->text();
	QTreeWidgetItem* item = ui->m_tree->currentItem();
	if (item) item->setText(0, name);
}

void CModelViewer::on_deleteButton_clicked()
{
	CModelTreeItem* item = dynamic_cast<CModelTreeItem*>(ui->m_tree->currentItem());
	if (item)
	{
		QVariant v = item->data(0, Qt::UserRole);
		int n = v.toInt();
		CGLObject* po = m_obj[n];
		if (po)
		{
			GetActiveDocument()->DeleteObject(po);
			item->SetObject(0);
			Update(true);
			m_wnd->RedrawGL();
		}
		else QMessageBox::information(this, "PostView2", "Cannot delete this object");
	}
}

void CModelViewer::on_props_dataChanged()
{
	CGLObject* po = selectedObject();
	if (po) po->Update();
	m_wnd->RedrawGL();
}

void CModelViewer::on_enabled_stateChanged(int nstate)
{
	QTreeWidgetItem* current = ui->m_tree->currentItem();
	CModelTreeItem* item = dynamic_cast<CModelTreeItem*>(current);
	if (item == 0) return;

	CGLObject* po = item->Object();
	if (po == 0) return;

	if (nstate == Qt::Unchecked)
	{
		po->Activate(false);
//		item->setTextColor(0, Qt::gray);
	}
	else if (nstate == Qt::Checked)
	{
		po->Activate(true);
//		item->setTextColor(0, Qt::black);
	}

	m_wnd->CheckUi();
	m_wnd->RedrawGL();
}
/*
void CModelViewer::on_autoUpdate_toggled(bool b)
{
	ui->apply->setEnabled(!b);	
}

void CModelViewer::on_applyButton_clicked()
{
	CGLObject* po = ui->currentObject();
	if (po)
	{
		CGLModel* mdl = po->GetModel();
		if (mdl) po->Update(mdl->currentTimeIndex(), 0.f, true);
	}
}
*/
