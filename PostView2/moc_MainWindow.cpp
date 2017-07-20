/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "MainWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CMainWindow_t {
    QByteArrayData data[88];
    char stringdata0[2251];
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
QT_MOC_LITERAL(5, 87, 27), // "on_actionFileInfo_triggered"
QT_MOC_LITERAL(6, 115, 27), // "on_actionSnapShot_triggered"
QT_MOC_LITERAL(7, 143, 30), // "on_actionOpenSession_triggered"
QT_MOC_LITERAL(8, 174, 30), // "on_actionSaveSession_triggered"
QT_MOC_LITERAL(9, 205, 23), // "on_actionQuit_triggered"
QT_MOC_LITERAL(10, 229, 24), // "on_selectNodes_triggered"
QT_MOC_LITERAL(11, 254, 24), // "on_selectEdges_triggered"
QT_MOC_LITERAL(12, 279, 24), // "on_selectFaces_triggered"
QT_MOC_LITERAL(13, 304, 24), // "on_selectElems_triggered"
QT_MOC_LITERAL(14, 329, 29), // "on_actionSelectRect_triggered"
QT_MOC_LITERAL(15, 359, 31), // "on_actionSelectCircle_triggered"
QT_MOC_LITERAL(16, 391, 29), // "on_actionSelectFree_triggered"
QT_MOC_LITERAL(17, 421, 29), // "on_actionSelectItem_triggered"
QT_MOC_LITERAL(18, 451, 29), // "on_actionSelectConn_triggered"
QT_MOC_LITERAL(19, 481, 31), // "on_actionZoomSelected_triggered"
QT_MOC_LITERAL(20, 513, 30), // "on_actionZoomExtents_triggered"
QT_MOC_LITERAL(21, 544, 27), // "on_actionPlaneCut_triggered"
QT_MOC_LITERAL(22, 572, 29), // "on_actionVectorPlot_triggered"
QT_MOC_LITERAL(23, 602, 33), // "on_actionIsosurfacePlot_trigg..."
QT_MOC_LITERAL(24, 636, 28), // "on_actionSlicePlot_triggered"
QT_MOC_LITERAL(25, 665, 34), // "on_actionDisplacementMap_trig..."
QT_MOC_LITERAL(26, 700, 24), // "on_actionGraph_triggered"
QT_MOC_LITERAL(27, 725, 26), // "on_actionSummary_triggered"
QT_MOC_LITERAL(28, 752, 24), // "on_actionStats_triggered"
QT_MOC_LITERAL(29, 777, 28), // "on_actionIntegrate_triggered"
QT_MOC_LITERAL(30, 806, 31), // "on_actionHideSelected_triggered"
QT_MOC_LITERAL(31, 838, 33), // "on_actionHideUnselected_trigg..."
QT_MOC_LITERAL(32, 872, 34), // "on_actionInvertSelection_trig..."
QT_MOC_LITERAL(33, 907, 28), // "on_actionUnhideAll_triggered"
QT_MOC_LITERAL(34, 936, 28), // "on_actionSelectAll_triggered"
QT_MOC_LITERAL(35, 965, 30), // "on_actionSelectRange_triggered"
QT_MOC_LITERAL(36, 996, 33), // "on_actionClearSelection_trigg..."
QT_MOC_LITERAL(37, 1030, 23), // "on_actionFind_triggered"
QT_MOC_LITERAL(38, 1054, 25), // "on_actionDelete_triggered"
QT_MOC_LITERAL(39, 1080, 29), // "on_actionProperties_triggered"
QT_MOC_LITERAL(40, 1110, 25), // "on_actionColorMap_toggled"
QT_MOC_LITERAL(41, 1136, 8), // "bchecked"
QT_MOC_LITERAL(42, 1145, 33), // "on_selectData_currentIndexCha..."
QT_MOC_LITERAL(43, 1179, 1), // "i"
QT_MOC_LITERAL(44, 1181, 21), // "on_actionPlay_toggled"
QT_MOC_LITERAL(45, 1203, 24), // "on_actionFirst_triggered"
QT_MOC_LITERAL(46, 1228, 23), // "on_actionPrev_triggered"
QT_MOC_LITERAL(47, 1252, 23), // "on_actionNext_triggered"
QT_MOC_LITERAL(48, 1276, 23), // "on_actionLast_triggered"
QT_MOC_LITERAL(49, 1300, 31), // "on_actionTimeSettings_triggered"
QT_MOC_LITERAL(50, 1332, 31), // "on_actionViewSettings_triggered"
QT_MOC_LITERAL(51, 1364, 28), // "on_actionViewCapture_toggled"
QT_MOC_LITERAL(52, 1393, 31), // "on_actionViewProjection_toggled"
QT_MOC_LITERAL(53, 1425, 28), // "on_actionViewFront_triggered"
QT_MOC_LITERAL(54, 1454, 27), // "on_actionViewBack_triggered"
QT_MOC_LITERAL(55, 1482, 27), // "on_actionViewLeft_triggered"
QT_MOC_LITERAL(56, 1510, 28), // "on_actionViewRight_triggered"
QT_MOC_LITERAL(57, 1539, 26), // "on_actionViewTop_triggered"
QT_MOC_LITERAL(58, 1566, 29), // "on_actionViewBottom_triggered"
QT_MOC_LITERAL(59, 1596, 25), // "on_actionViewMesh_toggled"
QT_MOC_LITERAL(60, 1622, 28), // "on_actionViewOutline_toggled"
QT_MOC_LITERAL(61, 1651, 29), // "on_actionViewShowTags_toggled"
QT_MOC_LITERAL(62, 1681, 27), // "on_actionViewSmooth_toggled"
QT_MOC_LITERAL(63, 1709, 26), // "on_actionViewTrack_toggled"
QT_MOC_LITERAL(64, 1736, 29), // "on_actionViewVPSave_triggered"
QT_MOC_LITERAL(65, 1766, 29), // "on_actionViewVPPrev_triggered"
QT_MOC_LITERAL(66, 1796, 29), // "on_actionViewVPNext_triggered"
QT_MOC_LITERAL(67, 1826, 28), // "on_actionRecordNew_triggered"
QT_MOC_LITERAL(68, 1855, 30), // "on_actionRecordStart_triggered"
QT_MOC_LITERAL(69, 1886, 30), // "on_actionRecordPause_triggered"
QT_MOC_LITERAL(70, 1917, 29), // "on_actionRecordStop_triggered"
QT_MOC_LITERAL(71, 1947, 23), // "on_actionHelp_triggered"
QT_MOC_LITERAL(72, 1971, 24), // "on_actionAbout_triggered"
QT_MOC_LITERAL(73, 1996, 31), // "on_fontStyle_currentFontChanged"
QT_MOC_LITERAL(74, 2028, 4), // "font"
QT_MOC_LITERAL(75, 2033, 24), // "on_fontSize_valueChanged"
QT_MOC_LITERAL(76, 2058, 19), // "on_fontBold_toggled"
QT_MOC_LITERAL(77, 2078, 7), // "checked"
QT_MOC_LITERAL(78, 2086, 21), // "on_fontItalic_toggled"
QT_MOC_LITERAL(79, 2108, 26), // "on_selectTime_valueChanged"
QT_MOC_LITERAL(80, 2135, 19), // "finishedReadingFile"
QT_MOC_LITERAL(81, 2155, 7), // "success"
QT_MOC_LITERAL(82, 2163, 11), // "errorString"
QT_MOC_LITERAL(83, 2175, 17), // "checkFileProgress"
QT_MOC_LITERAL(84, 2193, 16), // "onCancelFileRead"
QT_MOC_LITERAL(85, 2210, 24), // "on_recentFiles_triggered"
QT_MOC_LITERAL(86, 2235, 8), // "QAction*"
QT_MOC_LITERAL(87, 2244, 6) // "action"

    },
    "CMainWindow\0on_actionOpen_triggered\0"
    "\0on_actionSave_triggered\0"
    "on_actionUpdate_triggered\0"
    "on_actionFileInfo_triggered\0"
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
    "on_actionSelectItem_triggered\0"
    "on_actionSelectConn_triggered\0"
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
    "on_actionRecordNew_triggered\0"
    "on_actionRecordStart_triggered\0"
    "on_actionRecordPause_triggered\0"
    "on_actionRecordStop_triggered\0"
    "on_actionHelp_triggered\0"
    "on_actionAbout_triggered\0"
    "on_fontStyle_currentFontChanged\0font\0"
    "on_fontSize_valueChanged\0on_fontBold_toggled\0"
    "checked\0on_fontItalic_toggled\0"
    "on_selectTime_valueChanged\0"
    "finishedReadingFile\0success\0errorString\0"
    "checkFileProgress\0onCancelFileRead\0"
    "on_recentFiles_triggered\0QAction*\0"
    "action"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CMainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      78,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  404,    2, 0x0a /* Public */,
       3,    0,  405,    2, 0x0a /* Public */,
       4,    0,  406,    2, 0x0a /* Public */,
       5,    0,  407,    2, 0x0a /* Public */,
       6,    0,  408,    2, 0x0a /* Public */,
       7,    0,  409,    2, 0x0a /* Public */,
       8,    0,  410,    2, 0x0a /* Public */,
       9,    0,  411,    2, 0x0a /* Public */,
      10,    0,  412,    2, 0x0a /* Public */,
      11,    0,  413,    2, 0x0a /* Public */,
      12,    0,  414,    2, 0x0a /* Public */,
      13,    0,  415,    2, 0x0a /* Public */,
      14,    0,  416,    2, 0x0a /* Public */,
      15,    0,  417,    2, 0x0a /* Public */,
      16,    0,  418,    2, 0x0a /* Public */,
      17,    0,  419,    2, 0x0a /* Public */,
      18,    0,  420,    2, 0x0a /* Public */,
      19,    0,  421,    2, 0x0a /* Public */,
      20,    0,  422,    2, 0x0a /* Public */,
      21,    0,  423,    2, 0x0a /* Public */,
      22,    0,  424,    2, 0x0a /* Public */,
      23,    0,  425,    2, 0x0a /* Public */,
      24,    0,  426,    2, 0x0a /* Public */,
      25,    0,  427,    2, 0x0a /* Public */,
      26,    0,  428,    2, 0x0a /* Public */,
      27,    0,  429,    2, 0x0a /* Public */,
      28,    0,  430,    2, 0x0a /* Public */,
      29,    0,  431,    2, 0x0a /* Public */,
      30,    0,  432,    2, 0x0a /* Public */,
      31,    0,  433,    2, 0x0a /* Public */,
      32,    0,  434,    2, 0x0a /* Public */,
      33,    0,  435,    2, 0x0a /* Public */,
      34,    0,  436,    2, 0x0a /* Public */,
      35,    0,  437,    2, 0x0a /* Public */,
      36,    0,  438,    2, 0x0a /* Public */,
      37,    0,  439,    2, 0x0a /* Public */,
      38,    0,  440,    2, 0x0a /* Public */,
      39,    0,  441,    2, 0x0a /* Public */,
      40,    1,  442,    2, 0x0a /* Public */,
      42,    1,  445,    2, 0x0a /* Public */,
      44,    1,  448,    2, 0x0a /* Public */,
      45,    0,  451,    2, 0x0a /* Public */,
      46,    0,  452,    2, 0x0a /* Public */,
      47,    0,  453,    2, 0x0a /* Public */,
      48,    0,  454,    2, 0x0a /* Public */,
      49,    0,  455,    2, 0x0a /* Public */,
      50,    0,  456,    2, 0x0a /* Public */,
      51,    1,  457,    2, 0x0a /* Public */,
      52,    1,  460,    2, 0x0a /* Public */,
      53,    0,  463,    2, 0x0a /* Public */,
      54,    0,  464,    2, 0x0a /* Public */,
      55,    0,  465,    2, 0x0a /* Public */,
      56,    0,  466,    2, 0x0a /* Public */,
      57,    0,  467,    2, 0x0a /* Public */,
      58,    0,  468,    2, 0x0a /* Public */,
      59,    1,  469,    2, 0x0a /* Public */,
      60,    1,  472,    2, 0x0a /* Public */,
      61,    1,  475,    2, 0x0a /* Public */,
      62,    1,  478,    2, 0x0a /* Public */,
      63,    1,  481,    2, 0x0a /* Public */,
      64,    0,  484,    2, 0x0a /* Public */,
      65,    0,  485,    2, 0x0a /* Public */,
      66,    0,  486,    2, 0x0a /* Public */,
      67,    0,  487,    2, 0x0a /* Public */,
      68,    0,  488,    2, 0x0a /* Public */,
      69,    0,  489,    2, 0x0a /* Public */,
      70,    0,  490,    2, 0x0a /* Public */,
      71,    0,  491,    2, 0x0a /* Public */,
      72,    0,  492,    2, 0x0a /* Public */,
      73,    1,  493,    2, 0x0a /* Public */,
      75,    1,  496,    2, 0x0a /* Public */,
      76,    1,  499,    2, 0x0a /* Public */,
      78,    1,  502,    2, 0x0a /* Public */,
      79,    1,  505,    2, 0x0a /* Public */,
      80,    2,  508,    2, 0x0a /* Public */,
      83,    0,  513,    2, 0x0a /* Public */,
      84,    0,  514,    2, 0x0a /* Public */,
      85,    1,  515,    2, 0x0a /* Public */,

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
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   41,
    QMetaType::Void, QMetaType::Int,   43,
    QMetaType::Void, QMetaType::Bool,   41,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   41,
    QMetaType::Void, QMetaType::Bool,   41,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   41,
    QMetaType::Void, QMetaType::Bool,   41,
    QMetaType::Void, QMetaType::Bool,   41,
    QMetaType::Void, QMetaType::Bool,   41,
    QMetaType::Void, QMetaType::Bool,   41,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QFont,   74,
    QMetaType::Void, QMetaType::Int,   43,
    QMetaType::Void, QMetaType::Bool,   77,
    QMetaType::Void, QMetaType::Bool,   41,
    QMetaType::Void, QMetaType::Int,   43,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   81,   82,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 86,   87,

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
        case 3: _t->on_actionFileInfo_triggered(); break;
        case 4: _t->on_actionSnapShot_triggered(); break;
        case 5: _t->on_actionOpenSession_triggered(); break;
        case 6: _t->on_actionSaveSession_triggered(); break;
        case 7: _t->on_actionQuit_triggered(); break;
        case 8: _t->on_selectNodes_triggered(); break;
        case 9: _t->on_selectEdges_triggered(); break;
        case 10: _t->on_selectFaces_triggered(); break;
        case 11: _t->on_selectElems_triggered(); break;
        case 12: _t->on_actionSelectRect_triggered(); break;
        case 13: _t->on_actionSelectCircle_triggered(); break;
        case 14: _t->on_actionSelectFree_triggered(); break;
        case 15: _t->on_actionSelectItem_triggered(); break;
        case 16: _t->on_actionSelectConn_triggered(); break;
        case 17: _t->on_actionZoomSelected_triggered(); break;
        case 18: _t->on_actionZoomExtents_triggered(); break;
        case 19: _t->on_actionPlaneCut_triggered(); break;
        case 20: _t->on_actionVectorPlot_triggered(); break;
        case 21: _t->on_actionIsosurfacePlot_triggered(); break;
        case 22: _t->on_actionSlicePlot_triggered(); break;
        case 23: _t->on_actionDisplacementMap_triggered(); break;
        case 24: _t->on_actionGraph_triggered(); break;
        case 25: _t->on_actionSummary_triggered(); break;
        case 26: _t->on_actionStats_triggered(); break;
        case 27: _t->on_actionIntegrate_triggered(); break;
        case 28: _t->on_actionHideSelected_triggered(); break;
        case 29: _t->on_actionHideUnselected_triggered(); break;
        case 30: _t->on_actionInvertSelection_triggered(); break;
        case 31: _t->on_actionUnhideAll_triggered(); break;
        case 32: _t->on_actionSelectAll_triggered(); break;
        case 33: _t->on_actionSelectRange_triggered(); break;
        case 34: _t->on_actionClearSelection_triggered(); break;
        case 35: _t->on_actionFind_triggered(); break;
        case 36: _t->on_actionDelete_triggered(); break;
        case 37: _t->on_actionProperties_triggered(); break;
        case 38: _t->on_actionColorMap_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 39: _t->on_selectData_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 40: _t->on_actionPlay_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 41: _t->on_actionFirst_triggered(); break;
        case 42: _t->on_actionPrev_triggered(); break;
        case 43: _t->on_actionNext_triggered(); break;
        case 44: _t->on_actionLast_triggered(); break;
        case 45: _t->on_actionTimeSettings_triggered(); break;
        case 46: _t->on_actionViewSettings_triggered(); break;
        case 47: _t->on_actionViewCapture_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 48: _t->on_actionViewProjection_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 49: _t->on_actionViewFront_triggered(); break;
        case 50: _t->on_actionViewBack_triggered(); break;
        case 51: _t->on_actionViewLeft_triggered(); break;
        case 52: _t->on_actionViewRight_triggered(); break;
        case 53: _t->on_actionViewTop_triggered(); break;
        case 54: _t->on_actionViewBottom_triggered(); break;
        case 55: _t->on_actionViewMesh_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 56: _t->on_actionViewOutline_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 57: _t->on_actionViewShowTags_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 58: _t->on_actionViewSmooth_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 59: _t->on_actionViewTrack_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 60: _t->on_actionViewVPSave_triggered(); break;
        case 61: _t->on_actionViewVPPrev_triggered(); break;
        case 62: _t->on_actionViewVPNext_triggered(); break;
        case 63: _t->on_actionRecordNew_triggered(); break;
        case 64: _t->on_actionRecordStart_triggered(); break;
        case 65: _t->on_actionRecordPause_triggered(); break;
        case 66: _t->on_actionRecordStop_triggered(); break;
        case 67: _t->on_actionHelp_triggered(); break;
        case 68: _t->on_actionAbout_triggered(); break;
        case 69: _t->on_fontStyle_currentFontChanged((*reinterpret_cast< const QFont(*)>(_a[1]))); break;
        case 70: _t->on_fontSize_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 71: _t->on_fontBold_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 72: _t->on_fontItalic_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 73: _t->on_selectTime_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 74: _t->finishedReadingFile((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 75: _t->checkFileProgress(); break;
        case 76: _t->onCancelFileRead(); break;
        case 77: _t->on_recentFiles_triggered((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
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
        if (_id < 78)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 78;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 78)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 78;
    }
    return _id;
}
struct qt_meta_stringdata_CFileThread_t {
    QByteArrayData data[3];
    char stringdata0[25];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CFileThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CFileThread_t qt_meta_stringdata_CFileThread = {
    {
QT_MOC_LITERAL(0, 0, 11), // "CFileThread"
QT_MOC_LITERAL(1, 12, 11), // "resultReady"
QT_MOC_LITERAL(2, 24, 0) // ""

    },
    "CFileThread\0resultReady\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CFileThread[] = {

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
       1,    2,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    2,    2,

       0        // eod
};

void CFileThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CFileThread *_t = static_cast<CFileThread *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->resultReady((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CFileThread::*_t)(bool , const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CFileThread::resultReady)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject CFileThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_CFileThread.data,
      qt_meta_data_CFileThread,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CFileThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CFileThread::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CFileThread.stringdata0))
        return static_cast<void*>(const_cast< CFileThread*>(this));
    return QThread::qt_metacast(_clname);
}

int CFileThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
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
void CFileThread::resultReady(bool _t1, const QString & _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
