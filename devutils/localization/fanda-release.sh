#!/bin/bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
. $DIR/fanda-env.sh

#$PROJ_HOME/devutils/localization/lctool.sh --src-dir $PROJ_HOME import-ts $TS_HOME
mkdir -p $QM_DEST_DIR
$PROJ_HOME/devutils/localization/lctool.sh --src-dir $PROJ_HOME --qt-bin-dir $QT_HOME release $QM_DEST_DIR
