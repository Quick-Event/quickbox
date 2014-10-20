TARGET = qfsqlmysql

QT = core-private

SOURCES = main.cpp
OTHER_FILES += mysql.json
include(./qsql_mysql.pri)

PLUGIN_CLASS_NAME = QFMYSQLDriverPlugin
include(../qsqldriverbase.pri)
