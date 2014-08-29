/****************************************************************************
** Meta object code from reading C++ file 'data_plot.h'
**
** Created: Thu Jul 31 13:31:26 2014
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../data_plot.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'data_plot.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DataPlot[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      19,   10,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_DataPlot[] = {
    "DataPlot\0\0interval\0setTimerInterval(double)\0"
};

void DataPlot::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        DataPlot *_t = static_cast<DataPlot *>(_o);
        switch (_id) {
        case 0: _t->setTimerInterval((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DataPlot::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DataPlot::staticMetaObject = {
    { &QwtPlot::staticMetaObject, qt_meta_stringdata_DataPlot,
      qt_meta_data_DataPlot, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DataPlot::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DataPlot::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DataPlot::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DataPlot))
        return static_cast<void*>(const_cast< DataPlot*>(this));
    return QwtPlot::qt_metacast(_clname);
}

int DataPlot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QwtPlot::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
