#!/bin/sh

DISTRO_VER=0.1.14

WORK_DIR=/home/fanda/t/_distro
DISTRO_NAME=quickevent-linux64
# QT_DIR=/home/fanda/programs/qt/5.5/gcc
QT_DIR=/home/fanda/programs/qt5/5.5/gcc_64
BUILD_DIR=/home/fanda/proj/_build/quickbox-release
DIST_DIR=$WORK_DIR/$DISTRO_NAME
LIB_DIR=$DIST_DIR/lib
BIN_DIR=$DIST_DIR/bin

mkdir -p $DIST_DIR
rm -r $DIST_DIR/*

rsync -av --exclude '*.debug' $BUILD_DIR/lib/ $LIB_DIR
rsync -av --exclude '*.debug' $BUILD_DIR/bin/ $BIN_DIR

rsync -av $QT_DIR/lib/libicu* $LIB_DIR

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
rsync -av $QT_DIR/lib/libQt5DBus.so* $LIB_DIR
rsync -av $QT_DIR/lib/libQt5Multimedia.so* $LIB_DIR
rsync -av $QT_DIR/lib/libQt5XcbQpa.so* $LIB_DIR

rsync -av $QT_DIR/plugins/platforms/ $BIN_DIR/platforms
rsync -av $QT_DIR/plugins/printsupport/ $BIN_DIR/printsupport
mkdir -p $BIN_DIR/imageformats
rsync -av $QT_DIR/plugins/imageformats/libqjpeg.so $BIN_DIR/imageformats/
rsync -av $QT_DIR/plugins/imageformats/libqsvg.so $BIN_DIR/imageformats/
mkdir -p $BIN_DIR/sqldrivers
rsync -av $QT_DIR/plugins/sqldrivers/libqsqlite.so $BIN_DIR/sqldrivers/
rsync -av $QT_DIR/plugins/sqldrivers/libqsqlpsql.so $BIN_DIR/sqldrivers/

mkdir -p $BIN_DIR/QtQuick/Window.2
rsync -av $QT_DIR/qml/QtQuick/Window.2/ $BIN_DIR/QtQuick/Window.2
rsync -av $QT_DIR/qml/QtQuick.2/ $BIN_DIR/QtQuick.2

tar -cvzf $WORK_DIR/$DISTRO_NAME-$DISTRO_VER.tgz  -C $WORK_DIR ./$DISTRO_NAME
