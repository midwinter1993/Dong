#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <qwt_polygon.h>

class QMainWindow;
class QToolBar;
class QWidget;
class QHBoxLayout;
class DataPlot;
class QwtCounter;
class QMenu;
class QMenuBar;
class QLabel;
class QTextEdit;
class QString;

class QwtPlotZoomer;
class QwtPlotPicker;
class QwtPlotPanner;
class QToolButton;
class settingWindow;

//class QTcpServer;
//class QTcpSocket;

class MainWindow: public QMainWindow
{
	Q_OBJECT

private slots:
	void aboutSlot();
	void helpSlot();
	void movedSlot(const QPoint &);
    void selectedSlot(const QwtPolygon &);
    void enableZoomModeSlot(bool);
	void settingSlot();
	void changeOpacitySlot(qreal opacity);
	//void acceptConnevtionSlot();
	//void readDataSlot();

signals:
	void setOpacitySignal(qreal opacity);
	//void updataPlotSignal();

public:
    MainWindow();
	~MainWindow(); 
private:
	QToolBar *toolBar;
	QWidget *hBox;
	QLabel *label;
	QwtCounter *counter;
	QHBoxLayout *layout;
	DataPlot *plot;

	QAction *btnStart;
	QAction *btnStop;
	QAction *helpAction;	
	QAction *btnClear;

	void createMenus();
	void createButtons_Actions();
	//void initTcp();
	
	void showInfo(QString text = QString::null);
    QwtPlotZoomer *d_zoomer;
    QwtPlotPicker *d_picker;
    QwtPlotPanner *d_panner;
    QAction*btnZoom;
    
	QAction *aboutQtAction;
	QAction *aboutAction;
	QAction *fileAction;
	QAction *exitAction;

	QMenu *fileMenu;
	QMenu *aboutMenu;
	QTextEdit *textEdit;

	QString styleSheet;

	settingWindow *settingWin; 
	QAction *settingAction;
	// QString styleItem;
	//QTcpServer *server;
	//QTcpSocket *client;

	//char *buf;
};

#endif
