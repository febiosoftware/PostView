#include "stdafx.h"
#include "PostViewApp.h"
#include <QMessageBox>
#include <QtNetwork/QLocalSocket>

// implementation in part based on https://github.com/itay-grudev/SingleApplication

PostViewApplication::PostViewApplication(int& argc, char* argv[]) : QApplication(argc, argv), m_sharedMem("PostViewAppData")
{
	m_socket = nullptr;
	m_isPrimary = true;

	if (m_sharedMem.create(256))
	{
		m_localServer.listen("PostViewLocalServer");
		QObject::connect(&m_localServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
	}
	else
	{
		m_isPrimary = false;

#ifdef _DEBUG
		QMessageBox::critical(nullptr, "PostView2", "An instance of PostView is already running");
#endif

		if (argc == 2)
		{
			QLocalSocket* socket = new QLocalSocket();
			socket->connectToServer("PostViewLocalServer");

			if (socket->waitForConnected())
			{
#ifdef _DEBUG
				QMessageBox::information(nullptr, "PostView", "Connected to PostView server!");
#endif
				QByteArray data(argv[1]);

				socket->write(data);
				bool dataWritten = socket->waitForBytesWritten();
				socket->flush();
			}

			socket->deleteLater();
		}
	}
}

PostViewApplication::~PostViewApplication()
{
	if (m_sharedMem.isAttached())
	{
		m_sharedMem.detach();
	}

	if (m_localServer.isListening())
	{
		m_localServer.close();
	}
}

bool PostViewApplication::IsPrimary()
{
	return m_isPrimary;
}

void PostViewApplication::onNewConnection()
{
//	QMessageBox::information(0, "PostView", "A new connection was detected.");

	m_socket = m_localServer.nextPendingConnection();

	QObject::connect(m_socket, SIGNAL(readyRead()), this, SLOT(onReadReady()));
}

void PostViewApplication::onReadReady()
{
	if (m_socket == nullptr) return;

	qint64 bytesReady = m_socket->bytesAvailable();
	QByteArray buf = m_socket->read(bytesReady);

	QString s(buf);

	std::string ss = s.toStdString();

	fprintf(stderr, "\nPostView message: %s\n", ss.c_str());

	emit loadFile(s);
}
