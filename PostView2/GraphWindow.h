#pragma once
#include <QMainWindow>

class CMainWindow;

namespace Ui {
	class CGraphWindow;
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
	void TrackNodeHistory   (int node , float* pval, int nfield, int nmin=0, int nmax=-1);

private slots:
	void on_selectTime_currentIndexChanged(int);
	void on_selectX_currentIndexChanged(int);
	void on_selectY_currentIndexChanged(int);
	void on_selectPlot_currentIndexChanged(int);

private:
	CMainWindow*		m_wnd;
	Ui::CGraphWindow*	ui;

	bool	m_bUserRange;			//!< user sets range
	bool	m_bAutoRange;			//!< track timer bar range
	bool	m_bTrackTime;			//!< track current time point only
	int		m_nUserMin, m_nUserMax;	//!< manual time step range

	int	m_nTimeMin, m_nTimeMax;	//!< actual time range being plotted
};
