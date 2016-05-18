/****************************************************************************
** Meta object code from reading C++ file 'GraphWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "GraphWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GraphWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CGraphWindow_t {
    QByteArrayData data[12];
    char stringdata0[308];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CGraphWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CGraphWindow_t qt_meta_stringdata_CGraphWindow = {
    {
QT_MOC_LITERAL(0, 0, 12), // "CGraphWindow"
QT_MOC_LITERAL(1, 13, 33), // "on_selectTime_currentIndexCha..."
QT_MOC_LITERAL(2, 47, 0), // ""
QT_MOC_LITERAL(3, 48, 30), // "on_selectX_currentIndexChanged"
QT_MOC_LITERAL(4, 79, 30), // "on_selectY_currentIndexChanged"
QT_MOC_LITERAL(5, 110, 33), // "on_selectPlot_currentIndexCha..."
QT_MOC_LITERAL(6, 144, 23), // "on_actionSave_triggered"
QT_MOC_LITERAL(7, 168, 28), // "on_actionClipboard_triggered"
QT_MOC_LITERAL(8, 197, 24), // "on_actionProps_triggered"
QT_MOC_LITERAL(9, 222, 28), // "on_actionZoomWidth_triggered"
QT_MOC_LITERAL(10, 251, 29), // "on_actionZoomHeight_triggered"
QT_MOC_LITERAL(11, 281, 26) // "on_actionZoomFit_triggered"

    },
    "CGraphWindow\0on_selectTime_currentIndexChanged\0"
    "\0on_selectX_currentIndexChanged\0"
    "on_selectY_currentIndexChanged\0"
    "on_selectPlot_currentIndexChanged\0"
    "on_actionSave_triggered\0"
    "on_actionClipboard_triggered\0"
    "on_actionProps_triggered\0"
    "on_actionZoomWidth_triggered\0"
    "on_actionZoomHeight_triggered\0"
    "on_actionZoomFit_triggered"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CGraphWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x08 /* Private */,
       3,    1,   67,    2, 0x08 /* Private */,
       4,    1,   70,    2, 0x08 /* Private */,
       5,    1,   73,    2, 0x08 /* Private */,
       6,    0,   76,    2, 0x08 /* Private */,
       7,    0,   77,    2, 0x08 /* Private */,
       8,    0,   78,    2, 0x08 /* Private */,
       9,    0,   79,    2, 0x08 /* Private */,
      10,    0,   80,    2, 0x08 /* Private */,
      11,    0,   81,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CGraphWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CGraphWindow *_t = static_cast<CGraphWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_selectTime_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->on_selectX_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->on_selectY_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->on_selectPlot_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->on_actionSave_triggered(); break;
        case 5: _t->on_actionClipboard_triggered(); break;
        case 6: _t->on_actionProps_triggered(); break;
        case 7: _t->on_actionZoomWidth_triggered(); break;
        case 8: _t->on_actionZoomHeight_triggered(); break;
        case 9: _t->on_actionZoomFit_triggered(); break;
        default: ;
        }
    }
}

const QMetaObject CGraphWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_CGraphWindow.data,
      qt_meta_data_CGraphWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CGraphWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CGraphWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CGraphWindow.stringdata0))
        return static_cast<void*>(const_cast< CGraphWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int CGraphWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
