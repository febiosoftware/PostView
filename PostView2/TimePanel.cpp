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
	CDocument* doc = m_wnd->GetDocument();
	if (doc->IsValid())
	{
		if (reset)
		{
			ui->timer->clearData();
			FEModel& fem = *doc->GetFEModel();
			vector<double> data(fem.GetStates());
			for (int i=0; i<fem.GetStates(); ++i) data[i] = fem.GetState(i)->m_time;

			ui->timer->setTimePoints(data);
		}

		int ntime = doc->currentTime();
		ui->timer->setSelection(ntime);
	}
	else ui->timer->clearData();
}

void CTimePanel::SetCurrentTime(int n)
{
	ui->timer->setSelection(n);
}

void CTimePanel::on_timer_pointClicked(int i)
{
	m_wnd->SetCurrentTime(i);
}
