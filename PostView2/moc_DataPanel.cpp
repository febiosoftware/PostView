/****************************************************************************
** Meta object code from reading C++ file 'DataPanel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "DataPanel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DataPanel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CDataPanel_t {
    QByteArrayData data[12];
    char stringdata0[251];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CDataPanel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CDataPanel_t qt_meta_stringdata_CDataPanel = {
    {
QT_MOC_LITERAL(0, 0, 10), // "CDataPanel"
QT_MOC_LITERAL(1, 11, 24), // "on_AddStandard_triggered"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 24), // "on_AddFromFile_triggered"
QT_MOC_LITERAL(4, 62, 24), // "on_AddEquation_triggered"
QT_MOC_LITERAL(5, 87, 21), // "on_CopyButton_clicked"
QT_MOC_LITERAL(6, 109, 23), // "on_DeleteButton_clicked"
QT_MOC_LITERAL(7, 133, 23), // "on_FilterButton_clicked"
QT_MOC_LITERAL(8, 157, 23), // "on_ExportButton_clicked"
QT_MOC_LITERAL(9, 181, 19), // "on_dataList_clicked"
QT_MOC_LITERAL(10, 201, 28), // "on_fieldName_editingFinished"
QT_MOC_LITERAL(11, 230, 20) // "on_props_dataChanged"

    },
    "CDataPanel\0on_AddStandard_triggered\0"
    "\0on_AddFromFile_triggered\0"
    "on_AddEquation_triggered\0on_CopyButton_clicked\0"
    "on_DeleteButton_clicked\0on_FilterButton_clicked\0"
    "on_ExportButton_clicked\0on_dataList_clicked\0"
    "on_fieldName_editingFinished\0"
    "on_props_dataChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CDataPanel[] = {

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
       1,    0,   64,    2, 0x08 /* Private */,
       3,    0,   65,    2, 0x08 /* Private */,
       4,    0,   66,    2, 0x08 /* Private */,
       5,    0,   67,    2, 0x08 /* Private */,
       6,    0,   68,    2, 0x08 /* Private */,
       7,    0,   69,    2, 0x08 /* Private */,
       8,    0,   70,    2, 0x08 /* Private */,
       9,    1,   71,    2, 0x08 /* Private */,
      10,    0,   74,    2, 0x08 /* Private */,
      11,    0,   75,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QModelIndex,    2,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CDataPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CDataPanel *_t = static_cast<CDataPanel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_AddStandard_triggered(); break;
        case 1: _t->on_AddFromFile_triggered(); break;
        case 2: _t->on_AddEquation_triggered(); break;
        case 3: _t->on_CopyButton_clicked(); break;
        case 4: _t->on_DeleteButton_clicked(); break;
        case 5: _t->on_FilterButton_clicked(); break;
        case 6: _t->on_ExportButton_clicked(); break;
        case 7: _t->on_dataList_clicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 8: _t->on_fieldName_editingFinished(); break;
        case 9: _t->on_props_dataChanged(); break;
        default: ;
        }
    }
}

const QMetaObject CDataPanel::staticMetaObject = {
    { &CCommandPanel::staticMetaObject, qt_meta_stringdata_CDataPanel.data,
      qt_meta_data_CDataPanel,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CDataPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CDataPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CDataPanel.stringdata0))
        return static_cast<void*>(const_cast< CDataPanel*>(this));
    return CCommandPanel::qt_metacast(_clname);
}

int CDataPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CCommandPanel::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_CDlgAddDataFile_t {
    QByteArrayData data[3];
    char stringdata0[26];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CDlgAddDataFile_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CDlgAddDataFile_t qt_meta_stringdata_CDlgAddDataFile = {
    {
QT_MOC_LITERAL(0, 0, 15), // "CDlgAddDataFile"
QT_MOC_LITERAL(1, 16, 8), // "onBrowse"
QT_MOC_LITERAL(2, 25, 0) // ""

    },
    "CDlgAddDataFile\0onBrowse\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CDlgAddDataFile[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void CDlgAddDataFile::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CDlgAddDataFile *_t = static_cast<CDlgAddDataFile *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onBrowse(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject CDlgAddDataFile::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_CDlgAddDataFile.data,
      qt_meta_data_CDlgAddDataFile,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CDlgAddDataFile::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CDlgAddDataFile::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CDlgAddDataFile.stringdata0))
        return static_cast<void*>(const_cast< CDlgAddDataFile*>(this));
    return QDialog::qt_metacast(_clname);
}

int CDlgAddDataFile::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
