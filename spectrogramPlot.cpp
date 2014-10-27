#include <qwt_plot.h>
#include <qwt_double_interval.h>
#include <qwt_interval_data.h>
#include <qwt_plot_canvas.h>
#include <qwt_painter.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>
#include <qwt_math.h>

#include "spectrogramPlot.h"
#include "spectrogramItem.h"
#include "def.h"
#include <iostream>

SpectrogramPlot::SpectrogramPlot(QWidget *parent):
	QwtPlot(parent)
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
    setTitle("Spectrogram Plot");
    insertLegend(new QwtLegend(), QwtPlot::BottomLegend);
    setCanvasBackground(Qt::white);

	spectrogram = new SpectrogramItem("Spectrogram");
	spectrogram->attach(this);

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
    setAxisScale(QwtPlot::xBottom, 0, SPECTROGRAM_SIZE_X);

    setAxisTitle(QwtPlot::yLeft, "Values");
    setAxisScale(QwtPlot::yLeft, 0, SPECTROGRAM_SIZE_Y);

    QwtArray<QwtDoubleInterval> intervals(SPECTROGRAM_SIZE_X);
    QwtArray<double> values(SPECTROGRAM_SIZE_Y);

	QVector<int> val;
    double pos = 0.0;
    for ( int i = 0; i < SPECTROGRAM_SIZE_X; i++ )
    {
        const int width = 1;// + rand() % 15;
        // const int value = rand() % 100;
		for (int j = 0; j < SPECTROGRAM_SIZE_Y; j++) {
			val.append(rand() % 255);
			// std::cout << val[i] << ' ';
		}
		// std::cout << std::endl;

        intervals[i] = QwtDoubleInterval(pos, pos + double(width));

        pos += width;
    }

    spectrogram->setOriginalData(intervals, val);
}

SpectrogramPlot::~SpectrogramPlot()
{

}

//  Set a plain canvas frame and align the scales to it
void SpectrogramPlot::alignScales()
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


void SpectrogramPlot::insertValues(const QVector<int> &val)
{
	spectrogram->insertValues(val);
}
