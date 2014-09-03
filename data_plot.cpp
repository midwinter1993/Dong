#include "data_plot.h"
#include <QByteArray>
#include <QFileDialog>
#include <QHostAddress>
#include <QTcpServer>
#include <QTcpSocket>

#include <qwt_plot_canvas.h>
#include <qwt_painter.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>
#include <qwt_math.h>

#include <QtGlobal>
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include "def.h"

using namespace std;
//
//  Initialize main window
//
DataPlot::DataPlot(QWidget *parent):
    QwtPlot(parent),
	status(false),
    d_interval(20),
    d_timerId(-1),
	tmp(0.0),
	dataPoint(0),
	startPoint(0),
	timeStamp(0.0),
	actionTime(0),
	dataCount(0),
	lastCount(0)
{
	dataX = new double[BUF_SIZE];
	dataY = new double[BUF_SIZE];
	csibuf = new double[BUF_SIZE];
    // Disable polygon clipping
    QwtPainter::setDeviceClipping(false);

    // We don't need the cache here
    canvas()->setPaintAttribute(QwtPlotCanvas::PaintCached, false);
    canvas()->setPaintAttribute(QwtPlotCanvas::PaintPacked, false);

#if QT_VERSION >= 0x040000
#ifdef Q_WS_X11
    /*
       Qt::WA_PaintOnScreen is only supported for X11, but leads
       to substantial bugs with Qt 4.2.x/Windows
     */
    canvas()->setAttribute(Qt::WA_PaintOnScreen, true);
#endif
#endif

    alignScales();
    
    //  Initialize data
    for (int i = 0; i< PLOT_SIZE; i++)
    {
        dataX[i] = 0.5 * i;     // time axis
        dataY[i] = 0;
    }

    // Assign a title
    setTitle("A Test for Innovation Project");
    insertLegend(new QwtLegend(), QwtPlot::BottomLegend);
    setCanvasBackground(Qt::white);

    // Insert new curves
    QwtPlotCurve *cRight = new QwtPlotCurve("Data Moving Right");
    cRight->attach(this);

    // Set curve styles
    cRight->setPen(QPen(Qt::red));
    // cLeft->setPen(QPen(Qt::blue));

    // Attach (don't copy) data. Both curves use the same x array.
    cRight->setRawData(dataX, dataY, PLOT_SIZE);
    // cLeft->setRawData(dataX, d_z, PLOT_SIZE);

#if 0
    //  Insert zero line at y = 0
    QwtPlotMarker *mY = new QwtPlotMarker();
    mY->setLabelAlignment(Qt::AlignRight|Qt::AlignTop);
    mY->setLineStyle(QwtPlotMarker::HLine);
    mY->setYValue(0.0);
    mY->attach(this);
#endif

    // Axis 
    setAxisTitle(QwtPlot::xBottom, "Time/seconds");
    setAxisScale(QwtPlot::xBottom, 0, PLOT_SIZE * 0.5);

    setAxisTitle(QwtPlot::yLeft, "Values");
    setAxisScale(QwtPlot::yLeft, 20, 40);
    
    // setTimerIntervalSlot(0.0); 
	setTimerIntervalSlot(d_interval);
	// connect(this, SIGNAL(updataPlotSignal()), this, SLOT(updataPlotSlot()));
	
	initTcp();
	buf = new char[MSG_SIZE + 1];
	buf[MSG_SIZE] = '\0';

	//TODO delete
	setStatusStartSlot();
}

DataPlot::~DataPlot()
{
	delete []dataX;
	delete []dataY;
	delete []csibuf;
	delete []buf;
}

void DataPlot::initTcp()
{
	server = new QTcpServer(this);
	connect(server, SIGNAL(newConnection()), this, SLOT(acceptConnectionSlot()));
	// server->listen(QHostAddress::Any, 12345);
}
//  Set a plain canvas frame and align the scales to it
//
void DataPlot::alignScales()
{
    // The code below shows how to align the scales to
    // the canvas frame, but is also a good example demonstrating
    // why the spreaded API needs polishing.

    canvas()->setFrameStyle(QFrame::Box | QFrame::Plain );
    canvas()->setLineWidth(1);

    for ( int i = 0; i < QwtPlot::axisCnt; i++ )
    {
        QwtScaleWidget *scaleWidget = (QwtScaleWidget *)axisWidget(i);
        if ( scaleWidget )
            scaleWidget->setMargin(0);

        QwtScaleDraw *scaleDraw = (QwtScaleDraw *)axisScaleDraw(i);
        if ( scaleDraw )
            scaleDraw->enableComponent(QwtAbstractScaleDraw::Backbone, false);
    }
}

void DataPlot::setTimerIntervalSlot(double ms)
{
    d_interval = qRound(ms);

    if ( d_timerId >= 0 )
    {
        killTimer(d_timerId);
        d_timerId = -1;
    }
    if (d_interval >= 0 )
        d_timerId = startTimer(d_interval);
}

//  Generate new values 
/*
void DataPlot::timerEvent(QTimerEvent *)
{
    static double phase = 0.0;

	if (status == START)
	{
		if (phase > (M_PI - 0.0001)) 
			phase = 0.0;

		// y moves from left to right:
		// Shift y array right and assign new value to y[0].

		for ( int i = PLOT_SIZE - 1; i > 0; i-- )
			dataY[i] = dataY[i-1];
		dataY[0] = (sin(phase) * (-1.0 + 2.0 * double(rand()) / double(RAND_MAX))) * 50 + 75.0;

	// update the display
		replot();

		phase += M_PI * 0.02;
	}
}
*/

void DataPlot::setStatusStartSlot()
{
	if (!status){
		status = true;
		server->listen(QHostAddress::Any, PORT);
	}
}

void DataPlot::setStatusStopSlot()
{
	if (status){
		status = false;
		server->close();
	}
}

bool DataPlot::isStatus()
{
	return this->status;
}

void DataPlot::clearSlot()
{
	for (int i = 0; i < PLOT_SIZE; i++)
	{
		dataY[i] = 0.0;
	}
	replot();
}

void DataPlot::setData_x(double *data, int begin, int end)
{
	Q_ASSERT(end <= PLOT_SIZE && data != NULL);
	for (int i = begin; i < end; i++)
	{
		dataX[i] = data[i];
		printf("%s%d ", "data:", i);
	}
}

void DataPlot::setData_y(double *data, int begin, int end)
{
	Q_ASSERT(end <= PLOT_SIZE && data != NULL);

	int j;
	for (j = begin; j < end; j++)
	{
		dataY[j] = data[j];
	}
}
/*
void DataPlot::updataData()
{
	// cout << "updataData  begin----------------------" << endl;
	// int i = 0;
	// for (i = PLOT_SIZE - 1; i > len - 1; i--)
	// {
	// 	cout << i << endl;
	// 	dataX[i] = dataX[i-1];
	// 	dataY[i] = dataY[i-1];
	// }
	// cout << buf_x[0] << "------------------------ok" << endl;
	// for (i = 0; i < len; i++)
	// {
	// 	cout << i << endl;
	// 	// dataX[i] = buf_x[i];
	// 	dataY[i] = buf_y[i];
	// 	cout << buf_y[i] << endl;
	// }
	// cout << "updataData end-----------------------------" << endl;
	for (int i = 0; i < 10; i++)
		dataY[i] = i * 10;
	cout << dataY[0] << ' ' << dataY[1] << endl;
	replot();
}
*/

void DataPlot::fileDrawSlot()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("File"), tr("xxx (*.dat)"));
	QByteArray tmp = fileName.toLatin1();
	char *file_name = tmp.data();

	std::ifstream fin(file_name);
	double x_tmp[PLOT_SIZE], y_tmp[PLOT_SIZE];
	int cnt = 0;
	while (!fin.eof())
	{
		fin >> x_tmp[cnt] >> y_tmp[cnt];
		cnt++;
	}
	fin.close();

	// setData_x(x_tmp, 0, cnt - 2);

	setData_y(y_tmp, 0, cnt - 2);

	replot();
}
/*
void DataPlot::updataPlotSlot()
{
	replot();
}
*/
void DataPlot::acceptConnectionSlot()
{
	client = server->nextPendingConnection();
	connect(client, SIGNAL(readyRead()), this, SLOT(readDataSlot()));
	connect(client, SIGNAL(disconnected()), this, SLOT(disconnectedSlot()));
	client->setReadBufferSize(65536);
}
/*
void DataPlot::readDataSlot()
{
	if (status)
	{
		client->read(buf, client->bytesAvailable());	
	
		// cout << *(double*)buf << ' ' << *(double*)(buf+8) << ' ' << *(double*)(buf+16) << ' ' << *(double*)(buf+24) << endl;
		double buf_x[2] = {*(double*)buf, *(double*)(buf + 8)};
		double buf_y[2] = {*(double*)(buf + 16), *(double*)(buf + 24)};
		int i = 0;
		int len = 2;
		for (i = PLOT_SIZE - 1; i > len - 1; i--)
		{
			// cout << i << endl;
			// dataX[i] = dataX[i-1];
			dataY[i] = dataY[i-1];
		}
		// cout << buf_x[0] << "------------------------ok" << endl;
		for (i = 0; i < len; i++)
		{
			// cout << i << endl;
			// dataX[i] = buf_x[i];
			dataY[i] = buf_y[i];
			// cout << buf_y[i] << endl;
		}
		// cout << "-----------------here------------------" << endl;
		replot();
		// cout << "fjklafjl" << endl;
		// plot->update();
	}
}
*/

void DataPlot::readDataSlot()
{
	if (status) 
	{
		//-----------------------------------
		qDebug() << "bytes available: " << client->bytesAvailable() << '\n';
		if (client->bytesAvailable() < MSG_SIZE)
			return;
		// Q_ASSERT(client->read(buf, MSG_SIZE) == MSG_SIZE);
		quint64 temp = client->read(buf, MSG_SIZE);
		qDebug() << "temp " << temp << '\n';
		//TODO timestamp = ...
		// qDebug() << "buf--->" << (char*)buf << '\n';
		timeStamp = *(unsigned long*)(buf + 2) / 1e6;
		timeStamp = buf[0] + (buf[1] << 8) + (buf[2] << 16) + (buf[3] << 24);
		// qDebug() << "timeStamp: " << timeStamp << "actionTime: " << actionTime << '\n';
		dataCount += 1;

		tmp = 0;
		for (int i = 0; i < 30; i++)
		{
			tmp += sqrt(((double)buf[22 + i]) * ((double)buf[22 + i]) + ((double)buf[52 + i]) * ((double)buf[ 52 + i]));
		}
		// qDebug() << "receive data:" << dataCount << " tmp:-------->" << tmp / 30.0 << "\n";
		csibuf[dataPoint] = tmp / 30.0;
		// qDebug() << csibuf[dataPoint] << "------csibuf=---\n";
		// qDebug() << "timeStamp" << timeStamp << "timeStamp-actionTime" << fabs(timeStamp - actionTime) << endl;

		if ((fabs(timeStamp - actionTime) > LIMIT_TIME) && dataCount > 200) 
		{
			actionTime = timeStamp;
			// qDebug() << "startPoint" << startPoint << ' ' << "dataPoint" << dataPoint << ' ' << "receive sum" << dataCount - lastCount << "dataCount" << dataCount << "lastCount" << lastCount << '\n';
			for (unsigned int i = PLOT_SIZE - 1; i > dataCount - lastCount -  1; i--)
			{
				dataY[i] = dataY[i - 1];
			}
			for(unsigned int i =0; i < dataCount - lastCount; i++)
			{
				dataY[i] = csibuf[(i + startPoint) % BUF_SIZE];
				// qDebug() << "datay:" << dataY[i] << ' ';
				// setStatusStopSlot();
			}
			replot();

			startPoint = dataPoint - 2 * OVERHEAD;
			if (startPoint < 0)
			{
				startPoint += BUF_SIZE;
			}
			lastCount = dataCount;
		}
		dataPoint += 1;
		if (dataPoint >= BUF_SIZE)
		{
			dataPoint = 0;
		}
	}
}

void DataPlot::disconnectedSlot()
{
	qDebug() << "disconnectedSlot" << '\n';
	// if (client != NULL)
		// delete client;
	// qDebug() << "Disable:";
}
