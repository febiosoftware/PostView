/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "MainWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CMainWindow_t {
    QByteArrayData data[102];
    char stringdata0[2617];
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
QT_MOC_LITERAL(17, 421, 27), // "on_actionSelectConn_toggled"
QT_MOC_LITERAL(18, 449, 1), // "b"
QT_MOC_LITERAL(19, 451, 31), // "on_actionZoomSelected_triggered"
QT_MOC_LITERAL(20, 483, 30), // "on_actionZoomExtents_triggered"
QT_MOC_LITERAL(21, 514, 27), // "on_actionPlaneCut_triggered"
QT_MOC_LITERAL(22, 542, 30), // "on_actionMirrorPlane_triggered"
QT_MOC_LITERAL(23, 573, 29), // "on_actionVectorPlot_triggered"
QT_MOC_LITERAL(24, 603, 29), // "on_actionTensorPlot_triggered"
QT_MOC_LITERAL(25, 633, 33), // "on_actionIsosurfacePlot_trigg..."
QT_MOC_LITERAL(26, 667, 28), // "on_actionSlicePlot_triggered"
QT_MOC_LITERAL(27, 696, 34), // "on_actionDisplacementMap_trig..."
QT_MOC_LITERAL(28, 731, 33), // "on_actionStreamLinePlot_trigg..."
QT_MOC_LITERAL(29, 765, 35), // "on_actionParticleFlowPlot_tri..."
QT_MOC_LITERAL(30, 801, 30), // "on_actionImageSlicer_triggered"
QT_MOC_LITERAL(31, 832, 31), // "on_actionVolumeRender_triggered"
QT_MOC_LITERAL(32, 864, 32), // "on_actionMarchingCubes_triggered"
QT_MOC_LITERAL(33, 897, 24), // "on_actionGraph_triggered"
QT_MOC_LITERAL(34, 922, 26), // "on_actionSummary_triggered"
QT_MOC_LITERAL(35, 949, 24), // "on_actionStats_triggered"
QT_MOC_LITERAL(36, 974, 28), // "on_actionIntegrate_triggered"
QT_MOC_LITERAL(37, 1003, 31), // "on_actionHideSelected_triggered"
QT_MOC_LITERAL(38, 1035, 33), // "on_actionHideUnselected_trigg..."
QT_MOC_LITERAL(39, 1069, 34), // "on_actionInvertSelection_trig..."
QT_MOC_LITERAL(40, 1104, 28), // "on_actionUnhideAll_triggered"
QT_MOC_LITERAL(41, 1133, 28), // "on_actionSelectAll_triggered"
QT_MOC_LITERAL(42, 1162, 30), // "on_actionSelectRange_triggered"
QT_MOC_LITERAL(43, 1193, 33), // "on_actionClearSelection_trigg..."
QT_MOC_LITERAL(44, 1227, 23), // "on_actionFind_triggered"
QT_MOC_LITERAL(45, 1251, 25), // "on_actionDelete_triggered"
QT_MOC_LITERAL(46, 1277, 29), // "on_actionProperties_triggered"
QT_MOC_LITERAL(47, 1307, 25), // "on_actionColorMap_toggled"
QT_MOC_LITERAL(48, 1333, 8), // "bchecked"
QT_MOC_LITERAL(49, 1342, 33), // "on_selectData_currentValueCha..."
QT_MOC_LITERAL(50, 1376, 1), // "i"
QT_MOC_LITERAL(51, 1378, 21), // "on_actionPlay_toggled"
QT_MOC_LITERAL(52, 1400, 24), // "on_actionFirst_triggered"
QT_MOC_LITERAL(53, 1425, 23), // "on_actionPrev_triggered"
QT_MOC_LITERAL(54, 1449, 23), // "on_actionNext_triggered"
QT_MOC_LITERAL(55, 1473, 23), // "on_actionLast_triggered"
QT_MOC_LITERAL(56, 1497, 31), // "on_actionTimeSettings_triggered"
QT_MOC_LITERAL(57, 1529, 31), // "on_actionViewSettings_triggered"
QT_MOC_LITERAL(58, 1561, 28), // "on_actionViewCapture_toggled"
QT_MOC_LITERAL(59, 1590, 28), // "on_actionViewWidgets_toggled"
QT_MOC_LITERAL(60, 1619, 31), // "on_actionViewProjection_toggled"
QT_MOC_LITERAL(61, 1651, 28), // "on_actionViewFront_triggered"
QT_MOC_LITERAL(62, 1680, 27), // "on_actionViewBack_triggered"
QT_MOC_LITERAL(63, 1708, 27), // "on_actionViewLeft_triggered"
QT_MOC_LITERAL(64, 1736, 28), // "on_actionViewRight_triggered"
QT_MOC_LITERAL(65, 1765, 26), // "on_actionViewTop_triggered"
QT_MOC_LITERAL(66, 1792, 29), // "on_actionViewBottom_triggered"
QT_MOC_LITERAL(67, 1822, 25), // "on_actionViewMesh_toggled"
QT_MOC_LITERAL(68, 1848, 28), // "on_actionViewOutline_toggled"
QT_MOC_LITERAL(69, 1877, 29), // "on_actionViewShowTags_toggled"
QT_MOC_LITERAL(70, 1907, 27), // "on_actionViewSmooth_toggled"
QT_MOC_LITERAL(71, 1935, 26), // "on_actionViewTrack_toggled"
QT_MOC_LITERAL(72, 1962, 29), // "on_actionViewVPSave_triggered"
QT_MOC_LITERAL(73, 1992, 29), // "on_actionViewVPPrev_triggered"
QT_MOC_LITERAL(74, 2022, 29), // "on_actionViewVPNext_triggered"
QT_MOC_LITERAL(75, 2052, 28), // "on_actionSyncViews_triggered"
QT_MOC_LITERAL(76, 2081, 28), // "on_actionRecordNew_triggered"
QT_MOC_LITERAL(77, 2110, 30), // "on_actionRecordStart_triggered"
QT_MOC_LITERAL(78, 2141, 30), // "on_actionRecordPause_triggered"
QT_MOC_LITERAL(79, 2172, 29), // "on_actionRecordStop_triggered"
QT_MOC_LITERAL(80, 2202, 23), // "on_actionHelp_triggered"
QT_MOC_LITERAL(81, 2226, 24), // "on_actionAbout_triggered"
QT_MOC_LITERAL(82, 2251, 31), // "on_fontStyle_currentFontChanged"
QT_MOC_LITERAL(83, 2283, 4), // "font"
QT_MOC_LITERAL(84, 2288, 24), // "on_fontSize_valueChanged"
QT_MOC_LITERAL(85, 2313, 19), // "on_fontBold_toggled"
QT_MOC_LITERAL(86, 2333, 7), // "checked"
QT_MOC_LITERAL(87, 2341, 21), // "on_fontItalic_toggled"
QT_MOC_LITERAL(88, 2363, 26), // "on_selectTime_valueChanged"
QT_MOC_LITERAL(89, 2390, 27), // "on_selectAngle_valueChanged"
QT_MOC_LITERAL(90, 2418, 21), // "on_tab_currentChanged"
QT_MOC_LITERAL(91, 2440, 24), // "on_tab_tabCloseRequested"
QT_MOC_LITERAL(92, 2465, 19), // "finishedReadingFile"
QT_MOC_LITERAL(93, 2485, 7), // "success"
QT_MOC_LITERAL(94, 2493, 11), // "errorString"
QT_MOC_LITERAL(95, 2505, 17), // "checkFileProgress"
QT_MOC_LITERAL(96, 2523, 16), // "onCancelFileRead"
QT_MOC_LITERAL(97, 2540, 24), // "on_recentFiles_triggered"
QT_MOC_LITERAL(98, 2565, 8), // "QAction*"
QT_MOC_LITERAL(99, 2574, 6), // "action"
QT_MOC_LITERAL(100, 2581, 27), // "on_recentSessions_triggered"
QT_MOC_LITERAL(101, 2609, 7) // "onTimer"

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
    "on_actionSelectConn_toggled\0b\0"
    "on_actionZoomSelected_triggered\0"
    "on_actionZoomExtents_triggered\0"
    "on_actionPlaneCut_triggered\0"
    "on_actionMirrorPlane_triggered\0"
    "on_actionVectorPlot_triggered\0"
    "on_actionTensorPlot_triggered\0"
    "on_actionIsosurfacePlot_triggered\0"
    "on_actionSlicePlot_triggered\0"
    "on_actionDisplacementMap_triggered\0"
    "on_actionStreamLinePlot_triggered\0"
    "on_actionParticleFlowPlot_triggered\0"
    "on_actionImageSlicer_triggered\0"
    "on_actionVolumeRender_triggered\0"
    "on_actionMarchingCubes_triggered\0"
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
    "on_selectData_currentValueChanged\0i\0"
    "on_actionPlay_toggled\0on_actionFirst_triggered\0"
    "on_actionPrev_triggered\0on_actionNext_triggered\0"
    "on_actionLast_triggered\0"
    "on_actionTimeSettings_triggered\0"
    "on_actionViewSettings_triggered\0"
    "on_actionViewCapture_toggled\0"
    "on_actionViewWidgets_toggled\0"
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
    "on_actionSyncViews_triggered\0"
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
    "on_selectAngle_valueChanged\0"
    "on_tab_currentChanged\0on_tab_tabCloseRequested\0"
    "finishedReadingFile\0success\0errorString\0"
    "checkFileProgress\0onCancelFileRead\0"
    "on_recentFiles_triggered\0QAction*\0"
    "action\0on_recentSessions_triggered\0"
    "onTimer"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CMainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      91,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  469,    2, 0x0a /* Public */,
       3,    0,  470,    2, 0x0a /* Public */,
       4,    0,  471,    2, 0x0a /* Public */,
       5,    0,  472,    2, 0x0a /* Public */,
       6,    0,  473,    2, 0x0a /* Public */,
       7,    0,  474,    2, 0x0a /* Public */,
       8,    0,  475,    2, 0x0a /* Public */,
       9,    0,  476,    2, 0x0a /* Public */,
      10,    0,  477,    2, 0x0a /* Public */,
      11,    0,  478,    2, 0x0a /* Public */,
      12,    0,  479,    2, 0x0a /* Public */,
      13,    0,  480,    2, 0x0a /* Public */,
      14,    0,  481,    2, 0x0a /* Public */,
      15,    0,  482,    2, 0x0a /* Public */,
      16,    0,  483,    2, 0x0a /* Public */,
      17,    1,  484,    2, 0x0a /* Public */,
      19,    0,  487,    2, 0x0a /* Public */,
      20,    0,  488,    2, 0x0a /* Public */,
      21,    0,  489,    2, 0x0a /* Public */,
      22,    0,  490,    2, 0x0a /* Public */,
      23,    0,  491,    2, 0x0a /* Public */,
      24,    0,  492,    2, 0x0a /* Public */,
      25,    0,  493,    2, 0x0a /* Public */,
      26,    0,  494,    2, 0x0a /* Public */,
      27,    0,  495,    2, 0x0a /* Public */,
      28,    0,  496,    2, 0x0a /* Public */,
      29,    0,  497,    2, 0x0a /* Public */,
      30,    0,  498,    2, 0x0a /* Public */,
      31,    0,  499,    2, 0x0a /* Public */,
      32,    0,  500,    2, 0x0a /* Public */,
      33,    0,  501,    2, 0x0a /* Public */,
      34,    0,  502,    2, 0x0a /* Public */,
      35,    0,  503,    2, 0x0a /* Public */,
      36,    0,  504,    2, 0x0a /* Public */,
      37,    0,  505,    2, 0x0a /* Public */,
      38,    0,  506,    2, 0x0a /* Public */,
      39,    0,  507,    2, 0x0a /* Public */,
      40,    0,  508,    2, 0x0a /* Public */,
      41,    0,  509,    2, 0x0a /* Public */,
      42,    0,  510,    2, 0x0a /* Public */,
      43,    0,  511,    2, 0x0a /* Public */,
      44,    0,  512,    2, 0x0a /* Public */,
      45,    0,  513,    2, 0x0a /* Public */,
      46,    0,  514,    2, 0x0a /* Public */,
      47,    1,  515,    2, 0x0a /* Public */,
      49,    1,  518,    2, 0x0a /* Public */,
      51,    1,  521,    2, 0x0a /* Public */,
      52,    0,  524,    2, 0x0a /* Public */,
      53,    0,  525,    2, 0x0a /* Public */,
      54,    0,  526,    2, 0x0a /* Public */,
      55,    0,  527,    2, 0x0a /* Public */,
      56,    0,  528,    2, 0x0a /* Public */,
      57,    0,  529,    2, 0x0a /* Public */,
      58,    1,  530,    2, 0x0a /* Public */,
      59,    1,  533,    2, 0x0a /* Public */,
      60,    1,  536,    2, 0x0a /* Public */,
      61,    0,  539,    2, 0x0a /* Public */,
      62,    0,  540,    2, 0x0a /* Public */,
      63,    0,  541,    2, 0x0a /* Public */,
      64,    0,  542,    2, 0x0a /* Public */,
      65,    0,  543,    2, 0x0a /* Public */,
      66,    0,  544,    2, 0x0a /* Public */,
      67,    1,  545,    2, 0x0a /* Public */,
      68,    1,  548,    2, 0x0a /* Public */,
      69,    1,  551,    2, 0x0a /* Public */,
      70,    1,  554,    2, 0x0a /* Public */,
      71,    1,  557,    2, 0x0a /* Public */,
      72,    0,  560,    2, 0x0a /* Public */,
      73,    0,  561,    2, 0x0a /* Public */,
      74,    0,  562,    2, 0x0a /* Public */,
      75,    0,  563,    2, 0x0a /* Public */,
      76,    0,  564,    2, 0x0a /* Public */,
      77,    0,  565,    2, 0x0a /* Public */,
      78,    0,  566,    2, 0x0a /* Public */,
      79,    0,  567,    2, 0x0a /* Public */,
      80,    0,  568,    2, 0x0a /* Public */,
      81,    0,  569,    2, 0x0a /* Public */,
      82,    1,  570,    2, 0x0a /* Public */,
      84,    1,  573,    2, 0x0a /* Public */,
      85,    1,  576,    2, 0x0a /* Public */,
      87,    1,  579,    2, 0x0a /* Public */,
      88,    1,  582,    2, 0x0a /* Public */,
      89,    1,  585,    2, 0x0a /* Public */,
      90,    1,  588,    2, 0x0a /* Public */,
      91,    1,  591,    2, 0x0a /* Public */,
      92,    2,  594,    2, 0x0a /* Public */,
      95,    0,  599,    2, 0x0a /* Public */,
      96,    0,  600,    2, 0x0a /* Public */,
      97,    1,  601,    2, 0x0a /* Public */,
     100,    1,  604,    2, 0x0a /* Public */,
     101,    0,  607,    2, 0x0a /* Public */,

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
    QMetaType::Void, QMetaType::Bool,   18,
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
    QMetaType::Void, QMetaType::Bool,   48,
    QMetaType::Void, QMetaType::Int,   50,
    QMetaType::Void, QMetaType::Bool,   48,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   48,
    QMetaType::Void, QMetaType::Bool,   48,
    QMetaType::Void, QMetaType::Bool,   48,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   48,
    QMetaType::Void, QMetaType::Bool,   48,
    QMetaType::Void, QMetaType::Bool,   48,
    QMetaType::Void, QMetaType::Bool,   48,
    QMetaType::Void, QMetaType::Bool,   48,
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
    QMetaType::Void, QMetaType::QFont,   83,
    QMetaType::Void, QMetaType::Int,   50,
    QMetaType::Void, QMetaType::Bool,   86,
    QMetaType::Void, QMetaType::Bool,   48,
    QMetaType::Void, QMetaType::Int,   50,
    QMetaType::Void, QMetaType::Int,   50,
    QMetaType::Void, QMetaType::Int,   50,
    QMetaType::Void, QMetaType::Int,   50,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   93,   94,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 98,   99,
    QMetaType::Void, 0x80000000 | 98,   99,
    QMetaType::Void,

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
        case 15: _t->on_actionSelectConn_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 16: _t->on_actionZoomSelected_triggered(); break;
        case 17: _t->on_actionZoomExtents_triggered(); break;
        case 18: _t->on_actionPlaneCut_triggered(); break;
        case 19: _t->on_actionMirrorPlane_triggered(); break;
        case 20: _t->on_actionVectorPlot_triggered(); break;
        case 21: _t->on_actionTensorPlot_triggered(); break;
        case 22: _t->on_actionIsosurfacePlot_triggered(); break;
        case 23: _t->on_actionSlicePlot_triggered(); break;
        case 24: _t->on_actionDisplacementMap_triggered(); break;
        case 25: _t->on_actionStreamLinePlot_triggered(); break;
        case 26: _t->on_actionParticleFlowPlot_triggered(); break;
        case 27: _t->on_actionImageSlicer_triggered(); break;
        case 28: _t->on_actionVolumeRender_triggered(); break;
        case 29: _t->on_actionMarchingCubes_triggered(); break;
        case 30: _t->on_actionGraph_triggered(); break;
        case 31: _t->on_actionSummary_triggered(); break;
        case 32: _t->on_actionStats_triggered(); break;
        case 33: _t->on_actionIntegrate_triggered(); break;
        case 34: _t->on_actionHideSelected_triggered(); break;
        case 35: _t->on_actionHideUnselected_triggered(); break;
        case 36: _t->on_actionInvertSelection_triggered(); break;
        case 37: _t->on_actionUnhideAll_triggered(); break;
        case 38: _t->on_actionSelectAll_triggered(); break;
        case 39: _t->on_actionSelectRange_triggered(); break;
        case 40: _t->on_actionClearSelection_triggered(); break;
        case 41: _t->on_actionFind_triggered(); break;
        case 42: _t->on_actionDelete_triggered(); break;
        case 43: _t->on_actionProperties_triggered(); break;
        case 44: _t->on_actionColorMap_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 45: _t->on_selectData_currentValueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 46: _t->on_actionPlay_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 47: _t->on_actionFirst_triggered(); break;
        case 48: _t->on_actionPrev_triggered(); break;
        case 49: _t->on_actionNext_triggered(); break;
        case 50: _t->on_actionLast_triggered(); break;
        case 51: _t->on_actionTimeSettings_triggered(); break;
        case 52: _t->on_actionViewSettings_triggered(); break;
        case 53: _t->on_actionViewCapture_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 54: _t->on_actionViewWidgets_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 55: _t->on_actionViewProjection_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 56: _t->on_actionViewFront_triggered(); break;
        case 57: _t->on_actionViewBack_triggered(); break;
        case 58: _t->on_actionViewLeft_triggered(); break;
        case 59: _t->on_actionViewRight_triggered(); break;
        case 60: _t->on_actionViewTop_triggered(); break;
        case 61: _t->on_actionViewBottom_triggered(); break;
        case 62: _t->on_actionViewMesh_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 63: _t->on_actionViewOutline_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 64: _t->on_actionViewShowTags_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 65: _t->on_actionViewSmooth_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 66: _t->on_actionViewTrack_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 67: _t->on_actionViewVPSave_triggered(); break;
        case 68: _t->on_actionViewVPPrev_triggered(); break;
        case 69: _t->on_actionViewVPNext_triggered(); break;
        case 70: _t->on_actionSyncViews_triggered(); break;
        case 71: _t->on_actionRecordNew_triggered(); break;
        case 72: _t->on_actionRecordStart_triggered(); break;
        case 73: _t->on_actionRecordPause_triggered(); break;
        case 74: _t->on_actionRecordStop_triggered(); break;
        case 75: _t->on_actionHelp_triggered(); break;
        case 76: _t->on_actionAbout_triggered(); break;
        case 77: _t->on_fontStyle_currentFontChanged((*reinterpret_cast< const QFont(*)>(_a[1]))); break;
        case 78: _t->on_fontSize_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 79: _t->on_fontBold_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 80: _t->on_fontItalic_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 81: _t->on_selectTime_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 82: _t->on_selectAngle_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 83: _t->on_tab_currentChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 84: _t->on_tab_tabCloseRequested((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 85: _t->finishedReadingFile((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 86: _t->checkFileProgress(); break;
        case 87: _t->onCancelFileRead(); break;
        case 88: _t->on_recentFiles_triggered((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 89: _t->on_recentSessions_triggered((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 90: _t->onTimer(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CMainWindow::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_CMainWindow.data,
    qt_meta_data_CMainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CMainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CMainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CMainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int CMainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 91)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 91;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 91)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 91;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
