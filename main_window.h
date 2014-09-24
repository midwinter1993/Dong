#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <qwt_polygon.h>
#include "tcpserver.h"
#include "data_plot.h"
class QMainWindow;
class QToolBar;
class QWidget;
class QHBoxLayout;
//class DataPlot;
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
class QVBoxLayout;

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

    QwtPlotZoomer *d_zoomer;
    QwtPlotPicker *d_picker;
    QwtPlotPanner *d_panner;
    QAction*btnZoom;

	QString styleSheet;
	settingWindow *settingWin; 
	QAction *settingAction;

	TcpServer *tcpServer;
	DataPlot *plot;
	DataPlot *plot_origin;
};

#endif
