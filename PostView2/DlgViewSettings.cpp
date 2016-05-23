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
#include "PropertyList.h"
#include "PropertyListView.h"

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
		}
		return v;
	}

	void SetPropertyValue(int i, const QVariant& v)
	{
		switch (i)
		{
		case 0: m_bconnect = v.toBool(); break;
		case 1: m_ntagInfo = v.toInt(); break;
		}
	}

public:
	bool	m_bconnect;
	int		m_ntagInfo;
};


//-----------------------------------------------------------------------------
class Ui::CDlgViewSettings
{
public:
	CRenderingProps*	m_render;
	CBackgroundProps*	m_bg;
	CLightingProps*		m_light;
	CCameraProps*		m_cam;
	CSelectionProps*	m_select;
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

		pt->addTab(pw1, "Rendering");
		pt->addTab(pw2, "Background");
		pt->addTab(pw3, "Lighting");
		pt->addTab(pw4, "Camera");
		pt->addTab(pw5, "Selection");
		pg->addWidget(pt);

		buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply); 
		pg->addWidget(buttonBox);

		QObject::connect(buttonBox, SIGNAL(accepted()), pwnd, SLOT(accept()));
		QObject::connect(buttonBox, SIGNAL(rejected()), pwnd, SLOT(reject()));
		QObject::connect(buttonBox, SIGNAL(clicked(QAbstractButton*)), pwnd, SLOT(onClicked(QAbstractButton*)));
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

	ui->setupUi(this);
	resize(400, 300);
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

	view.m_bconn = ui->m_select->m_bconnect;
	view.m_ntagInfo = ui->m_select->m_ntagInfo;

	m_pwnd->repaint();
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
