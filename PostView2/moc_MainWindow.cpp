/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "MainWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CMainWindow_t {
    QByteArrayData data[71];
    char stringdata0[1877];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CMainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CMainWindow_t qt_meta_stringdata_CMainWindow = {
    {
QT_MOC_LITERAL(0, 0, 11), // "CMainWindow"
QT_MOC_LITERAL(1, 12, 23), // "on_actionOpen_triggered"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 23), // "on_actionSave_triggered"
QT_MOC_LITERAL(4, 61, 25), // "on_actionUpdate_triggered"
QT_MOC_LITERAL(5, 87, 27), // "on_actionSnapShot_triggered"
QT_MOC_LITERAL(6, 115, 30), // "on_actionOpenSession_triggered"
QT_MOC_LITERAL(7, 146, 30), // "on_actionSaveSession_triggered"
QT_MOC_LITERAL(8, 177, 23), // "on_actionQuit_triggered"
QT_MOC_LITERAL(9, 201, 24), // "on_selectNodes_triggered"
QT_MOC_LITERAL(10, 226, 24), // "on_selectEdges_triggered"
QT_MOC_LITERAL(11, 251, 24), // "on_selectFaces_triggered"
QT_MOC_LITERAL(12, 276, 24), // "on_selectElems_triggered"
QT_MOC_LITERAL(13, 301, 29), // "on_actionSelectRect_triggered"
QT_MOC_LITERAL(14, 331, 31), // "on_actionSelectCircle_triggered"
QT_MOC_LITERAL(15, 363, 29), // "on_actionSelectFree_triggered"
QT_MOC_LITERAL(16, 393, 31), // "on_actionZoomSelected_triggered"
QT_MOC_LITERAL(17, 425, 30), // "on_actionZoomExtents_triggered"
QT_MOC_LITERAL(18, 456, 27), // "on_actionPlaneCut_triggered"
QT_MOC_LITERAL(19, 484, 29), // "on_actionVectorPlot_triggered"
QT_MOC_LITERAL(20, 514, 33), // "on_actionIsosurfacePlot_trigg..."
QT_MOC_LITERAL(21, 548, 28), // "on_actionSlicePlot_triggered"
QT_MOC_LITERAL(22, 577, 34), // "on_actionDisplacementMap_trig..."
QT_MOC_LITERAL(23, 612, 24), // "on_actionGraph_triggered"
QT_MOC_LITERAL(24, 637, 26), // "on_actionSummary_triggered"
QT_MOC_LITERAL(25, 664, 24), // "on_actionStats_triggered"
QT_MOC_LITERAL(26, 689, 28), // "on_actionIntegrate_triggered"
QT_MOC_LITERAL(27, 718, 31), // "on_actionHideSelected_triggered"
QT_MOC_LITERAL(28, 750, 33), // "on_actionHideUnselected_trigg..."
QT_MOC_LITERAL(29, 784, 34), // "on_actionInvertSelection_trig..."
QT_MOC_LITERAL(30, 819, 28), // "on_actionUnhideAll_triggered"
QT_MOC_LITERAL(31, 848, 28), // "on_actionSelectAll_triggered"
QT_MOC_LITERAL(32, 877, 30), // "on_actionSelectRange_triggered"
QT_MOC_LITERAL(33, 908, 33), // "on_actionClearSelection_trigg..."
QT_MOC_LITERAL(34, 942, 23), // "on_actionFind_triggered"
QT_MOC_LITERAL(35, 966, 25), // "on_actionDelete_triggered"
QT_MOC_LITERAL(36, 992, 29), // "on_actionProperties_triggered"
QT_MOC_LITERAL(37, 1022, 25), // "on_actionColorMap_toggled"
QT_MOC_LITERAL(38, 1048, 8), // "bchecked"
QT_MOC_LITERAL(39, 1057, 33), // "on_selectData_currentIndexCha..."
QT_MOC_LITERAL(40, 1091, 1), // "i"
QT_MOC_LITERAL(41, 1093, 21), // "on_actionPlay_toggled"
QT_MOC_LITERAL(42, 1115, 24), // "on_actionFirst_triggered"
QT_MOC_LITERAL(43, 1140, 23), // "on_actionPrev_triggered"
QT_MOC_LITERAL(44, 1164, 23), // "on_actionNext_triggered"
QT_MOC_LITERAL(45, 1188, 23), // "on_actionLast_triggered"
QT_MOC_LITERAL(46, 1212, 31), // "on_actionTimeSettings_triggered"
QT_MOC_LITERAL(47, 1244, 31), // "on_actionViewSettings_triggered"
QT_MOC_LITERAL(48, 1276, 28), // "on_actionViewCapture_toggled"
QT_MOC_LITERAL(49, 1305, 31), // "on_actionViewProjection_toggled"
QT_MOC_LITERAL(50, 1337, 28), // "on_actionViewFront_triggered"
QT_MOC_LITERAL(51, 1366, 27), // "on_actionViewBack_triggered"
QT_MOC_LITERAL(52, 1394, 27), // "on_actionViewLeft_triggered"
QT_MOC_LITERAL(53, 1422, 28), // "on_actionViewRight_triggered"
QT_MOC_LITERAL(54, 1451, 26), // "on_actionViewTop_triggered"
QT_MOC_LITERAL(55, 1478, 29), // "on_actionViewBottom_triggered"
QT_MOC_LITERAL(56, 1508, 25), // "on_actionViewMesh_toggled"
QT_MOC_LITERAL(57, 1534, 28), // "on_actionViewOutline_toggled"
QT_MOC_LITERAL(58, 1563, 29), // "on_actionViewShowTags_toggled"
QT_MOC_LITERAL(59, 1593, 27), // "on_actionViewSmooth_toggled"
QT_MOC_LITERAL(60, 1621, 26), // "on_actionViewTrack_toggled"
QT_MOC_LITERAL(61, 1648, 29), // "on_actionViewVPSave_triggered"
QT_MOC_LITERAL(62, 1678, 29), // "on_actionViewVPPrev_triggered"
QT_MOC_LITERAL(63, 1708, 29), // "on_actionViewVPNext_triggered"
QT_MOC_LITERAL(64, 1738, 31), // "on_fontStyle_currentFontChanged"
QT_MOC_LITERAL(65, 1770, 4), // "font"
QT_MOC_LITERAL(66, 1775, 24), // "on_fontSize_valueChanged"
QT_MOC_LITERAL(67, 1800, 19), // "on_fontBold_toggled"
QT_MOC_LITERAL(68, 1820, 7), // "checked"
QT_MOC_LITERAL(69, 1828, 21), // "on_fontItalic_toggled"
QT_MOC_LITERAL(70, 1850, 26) // "on_selectTime_valueChanged"

    },
    "CMainWindow\0on_actionOpen_triggered\0"
    "\0on_actionSave_triggered\0"
    "on_actionUpdate_triggered\0"
    "on_actionSnapShot_triggered\0"
    "on_actionOpenSession_triggered\0"
    "on_actionSaveSession_triggered\0"
    "on_actionQuit_triggered\0"
    "on_selectNodes_triggered\0"
    "on_selectEdges_triggered\0"
    "on_selectFaces_triggered\0"
    "on_selectElems_triggered\0"
    "on_actionSelectRect_triggered\0"
    "on_actionSelectCircle_triggered\0"
    "on_actionSelectFree_triggered\0"
    "on_actionZoomSelected_triggered\0"
    "on_actionZoomExtents_triggered\0"
    "on_actionPlaneCut_triggered\0"
    "on_actionVectorPlot_triggered\0"
    "on_actionIsosurfacePlot_triggered\0"
    "on_actionSlicePlot_triggered\0"
    "on_actionDisplacementMap_triggered\0"
    "on_actionGraph_triggered\0"
    "on_actionSummary_triggered\0"
    "on_actionStats_triggered\0"
    "on_actionIntegrate_triggered\0"
    "on_actionHideSelected_triggered\0"
    "on_actionHideUnselected_triggered\0"
    "on_actionInvertSelection_triggered\0"
    "on_actionUnhideAll_triggered\0"
    "on_actionSelectAll_triggered\0"
    "on_actionSelectRange_triggered\0"
    "on_actionClearSelection_triggered\0"
    "on_actionFind_triggered\0"
    "on_actionDelete_triggered\0"
    "on_actionProperties_triggered\0"
    "on_actionColorMap_toggled\0bchecked\0"
    "on_selectData_currentIndexChanged\0i\0"
    "on_actionPlay_toggled\0on_actionFirst_triggered\0"
    "on_actionPrev_triggered\0on_actionNext_triggered\0"
    "on_actionLast_triggered\0"
    "on_actionTimeSettings_triggered\0"
    "on_actionViewSettings_triggered\0"
    "on_actionViewCapture_toggled\0"
    "on_actionViewProjection_toggled\0"
    "on_actionViewFront_triggered\0"
    "on_actionViewBack_triggered\0"
    "on_actionViewLeft_triggered\0"
    "on_actionViewRight_triggered\0"
    "on_actionViewTop_triggered\0"
    "on_actionViewBottom_triggered\0"
    "on_actionViewMesh_toggled\0"
    "on_actionViewOutline_toggled\0"
    "on_actionViewShowTags_toggled\0"
    "on_actionViewSmooth_toggled\0"
    "on_actionViewTrack_toggled\0"
    "on_actionViewVPSave_triggered\0"
    "on_actionViewVPPrev_triggered\0"
    "on_actionViewVPNext_triggered\0"
    "on_fontStyle_currentFontChanged\0font\0"
    "on_fontSize_valueChanged\0on_fontBold_toggled\0"
    "checked\0on_fontItalic_toggled\0"
    "on_selectTime_valueChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CMainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      65,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  339,    2, 0x0a /* Public */,
       3,    0,  340,    2, 0x0a /* Public */,
       4,    0,  341,    2, 0x0a /* Public */,
       5,    0,  342,    2, 0x0a /* Public */,
       6,    0,  343,    2, 0x0a /* Public */,
       7,    0,  344,    2, 0x0a /* Public */,
       8,    0,  345,    2, 0x0a /* Public */,
       9,    0,  346,    2, 0x0a /* Public */,
      10,    0,  347,    2, 0x0a /* Public */,
      11,    0,  348,    2, 0x0a /* Public */,
      12,    0,  349,    2, 0x0a /* Public */,
      13,    0,  350,    2, 0x0a /* Public */,
      14,    0,  351,    2, 0x0a /* Public */,
      15,    0,  352,    2, 0x0a /* Public */,
      16,    0,  353,    2, 0x0a /* Public */,
      17,    0,  354,    2, 0x0a /* Public */,
      18,    0,  355,    2, 0x0a /* Public */,
      19,    0,  356,    2, 0x0a /* Public */,
      20,    0,  357,    2, 0x0a /* Public */,
      21,    0,  358,    2, 0x0a /* Public */,
      22,    0,  359,    2, 0x0a /* Public */,
      23,    0,  360,    2, 0x0a /* Public */,
      24,    0,  361,    2, 0x0a /* Public */,
      25,    0,  362,    2, 0x0a /* Public */,
      26,    0,  363,    2, 0x0a /* Public */,
      27,    0,  364,    2, 0x0a /* Public */,
      28,    0,  365,    2, 0x0a /* Public */,
      29,    0,  366,    2, 0x0a /* Public */,
      30,    0,  367,    2, 0x0a /* Public */,
      31,    0,  368,    2, 0x0a /* Public */,
      32,    0,  369,    2, 0x0a /* Public */,
      33,    0,  370,    2, 0x0a /* Public */,
      34,    0,  371,    2, 0x0a /* Public */,
      35,    0,  372,    2, 0x0a /* Public */,
      36,    0,  373,    2, 0x0a /* Public */,
      37,    1,  374,    2, 0x0a /* Public */,
      39,    1,  377,    2, 0x0a /* Public */,
      41,    1,  380,    2, 0x0a /* Public */,
      42,    0,  383,    2, 0x0a /* Public */,
      43,    0,  384,    2, 0x0a /* Public */,
      44,    0,  385,    2, 0x0a /* Public */,
      45,    0,  386,    2, 0x0a /* Public */,
      46,    0,  387,    2, 0x0a /* Public */,
      47,    0,  388,    2, 0x0a /* Public */,
      48,    1,  389,    2, 0x0a /* Public */,
      49,    1,  392,    2, 0x0a /* Public */,
      50,    0,  395,    2, 0x0a /* Public */,
      51,    0,  396,    2, 0x0a /* Public */,
      52,    0,  397,    2, 0x0a /* Public */,
      53,    0,  398,    2, 0x0a /* Public */,
      54,    0,  399,    2, 0x0a /* Public */,
      55,    0,  400,    2, 0x0a /* Public */,
      56,    1,  401,    2, 0x0a /* Public */,
      57,    1,  404,    2, 0x0a /* Public */,
      58,    1,  407,    2, 0x0a /* Public */,
      59,    1,  410,    2, 0x0a /* Public */,
      60,    1,  413,    2, 0x0a /* Public */,
      61,    0,  416,    2, 0x0a /* Public */,
      62,    0,  417,    2, 0x0a /* Public */,
      63,    0,  418,    2, 0x0a /* Public */,
      64,    1,  419,    2, 0x0a /* Public */,
      66,    1,  422,    2, 0x0a /* Public */,
      67,    1,  425,    2, 0x0a /* Public */,
      69,    1,  428,    2, 0x0a /* Public */,
      70,    1,  431,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   38,
    QMetaType::Void, QMetaType::Int,   40,
    QMetaType::Void, QMetaType::Bool,   38,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   38,
    QMetaType::Void, QMetaType::Bool,   38,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   38,
    QMetaType::Void, QMetaType::Bool,   38,
    QMetaType::Void, QMetaType::Bool,   38,
    QMetaType::Void, QMetaType::Bool,   38,
    QMetaType::Void, QMetaType::Bool,   38,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QFont,   65,
    QMetaType::Void, QMetaType::Int,   40,
    QMetaType::Void, QMetaType::Bool,   68,
    QMetaType::Void, QMetaType::Bool,   38,
    QMetaType::Void, QMetaType::Int,   40,

       0        // eod
};

void CMainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CMainWindow *_t = static_cast<CMainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_actionOpen_triggered(); break;
        case 1: _t->on_actionSave_triggered(); break;
        case 2: _t->on_actionUpdate_triggered(); break;
        case 3: _t->on_actionSnapShot_triggered(); break;
        case 4: _t->on_actionOpenSession_triggered(); break;
        case 5: _t->on_actionSaveSession_triggered(); break;
        case 6: _t->on_actionQuit_triggered(); break;
        case 7: _t->on_selectNodes_triggered(); break;
        case 8: _t->on_selectEdges_triggered(); break;
        case 9: _t->on_selectFaces_triggered(); break;
        case 10: _t->on_selectElems_triggered(); break;
        case 11: _t->on_actionSelectRect_triggered(); break;
        case 12: _t->on_actionSelectCircle_triggered(); break;
        case 13: _t->on_actionSelectFree_triggered(); break;
        case 14: _t->on_actionZoomSelected_triggered(); break;
        case 15: _t->on_actionZoomExtents_triggered(); break;
        case 16: _t->on_actionPlaneCut_triggered(); break;
        case 17: _t->on_actionVectorPlot_triggered(); break;
        case 18: _t->on_actionIsosurfacePlot_triggered(); break;
        case 19: _t->on_actionSlicePlot_triggered(); break;
        case 20: _t->on_actionDisplacementMap_triggered(); break;
        case 21: _t->on_actionGraph_triggered(); break;
        case 22: _t->on_actionSummary_triggered(); break;
        case 23: _t->on_actionStats_triggered(); break;
        case 24: _t->on_actionIntegrate_triggered(); break;
        case 25: _t->on_actionHideSelected_triggered(); break;
        case 26: _t->on_actionHideUnselected_triggered(); break;
        case 27: _t->on_actionInvertSelection_triggered(); break;
        case 28: _t->on_actionUnhideAll_triggered(); break;
        case 29: _t->on_actionSelectAll_triggered(); break;
        case 30: _t->on_actionSelectRange_triggered(); break;
        case 31: _t->on_actionClearSelection_triggered(); break;
        case 32: _t->on_actionFind_triggered(); break;
        case 33: _t->on_actionDelete_triggered(); break;
        case 34: _t->on_actionProperties_triggered(); break;
        case 35: _t->on_actionColorMap_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 36: _t->on_selectData_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 37: _t->on_actionPlay_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 38: _t->on_actionFirst_triggered(); break;
        case 39: _t->on_actionPrev_triggered(); break;
        case 40: _t->on_actionNext_triggered(); break;
        case 41: _t->on_actionLast_triggered(); break;
        case 42: _t->on_actionTimeSettings_triggered(); break;
        case 43: _t->on_actionViewSettings_triggered(); break;
        case 44: _t->on_actionViewCapture_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 45: _t->on_actionViewProjection_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 46: _t->on_actionViewFront_triggered(); break;
        case 47: _t->on_actionViewBack_triggered(); break;
        case 48: _t->on_actionViewLeft_triggered(); break;
        case 49: _t->on_actionViewRight_triggered(); break;
        case 50: _t->on_actionViewTop_triggered(); break;
        case 51: _t->on_actionViewBottom_triggered(); break;
        case 52: _t->on_actionViewMesh_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 53: _t->on_actionViewOutline_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 54: _t->on_actionViewShowTags_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 55: _t->on_actionViewSmooth_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 56: _t->on_actionViewTrack_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 57: _t->on_actionViewVPSave_triggered(); break;
        case 58: _t->on_actionViewVPPrev_triggered(); break;
        case 59: _t->on_actionViewVPNext_triggered(); break;
        case 60: _t->on_fontStyle_currentFontChanged((*reinterpret_cast< const QFont(*)>(_a[1]))); break;
        case 61: _t->on_fontSize_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 62: _t->on_fontBold_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 63: _t->on_fontItalic_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 64: _t->on_selectTime_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject CMainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_CMainWindow.data,
      qt_meta_data_CMainWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CMainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CMainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CMainWindow.stringdata0))
        return static_cast<void*>(const_cast< CMainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int CMainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 65)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 65;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 65)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 65;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
