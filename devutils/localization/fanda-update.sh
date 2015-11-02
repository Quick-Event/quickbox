#!/bin/bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
. $DIR/fanda-env.sh

mkdir -p $TS_HOME
rm $TS_HOME/*
$PROJ_HOME/devutils/localization/lctool.sh --src-dir $PROJ_HOME --qt-bin-dir $QT_HOME update
$PROJ_HOME/devutils/localization/lctool.sh --src-dir $PROJ_HOME --locale $PROJ_LC export-ts $TS_HOME
