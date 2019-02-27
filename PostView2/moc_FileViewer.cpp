/****************************************************************************
** Meta object code from reading C++ file 'FileViewer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "FileViewer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FileViewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CFileViewer_t {
    QByteArrayData data[8];
    char stringdata0[128];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CFileViewer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CFileViewer_t qt_meta_stringdata_CFileViewer = {
    {
QT_MOC_LITERAL(0, 0, 11), // "CFileViewer"
QT_MOC_LITERAL(1, 12, 25), // "on_fileList_doubleClicked"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 5), // "index"
QT_MOC_LITERAL(4, 45, 33), // "on_fileFilter_currentIndexCha..."
QT_MOC_LITERAL(5, 79, 25), // "on_folder_editTextChanged"
QT_MOC_LITERAL(6, 105, 4), // "text"
QT_MOC_LITERAL(7, 110, 17) // "on_toolUp_clicked"

    },
    "CFileViewer\0on_fileList_doubleClicked\0"
    "\0index\0on_fileFilter_currentIndexChanged\0"
    "on_folder_editTextChanged\0text\0"
    "on_toolUp_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CFileViewer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x08 /* Private */,
       4,    1,   37,    2, 0x08 /* Private */,
       5,    1,   40,    2, 0x08 /* Private */,
       7,    0,   43,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QModelIndex,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,

       0        // eod
};

void CFileViewer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CFileViewer *_t = static_cast<CFileViewer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_fileList_doubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 1: _t->on_fileFilter_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->on_folder_editTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->on_toolUp_clicked(); break;
        default: ;
        }
    }
}

const QMetaObject CFileViewer::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CFileViewer.data,
      qt_meta_data_CFileViewer,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CFileViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CFileViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CFileViewer.stringdata0))
        return static_cast<void*>(const_cast< CFileViewer*>(this));
    return QWidget::qt_metacast(_clname);
}

int CFileViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
