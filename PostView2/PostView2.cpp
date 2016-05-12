#include <glew.h>
#include "stdafx.h"
#include <QApplication>
#include "MainWindow.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	// initialize glew
	glewInit();

	// create the main window
	CMainWindow wnd;
	wnd.show();

	if (argc==2)
	{
		bool bok = wnd.OpenFile(argv[1], -1);
	}

	return app.exec();
}
