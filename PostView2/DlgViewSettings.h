#pragma once
#include <QDialog>
#include "PostViewLib/ColorMap.h"

class CMainWindow;

namespace Ui {
	class CDlgViewSettings;
};

class CDlgViewSettings : public QDialog
{
	Q_OBJECT

public:
	CDlgViewSettings(CMainWindow* pwnd);
	virtual ~CDlgViewSettings();

public slots:
	void accept();

protected:
	CMainWindow*			m_pwnd;
	Ui::CDlgViewSettings*	ui;
};
