#include "main_window.h"
#include "settingWindow.h"
#include "pixmaps.h"
#include "clear.xpm"
#include "data_plot.h"
#include "tcpserver.h"
#include "def.h"

#include <QToolBar>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <QTextEdit>
#include <QApplication>
#include <QWhatsThis>
#include <QStatusBar>
#include <QToolButton>
#include <QFile>
#include <QByteArray>
#include <QtGlobal>
#include <QVBoxLayout>
//#include <QHostAddress>
//#include <QTcpServer>
//#include <QTcpSocket>
#include <iostream>

#include <qwt_counter.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_text.h>
using namespace std;

class Zoomer: public QwtPlotZoomer
{
public:
    Zoomer(int xAxis, int yAxis, QwtPlotCanvas *canvas):
        QwtPlotZoomer(xAxis, yAxis, canvas)
    {
        setSelectionFlags(QwtPicker::DragSelection | QwtPicker::CornerToCorner);
        setTrackerMode(QwtPicker::AlwaysOff);
        setRubberBand(QwtPicker::NoRubberBand);

        // RightButton: zoom out by 1
        // Ctrl+RightButton: zoom out to full size

#if QT_VERSION < 0x040000
        setMousePattern(QwtEventPattern::MouseSelect2,
            Qt::RightButton, Qt::ControlButton);
#else
        setMousePattern(QwtEventPattern::MouseSelect2,
            Qt::RightButton, Qt::ControlModifier);
#endif
        setMousePattern(QwtEventPattern::MouseSelect3,
            Qt::RightButton);
    }
};

MainWindow::MainWindow()
{
	/* set the default theme, read the qss file */	
	QFile file(":/qss/abc.qss");
    file.open(QFile::ReadOnly);
    styleSheet = QLatin1String(file.readAll());
    qApp->setStyleSheet(styleSheet);
    file.close();
	// styleItem = "Theme1";
    
	DataPlot *plot = new DataPlot(this);
	DataPlot *plot_origin = new DataPlot(this);
	plot_origin->setAxisScale(QwtPlot::yLeft, 20,50);

	QToolBar *toolBar = new QToolBar(this);
	toolBar->setFixedHeight(80);

	toolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
	
	/* init the zoomer function */	
	d_zoomer = new Zoomer( QwtPlot::xBottom, QwtPlot::yLeft, 
	plot->canvas());
	d_zoomer->setRubberBand(QwtPicker::RectRubberBand);
	d_zoomer->setRubberBandPen(QColor(Qt::green));
	d_zoomer->setTrackerMode(QwtPicker::ActiveOnly);
	d_zoomer->setTrackerPen(QColor(Qt::magenta));
    
	d_panner = new QwtPlotPanner(plot->canvas());
	d_panner->setMouseButton(Qt::MidButton);

	d_picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
	QwtPicker::PointSelection | QwtPicker::DragSelection, 
	QwtPlotPicker::NoRubberBand, QwtPicker::AlwaysOn, 
        plot->canvas());
	d_picker->setRubberBandPen(QColor(Qt::green));
	d_picker->setRubberBand(QwtPicker::CrossRubberBand);
	d_picker->setTrackerPen(QColor(Qt::white));
	
	/* create actions and connect the signal with slots*/	
	createButtons_Actions();
	
	//connect(this, SIGNAL(updataPlotSignal()), plot, SLOT(replot()));
	connect(fileAction, SIGNAL(triggered()), plot, SLOT(fileDrawSlot()));
	connect(btnStart, SIGNAL(triggered()), plot, SLOT(setStatusStartSlot()));
	connect(btnStop, SIGNAL(triggered()), plot, SLOT(setStatusStopSlot()));
	connect(btnClear, SIGNAL(triggered()), plot, SLOT(clearSlot()));
	
	connect(btnZoom, SIGNAL(toggled(bool)), this, SLOT(enableZoomModeSlot(bool)));
	connect(d_picker, SIGNAL(moved(const QPoint &)), this, SLOT(movedSlot(const QPoint &)));
    connect(d_picker, SIGNAL(selected(const QwtPolygon &)), this, SLOT(selectedSlot(const QwtPolygon &)));

	/* init the tool bar */
	toolBar->addSeparator();
	toolBar->addAction(btnZoom);
	toolBar->addAction(btnStart);
    toolBar->addAction(btnStop);
	toolBar->addAction(btnClear);
	toolBar->addAction(settingAction);
    toolBar->addAction(helpAction);

    setIconSize(QSize(25, 25));

	QWidget *hBox = new QWidget(toolBar);
	QLabel *label = new QLabel(tr("<b>Action</b>:"), hBox);

	textEdit = new QTextEdit("running", this);
	textEdit->setReadOnly(true);
	textEdit->setFixedSize(80, label->height());

	QHBoxLayout *layout = new QHBoxLayout(hBox);
	layout->addWidget(label);

	layout->addWidget(textEdit);
	layout->addWidget(new QWidget(hBox), 10); // spacer);

	toolBar->addSeparator();
	toolBar->addWidget(hBox);
	addToolBar(toolBar);

	//* initial the menu
	createMenus();
    
    enableZoomModeSlot(false);
	d_picker->setEnabled(false);
    showInfo();

	QWidget *widget = new QWidget(this);
	QVBoxLayout *mainLayout = new QVBoxLayout(widget);
	mainLayout->addWidget(plot);
	mainLayout->addWidget(plot_origin);
	
	widget->setLayout(mainLayout);
	setCentralWidget(widget);
	
	//setCentralWidget(plot);
	setWindowTitle("( . )_( . )");
	
	this->setWindowOpacity(0.9);
	// setIcon(QIcon(clear_xpm));
	
	tcpServer = new TcpServer(this);
	
	// Server begins to listen the port
	tcpServer->listen(QHostAddress::Any, PORT);
	connect(tcpServer, SIGNAL(dataReadSignal()), plot, SLOT(dataProcessSlot())/*Qt::BlockingQueuedConnection*/);
	connect(tcpServer, SIGNAL(dataReadSignal()), plot_origin, SLOT(dataProcessOriginSlot())/*Qt::BlockingQueuedConnection*/);
}

MainWindow::~MainWindow()
{

}
void MainWindow::createButtons_Actions()
{
	// btnZoom = new QToolButton(this);
	btnZoom = new QAction(tr("Zoom"), this);
    // btnZoom->setText("Zoom");
    btnZoom->setIcon(QIcon(zoom_xpm));
    btnZoom->setCheckable(true);
    // btnZoom->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnZoom->setStatusTip(tr("Zoom in or Zoom out"));
    
	// btnStart = new QToolButton(this);
	btnStart = new QAction(tr("Start"), this);
	// btnStart->setText("Start");
	btnStart->setIcon(QIcon(QPixmap(":/images/264.png")));
	// btnStart->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	// startAction->setCheckable(true);
	btnStart->setStatusTip(tr("Start to paint curves"));
	
	// btnStop = new QToolButton(this);
	btnStop = new QAction(tr("Stop"), this);
	// btnStop->setText("Stop");
	btnStop->setIcon(QIcon(QPixmap(":/images/cancel.png")));
	// btnStop->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnStop->setStatusTip(tr("Stoping painting"));

	// btnClear = new QToolButton(this);
	btnClear = new QAction(tr("Clear"), this);
	// btnClear->setText("Clear");
	btnClear->setIcon(QIcon(clear_xpm));
	// btnClear->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	btnClear->setStatusTip(tr("Clear the canvas"));

	helpAction = QWhatsThis::createAction(this);
	helpAction->setStatusTip(tr("Show the instrutions"));
	connect(helpAction, SIGNAL(triggered()), this, SLOT(helpSlot()));

	fileAction = new QAction(tr("Open"), this);
	fileAction->setIcon(QIcon(":/images/open.png"));
	fileAction->setShortcut(tr("Ctrl+O"));
	fileAction->setStatusTip(tr("Open a file"));

	exitAction = new QAction(tr("Exit"), this);
	exitAction->setIcon(QIcon(QPixmap(":/images/268.png")));
	exitAction->setStatusTip(tr("Exit the application"));
	exitAction->setShortcut(tr("Ctrl+Q"));
	connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

	aboutQtAction = new QAction(tr("aboutQt"), this);
	aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
	connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	
	aboutAction = new QAction(tr("about"), this);
	aboutAction->setStatusTip(tr("Show the info about the project"));
	connect(aboutAction, SIGNAL(triggered()), this, SLOT(aboutSlot()));

	settingAction = new QAction(tr("setting"), this);
	settingAction->setIcon(QIcon(":/png/mount.png"));
	settingAction->setStatusTip(tr("Theme settings"));
	connect(settingAction, SIGNAL(triggered()), this, SLOT(settingSlot()));
}
void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(fileAction);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAction);

	aboutMenu = menuBar()->addMenu(tr("&About"));
	aboutMenu->addAction(aboutAction);
	aboutMenu->addAction(aboutQtAction);
}
/*
void MainWindow::initTcp()
{
	server = new QTcpServer(this);
	connect(server, SIGNAL(newConnection()), this, SLOT(acceptConnevtionSlot()));
	server->listen(QHostAddress::Any, 10000);
}
*/
void MainWindow::aboutSlot()
{
	QMessageBox::about(this, tr("About Project"),
		tr("<h2>Innovation Project </h2>"
			"<p>Action Recognization based on Wifi system </p>"
			"<p>This is our demo of the innovation project named \"Parttern Recognization\" based on Wifi."));
}

void MainWindow::helpSlot()
{
	QMessageBox::about(this, tr("Help :> SOS"),
		tr("<h2>Help You</h2>"
			"<p>1.Press \"start\" button to paint the curve </p>"
			"<p>2.Press \"stop\" button to stop</p>"
			"<p>3.Press \"clear\" button to clear the canvas</p>"
			"<p>4.Press \"Zoom\" button to zoom in or zoom out</p>"));
}

void MainWindow::enableZoomModeSlot(bool on)
{
    d_panner->setEnabled(on);

    d_zoomer->setEnabled(on);
    d_zoomer->zoom(0);

	d_picker->setEnabled(false);

    showInfo();
}

void MainWindow::showInfo(QString text)
{
    if ( text == QString::null )
    {
        if ( d_picker->rubberBand() )
            text = "Cursor Pos: Press left mouse button in plot region";
        else
            text = "Zoom: Press mouse button and drag";
    }

#ifndef QT_NO_STATUSBAR
#if QT_VERSION >= 0x040000
    statusBar()->showMessage(text);
#else
    statusBar()->message(text);
#endif
#endif
}

void MainWindow::movedSlot(const QPoint &pos)
{
    QString info;
    info.sprintf("Time=%g, Ampl=%g",
        plot->invTransform(QwtPlot::xBottom, pos.x()),
        plot->invTransform(QwtPlot::yLeft, pos.y())//,
    );
    showInfo(info);
}

void MainWindow::selectedSlot(const QwtPolygon &)
{
    showInfo();
}

void MainWindow::settingSlot()
{
	settingWin = new settingWindow(this);
	connect(settingWin, SIGNAL(setOpacitySignal(qreal)), this, SLOT(changeOpacitySlot(qreal)), Qt::DirectConnection);
}

void MainWindow::changeOpacitySlot(qreal opacity)
{
	this->setWindowOpacity(opacity);
}

