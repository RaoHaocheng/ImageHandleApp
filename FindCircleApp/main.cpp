#include "findCircleApp.h"

int main(int argc, char *argv[])
{
	//Q_INIT_RESOURCE(data);
	FindCircleApp application(argc, argv);
	application.newMainWindow();

	return application.exec();
}
