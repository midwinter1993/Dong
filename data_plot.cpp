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
#include <QThread>
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include "def.h"
#include "func.h"

using namespace std;

double packageBuf[MATRIX_SIZE][PACKAGE_LEN];
double data_cov[MATRIX_SIZE][MATRIX_SIZE];
double latent[MATRIX_SIZE];
//double vec[MATRIX_SIZE];

double dataToPlot[CURVE_BUF_SIZE];
double data_mean[MATRIX_SIZE];
double csi[MATRIX_SIZE];

double vec_tmp[30];

extern char socket_rec_buf[];

DataPlot::DataPlot(QWidget *parent):
    QwtPlot(parent),
	status(false),
	dataPoint(0),
	dataCount(0),
	lastPoint(0),
	packageCnt(0)
{
	dataX = new double[CURVE_BUF_SIZE];
	dataY = new double[CURVE_BUF_SIZE];

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
    for (int i = 0; i< CURVE_BUF_SIZE; i++)
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
    cRight->setRawData(dataX, dataY, CURVE_BUF_SIZE);

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
    setAxisScale(QwtPlot::xBottom, 0, CURVE_BUF_SIZE * 0.5);

    setAxisTitle(QwtPlot::yLeft, "Values");
    setAxisScale(QwtPlot::yLeft, -100, 100);
   
	//TODO delete
	setStatusStartSlot();
}

DataPlot::~DataPlot()
{
	delete []dataX;
	delete []dataY;
}

//  Set a plain canvas frame and align the scales to it
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


void DataPlot::setStatusStartSlot()
{
	if (!status){
		status = true;
		//server->listen(QHostAddress::Any, PORT);
	}
}

void DataPlot::setStatusStopSlot()
{
	if (status){
		status = false;
		//server->close();
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

	setData_y(y_tmp, 0, cnt - 2);

	replot();
}

void DataPlot::dataProcessSlot()
{
	// qDebug() << "data_plot thread ID: " << QThread::currentThreadId();
	char *buf_tmp = socket_rec_buf;
	
	//received package numbers，每个包长度为81byte，在接受socket时，已将接受到的byte数存入socket_rec_buf + SOCKET_REC_BUF_SIZE - 9位置处
	quint64 packet_num = *(quint64 *)(socket_rec_buf + SOCKET_REC_BUF_SIZE -9) / 81;
	qDebug() << "packet_num-------------->" << packet_num << '\n';

	while (packet_num > 0)
	{
		dataCount += 1;
		//收集满PACKAGE_LEN长度的数据，再做PCA处理，还要记录下数据的均值
		for (int i = 0; i < MATRIX_SIZE; i++) 
		{
			double tmp = sqrt(((double)buf_tmp[22 + i]) * ((double)buf_tmp[22 + i]) + ((double)buf_tmp[52 + i]) * ((double)buf_tmp[52 + i]));

			packageBuf[i][packageCnt] = tmp;
			data_mean[i] += tmp;
		}
		packageCnt++;
		qDebug() << "packageCnt--------->" << packageCnt;
		 
		if (packageCnt == PACKAGE_LEN)
		{
			for (int i = 0; i < MATRIX_SIZE; i++)
			{
				data_mean[i] /= PACKAGE_LEN;
			}
			pca(packageBuf, data_cov, data_mean, PACKAGE_LEN, latent);

			qDebug() << "----------------------after PCA---------------------\n";
			for (int i = 0; i < MATRIX_SIZE; i++)
			{
				vec_tmp[i] = data_cov[2][i];
				assert(vec_tmp[0] != 0);
			}
			packageCnt = 0;
			for (int i = 0; i < PACKAGE_LEN; i++) 
			{
				data_mean[i] = 0.0;
			}
		}

		if (dataCount > PACKAGE_LEN)
		{

			for (int i = 0; i < 30; i++)
			{
				csi[i] = sqrt(((double)buf_tmp[22 + i]) * ((double)buf_tmp[22 + i]) + ((double)buf_tmp[52 + i]) * ((double)buf_tmp[52 + i]));
			}
			
			double tmp = 0.0;
			for (int i = 0; i < MATRIX_SIZE; i++)
			{
				//tmp += csi[i] * vec[i];
				tmp += csi[i] * vec_tmp[i];
			}
			assert(tmp != 0);
			
			dataToPlot[dataPoint] = tmp;
			dataPoint = (dataPoint + 1) % CURVE_BUF_SIZE;
			qDebug() << "-----------dataPoint----lastPoint-----" << dataPoint <<  lastPoint << endl;
		}

			buf_tmp += 81;
			packet_num--;
	}
	qDebug() << "dataCount" << "------------------------------" << dataCount << '\n';
	/*
	 * ------------------------------------------------------------
	 * |       | lastPoint |   ...   | ***   | dataPoint |         |
	 *--------------------------------------------------------------
	 *               |                    |
	 *               ---------------------
	 *                         |
	 *  每10个数据求平均画出               
	 */
	 //当数据大于PACKAGE_LEN时，已经通过PCA得到第一个特征向量，可以用来处理数据，绘制图形
	if (dataCount > PACKAGE_LEN) 
	{
		assert(dataPoint < CURVE_BUF_SIZE);
		/*
		int cnt_tmp = (dataPoint - lastPoint) % CURVE_BUF_SIZE / 10;
		qDebug() << "-------------------cnt_tmp-----------------" << cnt_tmp << endl;
		if ( cnt_tmp > 0)
		{
			for (int i = CURVE_BUF_SIZE - 1; i > cnt_tmp - 1; i--)
			{
				dataY[i] = dataY[i - 1];
			}
			for(int i = 0; i < cnt_tmp; i++)
			{
				double tmp = 0.0;
				for (int j = 0; j < 10; j++) 
				{
					tmp += dataToPlot[(lastPoint+ j) % CURVE_BUF_SIZE];
				}
				lastPoint = (lastPoint + 10) % CURVE_BUF_SIZE;
				dataY[i] = tmp / 10.0;
				//qDebug() << dataY[i] << ' ';
			}

			replot();
		}
		*/
		int cnt_tmp = (dataPoint - lastPoint) % CURVE_BUF_SIZE;
		qDebug() << "-------------------cnt_tmp-----------------" << cnt_tmp << endl;
		if ( cnt_tmp > 0)
		{
			for (int i = CURVE_BUF_SIZE - 1; i > cnt_tmp - 1; i--)
			{
				dataY[i] = dataY[i - 1];
			}
			for(int i = 0; i < cnt_tmp; i++)
			{

				dataY[i] = dataToPlot[(lastPoint+ i) % CURVE_BUF_SIZE];
				
			}
			lastPoint = (lastPoint + cnt_tmp) % CURVE_BUF_SIZE;
			replot();
		}
	}
}

//用于绘制原始数据
void DataPlot::dataProcessOriginSlot()
{
	char *buf_tmp = socket_rec_buf;
	
	//received package numbers，每个包长度为81byte，在接受socket时，已将接受到的byte数存入socket_rec_buf + SOCKET_REC_BUF_SIZE - 9位置处
	quint64 packet_num = *(quint64 *)(socket_rec_buf + SOCKET_REC_BUF_SIZE - 9) / 81;
	qDebug() << "packet_num-------------->" << packet_num << '\n';

	//timeStamp = *(unsigned long*)(buf_tmp + 2) / 1e6;
	//timeStamp = buf_tmp[0] + (buf_tmp[1] << 8) + (buf_tmp[2] << 16) + (buf_tmp[3] << 24);

	int cnt_tmp = 0;
	double *dataToPlot = new double[packet_num];
	while(packet_num > 0)
	{
		double tmp = 0;
		for (int i = 0; i < MATRIX_SIZE; i++)
		{
			tmp += sqrt(((double)buf_tmp[22 + i]) * ((double)buf_tmp[22 + i]) + ((double)buf_tmp[52 + i]) * ((double)buf_tmp[ 52 + i]));
		}

		dataToPlot[cnt_tmp++] = tmp / 30.0;
		
		packet_num--;
		buf_tmp += 81;
	}

	for (int i = CURVE_BUF_SIZE - 1; i > cnt_tmp - 1; i--)
	{
		dataY[i] = dataY[i - 1];
	}
	for(int i =0; i < cnt_tmp; i++)
	{
		dataY[i] = dataToPlot[i];
	}
	delete []dataToPlot;
	replot();
}
