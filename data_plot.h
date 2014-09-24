#ifndef _DATA_PLOT_H
#define _DATA_PLOT_H 1

#include <qwt_plot.h>
#include "def.h"

class MainWindow;
class QTcpServer;
class QTcpSocket;

class DataPlot : public QwtPlot
{
    Q_OBJECT

public:
    DataPlot(QWidget* = NULL);
	~DataPlot();
	bool isStatus();
	void setData_x(double *data, int begin, int end);
	void setData_y(double *data, int begin, int end);
	friend class MainWindow;
	//buf: the same as buf_main_window
	//char *buf;

	
public slots:
	void setStatusStartSlot();
	void setStatusStopSlot();
	void clearSlot();
	void fileDrawSlot();
	void dataProcessSlot();
	void dataProcessOriginSlot();

signals:
	void updataPlotSignal();

protected:
	bool status;

private:
    void alignScales();
	
	double *dataX;
	double *dataY;

	int dataPoint;

	unsigned int dataCount;
	unsigned int lastPoint;

	int packageCnt;
};

#endif
