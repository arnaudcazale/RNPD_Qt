/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.9)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.9. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[19];
    char stringdata0[275];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 14), // "dataReady_left"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 26), // "QVector<QVector<double> >*"
QT_MOC_LITERAL(4, 54, 15), // "dataReady_right"
QT_MOC_LITERAL(5, 70, 14), // "dataReady_line"
QT_MOC_LITERAL(6, 85, 14), // "QVector<QLine>"
QT_MOC_LITERAL(7, 100, 21), // "dataReadyGravity_line"
QT_MOC_LITERAL(8, 122, 21), // "dataReadyGravity_left"
QT_MOC_LITERAL(9, 144, 22), // "dataReadyGravity_right"
QT_MOC_LITERAL(10, 167, 14), // "openSerialPort"
QT_MOC_LITERAL(11, 182, 15), // "closeSerialPort"
QT_MOC_LITERAL(12, 198, 5), // "about"
QT_MOC_LITERAL(13, 204, 9), // "writeData"
QT_MOC_LITERAL(14, 214, 4), // "data"
QT_MOC_LITERAL(15, 219, 8), // "readData"
QT_MOC_LITERAL(16, 228, 11), // "handleError"
QT_MOC_LITERAL(17, 240, 28), // "QSerialPort::SerialPortError"
QT_MOC_LITERAL(18, 269, 5) // "error"

    },
    "MainWindow\0dataReady_left\0\0"
    "QVector<QVector<double> >*\0dataReady_right\0"
    "dataReady_line\0QVector<QLine>\0"
    "dataReadyGravity_line\0dataReadyGravity_left\0"
    "dataReadyGravity_right\0openSerialPort\0"
    "closeSerialPort\0about\0writeData\0data\0"
    "readData\0handleError\0QSerialPort::SerialPortError\0"
    "error"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   74,    2, 0x06 /* Public */,
       4,    1,   77,    2, 0x06 /* Public */,
       5,    1,   80,    2, 0x06 /* Public */,
       7,    1,   83,    2, 0x06 /* Public */,
       8,    1,   86,    2, 0x06 /* Public */,
       9,    1,   89,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    0,   92,    2, 0x08 /* Private */,
      11,    0,   93,    2, 0x08 /* Private */,
      12,    0,   94,    2, 0x08 /* Private */,
      13,    1,   95,    2, 0x08 /* Private */,
      15,    0,   98,    2, 0x08 /* Private */,
      16,    1,   99,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 6,    2,
    QMetaType::Void, 0x80000000 | 6,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,   14,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 17,   18,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->dataReady_left((*reinterpret_cast< QVector<QVector<double> >*(*)>(_a[1]))); break;
        case 1: _t->dataReady_right((*reinterpret_cast< QVector<QVector<double> >*(*)>(_a[1]))); break;
        case 2: _t->dataReady_line((*reinterpret_cast< QVector<QLine>(*)>(_a[1]))); break;
        case 3: _t->dataReadyGravity_line((*reinterpret_cast< QVector<QLine>(*)>(_a[1]))); break;
        case 4: _t->dataReadyGravity_left((*reinterpret_cast< QVector<QVector<double> >*(*)>(_a[1]))); break;
        case 5: _t->dataReadyGravity_right((*reinterpret_cast< QVector<QVector<double> >*(*)>(_a[1]))); break;
        case 6: _t->openSerialPort(); break;
        case 7: _t->closeSerialPort(); break;
        case 8: _t->about(); break;
        case 9: _t->writeData((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 10: _t->readData(); break;
        case 11: _t->handleError((*reinterpret_cast< QSerialPort::SerialPortError(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<QLine> >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<QLine> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MainWindow::*)(QVector<QVector<double> > * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::dataReady_left)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MainWindow::*)(QVector<QVector<double> > * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::dataReady_right)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (MainWindow::*)(QVector<QLine> );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::dataReady_line)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (MainWindow::*)(QVector<QLine> );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::dataReadyGravity_line)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (MainWindow::*)(QVector<QVector<double> > * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::dataReadyGravity_left)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (MainWindow::*)(QVector<QVector<double> > * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::dataReadyGravity_right)) {
                *result = 5;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
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
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::dataReady_left(QVector<QVector<double> > * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MainWindow::dataReady_right(QVector<QVector<double> > * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MainWindow::dataReady_line(QVector<QLine> _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void MainWindow::dataReadyGravity_line(QVector<QLine> _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void MainWindow::dataReadyGravity_left(QVector<QVector<double> > * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void MainWindow::dataReadyGravity_right(QVector<QVector<double> > * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
