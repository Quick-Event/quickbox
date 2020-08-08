echo "making quickevent" $WORKSPACE
PATH=/c/Qt/Tools/mingw810_64/bin:$PATH
c:/Qt/5.15.0/mingw81_64/bin/qmake.exe DEFINES+=GIT_COMMIT=${CI_COMMIT_SHA} DEFINES+=GIT_BRANCH=${CI_COMMIT_REF_SLUG} MINGW_DIR=c:/Qt/Tools/mingw730_64 -r CONFIG+=release CONFIG+=force_debug_info CONFIG+=separate_debug_info quickbox.pro || exit 2
c:/Qt/Tools/mingw810_64/bin/mingw32-make.exe -j4 || exit 2

#[ -d bin/translations ] || mkdir bin/translations  || exit 2

QUICKEVENT_VERSION=`grep APP_VERSION quickevent/app/quickevent/src/appversion.h | cut -d\" -f2`

"C:\Program Files (x86)\Inno Setup 5\iscc.exe" "-DVERSION=${QUICKEVENT_VERSION}" quickevent/quickevent.iss  || exit /b 2




