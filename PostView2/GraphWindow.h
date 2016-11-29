#pragma once
#include <QMainWindow>

class CMainWindow;
class CPlotWidget;
class QLineEdit;

namespace Ui {
	class CGraphWindow;
};

class RegressionUi : public QWidget
{
	Q_OBJECT
public:
	QLineEdit*	a;
	QLineEdit*	b;

public:
	RegressionUi(CPlotWidget* graph, QWidget* parent = 0);

public slots:
	void onCalculate();

private:
	CPlotWidget* m_graph;
};


class CGraphWindow : public QMainWindow
{
	Q_OBJECT

public:
	CGraphWindow(CMainWindow* wnd);

	void Update(bool breset = true);

private:
	// track mesh data
	void TrackElementHistory(int nelem, float* pval, int nfield, int nmin=0, int nmax=-1);
	void TrackFaceHistory   (int nface, float* pval, int nfield, int nmin=0, int nmax=-1);
	void TrackEdgeHistory   (int edge , float* pval, int nfield, int nmin=0, int nmax=-1);
	void TrackNodeHistory   (int node , float* pval, int nfield, int nmin=0, int nmax=-1);

private slots:
	void on_selectTime_currentIndexChanged(int);
	void on_selectX_currentIndexChanged(int);
	void on_selectY_currentIndexChanged(int);
	void on_selectPlot_currentIndexChanged(int);
	void on_actionSave_triggered();
	void on_actionClipboard_triggered();
	void on_actionProps_triggered();
	void on_actionZoomWidth_triggered();
	void on_actionZoomHeight_triggered();
	void on_actionZoomFit_triggered();
	void on_actionZoomSelect_toggled(bool bchecked);
	void on_plot_doneZoomToRect();

private:
	void addSelectedNodes();
	void addSelectedEdges();
	void addSelectedFaces();
	void addSelectedElems();

private:
	CMainWindow*		m_wnd;
	Ui::CGraphWindow*	ui;

	bool	m_bUserRange;			//!< user sets range
	bool	m_bAutoRange;			//!< track timer bar range
	bool	m_bTrackTime;			//!< track current time point only
	int		m_nUserMin, m_nUserMax;	//!< manual time step range

	int	m_nTimeMin, m_nTimeMax;	//!< actual time range being plotted

private: // temporary variables used during update
	int	m_xtype;						// x-plot field option (0=time, 1=steps, 2=data field)
	int	m_firstState, m_lastState;		// first and last time step to be evaluated
	int	m_dataX, m_dataY;				// X and Y data field IDs
};
