#include <qstring.h>
#include <qpainter.h>
#include <qwt_plot.h>
#include <qwt_interval_data.h>
#include <qwt_painter.h>
#include <qwt_scale_map.h>
#include <iostream>
#include "spectrogramItem.h"
#include "def.h"

class SpectrogramItem::PrivateData
{
public:
    int attributes;
    QColor color;
	QwtArray<QwtDoubleInterval> intervals;
	QVector<int> values;
    double reference;
};

SpectrogramItem::SpectrogramItem(const QwtText &title):
    QwtPlotItem(title)
{
    init();
}

SpectrogramItem::SpectrogramItem(const QString &title):
    QwtPlotItem(QwtText(title))
{
    init();
}

SpectrogramItem::~SpectrogramItem()
{
    delete d_data;
}

void SpectrogramItem::init()
{
    d_data = new PrivateData();
    d_data->reference = 0.0;
    d_data->attributes = SpectrogramItem::Auto;

    setItemAttribute(QwtPlotItem::AutoScale, true);
    setItemAttribute(QwtPlotItem::Legend, true);

    setZ(20.0);
}

void SpectrogramItem::setBaseline(double reference)
{
    if ( d_data->reference != reference )
    {
        d_data->reference = reference;
        itemChanged();
    }
}

double SpectrogramItem::baseline() const
{
    return d_data->reference;
}

void SpectrogramItem::setOriginalData(const QwtArray<QwtDoubleInterval> &inter, const QVector<int> &val)
{
	d_data->intervals = inter;
	// for (int i = 0; i < 25 * 20; i++)
	// 	d_data->values.append(rand() % 255);
	d_data->values = val;
    itemChanged();
}

void SpectrogramItem::setValues(const QVector<int> &val)
{
	d_data->values = val;
	// itemChanged();
}

/*注意一维数组values对于二维频谱图值的一一对应关系
 *
 *   y(values)
 *      | | | | | | | | | | | | | | |                           
 *      | | | | | | | | | | | | | | |                           
 *      | | | | | | | | | | | | | | |                           
 *      | | | | | | | | | | | | | | |                            
 *      | | | | | | | | | | | | | | |                         
 *      | | | | | | | | | | | | | | |                        
 *      | | | | | | | | | | | | | | |                           
 *      | | | | | | | | | | | | | | |                        
 *      | | | | | | | | | | | | | | | values[SPECTROGRAM_SIZE_X...2 * SPECTROGRAM_SIZE_X - 1]
 *      | | | | | | | | | | | | | | | values[0...SPECTROGRAM_SIZE_X - 1] 
 *      ---------------------------------> x
 *    (0,0)
 *    频谱图的y值存在values数组中，values是一维数组，先存行再存列
 * */


void SpectrogramItem::insertValues(const QVector<int> &val)
{
	for (int i = SPECTROGRAM_SIZE_X - 1; i > 0; i--)
	{
		for (int j = 0; j < SPECTROGRAM_SIZE_Y; j++)
		{
			d_data->values[j * SPECTROGRAM_SIZE_X + i] = d_data->values[j * SPECTROGRAM_SIZE_Y + i - 1];
		}
	}

	for (int j = 0; j < SPECTROGRAM_SIZE_Y; j++)
		d_data->values[j * SPECTROGRAM_SIZE_X] = val[j];
}
//
// const QwtIntervalData &SpectrogramItem::data() const
// {
//     return d_data->data;
// }

void SpectrogramItem::setColor(const QColor &color)
{
    if ( d_data->color != color )
    {
        d_data->color = color;
        itemChanged();
    }
}

QColor SpectrogramItem::color() const
{
    return d_data->color;
}

// QwtDoubleRect SpectrogramItem::boundingRect() const
// {
//     QwtDoubleRect rect = d_data->data.boundingRect();
//     if ( !rect.isValid() ) 
//         return rect;
//
//     if ( d_data->attributes & Xfy ) 
//     {
//         rect = QwtDoubleRect( rect.y(), rect.x(), 
//             rect.height(), rect.width() );
//
//         if ( rect.left() > d_data->reference ) 
//             rect.setLeft( d_data->reference );
//         else if ( rect.right() < d_data->reference ) 
//             rect.setRight( d_data->reference );
//     } 
//     else 
//     {
//         if ( rect.bottom() < d_data->reference ) 
//             rect.setBottom( d_data->reference );
//         else if ( rect.top() > d_data->reference ) 
//             rect.setTop( d_data->reference );
//     }
//
//     return rect;
// }
//

int SpectrogramItem::rtti() const
{
    return QwtPlotItem::Rtti_PlotSpectrogram;
}

void SpectrogramItem::setSpectrogramAttribute(SpectrogramAttribute attribute, bool on)
{
    if ( bool(d_data->attributes & attribute) == on )
        return;

    if ( on )
        d_data->attributes |= attribute;
    else
        d_data->attributes &= ~attribute;

    itemChanged();
}

bool SpectrogramItem::testSpectrogramAttribute(SpectrogramAttribute attribute) const
{
    return d_data->attributes & attribute;
}

/*
 *   y(values)
 *      | | | | | | | | | | | | | | |                           
 *      | | | | | | | | | | | | | | |                           
 *      | | | | | | | | | | | | | | |                            
 *      | | | | | | | | | | | | | | |                         
 *      | | | | | | | | | | | | | | |                        
 *      | | | | | | | | | | | | | | |                           
 *      | | | | | | | | | | | | | | |                        
 *      | | | | | | | | | | | | | | |                         
 *      | | | | | | | | | | | | | | |                        
 *      ---------------------------------> x
 *    (0,0)
 *    频谱图的y值存在values数组中，values是一维数组，先存行再存列
 * */

void SpectrogramItem::draw(QPainter *painter, const QwtScaleMap &xMap, 
    const QwtScaleMap &yMap, const QRect &) const
{
    painter->setPen(QPen(d_data->color));

    const int y0 = yMap.transform(baseline());

    for ( int i = 0; i < (int)d_data->intervals.size(); i++ )
    {
		// const int y2 = yMap.transform(d_data->values[i]);
		const int y2 = yMap.transform(SPECTROGRAM_SIZE_Y);
		if ( y2 == y0 )
			continue;

		int x1 = xMap.transform(d_data->intervals[i].minValue());
		int x2 = xMap.transform(d_data->intervals[i].maxValue());
		if ( x1 > x2 )
			qSwap(x1, x2);

		if ( i < (int)d_data->intervals.size() - 2 )
		{
			const int xx1 = xMap.transform(d_data->intervals[i+1].minValue());
			const int xx2 = xMap.transform(d_data->intervals[i+1].maxValue());

			if ( x2 == qwtMin(xx1, xx2) )
			{
				x2--;
			}
		}
		for (int k = 0; k < SPECTROGRAM_SIZE_Y; k++)
		{
			painter->setPen(QColor(d_data->values[i + k * SPECTROGRAM_SIZE_X], 0, 0));
			drawBar(painter, Qt::Vertical, QRect(x1, y0 + (y2-y0)/SPECTROGRAM_SIZE_Y * k, x2 - x1, (y2 - y0) / SPECTROGRAM_SIZE_Y) );
		}
	}
}

void SpectrogramItem::drawBar(QPainter *painter,
   Qt::Orientation, const QRect& rect) const
{
   painter->save();

   const QColor color(painter->pen().color());
   const QRect r = rect.normalized();

   const int factor = 125;
   const QColor light(color.light(factor));
   const QColor dark(color.dark(factor));

   painter->setBrush(color);
   painter->setPen(Qt::NoPen);
   QwtPainter::drawRect(painter, r.x() + 1, r.y() + 1,
      r.width() - 2, r.height() - 2);
   painter->setBrush(Qt::NoBrush);

   painter->setPen(QPen(light, 2));
   QwtPainter::drawLine(painter,
      r.left() + 1, r.top() + 2, r.right() + 1, r.top() + 2);

   painter->setPen(QPen(dark, 2));
   QwtPainter::drawLine(painter, 
      r.left() + 1, r.bottom(), r.right() + 1, r.bottom());
   painter->setPen(QPen(light, 1));

   QwtPainter::drawLine(painter, 
      r.left(), r.top() + 1, r.left(), r.bottom());
   QwtPainter::drawLine(painter,
      r.left() + 1, r.top() + 2, r.left() + 1, r.bottom() - 1);
   painter->setPen(QPen(dark, 1));

   QwtPainter::drawLine(painter, 
      r.right() + 1, r.top() + 1, r.right() + 1, r.bottom());
   QwtPainter::drawLine(painter, 
      r.right(), r.top() + 2, r.right(), r.bottom() - 1);

   painter->restore();
}
