#pragma once
#include <QDialog>
#include "PostViewLib/ColorMap.h"

class CMainWindow;
class QAbstractButton;
class QGridLayout;
class QSpinBox;
class QComboBox;

namespace Ui {
	class CDlgViewSettings;
};

class ColorGradient : public QWidget
{
public:
	ColorGradient(QWidget* parent = 0);

	QSize sizeHint() const override;

	void paintEvent(QPaintEvent* ev) override;

	void setColorMap(const CColorMap& m);

private:
	CColorMap	m_map;
};

class CColormapWidget : public QWidget
{
	Q_OBJECT

public:
	CColormapWidget(QWidget* parent = 0);

	void updateColorMap(const CColorMap& map);

	void clearGrid();

protected slots:
	void currentMapChanged(int n);
	void onDataChanged();
	void onSpinValueChanged(int n);
	void onNew();
	void onDelete();
	void onEdit();
	void onInvert();

private:
	void updateMaps();

private:
	QGridLayout*	m_grid;
	QSpinBox*		m_spin;
	QComboBox*		m_maps;
	ColorGradient*	m_grad;
	int				m_currentMap;
};

class CDlgViewSettings : public QDialog
{
	Q_OBJECT

public:
	CDlgViewSettings(CMainWindow* pwnd);
	virtual ~CDlgViewSettings();

	void apply();

	void OnReset();

	void showEvent(QShowEvent* ev) override;
	void hideEvent(QHideEvent* ev) override;

public slots:
	void accept();
	void onClicked(QAbstractButton*);
	void on_load_clicked();
	void on_save_clicked();
	void on_create_clicked();
	void on_apply_clicked();

private:
	void UpdatePalettes();

protected:
	CMainWindow*			m_pwnd;
	Ui::CDlgViewSettings*	ui;
};
