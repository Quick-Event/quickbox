message("including $$PWD")

TEMPLATE = lib

unix:TARGET = $$OUT_PWD/../../lib/qfqmlwidgets

QT += widgets qml

include($$PWD/src/src.pri)
