#!/bin/sh

QT_DIR=/home/fanda/programs/qt/5.5/gcc
LIB_DIR=lib
BIN_DIR=bin

rsync -av $QT_DIR/lib/libQt5Core.so* $LIB_DIR
rsync -av $QT_DIR/lib/libQt5Gui.so* $LIB_DIR
rsync -av $QT_DIR/lib/libQt5Widgets.so* $LIB_DIR
rsync -av $QT_DIR/lib/libQt5XmlPatterns.so* $LIB_DIR
rsync -av $QT_DIR/lib/libQt5Network.so* $LIB_DIR
rsync -av $QT_DIR/lib/libQt5Sql.so* $LIB_DIR
rsync -av $QT_DIR/lib/libQt5Xml.so* $LIB_DIR
rsync -av $QT_DIR/lib/libQt5Qml.so* $LIB_DIR
rsync -av $QT_DIR/lib/libQt5Quick.so* $LIB_DIR
rsync -av $QT_DIR/lib/libQt5Svg.so* $LIB_DIR
rsync -av $QT_DIR/lib/libQt5Script.so* $LIB_DIR
rsync -av $QT_DIR/lib/libQt5ScriptTools.so* $LIB_DIR
rsync -av $QT_DIR/lib/libQt5PrintSupport.so* $LIB_DIR
rsync -av $QT_DIR/lib/libQt5SerialPort.so* $LIB_DIR
rsync -av $QT_DIR/lib/libQt5Multimedia.so* $LIB_DIR

rsync -av $QT_DIR/plugins/platforms/ $BIN_DIR/platforms
rsync -av $QT_DIR/plugins/printsupport/ $BIN_DIR/printsupport
rsync -av $QT_DIR/plugins/imageformats/libqjpeg.so $BIN_DIR/imageformats
rsync -av $QT_DIR/plugins/imageformats/libqsvg.so $BIN_DIR/imageformats
rsync -av $QT_DIR/plugins/sqldrivers/libqsqlite.so $BIN_DIR/sqldrivers
rsync -av $QT_DIR/plugins/sqldrivers/libqsqlpsql.so $BIN_DIR/sqldrivers

rsync -av $QT_DIR/qml/QtQuick/Window.2/ $BIN_DIR/QtQuick/Window.2
rsync -av $QT_DIR/qml/QtQuick.2/ $LIB_DIR/QtQuick.2

