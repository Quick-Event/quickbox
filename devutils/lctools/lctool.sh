#!/bin/bash

O_LOCALE='*'

while [[ $# > 0 ]]
do
	key="$1"

	case $key in
		-h|--help)
			O_HELP=1
			;;
		-s|--src-dir)
			O_SRC_DIR="$2"
			shift # past argument
			;;
		--qt-bin-dir)
			O_QT_BIN_DIR="$2"
			shift # past argument
			;;
		--locale)
			O_LOCALE="$2"
			shift # past argument
			;;
		*)
			if [[ -z $O_COMMAND ]]; then
				O_COMMAND=$1
			else
				O_ARGS="$O_ARGS $1"
			fi
			;;
	esac
	shift # past argument or value
done

if [[ -z $O_COMMAND ]]; then
	echo "command must be specified"
fi

if [[ -n $O_HELP ]] || [[ -z $O_COMMAND ]]; then
	echo "lctool [OPTIONS] [COMMAND] [ARGS]"
	echo "OPTIONS:"
	echo -e "\t" "--qt-bin-dir: directory where lrelease and lupdate are located"
	echo -e "\t" "--bin-dir: eyas binaries directory, pwd if not specified"
	echo -e "\t" "--src-dir: eyas sources directory"
	echo -e "\t" "--locale: locale for import-ts, export-ts, release commands, ie. pl_PL"
	echo "COMMAND:"
	echo -e "\t" "import-ts <directory with *.ts files>: copies ts files to right places in source tree"
	echo -e "\t" "export-ts <out directory>: copies ts files the out directory"
	echo -e "\t" "update: call lupdate for eyas project"
	echo -e "\t" "release: call lrelease and move *.qm files to the bin-dir"
	exit 1
fi

# echo "source dir: ${O_SRC_DIR}"
# echo "command: ${O_COMMAND}"
# echo "args: ${O_ARGS}"

declare -A paths
paths[libqfcore]=libqf/libqfcore
paths[libqfqmlwidgets]=libqf/libqfqmlwidgets

paths[shared]=quickevent/app/plugins/qml/shared
paths[quickevent]=quickevent/app/quickevent

paths[CardReader]=quickevent/app/plugins/qml/CardReader
paths[Classes]=quickevent/app/plugins/qml/Classes
paths[Competitors]=quickevent/app/plugins/qml/Competitors
paths[Core]=quickevent/app/plugins/qml/Core
paths[Event]=quickevent/app/plugins/qml/Event
paths[Oris]=quickevent/app/plugins/qml/Oris
paths[Receipts]=quickevent/app/plugins/qml/Receipts
paths[Relays]=quickevent/app/plugins/qml/Relays
paths[Runs]=quickevent/app/plugins/qml/Runs
paths[Speaker]=quickevent/app/plugins/qml/Speaker

paths[qsqlmon]=tools/qsqlmon

if [[ $O_COMMAND == "import-ts" ]]; then
	if [[ -z $O_SRC_DIR ]]; then
		echo "--src-dir must be set"
		exit 1
	fi
	for fname in "${!paths[@]}"; do
		fpath=${paths[$fname]}
		dest_path=$O_SRC_DIR/$fpath
		#echo "fname: $fname"
		#echo "dest_path: $dest_path"
		tsnames=$O_ARGS/${fname}.${O_LOCALE}.ts
		for tsname in $tsnames; do
			echo "copy: $tsname to $dest_path"
			cp $tsname $dest_path/
		done
	done
	exit 0
fi

if [[ $O_COMMAND == "export-ts" ]]; then
	if [[ -z $O_SRC_DIR ]]; then
		echo "--src-dir must be set"
		exit 1
	fi
	out_dir=$O_ARGS
	if [[ -z $out_dir ]]; then
		out_dir=`pwd`
	fi
	for fname in "${!paths[@]}"; do
		fpath=${paths[$fname]}
		src_path=$O_SRC_DIR/$fpath
		echo "copy tsfiles from: $src_path to $out_dir"
		cp $src_path/${fname}.${O_LOCALE}.ts $out_dir/
	done
	exit 0
fi

if [[ $O_COMMAND == "update" ]]; then
	if [[ -z $O_SRC_DIR ]]; then
		echo "--src-dir must be set"
		exit 1
	fi
	lupdate="lupdate -no-obsolete"
	if [[ -n $O_QT_BIN_DIR ]]; then
		lupdate=$O_QT_BIN_DIR/$lupdate
	fi
	for fname in "${!paths[@]}"; do
		fpath=${paths[$fname]}
		pro_file=$O_SRC_DIR/$fpath/$fname.pro
		echo "updating translations for project: $pro_file"
		$lupdate $pro_file
	done
	exit 0
fi

if [[ $O_COMMAND == "release" ]]; then
	if [[ -z $O_SRC_DIR ]]; then
		echo "--src-dir must be set"
		exit 1
	fi
	bin_dir=$O_ARGS
	if [[ -z $bin_dir ]]; then
		bin_dir=`pwd`
	fi
	lrelease=lrelease
	if [[ -n $O_QT_BIN_DIR ]]; then
		lrelease=$O_QT_BIN_DIR/$lrelease
	fi
	for fname in "${!paths[@]}"; do
		fpath=${paths[$fname]}
		pro_file=$O_SRC_DIR/$fpath/$fname.pro
		echo "compiling translations for project: $pro_file"
		$lrelease $pro_file
		echo "moving translations" $O_SRC_DIR/$fpath/$fname.${O_LOCALE}.qm "to bin directory: $bin_dir"
		mv $O_SRC_DIR/$fpath/$fname.${O_LOCALE}.qm $bin_dir/
	done
	exit 0
fi
