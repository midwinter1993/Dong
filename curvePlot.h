#ifndef CURVEPLOT_H
#define CURVEPLOT_H

#include <qwt_plot.h>
#include "def.h"

class CurveItem;
class MainWindow;
class QwtPlotZoomer;
class QwtPlotPicker;
class QwtPlotPanner;

class CurvePlot : public QwtPlot
{
    Q_OBJECT

public:
    CurvePlot(QWidget* = NULL);
	~CurvePlot();
	friend class MainWindow;
	//buf: the same as buf_main_window
	//char *buf;

public slots:
	// void setStatusStartSlot();
	// void setStatusStopSlot();
	void clearSlot();
	void fileDrawSlot();
	void dataDrawSlot();
	void dataProcessOriginSlot();
	
    void enableZoomModeSlot(bool);

signals:
	void updataPlotSignal();

private:
    void alignScales();
	
	//double *dataX;
	//double *dataY;
	double axisYMin;
	double axisYMax;
	int lastPoint;
	
	CurveItem *curve;
	QwtPlotZoomer *d_zoomer;
    QwtPlotPicker *d_picker;
    QwtPlotPanner *d_panner;
};

#endif
