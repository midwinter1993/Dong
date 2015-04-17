#include "curvePlot.h"
#include <QByteArray>
#include <QFileDialog>
#include <QHostAddress>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>

#include <qwt_plot_canvas.h>
#include <qwt_painter.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>
#include <qwt_math.h>

#include <qwt_counter.h>
#include <qwt_text.h>

#include <QtGlobal>
#include <QThread>
#include <QMutex>
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

QVector<double> plot_buf_1(CURVE_BUF_SIZE);
QVector<double> plot_buf_2(CURVE_BUF_SIZE);

// extern char socket_rec_buf[];
extern int plot_flag;
extern QMutex mutex;

CurvePlot::CurvePlot(QWidget *parent):
    QwtPlot(parent),
	axisYMin(0.0),
	axisYMax(0.0),
	// status(false),
	lastPoint(0)
{
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

    // Axis 
    setAxisTitle(QwtPlot::xBottom, "Time/seconds");
    setAxisScale(QwtPlot::xBottom, 0, CURVE_BUF_SIZE * 0.5);

    setAxisTitle(QwtPlot::yLeft, "Values");
    setAxisScale(QwtPlot::yLeft, -20, 20);
}

CurvePlot::~CurvePlot()
{
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

void CurvePlot::dataDrawSlot()
{
#ifdef TEST
	// qDebug() << "dataCount" << "------------------------------" << dataCount << '\n';
#endif
	 //当数据大于PACKET_LEN时，已经通过PCA得到第一个特征向量，可以用来处理数据，绘制图形
	if (plot_flag)
	{
		if (plot_flag == 1) 
			curve->setDataY(plot_buf_1, 128, axisYMax, axisYMin);
		else 
			curve->setDataY(plot_buf_2, 128, axisYMax, axisYMin);

		setAxisScale(QwtPlot::yLeft, axisYMin, axisYMax);
		replot();
		mutex.unlock();
	 }
}
