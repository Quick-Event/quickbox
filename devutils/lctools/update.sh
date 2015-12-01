#!/bin/bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
. $DIR/fanda-env.sh

mkdir -p $TS_HOME
rm $TS_HOME/*
$LCTOOL_HOME/lctool.sh --src-dir $PROJ_HOME --qt-bin-dir $QT_HOME update
$LCTOOL_HOME/lctool.sh --src-dir $PROJ_HOME --locale $PROJ_LC export-ts $TS_HOME
