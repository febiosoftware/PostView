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
#include "CommandPanel.h"
#include <QDialog>
#include <string>
#include <vector>

class CMainWindow;
class QModelIndex;

namespace Post {
	class FEDataField;
}

namespace Ui {
	class CDataPanel;
	class CDlgAddDataFile;
	class CDlgFilter;
}

class CDataPanel : public CCommandPanel
{
	Q_OBJECT

public:
	CDataPanel(CMainWindow* pwnd, QWidget* parent = 0);

	void Update(bool breset);

private slots:
	void on_AddStandard_triggered();
	void on_AddFromFile_triggered();
	void on_AddEquation_triggered();
	void on_CopyButton_clicked();
	void on_DeleteButton_clicked();
	void on_FilterButton_clicked();
	void on_ExportButton_clicked();
	void on_dataList_clicked(const QModelIndex&);
	void on_fieldName_editingFinished();
	void on_props_dataChanged();

private:
	Ui::CDataPanel* ui;
};

class CDlgAddDataFile : public QDialog
{
	Q_OBJECT

public:
	CDlgAddDataFile(QWidget* parent);

	void accept();

private slots:
	void onBrowse();

public:
	int	m_nclass;
	int	m_ntype;
	std::string	m_file;
	std::string m_name;

private:
	Ui::CDlgAddDataFile* ui;
};

class CDlgFilter : public QDialog
{
public:
	CDlgFilter(QWidget* parent);

	void setDataOperands(const std::vector<QString>& opNames);
	void setDataField(Post::FEDataField* pdf);

	int getArrayComponent();

	void accept();

	void setDefaultName(const QString& name);

	QString getNewName();

public:
	int	m_nflt;

	double	m_scale;

	double	m_theta;
	int		m_iters;

	int		m_nop;
	int		m_ndata;

private:
	Ui::CDlgFilter* ui;
};
