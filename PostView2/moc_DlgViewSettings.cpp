/****************************************************************************
** Meta object code from reading C++ file 'DlgViewSettings.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "DlgViewSettings.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DlgViewSettings.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CColormapWidget_t {
    QByteArrayData data[10];
    char stringdata0[101];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CColormapWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CColormapWidget_t qt_meta_stringdata_CColormapWidget = {
    {
QT_MOC_LITERAL(0, 0, 15), // "CColormapWidget"
QT_MOC_LITERAL(1, 16, 17), // "currentMapChanged"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 1), // "n"
QT_MOC_LITERAL(4, 37, 13), // "onDataChanged"
QT_MOC_LITERAL(5, 51, 18), // "onSpinValueChanged"
QT_MOC_LITERAL(6, 70, 5), // "onNew"
QT_MOC_LITERAL(7, 76, 8), // "onDelete"
QT_MOC_LITERAL(8, 85, 6), // "onEdit"
QT_MOC_LITERAL(9, 92, 8) // "onInvert"

    },
    "CColormapWidget\0currentMapChanged\0\0n\0"
    "onDataChanged\0onSpinValueChanged\0onNew\0"
    "onDelete\0onEdit\0onInvert"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CColormapWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x09 /* Protected */,
       4,    0,   52,    2, 0x09 /* Protected */,
       5,    1,   53,    2, 0x09 /* Protected */,
       6,    0,   56,    2, 0x09 /* Protected */,
       7,    0,   57,    2, 0x09 /* Protected */,
       8,    0,   58,    2, 0x09 /* Protected */,
       9,    0,   59,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CColormapWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CColormapWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->currentMapChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->onDataChanged(); break;
        case 2: _t->onSpinValueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->onNew(); break;
        case 4: _t->onDelete(); break;
        case 5: _t->onEdit(); break;
        case 6: _t->onInvert(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CColormapWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CColormapWidget.data,
    qt_meta_data_CColormapWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CColormapWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CColormapWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CColormapWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int CColormapWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
struct qt_meta_stringdata_CDlgViewSettings_t {
    QByteArrayData data[9];
    char stringdata0[119];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CDlgViewSettings_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CDlgViewSettings_t qt_meta_stringdata_CDlgViewSettings = {
    {
QT_MOC_LITERAL(0, 0, 16), // "CDlgViewSettings"
QT_MOC_LITERAL(1, 17, 6), // "accept"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 9), // "onClicked"
QT_MOC_LITERAL(4, 35, 16), // "QAbstractButton*"
QT_MOC_LITERAL(5, 52, 15), // "on_load_clicked"
QT_MOC_LITERAL(6, 68, 15), // "on_save_clicked"
QT_MOC_LITERAL(7, 84, 17), // "on_create_clicked"
QT_MOC_LITERAL(8, 102, 16) // "on_apply_clicked"

    },
    "CDlgViewSettings\0accept\0\0onClicked\0"
    "QAbstractButton*\0on_load_clicked\0"
    "on_save_clicked\0on_create_clicked\0"
    "on_apply_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CDlgViewSettings[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x0a /* Public */,
       3,    1,   45,    2, 0x0a /* Public */,
       5,    0,   48,    2, 0x0a /* Public */,
       6,    0,   49,    2, 0x0a /* Public */,
       7,    0,   50,    2, 0x0a /* Public */,
       8,    0,   51,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CDlgViewSettings::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CDlgViewSettings *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->accept(); break;
        case 1: _t->onClicked((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        case 2: _t->on_load_clicked(); break;
        case 3: _t->on_save_clicked(); break;
        case 4: _t->on_create_clicked(); break;
        case 5: _t->on_apply_clicked(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CDlgViewSettings::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_CDlgViewSettings.data,
    qt_meta_data_CDlgViewSettings,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CDlgViewSettings::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CDlgViewSettings::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CDlgViewSettings.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int CDlgViewSettings::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
