#ifndef _DATA_PLOT_H
#define _DATA_PLOT_H 1

#include <qwt_plot.h>

const int PLOT_SIZE = 201;      // 0 to 200
#define START true 
#define STOP false

class MainWindow;

class DataPlot : public QwtPlot
{
    Q_OBJECT

public:
    DataPlot(QWidget* = NULL);
	bool isStatus();
	void setData_x(double *data, int begin, int end);
	void setData_y(double *data, int begin, int end);
	friend class MainWindow;

public slots:
    void setTimerIntervalSlot(double interval);
	void setStatusStartSlot();
	void setStatusStopSlot();
	void clearSlot();
	void fileDrawSlot();

protected:
    virtual void timerEvent(QTimerEvent *e);
	bool status;

private:
    void alignScales();

    double dataX[PLOT_SIZE]; 
    double dataY[PLOT_SIZE]; 
    double d_z[PLOT_SIZE];

    int d_interval; // timer in ms
    int d_timerId;
};

#endif
