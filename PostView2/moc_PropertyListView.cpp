/****************************************************************************
** Meta object code from reading C++ file 'PropertyListView.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "PropertyListView.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PropertyListView.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CPropertyListView_t {
    QByteArrayData data[11];
    char stringdata0[154];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CPropertyListView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CPropertyListView_t qt_meta_stringdata_CPropertyListView = {
    {
QT_MOC_LITERAL(0, 0, 17), // "CPropertyListView"
QT_MOC_LITERAL(1, 18, 32), // "on_modelProps_currentCellChanged"
QT_MOC_LITERAL(2, 51, 0), // ""
QT_MOC_LITERAL(3, 52, 10), // "currentRow"
QT_MOC_LITERAL(4, 63, 13), // "currentColumn"
QT_MOC_LITERAL(5, 77, 11), // "previousRow"
QT_MOC_LITERAL(6, 89, 14), // "previousColumn"
QT_MOC_LITERAL(7, 104, 25), // "on_modelProps_cellClicked"
QT_MOC_LITERAL(8, 130, 3), // "row"
QT_MOC_LITERAL(9, 134, 6), // "column"
QT_MOC_LITERAL(10, 141, 12) // "comboChanged"

    },
    "CPropertyListView\0on_modelProps_currentCellChanged\0"
    "\0currentRow\0currentColumn\0previousRow\0"
    "previousColumn\0on_modelProps_cellClicked\0"
    "row\0column\0comboChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CPropertyListView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    4,   29,    2, 0x08 /* Private */,
       7,    2,   38,    2, 0x08 /* Private */,
      10,    1,   43,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,    3,    4,    5,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    8,    9,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void CPropertyListView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CPropertyListView *_t = static_cast<CPropertyListView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_modelProps_currentCellChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 1: _t->on_modelProps_cellClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->comboChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject CPropertyListView::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CPropertyListView.data,
      qt_meta_data_CPropertyListView,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CPropertyListView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CPropertyListView::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CPropertyListView.stringdata0))
        return static_cast<void*>(const_cast< CPropertyListView*>(this));
    return QWidget::qt_metacast(_clname);
}

int CPropertyListView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
