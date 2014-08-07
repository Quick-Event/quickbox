TARGET = $${TARGET}$$QF_LIBRARY_DEBUG_EXT
message($$TARGET)

MYSQL_OPT = opt

DEFINES += QF_PATCH
unix:MYSQL_DIR = /usr/include/mysql # fanda QF_PATCH
win32:MYSQL_DIR = c:/app/mysql5.6 # fanda QF_PATCH

HEADERS		= \
	$$PWD/qsql_mysql.h \

SOURCES		= \
	$$PWD/qsql_mysql.cpp \
	$$PWD/main.cpp \

unix:INCLUDEPATH += /usr/include/mysql  # output of: mysql_config --include
win32:INCLUDEPATH += $$MYSQL_DIR/include

qf-embedded-mysql {
	unix {
                LIBS     *= -rdynamic -L/usr/lib/mysql -lmysqld -lwrap -lrt # output of: mysql_config --libmysqld-libs
		LIBS += -lcrypt
	}
	win32 {
                LIBS += -lmysqld -L$$MYSQL_DIR/lib
	}
}
else {
	unix {
		use_libmysqlclient_r:LIBS     *= -lmysqlclient_r
		else:LIBS    *= -lmysqlclient
		LIBS += -L/usr/lib
	}
	win32 {
		LIBS += -lmysql -L$$MYSQL_DIR/lib/opt
	}
}

# mysql howto:
#==============================
# D:\app\MySQL\5.0\lib\opt>\app\MinGW\bin\reimp.exe -d libmysql.lib
# D:\app\MySQL\5.0\lib\opt>\app\MinGW\bin\dlltool.exe -k --input-def libmysql.def --dllname libmysql.dll --output-lib libmysql.a
# zkopirovat libmysql.a do adresare, kde je fmysql.pro

#udajne funguje gcc -d libmysql.dll -o foo.exe *.o, je to potreba vyzkouset.

# nejnovejsi metoda ukazuje (kompilace funguje, ale asi by to chtelo jeste proverit, ze driver nepada), ze staci pouze
# D:\app\MySQL\5.0\lib\opt>\app\MinGW\bin\reimp.exe libmysql.lib
# zda se, ze to fakt funguje, vytvori to soubor liblibmysql.a, ten je treba prejmenovat na libmysql.a a voila.
