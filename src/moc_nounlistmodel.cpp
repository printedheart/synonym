/****************************************************************************
** Meta object code from reading C++ file 'nounlistmodel.h'
**
** Created: Mon May 28 18:37:05 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.0rc1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "model/nounlistmodel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'nounlistmodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.0rc1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_NounListModel[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_NounListModel[] = {
    "NounListModel\0\0reload()\0"
};

const QMetaObject NounListModel::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_NounListModel,
      qt_meta_data_NounListModel, 0 }
};

const QMetaObject *NounListModel::metaObject() const
{
    return &staticMetaObject;
}

void *NounListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NounListModel))
	return static_cast<void*>(const_cast< NounListModel*>(this));
    return QAbstractListModel::qt_metacast(_clname);
}

int NounListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: reload(); break;
        }
        _id -= 1;
    }
    return _id;
}
