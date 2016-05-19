/****************************************************************************
** Meta object code from reading C++ file 'PlotWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "PlotWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PlotWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CPlotWidget_t {
    QByteArrayData data[8];
    char stringdata0[99];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CPlotWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CPlotWidget_t qt_meta_stringdata_CPlotWidget = {
    {
QT_MOC_LITERAL(0, 0, 11), // "CPlotWidget"
QT_MOC_LITERAL(1, 12, 14), // "doneZoomToRect"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 13), // "OnZoomToWidth"
QT_MOC_LITERAL(4, 42, 14), // "OnZoomToHeight"
QT_MOC_LITERAL(5, 57, 11), // "OnZoomToFit"
QT_MOC_LITERAL(6, 69, 11), // "OnShowProps"
QT_MOC_LITERAL(7, 81, 17) // "OnCopyToClipboard"

    },
    "CPlotWidget\0doneZoomToRect\0\0OnZoomToWidth\0"
    "OnZoomToHeight\0OnZoomToFit\0OnShowProps\0"
    "OnCopyToClipboard"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CPlotWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   45,    2, 0x0a /* Public */,
       4,    0,   46,    2, 0x0a /* Public */,
       5,    0,   47,    2, 0x0a /* Public */,
       6,    0,   48,    2, 0x0a /* Public */,
       7,    0,   49,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CPlotWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CPlotWidget *_t = static_cast<CPlotWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->doneZoomToRect(); break;
        case 1: _t->OnZoomToWidth(); break;
        case 2: _t->OnZoomToHeight(); break;
        case 3: _t->OnZoomToFit(); break;
        case 4: _t->OnShowProps(); break;
        case 5: _t->OnCopyToClipboard(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CPlotWidget::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CPlotWidget::doneZoomToRect)) {
                *result = 0;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject CPlotWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CPlotWidget.data,
      qt_meta_data_CPlotWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CPlotWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CPlotWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CPlotWidget.stringdata0))
        return static_cast<void*>(const_cast< CPlotWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int CPlotWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void CPlotWidget::doneZoomToRect()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
