QT  = core core-private sql-private

PLUGIN_TYPE = sqldrivers
load(qt_plugin)

DEFINES += QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_ASCII

# DESTDIR have to be defined after load(qt_plugin)
DESTDIR = $$OUT_PWD/../../../bin/sqldrivers
