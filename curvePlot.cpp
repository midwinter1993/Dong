#include "curvePlot.h"
#include <QByteArray>
#include <QFileDialog>
#include <QHostAddress>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>

#include <qwt_plot_canvas.h>
#include <qwt_painter.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>
#include <qwt_math.h>

#include <qwt_counter.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_text.h>

#include <QtGlobal>
#include <QThread>
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include "def.h"
#include "func.h"
#include "curveItem.h"

#define TEST

using namespace std;

double packageBuf[MATRIX_SIZE][PACKET_LEN];
double data_cov[MATRIX_SIZE][MATRIX_SIZE];
double latent[MATRIX_SIZE];
//double vec[MATRIX_SIZE];

double fuck[CURVE_BUF_SIZE];
QVector<double> dataToPlotBuf;
QVector<double> dataToPlot(CURVE_BUF_SIZE);
QVector<double> dataToPlotOrigin(CURVE_BUF_SIZE);
double data_mean[MATRIX_SIZE];
double data_mean_tmp[MATRIX_SIZE];
double csi[MATRIX_SIZE];

double vec_tmp[30];

extern char socket_rec_buf[];

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

CurvePlot::CurvePlot(QWidget *parent):
    QwtPlot(parent),
	axisYMin(0.0),
	axisYMax(0.0),
	// status(false),
	dataCount(0),
	dataPoint(0),
	lastPoint(0),
	packetCnt(0)
{
	//dataX = new double[CURVE_BUF_SIZE];
	//dataY = new double[CURVE_BUF_SIZE];

	
	/* init the zoomer function */	
	d_zoomer = new Zoomer( QwtPlot::xBottom, QwtPlot::yLeft, this->canvas());
	d_zoomer->setRubberBand(QwtPicker::RectRubberBand);
	d_zoomer->setRubberBandPen(QColor(Qt::green));
	d_zoomer->setTrackerMode(QwtPicker::ActiveOnly);
	d_zoomer->setTrackerPen(QColor(Qt::magenta));
    
	d_panner = new QwtPlotPanner(this->canvas());
	d_panner->setMouseButton(Qt::MidButton);

	d_picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPicker::PointSelection | QwtPicker::DragSelection, 
	QwtPlotPicker::NoRubberBand, QwtPicker::AlwaysOn, this->canvas());
	d_picker->setRubberBandPen(QColor(Qt::green));
	d_picker->setRubberBand(QwtPicker::CrossRubberBand);
	d_picker->setTrackerPen(QColor(Qt::white));
	
	enableZoomModeSlot(false);
	d_picker->setEnabled(false);

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
	/*
    for (int i = 0; i< CURVE_BUF_SIZE; i++)
    {
        dataX[i] = 0.5 * i;     // time axis
        dataY[i] = 0;
    }
	*/
    // Assign a title
    setTitle("A Test for Innovation Project");
    insertLegend(new QwtLegend(), QwtPlot::BottomLegend);
    setCanvasBackground(Qt::white);

    // Insert new curves
    //QwtPlotCurve *cRight = new QwtPlotCurve("Data Moving Right");
    //cRight->attach(this);
	curve = new CurveItem("Data Moving Right");
	curve->attach(this);
	curve->setColor(Qt::red);
    // Set curve styles
   // cRight->setPen(QPen(Qt::red));
    // cLeft->setPen(QPen(Qt::blue));

    // Attach (don't copy) data. Both curves use the same x array.
    //cRight->setRawData(dataX, dataY, CURVE_BUF_SIZE);

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
    setAxisScale(QwtPlot::yLeft, -20, 20);
   
	//TODO delete
	// setStatusStartSlot();
}

CurvePlot::~CurvePlot()
{
	//delete []dataX;
	//delete []dataY;
}

//  Set a plain canvas frame and align the scales to it
void CurvePlot::alignScales()
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

/*
void CurvePlot::setStatusStartSlot()
{
	if (!status){
		status = true;
		//server->listen(QHostAddress::Any, PORT);
	}
}

void CurvePlot::setStatusStopSlot()
{
	if (status){
		status = false;
		//server->close();
	}
}
*/

void CurvePlot::clearSlot()
{
	/*
	for (int i = 0; i < PLOT_SIZE; i++)
	{
		dataY[i] = 0.0;
	}
	replot();
	*/
}

void CurvePlot::fileDrawSlot()
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

	// setData_y(y_tmp, 0, cnt - 2);

	replot();
}

void CurvePlot::enableZoomModeSlot(bool on)
{
    d_panner->setEnabled(on);

    d_zoomer->setEnabled(on);
    d_zoomer->zoom(0);

	d_picker->setEnabled(false);

    //showInfo();
}
/*
void CurvePlot::showInfo(QString text)
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
    //statusBar()->showMessage(text);
#else
    //statusBar()->message(text);
#endif
#endif
}
*/

void CurvePlot::dataProcessSlot()
{
	// qDebug() << "data_plot thread ID: " << QThread::currentThreadId();
	char *buf_tmp = socket_rec_buf;
	int dataToPlotCnt = 0;
	
	//received package numbers，每个包长度为81byte，在接受socket时，已将接受到的byte数存入socket_rec_buf + SOCKET_REC_BUF_SIZE - 9位置处
	quint64 packet_num = *(quint64 *)(socket_rec_buf + SOCKET_REC_BUF_SIZE -9) / 81;
#ifdef TEST
	qDebug() << "packet_num-------------->" << packet_num << '\n';
#endif

	while (packet_num > 0)
	{
		dataCount += 1;
		//收集满PACKET_LEN长度的数据，再做PCA处理，还要记录下数据的均值
		for (int i = 0; i < MATRIX_SIZE; i++) 
		{
			double tmp = sqrt(((double)buf_tmp[21 + i]) * ((double)buf_tmp[21 + i]) + ((double)buf_tmp[51 + i]) * ((double)buf_tmp[51 + i]));
			packageBuf[i][packetCnt] = tmp;
			data_mean[i] += tmp;
		}
		packetCnt++;
#ifdef TEST
		qDebug() << "packetCnt--------->" << packetCnt;
#endif
		 
		//长度达到PACKET_LEN时，做一次PCA
		if (packetCnt == PACKET_LEN)
		{
			//取平均值
			for (int i = 0; i < MATRIX_SIZE; i++)
			{
				data_mean[i] /= PACKET_LEN;
			}
			//PCA返回第二维特征向量对应下标
			int secIndex = pca(packageBuf, data_cov, data_mean, PACKET_LEN, latent);

#ifdef TEST
			qDebug() << "----------------------after PCA---------------------\n";
#endif
			//记录下第二维特征向量
			for (int i = 0; i < MATRIX_SIZE; i++)
			{
				// vec_tmp[i] = data_cov[i][secIndex];
				// TODO ??????????
				vec_tmp[i] = data_cov[i][secIndex];
				assert(vec_tmp[0] != 0);
			}

			packetCnt = 0;
			/* 重新开始收集数据，但上一次收集得到的平均值可以用来画图,做归一化
			 * 因此保留 */
			for (int i = 0; i < MATRIX_SIZE; i++) 
			{
				data_mean_tmp[i] = data_mean[i];
				data_mean[i] = 0.0;
			}
		}

		if (dataCount > PACKET_LEN)
		{
			//做完一次PCA后可以收集数据进行绘图
			for (int i = 0; i < 30; i++)
			{
				csi[i] = sqrt(((double)buf_tmp[21 + i]) * ((double)buf_tmp[21 + i]) + ((double)buf_tmp[51 + i]) * ((double)buf_tmp[51 + i]));
			}
			
			double tmp = 0.0;
			for (int i = 0; i < MATRIX_SIZE; i++)
			{
				//做归一化，然后和第二维向量相乘，得到的就是PCA后空间的坐标
				tmp += (csi[i] - data_mean_tmp[i]) * vec_tmp[i];
			}
			assert(tmp != 0);
			
			// dataToPlot[dataToPlotCnt++] = tmp;
			/* 绘制图形的数据按照NUM_OF_AVG平均
			 */
			dataToPlotBuf.append(tmp);
			if (dataToPlotBuf.size() >= NUM_OF_AVG)
			{
				for (int k = 0; k < NUM_OF_AVG; k++)
					dataToPlot[dataToPlotCnt] += dataToPlotBuf[k];
				dataToPlot[dataToPlotCnt] /= NUM_OF_AVG;
				dataToPlotCnt++;
				dataToPlotBuf.remove(0, NUM_OF_AVG);

				assert(dataToPlot[dataToPlotCnt - 1] != 0);
			}
			//重复记录，内存出错
			// fuck[dataToPlotCnt - 1] = dataToPlot[dataToPlotCnt - 1];
			//----------------------------------
		}
		buf_tmp += 81;
		packet_num--;
	}
#ifdef TEST
	qDebug() << "dataCount" << "------------------------------" << dataCount << '\n';
#endif
	 //当数据大于PACKET_LEN时，已经通过PCA得到第一个特征向量，可以用来处理数据，绘制图形
	if (dataCount > PACKET_LEN) 
	{
		int cnt_tmp = dataToPlotCnt;
		assert(cnt_tmp == dataToPlotCnt);
#ifdef TEST
		qDebug() << "-------------------cnt_tmp-----------------" << cnt_tmp << endl;
#endif
		if ( cnt_tmp > 0)
		{
			curve->setDataY(dataToPlot, cnt_tmp, axisYMax, axisYMin);
			setAxisScale(QwtPlot::yLeft, axisYMin, axisYMax);
			replot();
		}
	}
}

//用于绘制原始数据
void CurvePlot::dataProcessOriginSlot()
{
	char *buf_tmp = socket_rec_buf;
	
	//received package numbers，每个包长度为81byte，在接受socket时，已将接受到的byte数存入socket_rec_buf + SOCKET_REC_BUF_SIZE - 9位置处
	quint64 packet_num = *(quint64 *)(socket_rec_buf + SOCKET_REC_BUF_SIZE - 9) / 81;
#ifdef TEST
		qDebug() << "packet_num-------------->" << packet_num << '\n';
#endif

	//timeStamp = *(unsigned long*)(buf_tmp + 2) / 1e6;
	//timeStamp = buf_tmp[0] + (buf_tmp[1] << 8) + (buf_tmp[2] << 16) + (buf_tmp[3] << 24);

	int cnt_tmp = 0;
	while(packet_num > 0)
	{
		double tmp = 0;
		for (int i = 0; i < MATRIX_SIZE; i++)
		{
			tmp += sqrt(((double)buf_tmp[21 + i]) * ((double)buf_tmp[21 + i]) + ((double)buf_tmp[51 + i]) * ((double)buf_tmp[ 51 + i]));
		}

		dataToPlotOrigin[cnt_tmp++] = tmp / 30.0;
		
		packet_num--;
		buf_tmp += 81;
	}
/*
	for (int i = CURVE_BUF_SIZE - 1; i > cnt_tmp - 1; i--)
	{
		dataY[i] = dataY[i - 1];
	}
	for(int i =0; i < cnt_tmp; i++)
	{
		dataY[i] = dataToPlotOrigin[i];
	}
	// delete []dataToPlotOrigin;
	
    setAxisScale(QwtPlot::yLeft, axisYMin, axisYMax);
    */
	curve->setDataY(dataToPlotOrigin, cnt_tmp, axisYMax, axisYMin);
    setAxisScale(QwtPlot::yLeft, axisYMin, axisYMax);
	replot();
}
