#include "App.h"

#include <qmessagebox.h>

#include "NeuronSim/Log.h"

App::App(int &argc, char **argv, int flags) :
	QApplication(argc, argv, flags)
{
}

App::~App()
{
}

bool App::notify(QObject * receiver, QEvent * event)
{
	bool ret = true;
	try
	{
		ret = QApplication::notify(receiver, event);
	}
	catch (const std::exception& ex)
	{
		LOG("Exception caught [" << ex.what() << "]");
		QMessageBox::critical(0, "Unhandled Exception", ex.what());
	}
	catch (...)
	{
		LOG("Inexplicable exception caught");
		QMessageBox::critical(0, "Unhandled Exception", "Unknown error - sorry");
	}
	return ret;
}
