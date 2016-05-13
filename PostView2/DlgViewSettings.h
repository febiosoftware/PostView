#pragma once
#include <QDialog>
#include "PostViewLib/ColorMap.h"

class CMainWindow;
class QAbstractButton;

namespace Ui {
	class CDlgViewSettings;
};

class CDlgViewSettings : public QDialog
{
	Q_OBJECT

public:
	CDlgViewSettings(CMainWindow* pwnd);
	virtual ~CDlgViewSettings();

	void apply();

public slots:
	void accept();
	void onClicked(QAbstractButton*);

protected:
	CMainWindow*			m_pwnd;
	Ui::CDlgViewSettings*	ui;
};
