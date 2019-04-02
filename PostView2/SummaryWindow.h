#pragma once
#include <QMainWindow>
#include "GraphWindow.h"
#include "Document.h"

class CMainWindow;

class FEModel;

class CSummaryWindow : public CGraphWindow
{
	Q_OBJECT

public:
	struct RANGE
	{
		float	fmax, fmin, favg;
	};

public:
	CSummaryWindow(CMainWindow* wnd);

	void Update(bool breset, bool bfit = false) override;

private:
	RANGE EvalNodeRange(FEModel& fem, int ntime, bool bsel);
	RANGE EvalEdgeRange(FEModel& fem, int ntime, bool bsel);
	RANGE EvalFaceRange(FEModel& fem, int ntime, bool bsel, bool bvol);
	RANGE EvalElemRange(FEModel& fem, int ntime, bool bsel, bool bvol);

private:
	int		m_ncurrentData;
};
