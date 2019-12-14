#!/bin/bash

APP_VER=0.0.1
APP_NAME=quickevent
SRC_DIR=/home/fanda/proj/$APP_NAME
QT_DIR=/home/fanda/programs/qt5/5.13.2/gcc_64
WORK_DIR=/tmp/_distro
#DISTRO_VER=1.0.2
APP_IMAGE_TOOL=/home/fanda/programs/appimagetool-x86_64.AppImage

help() {
	echo "make-dist.sh"
	echo "\t--app-name application name, ie: myapp"
	echo "\t--app-version application version, ie: 1.0.0"
	echo "\t--src-dir application source dir, where *.pro file lies, ie: /home/me/myapp"
	echo "\t--qt-dir QT dir, ie: /home/me/qt5/5.13.1/gcc_64"
	echo "\t--work-dir directory where build will be made, ie: /tmp/myapp"
	echo "\t--no-clean do not rebuild whole project when set to 1"
	exit 0
}

while [[ $# -gt 0 ]]
do
key="$1"
# echo key: $key
case $key in
	--app-name)
	APP_NAME="$2"
	shift # past argument
	shift # past value
	;;
	--app-version)
	APP_VER="$2"
	shift # past argument
	shift # past value
	;;
	--qt-dir)
	QT_DIR="$2"
	shift # past argument
	shift # past value
	;;
	--src-dir)
	SRC_DIR="$2"
	shift # past argument
	shift # past value
	;;
	--work-dir)
	WORK_DIR="$2"
	shift # past argument
	shift # past value
	;;
	--no-clean)
	NO_CLEAN=1
	shift # past value
	;;
	-h|--help)
	shift # past value
	help
	;;
	*)    # unknown option
	echo "ignoring argument $1"
	shift # past argument
	;;
esac
done

if [ -z $DISTRO_VER ]; then
    DISTRO_VER=`grep APP_VERSION $SRC_DIR/quickevent/app/quickevent/src/appversion.h | cut -d\" -f2`
	echo "Distro version not specified, deduced from source code: $DISTRO_VER" >&2
	#exit 1
fi

echo APP_VER: $APP_VER
echo APP_NAME: $APP_NAME
echo SRC_DIR: $SRC_DIR
echo QT_DIR: $QT_DIR
echo WORK_DIR: $WORK_DIR
echo NO_CLEAN: $NO_CLEAN

if [ -z $USE_SYSTEM_QT ]; then
    QT_LIB_DIR=$QT_DIR/lib
    QMAKE=$QT_DIR/bin/qmake
    DISTRO_NAME=$APP_NAME-$DISTRO_VER-linux64
else
    echo using system QT
    QT_DIR=/usr/lib/i386-linux-gnu/qt5
    QT_LIB_DIR=/usr/lib/i386-linux-gnu
    QMAKE=/usr/bin/qmake
    DISTRO_NAME=$APP_NAME-$DISTRO_VER-linux32
fi

$QMAKE -v

BUILD_DIR=$WORK_DIR/_build
DIST_DIR=$WORK_DIR/$DISTRO_NAME
DIST_LIB_DIR=$DIST_DIR/lib
DIST_BIN_DIR=$DIST_DIR/bin

if [ -z $NO_CLEAN ]; then
	echo removing directory $WORK_DIR
	rm -r $BUILD_DIR
	mkdir -p $BUILD_DIR
fi

cd $BUILD_DIR
$QMAKE $SRC_DIR/$APP_NAME.pro CONFIG+=release CONFIG+=force_debug_info CONFIG+=separate_debug_info -r -spec linux-g++
make -j2
if [ $? -ne 0 ]; then
  echo "Make Error" >&2
  exit 1
fi

rm -r $DIST_DIR
mkdir -p $DIST_DIR

rsync -av --exclude '*.debug' $BUILD_DIR/lib/ $DIST_LIB_DIR
rsync -av --exclude '*.debug' $BUILD_DIR/bin/ $DIST_BIN_DIR

#rsync -a --exclude '*.debug'v $QT_DIR/lib/libicu* $DIST_LIB_DIR

rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5Core.so* $DIST_LIB_DIR
rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5Gui.so* $DIST_LIB_DIR
rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5Widgets.so* $DIST_LIB_DIR
rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5XmlPatterns.so* $DIST_LIB_DIR
rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5Network.so* $DIST_LIB_DIR
rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5Sql.so* $DIST_LIB_DIR
rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5Xml.so* $DIST_LIB_DIR
rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5Qml.so* $DIST_LIB_DIR
rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5Quick.so* $DIST_LIB_DIR
rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5Svg.so* $DIST_LIB_DIR
rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5Script.so* $DIST_LIB_DIR
rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5ScriptTools.so* $DIST_LIB_DIR
rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5PrintSupport.so* $DIST_LIB_DIR
rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5SerialPort.so* $DIST_LIB_DIR
rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5DBus.so* $DIST_LIB_DIR
rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5Multimedia.so* $DIST_LIB_DIR
rsync -av --exclude '*.debug' $QT_LIB_DIR/libQt5XcbQpa.so* $DIST_LIB_DIR

rsync -av --exclude '*.debug' $QT_LIB_DIR/libicu*.so* $DIST_LIB_DIR

rsync -av --exclude '*.debug' $QT_DIR/plugins/platforms/ $DIST_BIN_DIR/platforms
rsync -av --exclude '*.debug' $QT_DIR/plugins/printsupport/ $DIST_BIN_DIR/printsupport

mkdir -p $DIST_BIN_DIR/imageformats
rsync -av --exclude '*.debug' $QT_DIR/plugins/imageformats/libqjpeg.so $DIST_BIN_DIR/imageformats/
rsync -av --exclude '*.debug' $QT_DIR/plugins/imageformats/libqsvg.so $DIST_BIN_DIR/imageformats/

mkdir -p $DIST_BIN_DIR/sqldrivers
rsync -av --exclude '*.debug' $QT_DIR/plugins/sqldrivers/libqsqlite.so $DIST_BIN_DIR/sqldrivers/
rsync -av --exclude '*.debug' $QT_DIR/plugins/sqldrivers/libqsqlpsql.so $DIST_BIN_DIR/sqldrivers/

mkdir -p $DIST_BIN_DIR/audio
rsync -av --exclude '*.debug' $QT_DIR/plugins/audio/ $DIST_BIN_DIR/audio/

mkdir -p $DIST_BIN_DIR/QtQuick/Window.2
rsync -av --exclude '*.debug' $QT_DIR/qml/QtQuick/Window.2/ $DIST_BIN_DIR/QtQuick/Window.2
rsync -av --exclude '*.debug' $QT_DIR/qml/QtQuick.2/ $DIST_BIN_DIR/QtQuick.2

tar -cvzf $WORK_DIR/$DISTRO_NAME.tgz  -C $WORK_DIR ./$DISTRO_NAME

rsync -av $SRC_DIR/$APP_NAME/distro/QuickEvent.AppDir/* $DIST_DIR/
ARCH=x86_64 $APP_IMAGE_TOOL $DIST_DIR $WORK_DIR/$APP_NAME-${APP_VER}-x86_64.AppImage
