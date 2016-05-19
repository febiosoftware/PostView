#pragma once
#include <QWidget>
#include <vector>
using namespace std;

//-----------------------------------------------------------------------------
class QPainter;
class QAction;

//-----------------------------------------------------------------------------
// manages a set of (x,y) value pairs
class CPlotData
{
public:
	CPlotData();
	CPlotData(const CPlotData& d);
	CPlotData& operator = (const CPlotData& d);

	//! clear data
	void clear();

	// add a point to the data
	void addPoint(double x, double y);

	// number of points
	int size() const { return (int) m_data.size(); }

	// get a data point
	QPointF& Point(int i) { return m_data[i]; }

	// get the bounding rectangle
	QRectF boundRect() const;

	// set/get the label
	const QString& label() const { return m_label; }
	void setLabel(const QString& label) { m_label = label; }

	// set/get color
	QColor color() const { return m_col; }
	void setColor(const QColor& col) { m_col = col; }

protected:
	vector<QPointF>	m_data;
	QString			m_label;
	QColor			m_col;
};

//-----------------------------------------------------------------------------
//! This class implements a plotting widget. 
class CPlotWidget : public QWidget
{
	Q_OBJECT

public:
	struct Selection
	{
		int			ndataIndex;
		QPointF		point;
	};

public:
	//! constructor
	CPlotWidget(QWidget* parent = 0, int w = 0, int h = 0);

	//! Set the plot title
	void setTitle(const QString& s);
	QString title() const { return m_title; }

	// size hint
	QSize sizeHint() const { return m_sizeHint; }
	QSize minimumSizeHint() const { return QSize(200, 200); }

	// clear plot data
	// This just clears the data for each plot
	// but does not delete the plots
	void clearData();

	// clears everything
	void clear();

	// change the view so that it fits the data
	void fitWidthToData();
	void fitHeightToData();
	void fitToData();
	void fitToRect(const QRect& rt);

	// add a data field
	void addPlotData(CPlotData& p);

	// get a data field
	int plots() { return (int) m_data.size(); }
	CPlotData& getPlotData(int i) { return m_data[i]; }

	void ZoomToRect(bool b = true);

signals:
	void doneZoomToRect();

protected:
	void mousePressEvent  (QMouseEvent* ev);
	void mouseMoveEvent   (QMouseEvent* ev);
	void mouseReleaseEvent(QMouseEvent* ev);
	void contextMenuEvent (QContextMenuEvent* ev);
	void wheelEvent       (QWheelEvent* ev);

public:
	QString	m_title;
	QRectF	m_viewRect;
	QRect	m_screenRect;
	QPoint	m_mousePos, m_mouseInitPos;
	double	m_xscale, m_yscale;

	bool		m_bzoomRect;
	bool		m_bvalidRect;

	bool		m_select;
	Selection	m_selection;

	QPointF ScreenToView(const QPoint& p);
	QPoint ViewToScreen(const QPointF& p);

private:
	//! render the plot
	void paintEvent(QPaintEvent* pe);

public slots:
	void OnZoomToWidth();
	void OnZoomToHeight();
	void OnZoomToFit();
	void OnShowProps();
	void OnCopyToClipboard();

private: // drawing helper functions
	void drawAxes(QPainter& p);
	void drawAllData(QPainter& p);
	void drawData(QPainter& p, CPlotData& data);
	void drawGrid(QPainter& p);
	void drawTitle(QPainter& p);
	void drawSelection(QPainter& p);
	void drawLegend(QPainter& p);

private:
	vector<CPlotData>	m_data;

private:
	QAction*	m_pZoomToFit;
	QAction*	m_pShowProps;
	QAction*	m_pCopyToClip;
	QSize		m_sizeHint;
};
