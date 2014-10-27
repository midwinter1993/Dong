/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef SPECTROGRAMITEM_H 
#define SPECTROGRAMITEM_H 

#include <qglobal.h>
#include <qcolor.h>

#include "qwt_plot_item.h" 

class QwtIntervalData;
class QString;

class SpectrogramItem: public QwtPlotItem
{
public:
    explicit SpectrogramItem(const QString &title = QString::null);
    explicit SpectrogramItem(const QwtText &title);
    virtual ~SpectrogramItem();

    // void setOriginalData(const QwtIntervalData &data, const QVector<int> &val);
    void setOriginalData(const QwtArray<QwtDoubleInterval> &inter, const QVector<int> &val);
	void setValues(const QVector<int> &val);
    // const QwtIntervalData &data() const;

	void insertValues(const QVector<int> &val);
    void setColor(const QColor &);
    QColor color() const;

    // virtual QwtDoubleRect boundingRect() const;

    virtual int rtti() const;

    virtual void draw(QPainter *, const QwtScaleMap &xMap, 
        const QwtScaleMap &yMap, const QRect &) const;

    void setBaseline(double reference);
    double baseline() const;

    enum SpectrogramAttribute
    {
        Auto = 0,
        Xfy = 1
    };

    void setSpectrogramAttribute(SpectrogramAttribute, bool on = true);
    bool testSpectrogramAttribute(SpectrogramAttribute) const;

protected:
    virtual void drawBar(QPainter *,
        Qt::Orientation o, const QRect &) const;

private:
    void init();

    class PrivateData;
    PrivateData *d_data;
};

#endif
