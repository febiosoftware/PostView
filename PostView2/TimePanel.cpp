/*This file is part of the PostView source code and is licensed under the MIT license
listed below.

See Copyright-PostView.txt for details.

Copyright (c) 2020 University of Utah, The Trustees of Columbia University in 
the City of New York, and others.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include "stdafx.h"
#include "TimePanel.h"
#include "ui_timepanel.h"
#include "MainWindow.h"
#include "Document.h"
using namespace Post;

CTimePanel::CTimePanel(CMainWindow* wnd, QWidget* parent) : QWidget(parent), m_wnd(wnd), ui(new Ui::CTimePanel)
{
	ui->setupUi(this);
}

void CTimePanel::Update(bool reset)
{
	CDocument* doc = m_wnd->GetActiveDocument();
	if (doc && doc->IsValid())
	{
		FEPostModel* fem = doc->GetFEModel();
		if (fem)
		{
			if (reset)
			{
				ui->timer->clearData();
				FEPostModel& fem = *doc->GetFEModel();
				vector<double> data(fem.GetStates());
				int nstates = fem.GetStates();
				for (int i = 0; i < nstates; ++i) data[i] = fem.GetState(i)->m_time;

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
