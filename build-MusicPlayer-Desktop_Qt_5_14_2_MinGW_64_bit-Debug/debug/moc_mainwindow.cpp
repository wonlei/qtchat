/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../MusicPlayer/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[28];
    char stringdata0[434];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 10), // "openFolder"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 14), // "addMusicToList"
QT_MOC_LITERAL(4, 38, 8), // "filePath"
QT_MOC_LITERAL(5, 47, 19), // "onPlayButtonClicked"
QT_MOC_LITERAL(6, 67, 23), // "onListWidgetItemClicked"
QT_MOC_LITERAL(7, 91, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(8, 108, 4), // "item"
QT_MOC_LITERAL(9, 113, 36), // "on_listWidgetSongs_itemDouble..."
QT_MOC_LITERAL(10, 150, 20), // "onPlayerStateChanged"
QT_MOC_LITERAL(11, 171, 19), // "QMediaPlayer::State"
QT_MOC_LITERAL(12, 191, 5), // "state"
QT_MOC_LITERAL(13, 197, 21), // "onCurrentMediaChanged"
QT_MOC_LITERAL(14, 219, 5), // "index"
QT_MOC_LITERAL(15, 225, 17), // "onPositionChanged"
QT_MOC_LITERAL(16, 243, 8), // "position"
QT_MOC_LITERAL(17, 252, 17), // "onDurationChanged"
QT_MOC_LITERAL(18, 270, 8), // "duration"
QT_MOC_LITERAL(19, 279, 20), // "onSliderValueChanged"
QT_MOC_LITERAL(20, 300, 5), // "value"
QT_MOC_LITERAL(21, 306, 19), // "onModeButtonClicked"
QT_MOC_LITERAL(22, 326, 21), // "onVolumeSliderChanged"
QT_MOC_LITERAL(23, 348, 19), // "onMuteButtonClicked"
QT_MOC_LITERAL(24, 368, 21), // "onSearchButtonClicked"
QT_MOC_LITERAL(25, 390, 19), // "onSearchTextChanged"
QT_MOC_LITERAL(26, 410, 4), // "text"
QT_MOC_LITERAL(27, 415, 18) // "on_btnNext_clicked"

    },
    "MainWindow\0openFolder\0\0addMusicToList\0"
    "filePath\0onPlayButtonClicked\0"
    "onListWidgetItemClicked\0QListWidgetItem*\0"
    "item\0on_listWidgetSongs_itemDoubleClicked\0"
    "onPlayerStateChanged\0QMediaPlayer::State\0"
    "state\0onCurrentMediaChanged\0index\0"
    "onPositionChanged\0position\0onDurationChanged\0"
    "duration\0onSliderValueChanged\0value\0"
    "onModeButtonClicked\0onVolumeSliderChanged\0"
    "onMuteButtonClicked\0onSearchButtonClicked\0"
    "onSearchTextChanged\0text\0on_btnNext_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   94,    2, 0x08 /* Private */,
       3,    1,   95,    2, 0x08 /* Private */,
       5,    0,   98,    2, 0x08 /* Private */,
       6,    1,   99,    2, 0x08 /* Private */,
       9,    1,  102,    2, 0x08 /* Private */,
      10,    1,  105,    2, 0x08 /* Private */,
      13,    1,  108,    2, 0x08 /* Private */,
      15,    1,  111,    2, 0x08 /* Private */,
      17,    1,  114,    2, 0x08 /* Private */,
      19,    1,  117,    2, 0x08 /* Private */,
      21,    0,  120,    2, 0x08 /* Private */,
      22,    1,  121,    2, 0x08 /* Private */,
      23,    0,  124,    2, 0x08 /* Private */,
      24,    0,  125,    2, 0x08 /* Private */,
      25,    1,  126,    2, 0x08 /* Private */,
      27,    0,  129,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, QMetaType::Int,   14,
    QMetaType::Void, QMetaType::LongLong,   16,
    QMetaType::Void, QMetaType::LongLong,   18,
    QMetaType::Void, QMetaType::Int,   20,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   20,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   26,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->openFolder(); break;
        case 1: _t->addMusicToList((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->onPlayButtonClicked(); break;
        case 3: _t->onListWidgetItemClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 4: _t->on_listWidgetSongs_itemDoubleClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 5: _t->onPlayerStateChanged((*reinterpret_cast< QMediaPlayer::State(*)>(_a[1]))); break;
        case 6: _t->onCurrentMediaChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->onPositionChanged((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 8: _t->onDurationChanged((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 9: _t->onSliderValueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->onModeButtonClicked(); break;
        case 11: _t->onVolumeSliderChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->onMuteButtonClicked(); break;
        case 13: _t->onSearchButtonClicked(); break;
        case 14: _t->onSearchTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 15: _t->on_btnNext_clicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QMediaPlayer::State >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
