#ifndef __APPLE__
#include <glew.h>
#endif
#include "stdafx.h"
#include <QApplication>
#include "MainWindow.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

#ifndef __APPLE__
	// initialize glew
	glewInit();
#endif

	// create the main window
	CMainWindow wnd;
	wnd.show();

	if (argc==2)
	{
		wnd.OpenFile(argv[1], -1);
	}

	return app.exec();
}
