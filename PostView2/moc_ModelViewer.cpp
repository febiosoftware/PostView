/****************************************************************************
** Meta object code from reading C++ file 'ModelViewer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ModelViewer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ModelViewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CModelViewer_t {
    QByteArrayData data[8];
    char stringdata0[128];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CModelViewer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CModelViewer_t qt_meta_stringdata_CModelViewer = {
    {
QT_MOC_LITERAL(0, 0, 12), // "CModelViewer"
QT_MOC_LITERAL(1, 13, 31), // "on_modelTree_currentItemChanged"
QT_MOC_LITERAL(2, 45, 0), // ""
QT_MOC_LITERAL(3, 46, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(4, 63, 7), // "current"
QT_MOC_LITERAL(5, 71, 4), // "prev"
QT_MOC_LITERAL(6, 76, 27), // "on_nameEdit_editingFinished"
QT_MOC_LITERAL(7, 104, 23) // "on_deleteButton_clicked"

    },
    "CModelViewer\0on_modelTree_currentItemChanged\0"
    "\0QTreeWidgetItem*\0current\0prev\0"
    "on_nameEdit_editingFinished\0"
    "on_deleteButton_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CModelViewer[] = {

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
       1,    2,   29,    2, 0x08 /* Private */,
       6,    0,   34,    2, 0x08 /* Private */,
       7,    0,   35,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CModelViewer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CModelViewer *_t = static_cast<CModelViewer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_modelTree_currentItemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QTreeWidgetItem*(*)>(_a[2]))); break;
        case 1: _t->on_nameEdit_editingFinished(); break;
        case 2: _t->on_deleteButton_clicked(); break;
        default: ;
        }
    }
}

const QMetaObject CModelViewer::staticMetaObject = {
    { &CCommandPanel::staticMetaObject, qt_meta_stringdata_CModelViewer.data,
      qt_meta_data_CModelViewer,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CModelViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CModelViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CModelViewer.stringdata0))
        return static_cast<void*>(const_cast< CModelViewer*>(this));
    return CCommandPanel::qt_metacast(_clname);
}

int CModelViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CCommandPanel::qt_metacall(_c, _id, _a);
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
