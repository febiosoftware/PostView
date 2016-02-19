#include "stdafx.h"
#include "PlotWidget.h"
#include <QPainter>
#include <QFontDatabase>
#include <QMouseEvent>
#include <QAction>
#include <QMessageBox>
#include <QMenu>
#include <QApplication>
#include <QClipBoard>

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
}

//-----------------------------------------------------------------------------
CPlotData& CPlotData::operator = (const CPlotData& d)
{
	m_data = d.m_data;
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
	static int n = 0;

	m_ncol = 13 + n++;

	m_viewRect = QRectF(0.0, 0.0, 1.0, 1.0);
	m_xscale = findScale(m_viewRect.left(), m_viewRect.right());
	m_yscale = findScale(m_viewRect.top(), m_viewRect.bottom());

	if (w < 200) w = 200;
	if (h < 200) h = 200;
	m_sizeHint = QSize(w, h);

	m_pZoomToFit = new QAction(tr("Zoom to fit"), this);
	connect(m_pZoomToFit, SIGNAL(triggered()), this, SLOT(OnZoomToFit()));

	m_pShowProps = new QAction(tr("Properties"), this);
	connect(m_pShowProps, SIGNAL(triggered()), this, SLOT(OnShowProps()));

	m_pCopyToClip = new QAction(tr("Copy to clipboard"), this);
	connect(m_pCopyToClip, SIGNAL(triggered()), this, SLOT(OnCopyToClipboard()));
}

//-----------------------------------------------------------------------------
void CPlotWidget::contextMenuEvent(QContextMenuEvent* ev)
{
	QMenu menu(this);
	menu.addAction(m_pZoomToFit);
	menu.addAction(m_pCopyToClip);
	menu.addSeparator();
	menu.addAction(m_pShowProps);
	menu.exec(ev->globalPos());
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
	QMessageBox b;
	b.setText("Comgin soon!");
	b.setIcon(QMessageBox::Information);
	b.exec();
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
			QString s("x\ty\n");
			for (int i=0; i<d.size(); ++i)
			{
				QPointF& pi = d.Point(i);
				s.append(QString::asprintf("%lg\t%lg\n", pi.x(), pi.y()));
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
	for (int i=0; i<(int) m_data.size(); ++i) m_data[i].clear();
}

//-----------------------------------------------------------------------------
void CPlotWidget::clear()
{
	m_data.clear();
}

//-----------------------------------------------------------------------------
void CPlotWidget::addPlotData(const CPlotData& p)
{
	m_data.push_back(p);
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

	m_viewRect = r;

	double dx = 0.05*m_viewRect.width();
	double dy = 0.05*m_viewRect.height();
	m_viewRect.adjust(0.0, 0.0, dx, dy);

	m_xscale = findScale(m_viewRect.left(), m_viewRect.right());
	m_yscale = findScale(m_viewRect.top(), m_viewRect.bottom());
}

//-----------------------------------------------------------------------------
void CPlotWidget::mousePressEvent(QMouseEvent* ev)
{
	m_mousePos = ev->pos();
}

//-----------------------------------------------------------------------------
void CPlotWidget::mouseMoveEvent(QMouseEvent* ev)
{
	QPoint p = ev->pos();
	QPointF r0 = ScreenToView(m_mousePos);
	QPointF r1 = ScreenToView(p);
	m_viewRect.translate(r0.x() - r1.x(), r0.y() - r1.y());
	m_mousePos = p;
	repaint();
}

//-----------------------------------------------------------------------------
void CPlotWidget::mouseReleaseEvent(QMouseEvent* ev)
{

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

	// render the title
	drawTitle(p);

	m_screenRect.adjust(50, 0, -60, -30);
	p.setBrush(Qt::NoBrush);
	p.drawRect(m_screenRect);

	// draw the grid
	drawGrid(p);

	// draw the grid axes
	drawAxes(p);

	// render the data
	p.setClipRect(m_screenRect);
	drawAllData(p);
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

	// draw the x-grid lines
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

	p.setRenderHint(QPainter::Antialiasing, true);
}

//-----------------------------------------------------------------------------
void CPlotWidget::drawAxes(QPainter& p)
{
	// get the center in screen coordinates
	QPoint c = ViewToScreen(QPointF(0.0, 0.0));
	p.setPen(QPen(Qt::black, 2));

	// render the X-axis
	if ((c.y() > m_screenRect.top   ()) &&
		(c.y() < m_screenRect.bottom()))
	{
		QPainterPath xaxis;
		xaxis.moveTo(m_screenRect.left (), c.y());
		xaxis.lineTo(m_screenRect.right(), c.y());
		p.drawPath(xaxis);
	}

	// render the Y-axis
	if ((c.x() > m_screenRect.left ()) &&
		(c.x() < m_screenRect.right()))
	{
		QPainterPath yaxis;
		yaxis.moveTo(c.x(), m_screenRect.top   ());
		yaxis.lineTo(c.x(), m_screenRect.bottom());
		p.drawPath(yaxis);
	}
}

//-----------------------------------------------------------------------------
void CPlotWidget::drawAllData(QPainter& p)
{
	QStringList colorNames = QColor::colorNames();

	int N = m_data.size();
	for (int i=0; i<N; ++i)
	{
		QColor col(colorNames[(i+m_ncol)%colorNames.count()]);
		QPen pen(col, 2);
		p.setPen(pen);
		p.setBrush(col);
		drawData(p, m_data[i]);
	}
}

//-----------------------------------------------------------------------------
void CPlotWidget::drawData(QPainter& p, CPlotData& d)
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
