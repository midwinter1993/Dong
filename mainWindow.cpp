#include "mainWindow.h"
#include "pixmaps.h"
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

using namespace std;

MainWindow::MainWindow()
{
	/* set the default theme, read the qss file */	
	QFile file(":/qss/abc.qss");
    file.open(QFile::ReadOnly);
    styleSheet = QLatin1String(file.readAll());
    qApp->setStyleSheet(styleSheet);
    file.close();
    
	CurvePlot *curvePlot = new CurvePlot(this);
	curvePlot->setTitle("Data Plot after PCA");

	QToolBar *toolBar = new QToolBar(this);
	toolBar->setFixedHeight(80);

	toolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
	/* create actions and connect the signal with slots*/	
	createButtons_Actions();
	
	/* init the tool bar */
	toolBar->addSeparator();
	toolBar->addAction(btnStart);
    toolBar->addAction(btnStop);
    toolBar->addAction(exitAction);
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
	QHBoxLayout *mainLayout = new QHBoxLayout(widget);
	mainLayout->addWidget(curvePlot);
	
	widget->setLayout(mainLayout);
	setCentralWidget(widget);
	
	setWindowTitle("( . )_( . )");
	
	this->setWindowOpacity(0.9);
	
	tcpServer = new TcpServer(this);
	
	// Server begins to listen the port
	tcpServer->listen(QHostAddress::Any, PORT);
	connect(tcpServer, SIGNAL(dataReadSignal()), curvePlot, SLOT(dataDrawSlot())/*Qt::BlockingQueuedConnection*/);
}

MainWindow::~MainWindow()
{

}

void MainWindow::createButtons_Actions()
{
	btnStart = new QAction(tr("Start"), this);
	btnStart->setIcon(QIcon(QPixmap(":/images/264.png")));
	btnStart->setStatusTip(tr("Start to paint curves"));

	btnStop = new QAction(tr("Stop"), this);
	btnStop->setIcon(QIcon(QPixmap(":/images/cancel.png")));
	btnStop->setStatusTip(tr("Stoping painting"));

	helpAction = QWhatsThis::createAction(this);
	helpAction->setStatusTip(tr("Show the instrutions"));
	connect(helpAction, SIGNAL(triggered()), this, SLOT(helpSlot()));

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

}
void MainWindow::createMenus()
{
	aboutMenu = menuBar()->addMenu(tr("&About"));
	aboutMenu->addAction(aboutAction);
	aboutMenu->addAction(aboutQtAction);
}

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
			"<p>3.Press \"Zoom\" button to zoom in or zoom out</p>"));
}

