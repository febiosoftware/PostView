/****************************************************************************
** Meta object code from reading C++ file 'SummaryWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "SummaryWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SummaryWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CSummaryWindow_t {
    QByteArrayData data[14];
    char stringdata0[326];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CSummaryWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CSummaryWindow_t qt_meta_stringdata_CSummaryWindow = {
    {
QT_MOC_LITERAL(0, 0, 14), // "CSummaryWindow"
QT_MOC_LITERAL(1, 15, 33), // "on_selectData_currentValueCha..."
QT_MOC_LITERAL(2, 49, 0), // ""
QT_MOC_LITERAL(3, 50, 23), // "on_actionSave_triggered"
QT_MOC_LITERAL(4, 74, 23), // "on_actionClip_triggered"
QT_MOC_LITERAL(5, 98, 24), // "on_actionProps_triggered"
QT_MOC_LITERAL(6, 123, 28), // "on_actionZoomWidth_triggered"
QT_MOC_LITERAL(7, 152, 29), // "on_actionZoomHeight_triggered"
QT_MOC_LITERAL(8, 182, 26), // "on_actionZoomFit_triggered"
QT_MOC_LITERAL(9, 209, 27), // "on_actionZoomSelect_toggled"
QT_MOC_LITERAL(10, 237, 8), // "bchecked"
QT_MOC_LITERAL(11, 246, 29), // "on_summaryPlot_doneZoomToRect"
QT_MOC_LITERAL(12, 276, 24), // "on_selectionOnly_clicked"
QT_MOC_LITERAL(13, 301, 24) // "on_volumeAverage_clicked"

    },
    "CSummaryWindow\0on_selectData_currentValueChanged\0"
    "\0on_actionSave_triggered\0"
    "on_actionClip_triggered\0"
    "on_actionProps_triggered\0"
    "on_actionZoomWidth_triggered\0"
    "on_actionZoomHeight_triggered\0"
    "on_actionZoomFit_triggered\0"
    "on_actionZoomSelect_toggled\0bchecked\0"
    "on_summaryPlot_doneZoomToRect\0"
    "on_selectionOnly_clicked\0"
    "on_volumeAverage_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CSummaryWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x08 /* Private */,
       3,    0,   72,    2, 0x08 /* Private */,
       4,    0,   73,    2, 0x08 /* Private */,
       5,    0,   74,    2, 0x08 /* Private */,
       6,    0,   75,    2, 0x08 /* Private */,
       7,    0,   76,    2, 0x08 /* Private */,
       8,    0,   77,    2, 0x08 /* Private */,
       9,    1,   78,    2, 0x08 /* Private */,
      11,    0,   81,    2, 0x08 /* Private */,
      12,    0,   82,    2, 0x08 /* Private */,
      13,    0,   83,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CSummaryWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CSummaryWindow *_t = static_cast<CSummaryWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_selectData_currentValueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->on_actionSave_triggered(); break;
        case 2: _t->on_actionClip_triggered(); break;
        case 3: _t->on_actionProps_triggered(); break;
        case 4: _t->on_actionZoomWidth_triggered(); break;
        case 5: _t->on_actionZoomHeight_triggered(); break;
        case 6: _t->on_actionZoomFit_triggered(); break;
        case 7: _t->on_actionZoomSelect_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->on_summaryPlot_doneZoomToRect(); break;
        case 9: _t->on_selectionOnly_clicked(); break;
        case 10: _t->on_volumeAverage_clicked(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CSummaryWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_CSummaryWindow.data,
      qt_meta_data_CSummaryWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *CSummaryWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CSummaryWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CSummaryWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int CSummaryWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
