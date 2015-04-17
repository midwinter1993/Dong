#ifndef CURVEPLOT_H
#define CURVEPLOT_H

#include <qwt_plot.h>
#include "def.h"

class CurveItem;
class MainWindow;

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
	void dataDrawSlot();
	
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
};

#endif
