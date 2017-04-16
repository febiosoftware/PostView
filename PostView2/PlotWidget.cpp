#include "stdafx.h"
#include "PlotWidget.h"
#include <QPainter>
#include <QFontDatabase>
#include <QMouseEvent>
#include <QAction>
#include <QMessageBox>
#include <QMenu>
#include <QApplication>
#include <QClipboard>
#include <assert.h>
#include <math.h>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QBoxLayout>
#include <QLineEdit>
#include <QValidator>

class CDlgPlotWidgetProps_Ui
{
public:
	QLineEdit*	x[2];
	QLineEdit*	y[2];

public:
	void setup(CDlgPlotWidgetProps* dlg)
	{
		QFormLayout* form = new QFormLayout;

		form->addRow("X min:", x[0] = new QLineEdit); x[0]->setValidator(new QDoubleValidator);
		form->addRow("X max:", x[1] = new QLineEdit); x[1]->setValidator(new QDoubleValidator);

		form->addRow("Y min:", y[0] = new QLineEdit); y[0]->setValidator(new QDoubleValidator);
		form->addRow("Y max:", y[1] = new QLineEdit); y[1]->setValidator(new QDoubleValidator);

		QDialogButtonBox* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

		QVBoxLayout* l = new QVBoxLayout;
		l->addLayout(form);
		l->addWidget(bb);

		dlg->setLayout(l);

		QObject::connect(bb, SIGNAL(accepted()), dlg, SLOT(accept()));
		QObject::connect(bb, SIGNAL(rejected()), dlg, SLOT(reject()));
	}
};

CDlgPlotWidgetProps::CDlgPlotWidgetProps(QWidget* parent) : QDialog(parent), ui(new CDlgPlotWidgetProps_Ui)
{
	ui->setup(this);
}

void CDlgPlotWidgetProps::SetRange(double xmin, double xmax, double ymin, double ymax)
{
	if (ui)
	{
		ui->x[0]->setText(QString::number(xmin));
		ui->x[1]->setText(QString::number(xmax));

		ui->y[0]->setText(QString::number(ymin));
		ui->y[1]->setText(QString::number(ymax));
	}
}

void CDlgPlotWidgetProps::accept()
{
	m_xmin = ui->x[0]->text().toDouble();
	m_xmax = ui->x[1]->text().toDouble();
	m_ymin = ui->y[0]->text().toDouble();
	m_ymax = ui->y[1]->text().toDouble();

	QDialog::accept();
}

//=============================================================================

class CPalette
{
public:
	enum { Colors = 16 };

public:
	static QColor color(int i) { return m_col[i%m_col.size()]; }
	static void init()
	{
		QStringList colorNames = QColor::colorNames();
		m_col.resize(Colors);
		m_col[ 0] = QColor(colorNames[13]);
		m_col[ 1] = QColor(colorNames[14]);
		m_col[ 2] = QColor(colorNames[15]);
		m_col[ 3] = QColor(colorNames[16]);
		m_col[ 4] = QColor(colorNames[17]);
		m_col[ 5] = QColor(colorNames[19]);
		m_col[ 6] = QColor(colorNames[20]);
		m_col[ 7] = QColor(colorNames[21]);
		m_col[ 8] = QColor(colorNames[22]);
		m_col[ 9] = QColor(colorNames[23]);
		m_col[10] = QColor(colorNames[24]);
		m_col[11] = QColor(colorNames[25]);
		m_col[12] = QColor(colorNames[27]);
		m_col[13] = QColor(colorNames[28]);
		m_col[14] = QColor(colorNames[29]);
		m_col[15] = QColor(colorNames[30]);
	}

private:
	CPalette() {}
	CPalette(const CPalette&) {}

private:
	static vector<QColor>	m_col;
};
vector<QColor> CPalette::m_col;

//-----------------------------------------------------------------------------
double findScale(double fmin, double fmax)
{
	double dx = fmax - fmin;
	double p = floor(log10(dx));
	double f = pow(10.0, p - 1);
	double m = floor(dx / f);

	double dd = f;
	if      (m > 75) dd = 10*f;
	else if (m > 30) dd = 5*f;
	else if (m > 15) dd = 2*f;

	return dd;
}

//-----------------------------------------------------------------------------
CPlotData::CPlotData()
{
}

//-----------------------------------------------------------------------------
CPlotData::CPlotData(const CPlotData& d)
{
	m_data = d.m_data;
	m_label = d.m_label;
	m_col = d.m_col;
}

//-----------------------------------------------------------------------------
CPlotData& CPlotData::operator = (const CPlotData& d)
{
	m_data = d.m_data;
	m_label = d.m_label;
	m_col = d.m_col;
	return *this;
}

//-----------------------------------------------------------------------------
void CPlotData::clear()
{ 
	m_data.clear(); 
}

//-----------------------------------------------------------------------------
QRectF CPlotData::boundRect() const
{
	if (m_data.empty()) return QRectF(0., 0., 0., 0.);

	QRectF r(m_data[0].x(), m_data[0].y(), 0.0, 0.0);
	for (int i=1; i<(int)m_data.size(); ++i)
	{
		const QPointF& p = m_data[i];
		if (p.x() < r.left  ()) r.setLeft  (p.x());
		if (p.x() > r.right ()) r.setRight (p.x());
		if (p.y() > r.bottom()) r.setBottom(p.y());
		if (p.y() < r.top   ()) r.setTop   (p.y());
	}
	return r;
}

//-----------------------------------------------------------------------------
void CPlotData::addPoint(double x, double y)
{
	QPointF p(x, y);
	m_data.push_back(p);
}

//-----------------------------------------------------------------------------
CPlotWidget::CPlotWidget(QWidget* parent, int w, int h) : QWidget(parent)
{
	CPalette::init();

	m_select = false;
	m_bzoomRect = false;
	m_bvalidRect = false;

	m_bshowLegend = true;
	m_bviewLocked = false;
	m_bshowPopup = true;

	m_bdrawXLines = true;
	m_bdrawYLines = true;

	m_bdrawXAxis = true;
	m_bdrawYAxis = true;

	m_chartStyle = CPlotWidget::LineChart;

	m_viewRect = QRectF(0.0, 0.0, 1.0, 1.0);
	m_xscale = findScale(m_viewRect.left(), m_viewRect.right());
	m_yscale = findScale(m_viewRect.top(), m_viewRect.bottom());

	if (w < 200) w = 200;
	if (h < 200) h = 200;
	m_sizeHint = QSize(w, h);

	m_pZoomToFit = new QAction(QIcon(QString(":/icons/zoom_fit.png")), tr("Zoom to fit"), this);
	connect(m_pZoomToFit, SIGNAL(triggered()), this, SLOT(OnZoomToFit()));

	m_pShowProps = new QAction(QIcon(QString(":/icons/properties.png")), tr("Properties"), this);
	connect(m_pShowProps, SIGNAL(triggered()), this, SLOT(OnShowProps()));

	m_pCopyToClip = new QAction(QIcon(QString(":/icons/clipboard.png")), tr("Copy to clipboard"), this);
	connect(m_pCopyToClip, SIGNAL(triggered()), this, SLOT(OnCopyToClipboard()));
}

//-----------------------------------------------------------------------------
void CPlotWidget::setChartStyle(int chartStyle)
{
	m_chartStyle = chartStyle;
}

//-----------------------------------------------------------------------------
void CPlotWidget::contextMenuEvent(QContextMenuEvent* ev)
{
	if (m_bshowPopup)
	{
		QMenu menu(this);
		menu.addAction(m_pZoomToFit);
		menu.addAction(m_pCopyToClip);
		menu.addSeparator();
		menu.addAction(m_pShowProps);
		menu.exec(ev->globalPos());
	}
}

//-----------------------------------------------------------------------------
void CPlotWidget::ZoomToRect(bool b)
{
	m_bzoomRect = b;
	m_bvalidRect = false;
}

//-----------------------------------------------------------------------------
void CPlotWidget::OnZoomToWidth()
{
	fitWidthToData();
	repaint();
}

//-----------------------------------------------------------------------------
void CPlotWidget::OnZoomToHeight()
{
	fitHeightToData();
	repaint();
}

//-----------------------------------------------------------------------------
void CPlotWidget::OnZoomToFit()
{
	fitToData();
	repaint();
}

//-----------------------------------------------------------------------------
void CPlotWidget::OnShowProps()
{
	CDlgPlotWidgetProps dlg(this);
	QRectF rt = m_viewRect;
	dlg.SetRange(rt.left(), rt.right(), rt.top(), rt.bottom());

	if (dlg.exec())
	{
		rt.setLeft(dlg.m_xmin); rt.setRight(dlg.m_xmax);
		rt.setTop(dlg.m_ymin); rt.setBottom(dlg.m_ymax);

		setViewRect(rt);
		repaint();
	}
}

//-----------------------------------------------------------------------------
void CPlotWidget::OnCopyToClipboard()
{
	QClipboard* clipboard = QApplication::clipboard();

	if (plots() > 0)
	{
		CPlotData& d = m_data[0];
		if (d.size() > 0)
		{
			QString s("x");
			for (int i=0; i<plots(); ++i) s += "\t" + m_data[i].label();
			s += '\n';
			for (int i=0; i<d.size(); ++i)
			{
				QPointF& pi = d.Point(i);
				s.append(QString::asprintf("%lg", pi.x()));

				for (int j=0; j<plots(); ++j)
				{
					QPointF& pi = m_data[j].Point(i);
					s.append(QString::asprintf("\t%lg", pi.y()));
				}

				s += '\n';
			}
			clipboard->setText(s);
		}
	}
}

//-----------------------------------------------------------------------------
void CPlotWidget::setTitle(const QString& t)
{
	m_title = t;
}

//-----------------------------------------------------------------------------
void CPlotWidget::clearData()
{
	m_select = false;
	for (int i=0; i<(int) m_data.size(); ++i) m_data[i].clear();
}

//-----------------------------------------------------------------------------
void CPlotWidget::clear()
{
	m_select = false;
	m_data.clear();
	repaint();
}

//-----------------------------------------------------------------------------
void CPlotWidget::addPlotData(CPlotData& p)
{
	int N = m_data.size();

	p.setColor(CPalette::color(N));

	m_data.push_back(p);
}

//-----------------------------------------------------------------------------
void CPlotWidget::fitWidthToData()
{
	if (m_data.empty()) return;

	QRectF r = m_data[0].boundRect();
	for (int i=1; i<(int) m_data.size(); ++i)
	{
		QRectF ri = m_data[i].boundRect();
		r = ri.united(r);
	}

	r.setTop(m_viewRect.top());
	r.setBottom(m_viewRect.bottom());
	setViewRect(r);
}

//-----------------------------------------------------------------------------
void CPlotWidget::fitHeightToData()
{
	if (m_data.empty()) return;

	QRectF r = m_data[0].boundRect();
	for (int i=1; i<(int) m_data.size(); ++i)
	{
		QRectF ri = m_data[i].boundRect();
		r = ri.united(r);
	}

	r.setLeft (m_viewRect.left ());
	r.setRight(m_viewRect.right());
	setViewRect(r);
}

//-----------------------------------------------------------------------------
void CPlotWidget::fitToData()
{
	if (m_data.empty()) return;

	QRectF r = m_data[0].boundRect();
	for (int i=1; i<(int) m_data.size(); ++i)
	{
		QRectF ri = m_data[i].boundRect();
		r = ri.united(r);
	}
	setViewRect(r);
}

//-----------------------------------------------------------------------------
void CPlotWidget::setViewRect(const QRectF& rt)
{
	m_viewRect = rt;
	if (fabs(rt.height()) < 1e-7) m_viewRect.setHeight(1.0);
	if (fabs(rt.width ()) < 1e-7) m_viewRect.setWidth (1.0);

	double dx = 0.05*m_viewRect.width();
	double dy = 0.05*m_viewRect.height();
	m_viewRect.adjust(0.0, 0.0, dx, dy);

	m_xscale = findScale(m_viewRect.left(), m_viewRect.right());
	m_yscale = findScale(m_viewRect.top(), m_viewRect.bottom());
}

//-----------------------------------------------------------------------------
void CPlotWidget::fitToRect(const QRect& rt)
{
	QPoint p0(rt.topLeft());
	QPoint p1(rt.bottomRight());

	QPointF r0 = ScreenToView(p0);
	QPointF r1 = ScreenToView(p1);
	QRectF rf = QRectF(r0.x(), r1.y(), r1.x() - r0.x(), r0.y() - r1.y());

	setViewRect(rf);
}

//-----------------------------------------------------------------------------
void CPlotWidget::mousePressEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton)
	{
		QPoint pt = ev->pos();
		const int eps = 3;

		m_select = false;
		for (int i=0; i<(int)m_data.size(); ++i)
		{
			CPlotData& plot = m_data[i];
			for (int j=0; j<plot.size(); ++j)
			{
				QPointF& rj = plot.Point(j);
				QPoint p = ViewToScreen(rj);
				if ((abs(p.x() - pt.x()) <= eps) && (abs(p.y() - pt.y()) <= eps))
				{
					m_select = true;
					m_selection.ndataIndex = i;
					m_selection.point      = rj;
				}
			}
		}
		repaint();
	}
	m_mousePos = ev->pos();
	m_mouseInitPos = m_mousePos;
	if (m_bzoomRect) m_bvalidRect = true;
	ev->accept();
}

//-----------------------------------------------------------------------------
void CPlotWidget::mouseMoveEvent(QMouseEvent* ev)
{
	if (ev->buttons() & Qt::LeftButton)
	{
		QPoint p = ev->pos();
		if ((m_bzoomRect == false) && (m_bviewLocked == false))
		{
			QPointF r0 = ScreenToView(m_mousePos);
			QPointF r1 = ScreenToView(p);
			m_viewRect.translate(r0.x() - r1.x(), r0.y() - r1.y());
		}
		m_mousePos = p;
		repaint();
	}
	ev->accept();
}

//-----------------------------------------------------------------------------
void CPlotWidget::mouseReleaseEvent(QMouseEvent* ev)
{
	if (m_bzoomRect)
	{
		int X0 = m_mouseInitPos.x();
		int Y0 = m_mouseInitPos.y();
		int X1 = m_mousePos.x();
		int Y1 = m_mousePos.y();
		if (X1 < X0) { X0 ^= X1; X1 ^= X0; X0 ^= X1; }
		if (Y1 < Y0) { Y0 ^= Y1; Y1 ^= Y0; Y0 ^= Y1; }
		fitToRect(QRect(X0, Y0, X1-X0+1, Y1-Y0+1));
		m_bzoomRect = false;
		m_bvalidRect = false;
		emit doneZoomToRect();
		repaint();
	}
	ev->accept();
}

//-----------------------------------------------------------------------------
void CPlotWidget::wheelEvent(QWheelEvent* ev)
{
	if (m_bviewLocked == false)
	{
		double W = m_viewRect.width();
		double H = m_viewRect.height();
		double dx = W*0.05;
		double dy = H*0.05;
		if (ev->delta() < 0)
		{
			m_viewRect.adjust(-dx, -dy, dx, dy);
		}
		else
		{
			m_viewRect.adjust(dx, dy, -dx, -dy);
		}
		m_xscale = findScale(m_viewRect.left(), m_viewRect.right());
		m_yscale = findScale(m_viewRect.top(), m_viewRect.bottom());
		repaint();
		ev->accept();
	}
}

//-----------------------------------------------------------------------------
QPointF CPlotWidget::ScreenToView(const QPoint& p)
{
	qreal x = m_viewRect.left  () + (m_viewRect.width ()*(p.x() - m_screenRect.left())/(m_screenRect.width ()));
	qreal y = m_viewRect.bottom() + (m_viewRect.height()*(m_screenRect.top() - p.y() )/(m_screenRect.height()));
	return QPointF(x, y);
}

//-----------------------------------------------------------------------------
QPoint CPlotWidget::ViewToScreen(const QPointF& p)
{
	int x = m_screenRect.left() + (int)(m_screenRect.width ()*(p.x() - m_viewRect.left  ())/(m_viewRect.width ()));
	int y = m_screenRect.top () - (int)(m_screenRect.height()*(p.y() - m_viewRect.bottom())/(m_viewRect.height()));
	return QPoint(x, y);
}

//-----------------------------------------------------------------------------
void CPlotWidget::paintEvent(QPaintEvent* pe)
{
	// Process base event first
	QWidget::paintEvent(pe);

	// store the current rectangle
	m_screenRect = rect();
	if ((m_screenRect.width()==0)||
		(m_screenRect.height()==0)) return;

	// Create the painter class
	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing, true);

	// clear the background
	p.fillRect(m_screenRect, Qt::white);

/*	int W = rect().width();
	int H = rect().height();
	int NX = 10;
	QStringList colorNames = QColor::colorNames();
	int colors = colorNames.size();
	for (int i=0; i<colors; ++i)
	{
		int X = rect().x() + ((i%10)*(rect().width()))/NX;
		int Y = rect().y() + ((i/10)*(rect().width()))/NX;
		int w = rect().width()/NX, h = w;
		QColor col(colorNames[i]);
		p.fillRect(X, Y, w, h, col);
		QString s = QString("%1").arg(i);
		p.drawText(X, Y, w, h, Qt::AlignCenter, s);
	}
	return;
*/

	// render the title
	drawTitle(p);

	m_screenRect.adjust(50, 0, -90, -30);
	p.setBrush(Qt::NoBrush);
	p.drawRect(m_screenRect);

	// draw the grid
	drawGrid(p);

	// draw the grid axes
	drawAxes(p);

	// draw the legend
	if (m_bshowLegend) drawLegend(p);

	// render the data
	p.setClipRect(m_screenRect);
	drawAllData(p);

	if (m_bzoomRect && m_bvalidRect)
	{
		QRect rt(m_mouseInitPos, m_mousePos);
		p.setBrush(Qt::NoBrush);
		p.setPen(QPen(Qt::black, 1, Qt::DashLine));
		p.drawRect(rt);
	}

	// render the selection
	if (m_select) drawSelection(p);
}

void CPlotWidget::drawLegend(QPainter& p)
{
	int N = m_data.size();
	if (N == 0) return;

	QRect legendRect = m_screenRect;
	legendRect.setLeft(m_screenRect.right());
	legendRect.setRight(rect().right());

	QFontMetrics fm(p.font());
	int fh = fm.height();
	int fa = fm.ascent();

	int X0 = legendRect.left() + 10;
	int LW = 25;
	int X1 = X0 + LW + 5;
	int YC = legendRect.center().y();
	int H = legendRect.height() - 10;
	int Y0 = YC - N/2*(fh + 2);
	int Y1 = Y0 + N*(fh + 2);

	// draw the lines
	for (int i=0; i<N; ++i)
	{
		CPlotData& plot = m_data[i];
		p.setPen(QPen(plot.color(), 2));
		int Y = Y0 + i*(Y1 - Y0)/N;
		p.drawLine(X0, Y, X0 + LW, Y);
	}

	// draw the text
	p.setPen(Qt::black);
	for (int i=0; i<N; ++i)
	{
		CPlotData& plot = m_data[i];
		int Y = Y0 + i*(Y1 - Y0)/N;
		p.drawText(X1, Y + fa/3, plot.label());
	}
}

void CPlotWidget::drawSelection(QPainter& p)
{
	QPoint pt = ViewToScreen(m_selection.point);
	if (m_screenRect.contains(pt, true))
	{
		if ((m_selection.ndataIndex < 0) || (m_selection.ndataIndex >= m_data.size())) return;

		const QString& label = m_data[m_selection.ndataIndex].label();

		QFont font = p.font();
		QFont boldFont = font; boldFont.setBold(true);

		QFontMetrics fm(font);
		QString sx = QString("X:%1").arg(m_selection.point.x());
		QString sy = QString("Y:%1").arg(m_selection.point.y());
		int wx = fm.width(sx);
		int wy = fm.width(sy);
		int d = 3;
		int W = (wx > wy ? wx : wy) + 2*d;
		int H = 3*fm.height() + 4*d;
		if (W < H) W = H;
		p.setPen(Qt::black);

		int X = pt.x();
		int Y = pt.y();
		if (X + W > m_screenRect.right()) X = m_screenRect.right() - W;
		if (Y + H > m_screenRect.bottom()) Y = m_screenRect.bottom() - H;

		p.setBrush(Qt::black);
		p.drawEllipse(pt, 5, 5);
		p.setBrush(Qt::yellow);
		p.drawRect(X, Y, W, H);

		p.setFont(boldFont);
		p.drawText(X+d, Y + fm.ascent() + d, label);
		p.setFont(font);
		p.drawText(X+d, Y + fm.ascent() + fm.height() + 2*d, sx);
		p.drawText(X+d, Y + fm.ascent() + 2*fm.height() + 3*d, sy);
	}
}

//-----------------------------------------------------------------------------
void CPlotWidget::drawTitle(QPainter& p)
{
	QPen pen(Qt::black, 1);
	p.setPen(pen);
	QFont f("Times", 12, QFont::Bold);
	p.setFont(f);
	QFontMetrics fm(f);
	QRect titleRect = m_screenRect;
	titleRect.setHeight(fm.height() + 10);
	p.drawText(titleRect, Qt::AlignCenter, m_title);
	m_screenRect.setTop(titleRect.bottom());
}

//-----------------------------------------------------------------------------
void CPlotWidget::drawGrid(QPainter& p)
{
	char sz[256] = {0};
	QFont f("Arial", 10);
	QFontMetrics fm(f);
	p.setFont(f);
	p.setPen(QPen(Qt::black));

	int x0 = m_screenRect.left();
	int x1 = m_screenRect.right();
	int y0 = m_screenRect.top();
	int y1 = m_screenRect.bottom();

	double xscale = m_xscale;
	double yscale = m_yscale;

	// determine the y-scale
	double gy = 1;
	int nydiv = (int) log10(yscale);
	if (nydiv != 0)
	{
		gy = pow(10.0, nydiv);
		sprintf(sz, "x 1e%03d", nydiv);
		p.drawText(x0-30, y0 - fm.height() + fm.descent(), QString(sz));
	}

	// determine the x-scale
	double gx = 1;
	int nxdiv = (int) log10(xscale);
	if (nxdiv != 0)
	{
		gx = pow(10.0, nxdiv);
		sprintf(sz, "x 1e%03d", nxdiv);
		p.drawText(x1+5, y1, QString(sz));
	}

	p.setPen(QPen(Qt::black, 1));

	// draw the y-labels
	double fy = yscale*(int)(m_viewRect.top()/yscale);
	while (fy < m_viewRect.bottom())
	{
		int iy = ViewToScreen(QPointF(0.0, fy)).y();
		if (iy < y1)
		{
			double g = fy / gy;
			if (fabs(g) < 1e-7) g = 0;
			sprintf(sz, "%lg", g);
			QString s(sz);
			int w = p.fontMetrics().width(s);
			p.drawText(x0 -w - 5, iy + p.fontMetrics().height()/3, s);
//			fl_line(x0-3, iy, x0+3, iy);
		}
		fy += yscale;
	}

	// draw the x-labels
	double fx = xscale*(int)(m_viewRect.left()/xscale);
	while (fx < m_viewRect.right())
	{
		int ix = ViewToScreen(QPointF(fx, 0.0)).x();
		if (ix > x0)
		{
			double g = fx / gx;
			if (fabs(g) < 1e-7) g = 0;
			sprintf(sz, "%lg", g);
			QString s(sz);
			int w = p.fontMetrics().width(s);
			p.drawText(ix-w/2, y1+p.fontMetrics().height(), s);
//			fl_line(ix, y1-3, ix, y1+3);
		}
		fx += xscale;
	}

	p.setPen(QPen(Qt::lightGray, 1));
	p.setRenderHint(QPainter::Antialiasing, false);

	// draw the y-grid lines
	if (m_bdrawYLines)
	{
		fy = yscale*(int)(m_viewRect.top()/yscale);
		while (fy < m_viewRect.bottom())
		{
			int iy = ViewToScreen(QPointF(0.0, fy)).y();
			if (iy < y1)
			{
				QPainterPath path;
				path.moveTo(x0, iy);
				path.lineTo(x1-1, iy);
				p.drawPath(path);
			}
			fy += yscale;
		}
	}

	// draw the x-grid lines
	if (m_bdrawXLines)
	{
		fx = xscale*(int)(m_viewRect.left()/xscale);
		while (fx < m_viewRect.right())
		{
			int ix = ViewToScreen(QPointF(fx, 0.0)).x();
			if (ix > x0)
			{
				QPainterPath path;
				path.moveTo(ix, y0);
				path.lineTo(ix, y1-1);
				p.drawPath(path);
			}
			fx += xscale;
		}
	}

	p.setRenderHint(QPainter::Antialiasing, true);
}

//-----------------------------------------------------------------------------
void CPlotWidget::drawAxes(QPainter& p)
{
	// get the center in screen coordinates
	QPoint c = ViewToScreen(QPointF(0.0, 0.0));
	p.setPen(QPen(Qt::black, 2));

	// render the X-axis
	if (m_bdrawXAxis)
	{
		if ((c.y() > m_screenRect.top   ()) && (c.y() < m_screenRect.bottom()))
		{
			QPainterPath xaxis;
			xaxis.moveTo(m_screenRect.left (), c.y());
			xaxis.lineTo(m_screenRect.right(), c.y());
			p.drawPath(xaxis);
		}
	}

	// render the Y-axis
	if (m_bdrawYAxis)
	{
		if ((c.x() > m_screenRect.left ()) && (c.x() < m_screenRect.right()))
		{
			QPainterPath yaxis;
			yaxis.moveTo(c.x(), m_screenRect.top   ());
			yaxis.lineTo(c.x(), m_screenRect.bottom());
			p.drawPath(yaxis);
		}
	}
}

//-----------------------------------------------------------------------------
void CPlotWidget::drawAllData(QPainter& p)
{
	int N = m_data.size();
	for (int i=0; i<N; ++i)
	{
		QColor col = m_data[i].color();
		QPen pen(col, 2);
		p.setPen(pen);
		p.setBrush(col);
		drawData(p, m_data[i]);
	}
}

//-----------------------------------------------------------------------------
void CPlotWidget::drawData(QPainter& p, CPlotData& d)
{
	switch (m_chartStyle)
	{
	case LineChart: drawLineChartData(p, d); break;
	case BarChart : drawBarChartData(p, d); break;
	default:
		assert(false);
	}
}

//-----------------------------------------------------------------------------
void CPlotWidget::drawLineChartData(QPainter& p, CPlotData& d)
{
	int N = d.size();
	if (N == 0) return;

	QPainterPath path;
	QPoint pt = ViewToScreen(d.Point(0));
	path.moveTo(pt.x(), pt.y());
	QBrush b = p.brush();
	p.setBrush(Qt::NoBrush);
	for (int i=1; i<N; ++i)
	{
		pt = ViewToScreen(d.Point(i));
		path.lineTo(pt.x(), pt.y());
	}
	p.drawPath(path);

	// draw the marks
	p.setBrush(b);
	for (int i=0; i<N; ++i)
	{
		pt = ViewToScreen(d.Point(i));
		QRect r(pt.x()-2, pt.y()-2,5,5);
		p.drawRect(r);
	}
}

//-----------------------------------------------------------------------------
void CPlotWidget::drawBarChartData(QPainter& p, CPlotData& d)
{
	int N = d.size();
	if (N == 0) return;

	p.setPen(Qt::NoPen);
	p.setBrush(d.color());
	for (int i=0; i<N; ++i)
	{
		QPointF& pi = d.Point(i);
		QPoint p0 = ViewToScreen(pi);
		QPoint p1 = ViewToScreen(QPointF(pi.x(), 0.0));
		QRect r(p0.x()-5, p0.y(), 10, p1.y() - p0.y());
		p.drawRect(r);
	}
}

//-----------------------------------------------------------------------------
bool CPlotWidget::Save(const QString& fileName)
{
	// dump the data to a text file
	std::string sfile = fileName.toStdString();
	FILE* fp = fopen(sfile.c_str(), "wt");
	if (fp == 0) return false;

	std::string title = m_title.toStdString();
	fprintf(fp, "#Data : %s\n", title.c_str());
	fprintf(fp,"\n");

	CPlotData& plot = getPlotData(0);
	for (int i=0; i<plot.size(); i++)
	{
		double fx = plot.Point(i).x();
		fprintf(fp, "%16.9lg ", fx);
		for (int j=0; j<plots(); j++)
		{
			CPlotData& plotj = getPlotData(j);
			double fy = plotj.Point(i).y();
			fprintf(fp,"%16.9lg ", fy);
		}
		fprintf(fp,"\n");
	}
	fclose(fp);
	return true;
}
