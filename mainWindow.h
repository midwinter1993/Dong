#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "tcpserver.h"
#include "curvePlot.h"
#include "spectrogramPlot.h"
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
class settingWindow;
class QVBoxLayout;

class MainWindow: public QMainWindow
{
	Q_OBJECT

private slots:
	void aboutSlot();
	void helpSlot();
	
	void settingSlot();
	void changeOpacitySlot(qreal opacity);

	void test();

signals:
	void setOpacitySignal(qreal opacity);

public:
    MainWindow();
	~MainWindow(); 
	
private:
	void createMenus();
	void createButtons_Actions();
	void showInfo(QString text = QString::null);
	
	QWidget *hBox;
	QLabel *label;
	QwtCounter *counter;
	QHBoxLayout *layout;
	
	QMenu *fileMenu;
	QMenu *aboutMenu;
	QToolBar *toolBar;
	
	QTextEdit *textEdit;
	QAction *btnStart;
	QAction *btnStop;
	QAction *helpAction;	
	QAction *btnClear;
	QAction *aboutQtAction;
	QAction *aboutAction;
	QAction *fileAction;
	QAction *exitAction;


    QAction*btnZoom;

	QString styleSheet;
	settingWindow *settingWin; 
	QAction *settingAction;

	TcpServer *tcpServer;
	CurvePlot *curvePlot;
	// CurvePlot *curvePlot_origin;
	SpectrogramPlot *spectroPlot;
};

#endif
