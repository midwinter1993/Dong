#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "tcpserver.h"
#include "curvePlot.h"

class QMainWindow;
class QToolBar;
class QWidget;
class QHBoxLayout;
//class CurvePlot;
class QwtCounter;
class QMenu;
class QMenuBar;
class QLabel;
class QTextEdit;
class QString;
class QToolButton;
class QVBoxLayout;

class MainWindow: public QMainWindow
{
	Q_OBJECT

private slots:
	void aboutSlot();
	void helpSlot();
	
public:
    MainWindow();
	~MainWindow(); 
	
private:
	void createMenus();
	void createButtons_Actions();
	
	QWidget *hBox;
	QLabel *label;
	QwtCounter *counter;
	QHBoxLayout *layout;
	
	QMenu *aboutMenu;
	QToolBar *toolBar;
	
	QTextEdit *textEdit;
	QAction *btnStart;
	QAction *btnStop;
	QAction *helpAction;	
	QAction *aboutQtAction;
	QAction *aboutAction;
	QAction *exitAction;

	QString styleSheet;

	TcpServer *tcpServer;
	CurvePlot *curvePlot;
};

#endif
