#ifndef __APPLE__
#include <glew.h>
#endif
#include "stdafx.h"
#include <QApplication>
#include "MainWindow.h"

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
	QApplication app(argc, argv);
	app.setWindowIcon(QIcon(":/icons/postview.png"));

	// initialize glew
	glewInit();

	// create the main window
	CMainWindow wnd;
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
