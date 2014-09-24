#include <qapplication.h>
#include <QSplashScreen>
#include <QTextCodec>
#include "main_window.h"
#include "def.h"


int main(int argc, char **argv)
{
    QApplication a(argc, argv);
	
	QTextCodec *codec = QTextCodec::codecForName("system");    //获取系统编码
	QTextCodec::setCodecForCStrings(codec);
	QTextCodec::setCodecForLocale(codec);
	QTextCodec::setCodecForTr(codec);
	
    MainWindow mainWindow;

	//QSplashScreen *splash = new QSplashScreen;
	//splash->setPixmap(QPixmap(":/images/splash.png"));
	//splash->show();

	// Qt::Alignment topRight = Qt::AlignRight | Qt::AlignTop;
	// splash->showMessage(QObject::tr("loading..."), topRight, Qt::white);

	//splash->finish(&mainWindow);
	//delete splash;

    mainWindow.resize(1000, 600);
    mainWindow.show();

    return a.exec(); 
}
