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

#pragma once
#include <QDialog>
#include "PostLib/ColorMap.h"

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

	void setColorMap(const Post::CColorMap& m);

private:
	Post::CColorMap	m_map;
};

class CColormapWidget : public QWidget
{
	Q_OBJECT

public:
	CColormapWidget(QWidget* parent = 0);

	void updateColorMap(const Post::CColorMap& map);

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
	void accept() override;
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
