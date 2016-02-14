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
struct qt_meta_stringdata_CColorButton_t {
    QByteArrayData data[4];
    char stringdata0[31];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CColorButton_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CColorButton_t qt_meta_stringdata_CColorButton = {
    {
QT_MOC_LITERAL(0, 0, 12), // "CColorButton"
QT_MOC_LITERAL(1, 13, 12), // "colorChanged"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 3) // "col"

    },
    "CColorButton\0colorChanged\0\0col"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CColorButton[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QColor,    3,

       0        // eod
};

void CColorButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CColorButton *_t = static_cast<CColorButton *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->colorChanged((*reinterpret_cast< QColor(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CColorButton::*_t)(QColor );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CColorButton::colorChanged)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject CColorButton::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CColorButton.data,
      qt_meta_data_CColorButton,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CColorButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CColorButton::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CColorButton.stringdata0))
        return static_cast<void*>(const_cast< CColorButton*>(this));
    return QWidget::qt_metacast(_clname);
}

int CColorButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void CColorButton::colorChanged(QColor _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_CPropertyListView_t {
    QByteArrayData data[13];
    char stringdata0[169];
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
QT_MOC_LITERAL(10, 141, 12), // "comboChanged"
QT_MOC_LITERAL(11, 154, 12), // "colorChanged"
QT_MOC_LITERAL(12, 167, 1) // "c"

    },
    "CPropertyListView\0on_modelProps_currentCellChanged\0"
    "\0currentRow\0currentColumn\0previousRow\0"
    "previousColumn\0on_modelProps_cellClicked\0"
    "row\0column\0comboChanged\0colorChanged\0"
    "c"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CPropertyListView[] = {

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
       1,    4,   34,    2, 0x08 /* Private */,
       7,    2,   43,    2, 0x08 /* Private */,
      10,    1,   48,    2, 0x08 /* Private */,
      11,    1,   51,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,    3,    4,    5,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    8,    9,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::QColor,   12,

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
        case 3: _t->colorChanged((*reinterpret_cast< QColor(*)>(_a[1]))); break;
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
