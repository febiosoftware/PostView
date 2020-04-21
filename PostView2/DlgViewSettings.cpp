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
#include <PostLib/Palette.h>
#include <QSpinBox>
#include <QInputDialog>
#include "MainWindow.h"
using namespace Post;

//-----------------------------------------------------------------------------
class CRenderingProps : public CPropertyList
{
public:
	CRenderingProps()
	{
		addProperty("Render style", CProperty::Enum, "The rendering style sets defaults for colors and other attributes.")->setEnumValues(QStringList()<<"User"<<"Default"<<"CAD");
		addProperty("Perspective Projection", CProperty::Bool);
		addProperty("Line Smoothing", CProperty::Bool);
		addProperty("Line Thickness", CProperty::Float)->setFloatRange(0.0, 25.0).setFloatStep(0.5f);
		addProperty("Point Size"    , CProperty::Float)->setFloatRange(0.0, 25.0).setFloatStep(0.5f);
		addProperty("Spring thickness", CProperty::Float)->setFloatRange(0.0, 25.0).setFloatStep(0.5);
        addProperty("Multiview Projection", CProperty::Enum, "Convention for Front/Back/Left/Right/Top/Bottom views.")->setEnumValues(QStringList()<<"First-angle projection (XZ)"<<"First-angle projection (XY)"<<"Third-angle projection (XY)");

		m_nrender = 0;
		m_bproj = true;
		m_bline = true;
		m_linethick = 1.0f;
		m_springthick = 1.0f;
		m_point = 6.0f;
        m_nconv = 0;
	}

	QVariant GetPropertyValue(int i)
	{
		QVariant v;
		switch (i)
		{
		case 0: return m_nrender; break;
		case 1: return m_bproj; break;
		case 2: return m_bline; break;
		case 3: return m_linethick; break;
		case 4: return m_point; break;
		case 5: return m_springthick; break;
        case 6: return m_nconv; break;
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
		case 3: m_linethick   = v.toFloat(); break;
		case 4: m_point   = v.toFloat(); break;
		case 5: m_springthick = v.toFloat(); break;
        case 6: m_nconv   = v.toInt(); break;
		}
	}

public:
	int		m_nrender;
	bool	m_bproj;
	bool	m_bline;
	float	m_linethick;
	float	m_springthick;
	float	m_point;
    int     m_nconv;
    
};

//-----------------------------------------------------------------------------
class CBackgroundProps : public CPropertyList
{
public:
	CBackgroundProps()
	{
		addProperty("Theme", CProperty::Enum)->setEnumValues(QStringList() << "Default" << "Dark");
		addProperty("Background color 1", CProperty::Color);
		addProperty("Background color 2", CProperty::Color);
		addProperty("Background style", CProperty::Enum)->setEnumValues(QStringList()<<"Color 1"<<"Color 2" << "Fade horizontal" << "Fade vertical" << "Fade diagonally"); 

		m_col1 = GLColor(0,0,0);
		m_col2 = GLColor(0,0,0);
		m_nstyle = 0;
		m_ntheme = 0;
	}

	QVariant GetPropertyValue(int i)
	{
		QVariant v;
		switch (i)
		{
		case 0: return m_ntheme; break;
		case 1: return toQColor(m_col1); break;
		case 2: return toQColor(m_col2); break;
		case 3: return m_nstyle; break;
		}
		return v;
	}

	void SetPropertyValue(int i, const QVariant& v)
	{
		switch (i)
		{
		case 0: m_ntheme = v.toInt(); break;
		case 1: m_col1 = toGLColor(v.value<QColor>()); break;
		case 2: m_col2 = toGLColor(v.value<QColor>()); break;
		case 3: m_nstyle = v.toInt(); break;
		}
	}

public:
	GLColor	m_col1;
	GLColor	m_col2;
	int		m_nstyle;
	int		m_ntheme;
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
//		addProperty("Light direction"  , CProperty::Vec3);

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
		case 5: return vecToString(m_pos); break;
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
		case 5: m_pos = stringToVec(v.toString()); break;
		}
	}

public:
	bool	m_blight;
	float	m_diffuse;
	float	m_ambient;
	bool	m_bshadow;
	float	m_shadow;
	vec3f	m_pos;
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

//=================================================================================================
ColorGradient::ColorGradient(QWidget* parent) : QWidget(parent)
{
	m_map.jet();
}

QSize ColorGradient::sizeHint() const
{
	return QSize(20, 20);
}

void ColorGradient::setColorMap(const CColorMap& m)
{
	m_map = m;
	repaint();
}

void ColorGradient::paintEvent(QPaintEvent* ev)
{
	QRect rt = rect();
	QPainter p(this);

	int r = 3;
	int x0 = rt.left();
	int x1 = rt.right();
	int y0 = rt.top();
	int y1 = y0 + 2*r;
	int y2 = rt.bottom();
	int w = rt.width();

	p.fillRect(x0, y0, w, y1 - y0, Qt::white);

	int ncol = m_map.Colors();
	for (int i=0; i<ncol; ++i)
	{
		float x = m_map.GetColorPos(i);
		int xi = x0 + (int)(w*x) - r;
		p.setPen(Qt::gray);
		p.setBrush(Qt::gray);
		p.drawEllipse(xi, y0, 2*r, 2*r);
	}

	for (int i=0; i<rt.width(); ++i)
	{
		float w = (float) i / rt.width();
		GLColor c = m_map.map(w);
		p.setPen(QColor(c.r, c.g, c.b));
		p.drawLine(x0 + i, y1, x0 + i, y2);
	}
}

//=================================================================================================
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
	QPushButton* delButton = new QPushButton("Delete");
	QPushButton* editButton = new QPushButton("Edit ...");
	h->addWidget(new QLabel("Select color map:"));
	h->addWidget(l);
	h->addWidget(newButton);
	h->addWidget(delButton);
	h->addWidget(editButton);
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

	QPushButton* invertButton = new QPushButton("Invert");
	h = new QHBoxLayout();
	h->addStretch();
	h->addWidget(invertButton);
	v->addLayout(h);
	v->addStretch();

	h = new QHBoxLayout();
	h->addWidget(m_grad = new ColorGradient);
	v->addLayout(h);

	setLayout(v);

	QObject::connect(l, SIGNAL(currentIndexChanged(int)), this, SLOT(currentMapChanged(int)));
	QObject::connect(m_spin, SIGNAL(valueChanged(int)), this, SLOT(onSpinValueChanged(int)));
	QObject::connect(newButton, SIGNAL(clicked()), this, SLOT(onNew()));
	QObject::connect(delButton, SIGNAL(clicked()), this, SLOT(onDelete()));
	QObject::connect(editButton, SIGNAL(clicked()), this, SLOT(onEdit()));
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
	int n = ColorMapManager::UserColorMaps() + 1;
	QString name = QString("user%1").arg(n);
	bool bok = true;
	QString newName = QInputDialog::getText(this, "New color map", "name:", QLineEdit::Normal, name, &bok);
	if (bok && (newName.isEmpty() == false))
	{
		CColorMap& map = ColorMapManager::GetColorMap(m_currentMap);
		string sname = newName.toStdString();
		ColorMapManager::AddColormap(sname, map);

		updateMaps();
		m_maps->setCurrentIndex(ColorMapManager::ColorMaps() - 1);
	}
}

void CColormapWidget::onDelete()
{
	if (ColorMapManager::RemoveColormap(m_currentMap) == false)
	{
		QMessageBox::critical(this, "Delete Colormap", "Cannot delete default color maps.");
	}
	else
	{
		m_maps->removeItem(m_currentMap);
	}
}

void CColormapWidget::onEdit()
{
	string sname = ColorMapManager::GetColorMapName(m_currentMap);
	QString name = QString::fromStdString(sname);
	bool bok = true;
	QString newName = QInputDialog::getText(this, "Edit color map", "name:", QLineEdit::Normal, name, &bok);
	if (bok && (newName.isEmpty() == false))
	{
		ColorMapManager::SetColorMapName(m_currentMap, newName.toStdString());
		m_maps->setItemText(m_currentMap, newName);
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

	m_grad->setColorMap(map);
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
	m_grad->setColorMap(map);
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
	QTabWidget*			m_tab;

	::CPropertyListView* pw1;
	::CPropertyListView* pw2;
	::CPropertyListView* pw3;
	::CPropertyListView* pw4;
	::CPropertyListView* pw5;

	static int m_ntab;

public:
	void setupUi(::CDlgViewSettings* pwnd)
	{
		QVBoxLayout* pg = new QVBoxLayout(pwnd);
	
		m_tab = new QTabWidget;

		pw1 = new ::CPropertyListView; pw1->Update(m_render);
		pw2 = new ::CPropertyListView; pw2->Update(m_bg    );
		pw3 = new ::CPropertyListView; pw3->Update(m_light );
		pw4 = new ::CPropertyListView; pw4->Update(m_cam   );
		pw5 = new ::CPropertyListView; pw5->Update(m_select);

		m_pal = new CPaletteWidget;
		m_map = new CColormapWidget;

		m_tab->addTab(pw1, "Rendering");
		m_tab->addTab(pw2, "Background");
		m_tab->addTab(pw3, "Lighting");
		m_tab->addTab(pw4, "Camera");
		m_tab->addTab(pw5, "Selection");
		m_tab->addTab(m_pal, "Palette");
		m_tab->addTab(m_map, "Colormap");
		pg->addWidget(m_tab);

		buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply | QDialogButtonBox::Reset); 
		pg->addWidget(buttonBox);

		QObject::connect(buttonBox, SIGNAL(accepted()), pwnd, SLOT(accept()));
		QObject::connect(buttonBox, SIGNAL(rejected()), pwnd, SLOT(reject()));
		QObject::connect(buttonBox, SIGNAL(clicked(QAbstractButton*)), pwnd, SLOT(onClicked(QAbstractButton*)));
		QMetaObject::connectSlotsByName(pwnd);
	}

	void Set(::CMainWindow* wnd)
	{
		CDocument* doc = wnd->GetActiveDocument();

		VIEWSETTINGS& view = wnd->GetViewSettings();
		m_render->m_bproj = view.m_nproj == RENDER_PERSP;
        m_render->m_nconv = view.m_nconv;
		m_render->m_bline = view.m_blinesmooth;
		m_render->m_linethick = view.m_flinethick;
		m_render->m_point = view.m_fpointsize;
		m_render->m_springthick = view.m_fspringthick;

		m_bg->m_col1 = view.bgcol1;
		m_bg->m_col2 = view.bgcol2;
		m_bg->m_nstyle = view.bgstyle;
		m_bg->m_ntheme = wnd->currentTheme();

		m_light->m_blight = view.m_bLighting;
		m_light->m_diffuse = view.m_diffuse;
		m_light->m_ambient = view.m_ambient;
		m_light->m_bshadow = view.m_bShadows;
		m_light->m_shadow = view.m_shadow_intensity;
		if (doc) m_light->m_pos = doc->GetLightPosition();

		m_select->m_bconnect = view.m_bconn;
		m_select->m_ntagInfo = view.m_ntagInfo;
		m_select->m_backface = view.m_bignoreBackfacingItems;
		m_select->m_binterior = view.m_bext;
	}

	void Get(::CMainWindow* wnd)
	{
		CDocument* doc = wnd->GetActiveDocument();

		VIEWSETTINGS& view = wnd->GetViewSettings();
		view.m_nproj       = (m_render->m_bproj ? RENDER_PERSP : RENDER_ORTHO);
        view.m_nconv       = m_render->m_nconv;
		view.m_blinesmooth = m_render->m_bline;
		view.m_flinethick  = m_render->m_linethick;
		view.m_fpointsize  = m_render->m_point;
		view.m_fspringthick = m_render->m_springthick;

		view.bgcol1  = m_bg->m_col1;
		view.bgcol2  = m_bg->m_col2;
		view.bgstyle = m_bg->m_nstyle;
		wnd->setCurrentTheme(m_bg->m_ntheme);

		view.m_bLighting = m_light->m_blight;
		view.m_diffuse   = m_light->m_diffuse;
		view.m_ambient   = m_light->m_ambient;
		view.m_bShadows  = m_light->m_bshadow;
		view.m_shadow_intensity = m_light->m_shadow;
		if (doc) doc->SetLightPosition(m_light->m_pos);

		view.m_bconn    = m_select->m_bconnect;
		view.m_ntagInfo = m_select->m_ntagInfo;
		view.m_bignoreBackfacingItems = m_select->m_backface;
		view.m_bext     = m_select->m_binterior;

		update();
	}

	void update()
	{
		pw1->Update(m_render);
		pw2->Update(m_bg);
		pw3->Update(m_light);
		pw4->Update(m_cam);
		pw5->Update(m_select);
	}
};

int Ui::CDlgViewSettings::m_ntab = 0;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDlgViewSettings::CDlgViewSettings(CMainWindow* pwnd) : ui(new Ui::CDlgViewSettings)
{
	m_pwnd = pwnd;

	VIEWSETTINGS& view = m_pwnd->GetViewSettings();

	ui->m_render = new CRenderingProps;
	ui->m_bg     = new CBackgroundProps;
	ui->m_light  = new CLightingProps;
	ui->m_cam    = new CCameraProps;
	ui->m_select = new CSelectionProps;

	ui->Set(pwnd);

	// NOTE: view can be null if no document was loaded.
	// Probably need to make these settings view independent!
	CDocument* pdoc = m_pwnd->GetActiveDocument();
	CGView* cgview = (pdoc ? pdoc->GetView() : nullptr);
	if (cgview)
	{
		CGLCamera& cam = cgview->GetCamera();
		ui->m_cam->m_speed = cam.GetCameraSpeed();
		ui->m_cam->m_bias = cam.GetCameraBias();
	}

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

void CDlgViewSettings::showEvent(QShowEvent* ev)
{
	ui->m_tab->setCurrentIndex(ui->m_ntab);
}

void CDlgViewSettings::hideEvent(QHideEvent* ev)
{
	ui->m_ntab = ui->m_tab->currentIndex();
}

void CDlgViewSettings::apply()
{
	VIEWSETTINGS& view = m_pwnd->GetViewSettings();

	CDocument* pdoc = m_pwnd->GetActiveDocument();
	CGView* cgview = (pdoc ? pdoc->GetView() : nullptr);
	if (cgview)
	{
		CGLCamera& cam = cgview->GetCamera();
		cam.SetCameraSpeed(ui->m_cam->m_speed);
		cam.SetCameraBias(ui->m_cam->m_bias);
	}

	ui->Get(m_pwnd);

	CPaletteManager& PM = CPaletteManager::GetInstance();
	PM.SetCurrentIndex(ui->m_pal->pal->currentIndex());

	// we need to update the color maps that are used in the model tree
	if (pdoc) pdoc->UpdateColorMaps();
	m_pwnd->UpdateModelViewer(true);
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
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole) OnReset();
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
	CDocument& doc = *m_pwnd->GetActiveDocument();
	if (doc.IsValid() == false)
	{
		QMessageBox::critical(this, "PostView", "No model is loaded");
		return;
	}

	FEPostModel& fem = *doc.GetFEModel();
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
	CDocument& doc = *m_pwnd->GetActiveDocument();
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

void CDlgViewSettings::OnReset()
{
	VIEWSETTINGS& view = m_pwnd->GetViewSettings();
	view.Defaults();
	ui->Set(m_pwnd);
}
