#pragma once
#include <QListView>

class QFileSystemModel;
class CMainWindow;

class CFileViewer : public QListView
{
	Q_OBJECT

public:
	CFileViewer(CMainWindow* pwnd, QWidget* parent = 0);

private slots:
	void on_fileViewer_doubleClicked(const QModelIndex& index);

private:
	QFileSystemModel*	m_fileSystem;
	CMainWindow*		m_wnd;
};
