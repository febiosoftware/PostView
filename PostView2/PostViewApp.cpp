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

#include "stdafx.h"
#include "PostViewApp.h"
#include <QMessageBox>
#include <QtNetwork/QLocalSocket>

// implementation in part based on https://github.com/itay-grudev/SingleApplication

PostViewApplication::PostViewApplication(int& argc, char* argv[]) : QApplication(argc, argv)
{
	m_socket = nullptr;
	m_localServer = nullptr;
	m_isPrimary = true;

	// create a shared memory object
	m_sharedMem = new QSharedMemory("PostViewAppData");

	// try to create the shared memory segment. 
	// If this fails, an instance of PostView is already running.
	if (m_sharedMem->create(256))
	{
		// This is the first instance of PostView, so start a local server
		// that will listen to connections.
		m_localServer = new QLocalServer();
		m_localServer->listen("PostViewLocalServer");
		QObject::connect(m_localServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
	}
	else
	{
		// we don't need the shared memory object, so delete it
		delete m_sharedMem;
		m_sharedMem = nullptr;

		// Set flag that this is NOT the primary application
		m_isPrimary = false;

#ifdef _DEBUG
		QMessageBox::critical(nullptr, "PostView2", "An instance of PostView is already running");
#endif
	}
}

void PostViewApplication::SendToPrimary(int& args, char* argv[])
{
	// create a local socket and connect it to the server (which is running on the primary app).
	QLocalSocket* socket = new QLocalSocket();
	socket->connectToServer("PostViewLocalServer");

	if (socket->waitForConnected())
	{
#ifdef _DEBUG
		QMessageBox::information(nullptr, "PostView", "Connected to PostView server!");
#endif
		// convert the file name to a byte array
		QByteArray data(argv[1]);

		// send the file name
		socket->write(data);
		bool dataWritten = socket->waitForBytesWritten();
		socket->flush();
	}

	socket->deleteLater();
}

PostViewApplication::~PostViewApplication()
{
	// Delete the shared memory object. 
	// This will detach the process from the shared memory segment. 
	if (m_sharedMem)
	{
		delete m_sharedMem;
		m_sharedMem = nullptr;
	}

	// close the server
	if (m_localServer)
	{
		if (m_localServer->isListening())
			m_localServer->close();

		delete m_localServer; m_localServer = nullptr;
	}
}

bool PostViewApplication::IsPrimary()
{
	return m_isPrimary;
}

void PostViewApplication::onNewConnection()
{
	assert(m_localServer);

	// get the socket that is connecting. 
	m_socket = m_localServer->nextPendingConnection();

	// connect the socket's readyRead signal to a slot that will read the data from the socket.
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

	// emit request to load the file
	emit loadFile(s);
}
