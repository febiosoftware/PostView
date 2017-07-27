/****************************************************************************
** Meta object code from reading C++ file 'PlotWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "PlotWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PlotWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CPlotWidget_t {
    QByteArrayData data[11];
    char stringdata0[116];
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
QT_MOC_LITERAL(3, 28, 12), // "pointClicked"
QT_MOC_LITERAL(4, 41, 1), // "x"
QT_MOC_LITERAL(5, 43, 1), // "y"
QT_MOC_LITERAL(6, 45, 13), // "OnZoomToWidth"
QT_MOC_LITERAL(7, 59, 14), // "OnZoomToHeight"
QT_MOC_LITERAL(8, 74, 11), // "OnZoomToFit"
QT_MOC_LITERAL(9, 86, 11), // "OnShowProps"
QT_MOC_LITERAL(10, 98, 17) // "OnCopyToClipboard"

    },
    "CPlotWidget\0doneZoomToRect\0\0pointClicked\0"
    "x\0y\0OnZoomToWidth\0OnZoomToHeight\0"
    "OnZoomToFit\0OnShowProps\0OnCopyToClipboard"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CPlotWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x06 /* Public */,
       3,    2,   50,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   55,    2, 0x0a /* Public */,
       7,    0,   56,    2, 0x0a /* Public */,
       8,    0,   57,    2, 0x0a /* Public */,
       9,    0,   58,    2, 0x0a /* Public */,
      10,    0,   59,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double, QMetaType::Double,    4,    5,

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
        case 1: _t->pointClicked((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 2: _t->OnZoomToWidth(); break;
        case 3: _t->OnZoomToHeight(); break;
        case 4: _t->OnZoomToFit(); break;
        case 5: _t->OnShowProps(); break;
        case 6: _t->OnCopyToClipboard(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CPlotWidget::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CPlotWidget::doneZoomToRect)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (CPlotWidget::*_t)(double , double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CPlotWidget::pointClicked)) {
                *result = 1;
                return;
            }
        }
    }
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
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void CPlotWidget::doneZoomToRect()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void CPlotWidget::pointClicked(double _t1, double _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
struct qt_meta_stringdata_CDlgPlotWidgetProps_t {
    QByteArrayData data[1];
    char stringdata0[20];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CDlgPlotWidgetProps_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CDlgPlotWidgetProps_t qt_meta_stringdata_CDlgPlotWidgetProps = {
    {
QT_MOC_LITERAL(0, 0, 19) // "CDlgPlotWidgetProps"

    },
    "CDlgPlotWidgetProps"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CDlgPlotWidgetProps[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void CDlgPlotWidgetProps::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject CDlgPlotWidgetProps::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_CDlgPlotWidgetProps.data,
      qt_meta_data_CDlgPlotWidgetProps,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CDlgPlotWidgetProps::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CDlgPlotWidgetProps::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CDlgPlotWidgetProps.stringdata0))
        return static_cast<void*>(const_cast< CDlgPlotWidgetProps*>(this));
    return QDialog::qt_metacast(_clname);
}

int CDlgPlotWidgetProps::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
