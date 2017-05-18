#pragma once

#ifdef WIN32
#include <Windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "PostViewLib/ColorMap.h"
#include <QPainter>

class CGLView;
class CGLWidgetManager;
class CGLObject;
class CDlgWidgetProps;

#define GLW_ALIGN_LEFT		0x0001
#define GLW_ALIGN_RIGHT		0x0002
#define GLW_ALIGN_TOP		0x0004
#define GLW_ALIGN_BOTTOM	0x0008
#define GLW_ALIGN_HCENTER	0x0010
#define GLW_ALIGN_VCENTER	0x0020

//-----------------------------------------------------------------------------
class GLWidget
{
public:
	enum FillMode { NONE, COLOR1, COLOR2, HORIZONTAL, VERTICAL };

public:
	GLWidget(CGLObject* po, int x, int y, int w, int h, const char* szlabel = 0);
	virtual ~GLWidget();

	CGLObject* GetObject() { return m_po; }

	virtual void draw(QPainter* painter) = 0;

	void set_color(GLCOLOR fgc, GLCOLOR bgc);

	void set_fg_color(GLCOLOR c) { m_fgc = c; }
	void set_fg_color(GLubyte r, GLubyte g, GLubyte b, GLubyte a = 255) { m_fgc = GLCOLOR(r,g,b,a); }
	GLCOLOR get_fg_color() { return m_fgc; }

	void set_bg_style(int n) { m_nbg = n; }
	void set_bg_color(GLCOLOR c1, GLCOLOR c2) { m_bgc[0] = c1; m_bgc[1] = c2; }
	void set_bg_color(GLubyte r, GLubyte g, GLubyte b, GLubyte a = 255) { m_bgc[0] = GLCOLOR(r,g,b,a); }
	GLCOLOR get_bg_color(int i) { return m_bgc[i]; }
	int get_bg_style() { return m_nbg; }

	void copy_label(const char* szlabel);
	void set_label(const char* szlabel) { m_szlabel = (char*) szlabel; }
	const char* get_label() { return m_szlabel; }

	virtual bool is_inside(int x, int y);

	void set_focus() { m_pfocus = this; }
	static void set_focus(GLWidget* pw) { m_pfocus = pw; }

	static GLWidget* get_focus() { return m_pfocus; }

	bool has_focus() { return (this == m_pfocus); }

	int x() { return m_x; }
	int y() { return m_y; }
	int w() { return m_w; }
	int h() { return m_h; }

	virtual void resize(int x, int y, int w, int h)
	{
		m_x = x;
		m_y = y;
		m_w = w;
		m_h = h;

		if (m_w < m_minw) m_w = m_minw;
		if (m_h < m_minh) m_h = m_minh;
	}

	static CGLView* GetView() { return m_pview; }

	void show() { m_bshow = true; }
	void hide() { m_bshow = false; if (this == m_pfocus) m_pfocus = 0; }
	bool visible() { return m_bshow; }

	unsigned int GetSnap() { return m_nsnap; }

	void align(unsigned int n) { m_nsnap = n; }

	QFont get_font() const { return m_font; }
	void set_font(const QFont& f) { m_font = f; }
	void set_font_size(int nsize) { m_font.setPointSize(nsize); }

protected:
	int m_x, m_y;
	int m_w, m_h;
	int	m_minw, m_minh;
	bool	m_balloc;
	
	char*			m_szlabel;

	QFont	m_font;	// label font
	
	unsigned int	m_nsnap;	// alignment flag

	GLCOLOR	m_fgc;
	GLCOLOR m_bgc[2];
	int		m_nbg;	// background style

	static GLWidget* m_pfocus;	// the widget that has the focus
	static CGLView*	m_pview;	// view of the widget

	bool	m_bshow;	// show the widget or not

	CGLObject*	m_po;	// the object that manages the life of this widget

	friend class CGLWidgetManager;
};

//-----------------------------------------------------------------------------

class CDocument;

class GLBox : public GLWidget
{
public:
	GLBox(CGLObject* po, int x, int y, int w, int h, CDocument* pdoc, const char* szlabel = 0);

	void draw(QPainter* painter);

	void parse_label(char* szlabel, const char* szval, int nmax);

	void fit_to_size();

protected:
	void draw_bg(int x0, int y0, int x1, int y1, QPainter* painter);

protected:
	CDocument*	m_pdoc;

public:
	bool	m_bshadow;	// render shadows
	GLCOLOR	m_shc;		// shadow color
};

//-----------------------------------------------------------------------------

class GLLegendBar : public GLWidget
{
public:
	enum { GRADIENT, DISCRETE };
	enum { HORIZONTAL, VERTICAL };

public:
	GLLegendBar(CGLObject* po, CColorTexture* pm, int x, int y, int w, int h, int orientation = VERTICAL);

	void draw(QPainter* painter);

	void SetType(int n) { m_ntype = n; }

	void SetOrientation(int n);
	int Orientation() const { return m_nrot; }

	bool ShowLabels() { return m_blabels; }
	void ShowLabels(bool bshow) { m_blabels = bshow; }

	bool ShowTitle() const { return m_btitle; }
	void ShowTitle(bool b) { m_btitle = b; }

	int GetPrecision() { return m_nprec; }
	void SetPrecision(int n) { n = (n<1?1:(n>7?7:n)); m_nprec = n; }

	int GetLabelPosition() const { return m_labelPos; }
	void SetLabelPosition(int n) { m_labelPos = n; }

	void SetRange(float fmin, float fmax);
	void GetRange(float& fmin, float& fmax);

protected:
	void draw_gradient_vert(QPainter* painter);
	void draw_gradient_horz(QPainter* painter);
	void draw_discrete_vert(QPainter* painter);
	void draw_discrete_horz(QPainter* painter);

protected:
	int		m_ntype;	// type of bar
	int		m_nrot;		// orientation
	bool	m_btitle;	// show title
	bool	m_blabels;	// show labels
	int		m_labelPos;	// label placement
	int		m_nprec;	// precision
	float	m_fmin;		// min of range
	float	m_fmax;		// max of range

	CColorTexture*		m_pMap;
};

//-----------------------------------------------------------------------------

class CGLCamera;

class GLTriad : public GLWidget
{
public:
	GLTriad(CGLObject* po, int x, int y, int w, int h, CGLCamera* pcam);

	void draw(QPainter* painter);

	void show_coord_labels(bool bshow) { m_bcoord_labels = bshow; }
	bool show_coord_labels() { return m_bcoord_labels; }

protected:
	CGLCamera*	m_pcam;
	bool	m_bcoord_labels;
};

//-----------------------------------------------------------------------------

class GLSafeFrame : public GLWidget
{
public:
	GLSafeFrame(CGLObject* po, int x, int y, int w, int h);

	void resize(int x, int y, int w, int h)
	{
		if (!m_block) GLWidget::resize(x, y, w, h);
	}

	bool is_inside(int x, int y);

	void draw(QPainter* painter);

	void lock(bool b) { m_block = b; }
	bool islocked() { return m_block; }

protected:
	bool	m_block;
};
