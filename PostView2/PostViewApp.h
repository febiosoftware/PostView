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

	void SendToPrimary(int& args, char* argv[]);

public slots:
	void onNewConnection();
	void onReadReady();

signals:
	void loadFile(const QString& s);

private:
	QSharedMemory*	m_sharedMem;
	QLocalServer*	m_localServer;
	QLocalSocket*	m_socket;
	bool			m_isPrimary;
};
