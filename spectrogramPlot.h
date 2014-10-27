#ifndef SPECTROGRAMPLOT_H
#define SPECTROGRAMPLOT_H

#include <qwt_plot.h>
#include <QVector>

class SpectrogramItem;

class SpectrogramPlot : public QwtPlot
{
	Q_OBJECT

public:
	SpectrogramPlot(QWidget *parent = NULL);
	~SpectrogramPlot();
	void setValues(const QVector<int> &val);
	void insertValues(const QVector<int> &val);

private:
    void alignScales();
	SpectrogramItem *spectrogram;
	
};

#endif
