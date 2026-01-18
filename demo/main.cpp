#include <QApplication>
#include "demowidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	DemoWidget window;
	   
	   // Проверяем наличие аргументов командной строки
	   if (argc >= 3) {
	       QString imagePath = QString::fromLocal8Bit(argv[1]);
	       QString folderPath = QString::fromLocal8Bit(argv[2]);
	       
	       // Передаем аргументы в виджет
	       window.setInitialParameters(imagePath, folderPath);
	   }
	   
	   window.showMaximized(); // Открываем окно в полноэкранном режиме

    return a.exec();
}
