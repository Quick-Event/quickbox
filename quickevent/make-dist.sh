#!/bin/sh

#DISTRO_VER=1.0.2

if [ $1 ]; then
	DISTRO_VER=$1
fi

if [ -z $DISTRO_VER ]; then
	echo "specify distro version like:\n$0 1.0.4" >&2
	exit 1
fi

SRC_DIR=/home/fanda/proj/quickbox
WORK_DIR=/home/fanda/t/_distro
DISTRO_NAME=quickevent-linux64-$DISTRO_VER
# QT_DIR=/home/fanda/programs/qt/5.5/gcc
QT_DIR=/home/fanda/programs/qt5/5.9/gcc_64
QMAKE=$QT_DIR/bin/qmake

BUILD_DIR=$WORK_DIR/_build/gcc_64
DIST_DIR=$WORK_DIR/$DISTRO_NAME
DIST_LIB_DIR=$DIST_DIR/lib
DIST_BIN_DIR=$DIST_DIR/bin

rm -r $BUILD_DIR
mkdir -p $BUILD_DIR

cd $BUILD_DIR
$QMAKE $SRC_DIR/quickbox.pro -r -spec linux-g++
make -j2
if [ $? -ne 0 ]; then
  echo "Make Error" >&2
  exit 1
fi

rm -r $DIST_DIR
mkdir -p $DIST_DIR

rsync -av --exclude '*.debug' $BUILD_DIR/lib/ $DIST_LIB_DIR
rsync -av --exclude '*.debug' $BUILD_DIR/bin/ $DIST_BIN_DIR

#rsync -av $QT_DIR/lib/libicu* $DIST_LIB_DIR

rsync -av $QT_DIR/lib/libQt5Core.so* $DIST_LIB_DIR
rsync -av $QT_DIR/lib/libQt5Gui.so* $DIST_LIB_DIR
rsync -av $QT_DIR/lib/libQt5Widgets.so* $DIST_LIB_DIR
rsync -av $QT_DIR/lib/libQt5XmlPatterns.so* $DIST_LIB_DIR
rsync -av $QT_DIR/lib/libQt5Network.so* $DIST_LIB_DIR
rsync -av $QT_DIR/lib/libQt5Sql.so* $DIST_LIB_DIR
rsync -av $QT_DIR/lib/libQt5Xml.so* $DIST_LIB_DIR
rsync -av $QT_DIR/lib/libQt5Qml.so* $DIST_LIB_DIR
rsync -av $QT_DIR/lib/libQt5Quick.so* $DIST_LIB_DIR
rsync -av $QT_DIR/lib/libQt5Svg.so* $DIST_LIB_DIR
rsync -av $QT_DIR/lib/libQt5Script.so* $DIST_LIB_DIR
rsync -av $QT_DIR/lib/libQt5ScriptTools.so* $DIST_LIB_DIR
rsync -av $QT_DIR/lib/libQt5PrintSupport.so* $DIST_LIB_DIR
rsync -av $QT_DIR/lib/libQt5SerialPort.so* $DIST_LIB_DIR
rsync -av $QT_DIR/lib/libQt5DBus.so* $DIST_LIB_DIR
rsync -av $QT_DIR/lib/libQt5Multimedia.so* $DIST_LIB_DIR
rsync -av $QT_DIR/lib/libQt5XcbQpa.so* $DIST_LIB_DIR

rsync -av $QT_DIR/plugins/platforms/ $DIST_BIN_DIR/platforms
rsync -av $QT_DIR/plugins/printsupport/ $DIST_BIN_DIR/printsupport
mkdir -p $DIST_BIN_DIR/imageformats
rsync -av $QT_DIR/plugins/imageformats/libqjpeg.so $DIST_BIN_DIR/imageformats/
rsync -av $QT_DIR/plugins/imageformats/libqsvg.so $DIST_BIN_DIR/imageformats/
mkdir -p $DIST_BIN_DIR/sqldrivers
rsync -av $QT_DIR/plugins/sqldrivers/libqsqlite.so $DIST_BIN_DIR/sqldrivers/
rsync -av $QT_DIR/plugins/sqldrivers/libqsqlpsql.so $DIST_BIN_DIR/sqldrivers/

mkdir -p $DIST_BIN_DIR/QtQuick/Window.2
rsync -av $QT_DIR/qml/QtQuick/Window.2/ $DIST_BIN_DIR/QtQuick/Window.2
rsync -av $QT_DIR/qml/QtQuick.2/ $DIST_BIN_DIR/QtQuick.2

tar -cvzf $WORK_DIR/$DISTRO_NAME.tgz  -C $WORK_DIR ./$DISTRO_NAME
