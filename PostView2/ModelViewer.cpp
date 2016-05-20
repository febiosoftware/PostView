#include "ModelViewer.h"
#include <QBoxLayout>
#include <QTreeWidget>
#include <QTableWidget>
#include <QMessageBox>
#include <QHeaderView>
#include <QSplitter>
#include <QLabel>
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

//-----------------------------------------------------------------------------
class CModelProps : public CPropertyList
{
public:
	CModelProps(CGLModel* fem) : m_fem(fem) 
	{
		addProperty("Element subdivions"       , CProperty::Int)->setIntRange(0, 100).setAutoValue(true);
		addProperty("Render mode"              , CProperty::Enum, "Render mode")->setEnumValues(QStringList() << "default" << "wireframe" << "solid");
		addProperty("Render outline"           , CProperty::Bool);
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
		case 2: v = m_fem->m_boutline; break;
		case 3: v = m_fem->m_bghost; break;
		case 4: v = toQColor(m_fem->m_line_col); break;
		case 5: v = toQColor(m_fem->m_node_col); break;
		case 6: v = toQColor(m_fem->m_sel_col); break;
		case 7: v = m_fem->m_bsmooth; break;
		case 8: v = m_fem->m_bShell2Hex; break;
		case 9: v = m_fem->m_nshellref; break;
//		case 10: v = m_fem->m_
		}
		return v;
	}

	void SetPropertyValue(int i, const QVariant& v)
	{
		switch (i)
		{
		case 0: m_fem->m_nDivs    = v.toInt(); break;
		case 1: m_fem->m_nrender  = v.toInt(); break;
		case 2: m_fem->m_boutline = v.toBool(); break;
		case 3: m_fem->m_bghost   = v.toBool(); break;
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
		FEMesh& mesh = *fem->GetMesh();
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
			FEMesh& mesh = *m_fem->GetMesh();
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
	CDisplacementMapProps(CGLDisplacementMap* map) : m_map(map)
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
	}

private:
	CGLDisplacementMap*	m_map;
};

//-----------------------------------------------------------------------------
class CColorMapProps : public CPropertyList
{
public:
	CColorMapProps(CMainWindow* wnd, CGLColorMap* map) : m_wnd(wnd), m_map(map)
	{
		addProperty("Data field"        , CProperty::DataScalar);
		addProperty("Gradient smoothing", CProperty::Bool);
		addProperty("Nodal Values"      , CProperty::Bool);
		addProperty("Range type"        , CProperty::Enum)->setEnumValues(QStringList() << "dynamic" << "static" << "user");
		addProperty("Range divisions"   , CProperty::Int)->setIntRange(1, 100);
		addProperty("Show Legend"       , CProperty::Bool);
		addProperty("User max"          , CProperty::Float);
		addProperty("User min"          , CProperty::Float);
	}

	QVariant GetPropertyValue(int i)
	{
		if (m_map)
		{
			float fmin, fmax;
			m_map->GetColorMap()->GetRange(fmin, fmax);
			switch (i)
			{
			case 0: return m_map->GetEvalField(); break;
			case 1: return m_map->GetColorMap()->Smooth(); break;
			case 2: return m_map->m_bDispNodeVals; break;
			case 3: return m_map->GetRangeType(); break;
			case 4: return m_map->GetColorMap()->GetDivisions(); break;
			case 5: return m_map->ShowLegend(); break;
			case 6: return fmax; break;
			case 7: return fmin; break;
			}
		}
		return QVariant();
	}

	void SetPropertyValue(int i, const QVariant& v)
	{
		if (m_map == 0) return;
		float fmin, fmax;
		m_map->GetColorMap()->GetRange(fmin, fmax);

		switch (i)
		{
		case 0: m_wnd->SetCurrentDataField(v.toInt()); break;
		case 1: m_map->GetColorMap()->Smooth(v.toBool()); break;
		case 2: m_map->m_bDispNodeVals = v.toBool(); break;
		case 3: m_map->SetRangeType(v.toInt()); break;
		case 4: m_map->GetColorMap()->SetDivisions(v.toInt()); break;
		case 5: m_map->ShowLegend(v.toBool()); break;
		case 6: m_map->GetColorMap()->SetRange(fmin, v.toFloat()); break;
		case 7: m_map->GetColorMap()->SetRange(v.toFloat(), fmax); break;
		}
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

		cam.Update(true);
	}

private:
	CGView&	m_view;
};

//-----------------------------------------------------------------------------
class Ui::CModelViewer
{
public:
	QTreeWidget*			m_tree;
	::CPropertyListView*	m_props;
	QVector<CPropertyList*>	m_list;

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

		m_props = new ::CPropertyListView;

		psplitter->addWidget(m_tree);
		psplitter->addWidget(m_props);

		QMetaObject::connectSlotsByName(parent);
	}
};

CModelViewer::CModelViewer(CMainWindow* pwnd, QWidget* parent) : CCommandPanel(pwnd, parent), ui(new Ui::CModelViewer)
{
	ui->setupUi(this);
}

void CModelViewer::selectObject(CGLObject* po)
{
	if (po == 0) ui->m_tree->clearSelection();

	QString s(po->GetName());
	QTreeWidgetItemIterator it(ui->m_tree);
	while (*it)
	{
		if ((*it)->text(0) == s)
		{
			(*it)->setSelected(true);
			on_modelTree_currentItemChanged(*it, 0);
			break;
		}
		++it;
	}
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
		// clear all property lists
		if (ui->m_list.isEmpty() == false)
		{
			QVector<CPropertyList*>::iterator it;
			for (it=ui->m_list.begin(); it != ui->m_list.end(); ++it) delete (*it);
			ui->m_list.clear();
		}

		// rebuild the tree
		CDocument* pdoc = m_wnd->GetDocument();
		ui->m_props->Update(0);
		if (pdoc->IsValid())
		{
			FEModel* fem = pdoc->GetFEModel();
			CGLModel* mdl = pdoc->GetGLModel();

			char szfile[256] = {0};
			pdoc->GetDocTitle(szfile);

			ui->m_tree->clear();
			QTreeWidgetItem* pi1 = new QTreeWidgetItem(ui->m_tree);
			pi1->setText(0, szfile);
			pi1->setIcon(0, QIcon(QString(":/icons/postview_small.png")));
			ui->m_list.push_back(new CModelProps(mdl));
			pi1->setData(0, Qt::UserRole, (int) (ui->m_list.size()-1));
			pi1->setExpanded(true);

			// add the mesh
			QTreeWidgetItem* pi2 = new QTreeWidgetItem(pi1);
			pi2->setText(0, "Mesh");
			pi2->setIcon(0, QIcon(QString(":/icons/mesh.png")));
			ui->m_list.push_back(new CMeshProps(fem));
			pi2->setData(0, Qt::UserRole, (int) (ui->m_list.size()-1));
		
			pi2 = new QTreeWidgetItem(pi1);
			pi2->setText(0, "Displacement map");
			pi2->setIcon(0, QIcon(QString(":/icons/distort.png")));
			ui->m_list.push_back(new CDisplacementMapProps(mdl->GetDisplacementMap()));
			pi2->setData(0, Qt::UserRole, (int) (ui->m_list.size()-1));

			pi2 = new QTreeWidgetItem(pi1);
			pi2->setText(0, "Color map");
			pi2->setIcon(0, QIcon(QString(":/icons/colormap.png")));
			ui->m_list.push_back(new CColorMapProps(m_wnd, mdl->GetColorMap()));
			pi2->setData(0, Qt::UserRole, (int) (ui->m_list.size()-1));

			GPlotList& pl = pdoc->GetPlotList();
			GPlotList::iterator it;
			for (it = pl.begin(); it != pl.end(); ++it)
			{
				CGLPlot& plot = *(*it);
				QTreeWidgetItem* pi1 = new QTreeWidgetItem(ui->m_tree);

				if      (dynamic_cast<CGLPlaneCutPlot  *>(&plot))  pi1->setIcon(0, QIcon(QString(":/icons/cut.png")));
				else if (dynamic_cast<CGLVectorPlot    *>(&plot))  pi1->setIcon(0, QIcon(QString(":/icons/vectors.png")));
				else if (dynamic_cast<CGLSlicePlot     *>(&plot))  pi1->setIcon(0, QIcon(QString(":/icons/slice.png")));
				else if (dynamic_cast<CGLIsoSurfacePlot*>(&plot))  pi1->setIcon(0, QIcon(QString(":/icons/isosurface.png")));
			
				pi1->setText(0, plot.GetName());
				ui->m_list.push_back(plot.propertyList());
				pi1->setData(0, Qt::UserRole, (int) (ui->m_list.size()-1));
			}
	
			pi2 = new QTreeWidgetItem(ui->m_tree);
			pi2->setText(0, "View");
			pi2->setIcon(0, QIcon(QString(":/icons/view.png")));
			ui->m_list.push_back(new CViewProps(*pdoc->GetView()));
			pi2->setData(0, Qt::UserRole, (int) (ui->m_list.size() - 1));
		}
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
		QVariant v = current->data(0, Qt::UserRole);
		ui->m_props->Update(ui->m_list[v.toInt()]);
	}
	else ui->m_props->Update(0);
}
