#include "mainWindow.h"
#include "settingWindow.h"
#include "pixmaps.h"
#include "images/clear.xpm"
#include "curvePlot.h"
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
#include <QTimer>
//#include <QHostAddress>
//#include <QTcpServer>
//#include <QTcpSocket>
#include <iostream>
#include "stdlib.h"
#include "stdio.h"
#include "time.h"
int cccccnt = 0;


using namespace std;

MainWindow::MainWindow()
{
	/* set the default theme, read the qss file */	
	QFile file(":/qss/abc.qss");
    file.open(QFile::ReadOnly);
    styleSheet = QLatin1String(file.readAll());
    qApp->setStyleSheet(styleSheet);
    file.close();
	// styleItem = "Theme1";
    
	CurvePlot *curvePlot = new CurvePlot(this);
	curvePlot->setTitle("Data Plot after PCA");
	// CurvePlot *curvePlot_origin = new CurvePlot(this);
	// curvePlot_origin->setAxisScale(QwtPlot::yLeft, 0,55);
	// curvePlot_origin->setTitle("Original Data Plot");
	spectroPlot = new SpectrogramPlot(this);

	QToolBar *toolBar = new QToolBar(this);
	toolBar->setFixedHeight(80);

	toolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
	
	
	
	/* create actions and connect the signal with slots*/	
	createButtons_Actions();
	
	//connect(this, SIGNAL(updataPlotSignal()), curvePlot, SLOT(recurvePlot()));
	connect(fileAction, SIGNAL(triggered()), curvePlot, SLOT(fileDrawSlot()));
	//connect(btnStart, SIGNAL(triggered()), curvePlot, SLOT(setStatusStartSlot()));
	//connect(btnStop, SIGNAL(triggered()), curvePlot, SLOT(setStatusStopSlot()));
	connect(btnClear, SIGNAL(triggered()), curvePlot, SLOT(clearSlot()));
	
	connect(btnZoom, SIGNAL(toggled(bool)), curvePlot, SLOT(enableZoomModeSlot(bool)));


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
    
    
	QWidget *widget = new QWidget(this);
	// QVBoxLayout *mainLayout = new QVBoxLayout(widget);
	QHBoxLayout *mainLayout = new QHBoxLayout(widget);
	mainLayout->addWidget(curvePlot);
	// mainLayout->addWidget(curvePlot_origin);
	//TODO: 加入频谱图
	// mainLayout->addWidget(spectroPlot);
	
	widget->setLayout(mainLayout);
	setCentralWidget(widget);
	
	//setCentralWidget(curvePlot);
	setWindowTitle("( . )_( . )");
	
	this->setWindowOpacity(0.9);
	// setIcon(QIcon(clear_xpm));
	
	tcpServer = new TcpServer(this);
	
	// Server begins to listen the port
	tcpServer->listen(QHostAddress::Any, PORT);
	connect(tcpServer, SIGNAL(dataReadSignal()), curvePlot, SLOT(dataDrawSlot())/*Qt::BlockingQueuedConnection*/);
	// connect(tcpServer, SIGNAL(dataReadSignal()), curvePlot_origin, SLOT(dataProcessOriginSlot())/*Qt::BlockingQueuedConnection*/);
	
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(test()));
	timer->start(100);
}

void MainWindow::test()
{
	// cccccnt++;
	if (cccccnt == 10)
	{
		exit(0);
		printf("fucking");
	}
	// QVector<int> val(SPECTROGRAM_SIZE_Y);
	// for (int i = 0; i < SPECTROGRAM_SIZE_Y; i++)
	// {
	// 	val[i] = rand() % 255;
	// }
	// double begin = (double)clock();
	// spectroPlot->insertValues(val);
	// spectroPlot->replot();
	// double end = (double)clock();
	// printf("time: %.2fms\n", (end - begin));
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



void MainWindow::settingSlot()
{
	settingWin = new settingWindow(this);
	connect(settingWin, SIGNAL(setOpacitySignal(qreal)), this, SLOT(changeOpacitySlot(qreal)), Qt::DirectConnection);
}

void MainWindow::changeOpacitySlot(qreal opacity)
{
	this->setWindowOpacity(opacity);
}

