/****************************************************************************
** Meta object code from reading C++ file 'IntegrateWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "IntegrateWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'IntegrateWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CIntegrateWindow_t {
    QByteArrayData data[12];
    char stringdata0[278];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CIntegrateWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CIntegrateWindow_t qt_meta_stringdata_CIntegrateWindow = {
    {
QT_MOC_LITERAL(0, 0, 16), // "CIntegrateWindow"
QT_MOC_LITERAL(1, 17, 23), // "on_actionSave_triggered"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 23), // "on_actionClip_triggered"
QT_MOC_LITERAL(4, 66, 24), // "on_actionProps_triggered"
QT_MOC_LITERAL(5, 91, 28), // "on_actionZoomWidth_triggered"
QT_MOC_LITERAL(6, 120, 29), // "on_actionZoomHeight_triggered"
QT_MOC_LITERAL(7, 150, 26), // "on_actionZoomFit_triggered"
QT_MOC_LITERAL(8, 177, 27), // "on_actionZoomSelect_toggled"
QT_MOC_LITERAL(9, 205, 8), // "bchecked"
QT_MOC_LITERAL(10, 214, 29), // "on_summaryPlot_doneZoomToRect"
QT_MOC_LITERAL(11, 244, 33) // "on_dataSource_currentIndexCha..."

    },
    "CIntegrateWindow\0on_actionSave_triggered\0"
    "\0on_actionClip_triggered\0"
    "on_actionProps_triggered\0"
    "on_actionZoomWidth_triggered\0"
    "on_actionZoomHeight_triggered\0"
    "on_actionZoomFit_triggered\0"
    "on_actionZoomSelect_toggled\0bchecked\0"
    "on_summaryPlot_doneZoomToRect\0"
    "on_dataSource_currentIndexChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CIntegrateWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x08 /* Private */,
       3,    0,   60,    2, 0x08 /* Private */,
       4,    0,   61,    2, 0x08 /* Private */,
       5,    0,   62,    2, 0x08 /* Private */,
       6,    0,   63,    2, 0x08 /* Private */,
       7,    0,   64,    2, 0x08 /* Private */,
       8,    1,   65,    2, 0x08 /* Private */,
      10,    0,   68,    2, 0x08 /* Private */,
      11,    1,   69,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    9,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void CIntegrateWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CIntegrateWindow *_t = static_cast<CIntegrateWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_actionSave_triggered(); break;
        case 1: _t->on_actionClip_triggered(); break;
        case 2: _t->on_actionProps_triggered(); break;
        case 3: _t->on_actionZoomWidth_triggered(); break;
        case 4: _t->on_actionZoomHeight_triggered(); break;
        case 5: _t->on_actionZoomFit_triggered(); break;
        case 6: _t->on_actionZoomSelect_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->on_summaryPlot_doneZoomToRect(); break;
        case 8: _t->on_dataSource_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject CIntegrateWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_CIntegrateWindow.data,
      qt_meta_data_CIntegrateWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CIntegrateWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CIntegrateWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CIntegrateWindow.stringdata0))
        return static_cast<void*>(const_cast< CIntegrateWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int CIntegrateWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
