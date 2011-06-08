/****************************************************************************
** Meta object code from reading C++ file 'QtRenderer.h'
**
** Created: Wed Jun 8 21:30:00 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "QtRenderer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QtRenderer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Pixy__QtRenderer[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x05,
      40,   17,   17,   17, 0x05,
      91,   62,   17,   17, 0x05,
     142,  126,   17,   17, 0x05,
     178,  168,   17,   17, 0x05,
     223,  201,   17,   17, 0x05,
     277,  260,   17,   17, 0x05,
     304,  260,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
     336,   17,   17,   17, 0x0a,
     360,   17,   17,   17, 0x0a,
     384,   62,   17,   17, 0x0a,
     421,   17,   17,   17, 0x0a,
     443,  126,   17,   17, 0x0a,
     471,  168,   17,   17, 0x0a,
     496,  201,   17,   17, 0x0a,
     535,  260,   17,   17, 0x0a,
     564,  260,   17,   17, 0x0a,
     598,   17,   17,   17, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Pixy__QtRenderer[] = {
    "Pixy::QtRenderer\0\0emitUnableToConnect()\0"
    "emitValidateStarted()\0"
    "inNeedUpdate,inTargetVersion\0"
    "emitValidateComplete(bool,Version)\0"
    "inTargetVersion\0emitPatchStarted(Version)\0"
    "inPercent\0emitPatchProgress(int)\0"
    "inMsg,inTargetVersion\0"
    "emitPatchFailed(std::string,Version)\0"
    "inCurrentVersion\0emitPatchComplete(Version)\0"
    "emitApplicationPatched(Version)\0"
    "handleUnableToConnect()\0handleValidateStarted()\0"
    "handleValidateComplete(bool,Version)\0"
    "handlePatchAccepted()\0handlePatchStarted(Version)\0"
    "handlePatchProgress(int)\0"
    "handlePatchFailed(std::string,Version)\0"
    "handlePatchComplete(Version)\0"
    "handleApplicationPatched(Version)\0"
    "handleLaunchApplication()\0"
};

const QMetaObject Pixy::QtRenderer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Pixy__QtRenderer,
      qt_meta_data_Pixy__QtRenderer, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Pixy::QtRenderer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Pixy::QtRenderer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Pixy::QtRenderer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Pixy__QtRenderer))
        return static_cast<void*>(const_cast< QtRenderer*>(this));
    if (!strcmp(_clname, "Renderer"))
        return static_cast< Renderer*>(const_cast< QtRenderer*>(this));
    return QObject::qt_metacast(_clname);
}

int Pixy::QtRenderer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: emitUnableToConnect(); break;
        case 1: emitValidateStarted(); break;
        case 2: emitValidateComplete((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const Version(*)>(_a[2]))); break;
        case 3: emitPatchStarted((*reinterpret_cast< const Version(*)>(_a[1]))); break;
        case 4: emitPatchProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: emitPatchFailed((*reinterpret_cast< std::string(*)>(_a[1])),(*reinterpret_cast< const Version(*)>(_a[2]))); break;
        case 6: emitPatchComplete((*reinterpret_cast< const Version(*)>(_a[1]))); break;
        case 7: emitApplicationPatched((*reinterpret_cast< const Version(*)>(_a[1]))); break;
        case 8: handleUnableToConnect(); break;
        case 9: handleValidateStarted(); break;
        case 10: handleValidateComplete((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const Version(*)>(_a[2]))); break;
        case 11: handlePatchAccepted(); break;
        case 12: handlePatchStarted((*reinterpret_cast< const Version(*)>(_a[1]))); break;
        case 13: handlePatchProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: handlePatchFailed((*reinterpret_cast< std::string(*)>(_a[1])),(*reinterpret_cast< const Version(*)>(_a[2]))); break;
        case 15: handlePatchComplete((*reinterpret_cast< const Version(*)>(_a[1]))); break;
        case 16: handleApplicationPatched((*reinterpret_cast< const Version(*)>(_a[1]))); break;
        case 17: handleLaunchApplication(); break;
        default: ;
        }
        _id -= 18;
    }
    return _id;
}

// SIGNAL 0
void Pixy::QtRenderer::emitUnableToConnect()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void Pixy::QtRenderer::emitValidateStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void Pixy::QtRenderer::emitValidateComplete(bool _t1, Version const & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Pixy::QtRenderer::emitPatchStarted(Version const & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Pixy::QtRenderer::emitPatchProgress(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Pixy::QtRenderer::emitPatchFailed(std::string _t1, Version const & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Pixy::QtRenderer::emitPatchComplete(Version const & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Pixy::QtRenderer::emitApplicationPatched(Version const & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_END_MOC_NAMESPACE
