#!/bin/bash

PROJ_LC=cs_CZ
PROJ_NAME=quickbox
FANDA_HOME=/home/fanda

PROJ_HOME=$FANDA_HOME/proj/${PROJ_NAME}
QT_HOME=$FANDA_HOME/programs/qt5/5.9.1/gcc_64/bin
TS_HOME=$FANDA_HOME/t/${PROJ_NAME}-ts
PROJ_BUILD=$FANDA_HOME/t/_distro/_build/gcc_64
QM_DEST_DIR=$PROJ_BUILD/bin/translations
LCTOOL_HOME=$PROJ_HOME/devutils/lctools
