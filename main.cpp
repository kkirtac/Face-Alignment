#include "facealignment.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	FaceAlignment w;
	w.show();
	return a.exec();
}
