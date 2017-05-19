#pragma once
#include <QWidget>
#include <vector>
using namespace std;

namespace Ui {
	class CFileViewer;
}

class CMainWindow;
class QFileSystemModel;

class CFileViewer : public QWidget
{
	Q_OBJECT

public:
	CFileViewer(CMainWindow* pwnd, QWidget* parent = 0);

	QString currentPath() const;
	void setCurrentPath(const QString& s);

private slots:
	void on_fileList_doubleClicked(const QModelIndex& index);
	void on_fileFilter_currentIndexChanged(int index);
	void on_folder_editTextChanged(const QString& text);
	void on_toolUp_clicked();

private:
	CMainWindow*		m_wnd;
	QFileSystemModel*	m_fileSystem;
	vector<pair<QString, QString> > m_filters;

	Ui::CFileViewer*	ui;
};
