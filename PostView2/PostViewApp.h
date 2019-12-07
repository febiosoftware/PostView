#pragma once
#include <QApplication>
#include <QtCore/QSharedMemory>
#include <QtNetwork/QLocalServer>

class PostViewApplication : public QApplication
{
	Q_OBJECT

public:
	PostViewApplication(int& argc, char* argv[]);

	~PostViewApplication();

	bool IsPrimary();

public slots:
	void onNewConnection();
	void onReadReady();

signals:
	void loadFile(const QString& s);

private:
	QSharedMemory	m_sharedMem;
	QLocalServer	m_localServer;
	QLocalSocket*	m_socket;
	bool			m_isPrimary;
};
