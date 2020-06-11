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

#ifdef WIN32
#include <glew.h>
#endif
#include "stdafx.h"
#include <QApplication>
#include <QMessageBox>
#include "MainWindow.h"
#include "PostViewApp.h"

#ifdef __APPLE__
#include <QFileOpenEvent>
class MyApplication : public QApplication
{
public:
    MyApplication(int &argc, char **argv)
    : QApplication(argc, argv)
    {
    }
    void SetMainWindow(CMainWindow* wnd) { m_pWnd = wnd; }
    
    bool event(QEvent *event)
    {
        if (event->type() == QEvent::FileOpen) {
            QFileOpenEvent *openEvent = static_cast<QFileOpenEvent *>(event);
            m_pWnd->OpenFile(openEvent->file(), -1);
        }
        
        return QApplication::event(event);
    }
    
public:
    CMainWindow* m_pWnd;
};
#endif

int main(int argc, char* argv[])
{
#ifndef __APPLE__

#ifdef WIN32
	PostViewApplication app(argc, argv);
	app.setWindowIcon(QIcon(":/icons/postview.png"));

	// If we are not the primary application
	// and a file name is specified, we send the file name
	// to the primary app and end this one
	if ((app.IsPrimary() == false) && (argc >= 2))
	{
		app.SendToPrimary(argc, argv);
		return 0;
	}

#else
	QApplication app(argc, argv);
	app.setWindowIcon(QIcon(":/icons/postview.png"));
#endif

	// initialize glew
#ifdef WIN32
	glewInit();
#endif
	// create the main window
	CMainWindow wnd;
	
#ifdef WIN32
	QObject::connect(&app, SIGNAL(loadFile(const QString&)), &wnd, SLOT(onAppLoadFile(const QString&)));
#endif
	
	wnd.show();

	if (argc==2)
	{
		wnd.OpenFile(argv[1], -1);
	}

	return app.exec();
#else
    MyApplication app(argc, argv);
    
    // create the main window
    CMainWindow wnd;
    wnd.show();
    
    app.SetMainWindow(&wnd);
    
    if (argc==2)
    {
        wnd.OpenFile(argv[1], -1);
    }
    
    return app.exec();
#endif
}
