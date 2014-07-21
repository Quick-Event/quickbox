#!/bin/sh
build_dir=../../../_build
app_name="qsqlmon"
src_dir=../../../$app_name
app_version="1.3.3"
app_arch="ia64"
pack_dir=$app_name

rm -rf $pack_dir
mkdir $pack_dir

cp -r ${build_dir}/bin ./$pack_dir/bin 
cp -r ${build_dir}/lib ./$pack_dir/lib 
cp ${src_dir}/$app_name.conf.xml ./$pack_dir/bin 
cp $app_name.desktop ./$pack_dir 
cp README ./$pack_dir 
mkdir -p $pack_dir/share/icons
cp ${src_dir}/images/${app_name}.svg ./$pack_dir/share/icons
cp ${src_dir}/images/${app_name}32x32.png ./$pack_dir/share/icons
cp ${src_dir}/images/${app_name}48x48.png ./$pack_dir/share/icons
cp ${src_dir}/images/${app_name}96x96.png ./$pack_dir/share/icons

 archiv_name=${app_name}-${app_version}-${app_arch}.tgz
 echo "saving backup to ${archiv_name}"
# 
# if [ -e $archiv_name ]; then
# 	echo "file $1 exists"
# 	rm -f $archiv_name
# 	#exit 1
# fi
# 
#cd $pack_dir
tar -cvz -f "${archiv_name}" $pack_dir
#cd ..
rm -rf $pack_dir
# tar -rv -f "${archiv_name}" "lib/"
# gzip ${archiv_name}
# 
# cd $curr_dir
# rm -f ./tmp
# 
# curr_dir=`pwd`
# cd $build_dir

# find . \
# 	\( \
# 		-name "*.[ch]" -or -name "*.cc" \
# 		-or -name "*.cpp" -or -name "*.ui" \
# 		-or -name "*.qrc" -or -name "*.rc" \
# 		-or -name "*.pro" -or -name "*.pri" \
# 		-or -name "*.ts" \
# 		-or -name "*.patch" -or -name "*.outl.txt" \
# 		-or -name "*.bat" -or -name "*.sh" \
# 		-or -name "README" -or -name "INSTALL" \
# 		-or -name "ChangeLog" \
# 		-or -name "Doxyfile" \
# 		-or -name "*.png" -or -name "*.jpg" -or -name "*.svg" \
# 		-or -name "*.sql" -or -name "*.cnf" \
# 		-or -name "*.ico" -or -name "*.rnc" \
# 		-or -name "*.rep" -or -name "*.inc.xml"  -or -name "*.html"  -or -name "*.csv" \
# 		-or -name "*.css" -or -name "*.adp" -or -name "*.xml" \
# 		-or -name "*.kdevelop" -or -name "*.kdevelop.filelist" \
# 		-or -name "*.iss" -or -name "*.nb" \
# 		-or -name "*.js" -or -name "*.py" \
# 	\) \
#     -and -not \
# 	\( \
# 		-path "*/_*" -or -path "*/bin/*" \
# 		-or -path "*/CVS/*" -or -path "*/.bzr/*"  -or -path "*/_darcs/*" \
# 	\) \
# 	| tar -cvz --files-from=- -f "${archiv_name}"

