// DlgViewSettings.cpp: implementation of the CDlgViewSettings class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DlgViewSettings.h"
#include "MainWindow.h"
#include "Document.h"
#include <QBoxLayout>
#include <QPushButton>
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QLabel>
#include "PropertyList.h"
#include "PropertyListView.h"
#include <PostViewLib/Palette.h>
#include <QSpinBox>
#include <QInputDialog>

//-----------------------------------------------------------------------------
class CRenderingProps : public CPropertyList
{
public:
	CRenderingProps()
	{
		addProperty("Render style", CProperty::Enum, "The rendering style sets defaults for colors and other attributes.")->setEnumValues(QStringList()<<"User"<<"Default"<<"CAD");
		addProperty("Perspective Projection", CProperty::Bool);
		addProperty("Line smoothing", CProperty::Bool);
		addProperty("Line thickness", CProperty::Float)->setFloatRange(0.0, 25.0).setFloatStep(0.5f);
		addProperty("point size"    , CProperty::Float)->setFloatRange(0.0, 25.0).setFloatStep(0.5f);

		m_nrender = 0;
		m_bproj = true;
		m_bline = true;
		m_thick = 1.0f;
		m_point = 6.0f;
	}

	QVariant GetPropertyValue(int i)
	{
		QVariant v;
		switch (i)
		{
		case 0: return m_nrender; break;
		case 1: return m_bproj; break;
		case 2: return m_bline; break;
		case 3: return m_thick; break;
		case 4: return m_point; break;
		}
		return v;
	}

	void SetPropertyValue(int i, const QVariant& v)
	{
		switch (i)
		{
		case 0: m_nrender = v.toInt(); break;
		case 1: m_bproj   = v.toBool(); break;
		case 2: m_bline   = v.toBool(); break;
		case 3: m_thick   = v.toFloat(); break;
		case 4: m_point   = v.toFloat(); break;
		}
	}

public:
	int		m_nrender;
	bool	m_bproj;
	bool	m_bline;
	float	m_thick;
	float	m_point;
};

//-----------------------------------------------------------------------------
class CBackgroundProps : public CPropertyList
{
public:
	CBackgroundProps()
	{
		addProperty("Background color 1", CProperty::Color);
		addProperty("Background color 2", CProperty::Color);
		addProperty("Background style", CProperty::Enum)->setEnumValues(QStringList()<<"Color 1"<<"Color 2" << "Fade horizontal" << "Fade vertical" << "Fade diagonally"); 

		m_col1 = GLCOLOR(0,0,0);
		m_col2 = GLCOLOR(0,0,0);
		m_nstyle = 0;
	}

	QVariant GetPropertyValue(int i)
	{
		QVariant v;
		switch (i)
		{
		case 0: return toQColor(m_col1); break;
		case 1: return toQColor(m_col2); break;
		case 2: return m_nstyle; break;
		}
		return v;
	}

	void SetPropertyValue(int i, const QVariant& v)
	{
		switch (i)
		{
		case 0: m_col1 = toGLColor(v.value<QColor>()); break;
		case 1: m_col2 = toGLColor(v.value<QColor>()); break;
		case 2: m_nstyle = v.toInt(); break;
		}
	}

public:
	GLCOLOR	m_col1;
	GLCOLOR	m_col2;
	int		m_nstyle;
};

//-----------------------------------------------------------------------------
class CLightingProps : public CPropertyList
{
public:
	CLightingProps()
	{
		addProperty("Enable lighting"  , CProperty::Bool );
		addProperty("Diffuse intensity", CProperty::Float)->setFloatRange(0.0, 1.0);
		addProperty("Ambient intensity", CProperty::Float)->setFloatRange(0.0, 1.0);
		addProperty("Render shadows"   , CProperty::Bool );
		addProperty("Shadow intensity" , CProperty::Float)->setFloatRange(0.0, 1.0);

		m_blight = true;
		m_diffuse = 0.7f;
		m_ambient = 0.3f;
		m_bshadow = false;
		m_shadow = 0.1f;
	}

	QVariant GetPropertyValue(int i)
	{
		QVariant v;
		switch (i)
		{
		case 0: return m_blight; break;
		case 1: return m_diffuse; break;
		case 2: return m_ambient; break;
		case 3: return m_bshadow; break;
		case 4: return m_shadow; break;
		}
		return v;
	}

	void SetPropertyValue(int i, const QVariant& v)
	{
		switch (i)
		{
		case 0: m_blight = v.toBool(); break;
		case 1: m_diffuse = v.toFloat(); break;
		case 2: m_ambient = v.toFloat(); break;
		case 3: m_bshadow = v.toBool(); break;
		case 4: m_shadow = v.toFloat(); break;
		}
	}

public:
	bool	m_blight;
	float	m_diffuse;
	float	m_ambient;
	bool	m_bshadow;
	float	m_shadow;
};

//-----------------------------------------------------------------------------
class CCameraProps : public CPropertyList
{
public:
	CCameraProps()
	{
		addProperty("Animate camera" , CProperty::Bool);
		addProperty("Animation speed", CProperty::Float)->setFloatRange(0.0, 1.0);
		addProperty("Animation bias" , CProperty::Float)->setFloatRange(0.0, 1.0);

		m_banim = true;
		m_speed = 0.8f;
		m_bias = 0.8f;
	}

	QVariant GetPropertyValue(int i)
	{
		QVariant v;
		switch (i)
		{
		case 0: return m_banim; break;
		case 1: return m_speed; break;
		case 2: return m_bias; break;
		}
		return v;
	}

	void SetPropertyValue(int i, const QVariant& v)
	{
		switch (i)
		{
		case 0: m_banim = v.toBool(); break;
		case 1: m_speed = v.toFloat(); break;
		case 2: m_bias = v.toFloat(); break;
		}
	}

public:
	bool	m_banim;
	float	m_speed;
	float	m_bias;
};

//-----------------------------------------------------------------------------
class CSelectionProps : public CPropertyList
{
public:
	CSelectionProps()
	{
		addProperty("Select connected" , CProperty::Bool);
		addProperty("Tag info", CProperty::Enum)->setEnumValues(QStringList() << "Item numbers" << "Item numbers and connecting nodes");
		addProperty("Ignore backfacing items", CProperty::Bool);
		addProperty("Ignore interior items", CProperty::Bool);
		m_bconnect = false;
		m_ntagInfo = 0;
	}

	QVariant GetPropertyValue(int i)
	{
		QVariant v;
		switch (i)
		{
		case 0: return m_bconnect; break;
		case 1: return m_ntagInfo; break;
		case 2: return m_backface; break;
		case 3: return m_binterior; break;
		}
		return v;
	}

	void SetPropertyValue(int i, const QVariant& v)
	{
		switch (i)
		{
		case 0: m_bconnect = v.toBool(); break;
		case 1: m_ntagInfo = v.toInt(); break;
		case 2: m_backface = v.toBool(); break;
		case 3: m_binterior = v.toBool(); break;
		}
	}

public:
	bool	m_bconnect;
	int		m_ntagInfo;
	bool	m_backface;
	bool	m_binterior;
};

//-----------------------------------------------------------------------------
class CPaletteWidget : public QWidget
{
public:
	QComboBox*	pal;

public:
	CPaletteWidget(QWidget* parent = 0) : QWidget(parent)
	{
		QLabel* label = new QLabel("Current palette:");
		label->setFixedWidth(100);
		label->setAlignment(Qt::AlignRight | Qt::AlignCenter);
		pal = new QComboBox; label->setBuddy(label);

		QHBoxLayout* h0 = new QHBoxLayout;
		h0->addWidget(label);
		h0->addWidget(pal);

		QPushButton* load   = new QPushButton("Load Palette ..."); load->setObjectName("load");
		QPushButton* save   = new QPushButton("Save Palette ..."); save->setObjectName("save");
		QPushButton* create = new QPushButton("Create palette from materials ..."); create->setObjectName("create");
		QPushButton* apply  = new QPushButton("Apply palette to materials ..."   ); apply ->setObjectName("apply");

		QVBoxLayout* buttons = new QVBoxLayout;
		buttons->addWidget(load);
		buttons->addWidget(save);
		buttons->addWidget(create);
		buttons->addWidget(apply);

		QHBoxLayout* h1 = new QHBoxLayout;
		h1->addStretch();
		h1->addLayout(buttons);

		QVBoxLayout* pl = new QVBoxLayout;
		pl->addLayout(h0);
		pl->addLayout(h1);
		pl->addStretch();

		setLayout(pl);
	}
};

//-----------------------------------------------------------------------------
CColormapWidget::CColormapWidget(QWidget* parent) : QWidget(parent)
{
	QHBoxLayout* h = new QHBoxLayout;
	QComboBox* l = m_maps = new QComboBox;
	l->setMinimumWidth(120);
	l->setCurrentIndex(0);

	QPushButton* newButton = new QPushButton("New ...");
	QPushButton* invertButton = new QPushButton("Invert");
	h->addWidget(new QLabel("Select color map:"));
	h->addWidget(l);
	h->addWidget(newButton);
	h->addWidget(invertButton);
	h->addStretch();

	QVBoxLayout* v = new QVBoxLayout;
	v->addLayout(h);

	h = new QHBoxLayout;

	m_spin = new QSpinBox;
	m_spin->setRange(2, 9);
	m_spin->setMaximumWidth(75);
	m_spin->setMinimumWidth(50);

	h->addWidget(new QLabel("Number of colors:"));
	h->addWidget(m_spin);
	h->addStretch();
	v->addLayout(h);

	m_grid = new QGridLayout;

	v->addLayout(m_grid);
	v->addStretch();

	setLayout(v);

	QObject::connect(l, SIGNAL(currentIndexChanged(int)), this, SLOT(currentMapChanged(int)));
	QObject::connect(m_spin, SIGNAL(valueChanged(int)), this, SLOT(onSpinValueChanged(int)));
	QObject::connect(newButton, SIGNAL(clicked()), this, SLOT(onNew()));
	QObject::connect(invertButton, SIGNAL(clicked()), this, SLOT(onInvert()));

	updateMaps();
	m_currentMap = 0;
	currentMapChanged(0);
}

void CColormapWidget::updateMaps()
{
	m_maps->clear();
	for (int i = 0; i < ColorMapManager::ColorMaps(); ++i)
	{
		string name = ColorMapManager::GetColorMapName(i);
		m_maps->addItem(QString(name.c_str()));
	}
}

void CColormapWidget::onNew()
{
	static int n = 1;
	QString name = QString("user%1").arg(n);
	bool bok = true;
	QString newName = QInputDialog::getText(this, "New color map", "name:", QLineEdit::Normal, name, &bok);
	if (bok && (newName.isEmpty() == false))
	{
		CColorMap& map = ColorMapManager::GetColorMap(m_currentMap);
		string sname = name.toStdString();
		ColorMapManager::AddColormap(sname, map);

		updateMaps();
		m_maps->setCurrentIndex(ColorMapManager::ColorMaps() - 1);
	}
}

void CColormapWidget::onInvert()
{
	CColorMap& map = ColorMapManager::GetColorMap(m_currentMap);
	map.Invert();
	updateColorMap(map);
}

void CColormapWidget::updateColorMap(const CColorMap& map)
{
	clearGrid();

	m_spin->setValue(map.Colors());

	QLineEdit* l;
	CColorButton* b;
	for (int i=0; i<map.Colors(); ++i)
	{
		QColor c = toQColor(map.GetColor(i));
		float f = map.GetColorPos(i);

		m_grid->addWidget(new QLabel(QString("Color %1").arg(i+1)), i, 0, Qt::AlignRight);
		m_grid->addWidget(l = new QLineEdit, i, 1); l->setValidator(new QDoubleValidator); l->setText(QString::number(f)); l->setMaximumWidth(100);
		m_grid->addWidget(b = new CColorButton, i, 2); b->setColor(c);

		QObject::connect(l, SIGNAL(editingFinished()), this, SLOT(onDataChanged()));
		QObject::connect(b, SIGNAL(colorChanged(QColor)), this, SLOT(onDataChanged()));
	}
}

void CColormapWidget::clearGrid()
{
	while (m_grid->count())
	{
		QLayoutItem* item = m_grid->takeAt(0);
		delete item->widget();
		delete item;
	}
}

void CColormapWidget::currentMapChanged(int n)
{
	if (n != -1)
	{
		m_currentMap = n;
		updateColorMap(ColorMapManager::GetColorMap(n));
	}
}

void CColormapWidget::onDataChanged()
{
	CColorMap& map = ColorMapManager::GetColorMap(m_currentMap);
	for (int i=0; i<map.Colors(); ++i)
	{
		QLineEdit* pos = dynamic_cast<QLineEdit*>(m_grid->itemAtPosition(i, 1)->widget()); assert(pos);
		if (pos)
		{
			float f = pos->text().toFloat();
			map.SetColorPos(i, f);
		}

		CColorButton* col = dynamic_cast<CColorButton*>(m_grid->itemAtPosition(i, 2)->widget()); assert(col);
		if (col)
		{
			QColor c = col->color();
			map.SetColor(i, toGLColor(c));
		}
	}
}

void CColormapWidget::onSpinValueChanged(int n)
{
	CColorMap& map = ColorMapManager::GetColorMap(m_currentMap);
	if (map.Colors() != n)
	{
		map.SetColors(n);
		updateColorMap(map);
	}
}

//-----------------------------------------------------------------------------
class Ui::CDlgViewSettings
{
public:
	CRenderingProps*	m_render;
	CBackgroundProps*	m_bg;
	CLightingProps*		m_light;
	CCameraProps*		m_cam;
	CSelectionProps*	m_select;
	CPaletteWidget*		m_pal;
	CColormapWidget*	m_map;
	QDialogButtonBox*	buttonBox;

public:
	void setupUi(::CDlgViewSettings* pwnd)
	{
		QVBoxLayout* pg = new QVBoxLayout(pwnd);
	
		QTabWidget* pt = new QTabWidget;

		::CPropertyListView* pw1 = new ::CPropertyListView; pw1->Update(m_render);
		::CPropertyListView* pw2 = new ::CPropertyListView; pw2->Update(m_bg    );
		::CPropertyListView* pw3 = new ::CPropertyListView; pw3->Update(m_light );
		::CPropertyListView* pw4 = new ::CPropertyListView; pw4->Update(m_cam   );
		::CPropertyListView* pw5 = new ::CPropertyListView; pw5->Update(m_select);

		m_pal = new CPaletteWidget;
		m_map = new CColormapWidget;

		pt->addTab(pw1, "Rendering");
		pt->addTab(pw2, "Background");
		pt->addTab(pw3, "Lighting");
		pt->addTab(pw4, "Camera");
		pt->addTab(pw5, "Selection");
		pt->addTab(m_pal, "Palette");
		pt->addTab(m_map, "Colormap");
		pg->addWidget(pt);

		buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply); 
		pg->addWidget(buttonBox);

		QObject::connect(buttonBox, SIGNAL(accepted()), pwnd, SLOT(accept()));
		QObject::connect(buttonBox, SIGNAL(rejected()), pwnd, SLOT(reject()));
		QObject::connect(buttonBox, SIGNAL(clicked(QAbstractButton*)), pwnd, SLOT(onClicked(QAbstractButton*)));
		QMetaObject::connectSlotsByName(pwnd);
	}
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDlgViewSettings::CDlgViewSettings(CMainWindow* pwnd) : ui(new Ui::CDlgViewSettings)
{
	m_pwnd = pwnd;

	CDocument* pdoc = m_pwnd->GetDocument();
	VIEWSETTINGS& view = pdoc->GetViewSettings();
	CGLCamera& cam = pdoc->GetView()->GetCamera();

	ui->m_render = new CRenderingProps;
	ui->m_bg     = new CBackgroundProps;
	ui->m_light  = new CLightingProps;
	ui->m_cam    = new CCameraProps;
	ui->m_select = new CSelectionProps;

	ui->m_render->m_bproj = view.m_nproj == RENDER_PERSP;
	ui->m_render->m_bline = view.m_blinesmooth;
	ui->m_render->m_thick = view.m_flinethick;
	ui->m_render->m_point = view.m_fpointsize;

	ui->m_bg->m_col1 = view.bgcol1;
	ui->m_bg->m_col2 = view.bgcol2;
	ui->m_bg->m_nstyle = view.bgstyle;

	ui->m_light->m_blight = view.m_bLighting;
	ui->m_light->m_diffuse = view.m_diffuse;
	ui->m_light->m_ambient = view.m_ambient;
	ui->m_light->m_bshadow = view.m_bShadows;
	ui->m_light->m_shadow = view.m_shadow_intensity;

	ui->m_cam->m_speed = cam.GetCameraSpeed();
	ui->m_cam->m_bias  = cam.GetCameraBias();

	ui->m_select->m_bconnect = view.m_bconn;
	ui->m_select->m_ntagInfo = view.m_ntagInfo;
	ui->m_select->m_backface = view.m_bignoreBackfacingItems;
	ui->m_select->m_binterior = view.m_bext;

	ui->setupUi(this);

	// fill the palette list
	UpdatePalettes();

	setWindowTitle("PostView Settings");
	resize(600, 400);
}

void CDlgViewSettings::UpdatePalettes()
{
	ui->m_pal->pal->clear();

	CPaletteManager& PM = CPaletteManager::GetInstance();
	int pals = PM.Palettes();
	for (int i = 0; i<pals; ++i)
	{
		ui->m_pal->pal->addItem(QString::fromStdString(PM.Palette(i).Name()));
	}

	ui->m_pal->pal->setCurrentIndex(PM.CurrentIndex());
}

CDlgViewSettings::~CDlgViewSettings()
{

}

void CDlgViewSettings::apply()
{
	CDocument* pdoc = m_pwnd->GetDocument();
	VIEWSETTINGS& view = pdoc->GetViewSettings();
	CGLCamera& cam = pdoc->GetView()->GetCamera();

	view.m_nproj = (ui->m_render->m_bproj ? RENDER_PERSP : RENDER_ORTHO);
	view.m_blinesmooth = ui->m_render->m_bline;
	view.m_flinethick  = ui->m_render->m_thick;
	view.m_fpointsize  = ui->m_render->m_point;

	view.bgcol1 = ui->m_bg->m_col1;
	view.bgcol2 = ui->m_bg->m_col2;
	view.bgstyle = ui->m_bg->m_nstyle;

	view.m_bLighting = ui->m_light->m_blight;
	view.m_diffuse = ui->m_light->m_diffuse;
	view.m_ambient = ui->m_light->m_ambient;
	view.m_bShadows = ui->m_light->m_bshadow;
	view.m_shadow_intensity = ui->m_light->m_shadow;

	cam.SetCameraSpeed(ui->m_cam->m_speed);
	cam.SetCameraBias(ui->m_cam->m_bias);

	view.m_bconn    = ui->m_select->m_bconnect;
	view.m_ntagInfo = ui->m_select->m_ntagInfo;
	view.m_bignoreBackfacingItems = ui->m_select->m_backface;
	view.m_bext     = ui->m_select->m_binterior;

	CPaletteManager& PM = CPaletteManager::GetInstance();
	PM.SetCurrentIndex(ui->m_pal->pal->currentIndex());

	m_pwnd->RedrawGL();
}

void CDlgViewSettings::accept()
{
	apply();
	QDialog::accept();
}

void CDlgViewSettings::onClicked(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) apply();
}

void CDlgViewSettings::on_load_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Load Palette", "", "PostView Palette (*.xpr)");
	if (fileName.isEmpty() == false)
	{
		CPaletteManager& PM = CPaletteManager::GetInstance();
		string sfile = fileName.toStdString();
		if (PM.Load(sfile) == false)
		{
			QMessageBox::critical(this, "Load Palette", "Failed loading palette(s)");
			return;
		}

		// make the last palette loaded the active one
		int N = PM.Palettes();
		if (N > 0)
		{
			PM.SetCurrentIndex(N - 1);
			UpdatePalettes();
		}
	}
}

void CDlgViewSettings::on_save_clicked()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Save Palette", "", "PostView Palette (*.xpr)");
	if (fileName.isEmpty() == false)
	{
		CPaletteManager& PM = CPaletteManager::GetInstance();
		int n = ui->m_pal->pal->currentIndex();
		const CPalette& p = PM.Palette(n);

		string sfile = fileName.toStdString();
		if (PM.Save(sfile, p) == false)
		{
			QMessageBox::critical(this, "Save Palette", "Failed saving palette");
		}
	}
}

void CDlgViewSettings::on_create_clicked()
{
	CDocument& doc = *m_pwnd->GetDocument();
	if (doc.IsValid() == false)
	{
		QMessageBox::critical(this, "PostView", "No model is loaded");
		return;
	}

	FEModel& fem = *doc.GetFEModel();
	int NMAT = fem.Materials();
	if (NMAT == 0)
	{
		QMessageBox::critical(this, "PostView", "Model does not define materials.");
		return;
	}

	QString name = QInputDialog::getText(this, "New Palette", "Name");
	if (name.isEmpty() == false)
	{
		string sname = name.toStdString();
		CPalette pal(sname);
		for (int i=0; i<NMAT; ++i)
		{
			FEMaterial& m = *fem.GetMaterial(i);
			pal.AddColor(m.diffuse);
		}

		CPaletteManager& PM = CPaletteManager::GetInstance();
		PM.AddPalette(pal);

		UpdatePalettes();
	}
}

void CDlgViewSettings::on_apply_clicked()
{
	CDocument& doc = *m_pwnd->GetDocument();
	if (doc.IsValid() == false)
	{
		QMessageBox::critical(this, "PostView", "No model is loaded");
		return;
	}

	CPaletteManager& PM = CPaletteManager::GetInstance();
	const CPalette& pal = PM.Palette(ui->m_pal->pal->currentIndex());
	doc.ApplyPalette(pal);

	m_pwnd->UpdateUi(true);
}
