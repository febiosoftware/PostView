#pragma once
#include "CommandPanel.h"
#include <QDialog>
#include <string>
#include <vector>

class CMainWindow;
class QModelIndex;

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
