echo "making quickevent" $WORKSPACE "in" `pwd`
QT_DIR=/c/Qt/5.15.0/mingw81_64
MINGW_DIR=/c/Qt/Tools/mingw810_64
PATH=$MINGW_DIR/bin:$PATH
$QT_DIR/bin/qmake DEFINES+=GIT_COMMIT=${CI_COMMIT_SHA} DEFINES+=GIT_BRANCH=${CI_COMMIT_REF_SLUG} -r CONFIG+=release CONFIG+=force_debug_info CONFIG+=separate_debug_info quickbox.pro || exit 2
$MINGW_DIR/bin/mingw32-make.exe -j2 || exit 2

TRANS_DIR=bin/translations
mkdir -p $TRANS_DIR
for tsfile in `/usr/bin/find . -name "*.ts"` ; do
	qmfile=`basename "${tsfile%.*}.qm"`
	echo "$QT_DIR/bin/lrelease $tsfile -qm $TRANS_DIR/$qmfile"
	$QT_DIR/bin/lrelease $tsfile -qm $TRANS_DIR/$qmfile
done

LANGUAGES=("cs_CZ" "fr_FR" "nb_NO" "nl_BE" "pl_PL" "ru_RU" "uk_UA")
for language in ${LANGUAGES[@]} ; do
	# some languages don't have qtbase file translated
	cp --verbose "${QT_DIR}/translations/qtbase_${language:0:2}.qm" "${TRANS_DIR}/qtbase.${language}.qm" || true
done

QUICKEVENT_VERSION=`grep APP_VERSION quickevent/app/quickevent/src/appversion.h | cut -d\" -f2`

"C:\Program Files (x86)\Inno Setup 5\iscc.exe" "-DVERSION=${QUICKEVENT_VERSION}" quickevent/quickevent.iss  || exit 2




