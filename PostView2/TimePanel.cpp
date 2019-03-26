#include "stdafx.h"
#include "TimePanel.h"
#include "ui_timepanel.h"
#include "MainWindow.h"
#include "Document.h"

CTimePanel::CTimePanel(CMainWindow* wnd, QWidget* parent) : QWidget(parent), m_wnd(wnd), ui(new Ui::CTimePanel)
{
	ui->setupUi(this);
}

void CTimePanel::Update(bool reset)
{
	CDocument* doc = m_wnd->GetActiveDocument();
	if (doc && doc->IsValid())
	{
		if (reset)
		{
			ui->timer->clearData();
			FEModel& fem = *doc->GetFEModel();
			vector<double> data(fem.GetStates());
			int nstates = fem.GetStates();
			for (int i=0; i<nstates; ++i) data[i] = fem.GetState(i)->m_time;

			ui->timer->setTimePoints(data);

			TIMESETTINGS& time = doc->GetTimeSettings();
			time.m_start = 0;
			time.m_end = nstates - 1;
			ui->timer->setRange(time.m_start, time.m_end);
		}

		int ntime = doc->currentTime();
		ui->timer->setSelection(ntime);

		double ftime = doc->GetTimeValue();
		ui->timer->setCurrentTime(ftime);
	}
	else ui->timer->clearData();
}

void CTimePanel::SetRange(int nmin, int nmax)
{
	ui->timer->setRange(nmin, nmax);
}

void CTimePanel::on_timer_pointClicked(int i)
{
	m_wnd->SetCurrentTime(i);
}

void CTimePanel::on_timer_rangeChanged(int nmin, int nmax)
{
	CDocument* doc = m_wnd->GetActiveDocument();
	if (doc->IsValid())
	{
		TIMESETTINGS& time = doc->GetTimeSettings();
		time.m_start = nmin;
		time.m_end   = nmax;

		int ntime = doc->currentTime();

		if ((ntime < nmin) || (ntime > nmax))
		{
			if (ntime < nmin) ntime = nmin;
			if (ntime > nmax) ntime = nmax;
		}
		m_wnd->SetCurrentTime(ntime);
	}
}
