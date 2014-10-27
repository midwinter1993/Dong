#include "curveItem.h"
#include <QColor>
#include <QList>
#include <QPainter>
#include <QString>
#include <qwt_plot_curve.h>
#include <qwt_painter.h>
#include <qwt_scale_map.h>
#include <cassert>
#include <iostream>
#include "def.h"
class DataPair
{
public:
	QVector<double> dataX;
	QVector<double> dataY;
	int dataStart;
};

CurveItem::CurveItem(QString title):
	QwtPlotItem(title)
{
	curveData = new DataPair;
	for (int i = 0; i < CURVE_BUF_SIZE; i++)
	{
		curveData->dataX.append(i * 0.5);
		curveData->dataY.append(10);
	}
	curveData->dataStart = 0;
	// setPen(QPen(Qt::red));
}

CurveItem::~CurveItem()
{
	delete curveData;
}

void CurveItem::setDataY(const QVector<double> &data, const int &len, double &max, double &min)
{
	for (int i = 0; i < len; i++)
	{
		if (curveData->dataStart == 0)
			curveData->dataStart = CURVE_BUF_SIZE - 1;
		else
			curveData->dataStart--;
		curveData->dataY[curveData->dataStart] = data[i];
	}
	assert(curveData->dataY.size() == CURVE_BUF_SIZE);
	for (int i = 0; i < CURVE_BUF_SIZE; i++) 
	{
		if (curveData->dataY[i] < min)
			min = curveData->dataY[i];
		if (curveData->dataY[i] > max)
			max = curveData->dataY[i];
	}
}

void CurveItem::setColor(const QColor color)
{
	if (this->color != color)
	{
		this->color = color;
		itemChanged();
	}
}

void CurveItem::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
	painter->setPen(QPen(color));
	painter->save();

	assert(curveData->dataY.size() == CURVE_BUF_SIZE);

	double x1, y1, x2, y2;
	int tmpStart = curveData->dataStart;
	x1 = xMap.transform(curveData->dataX[tmpStart]);
	y1 = yMap.transform(curveData->dataX[tmpStart]);
	for (int i = 0; i < CURVE_BUF_SIZE - 2; i++)
	{
		// std::cout << "(" << dataX.at(i) << " " << dataY.at(i) << ")--->" << "(" << x1 << ' ' << y1 << ")"  << std::endl;
		x2 = xMap.transform(curveData->dataX[(tmpStart + 1) % CURVE_BUF_SIZE]);
		y2 = yMap.transform(curveData->dataY[(tmpStart + 1) % CURVE_BUF_SIZE]);
		QwtPainter::drawLine(painter, x1, y1, x2, y2);
		x1 = x2;
		y1 = y2;
		tmpStart = (tmpStart + 1) % CURVE_BUF_SIZE;
	}
	painter->restore();
}
