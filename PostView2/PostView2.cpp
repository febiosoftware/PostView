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

	return app.exec();
}
