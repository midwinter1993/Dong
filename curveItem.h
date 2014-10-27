#ifndef CURVEITEM_H
#define CURVEITEM_H

//#define CURVE_BUF_SIZE 100
#include <qwt_plot_curve.h>

class QColor;
// class QList<int>;
// class QVector<double>;
class QString;
class DataPair;

class CurveItem : public QwtPlotItem
{
public:
	CurveItem(QString title);
	~CurveItem();
	void setDataY(const QVector<double> &data, const int &len, double &max, double &min);
	void setColor(const QColor color);
	void draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const;

private:
	DataPair *curveData;
	QColor color;
};

#endif
