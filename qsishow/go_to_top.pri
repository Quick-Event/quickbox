# utility how to find project top dir looking for the top.pri file
# on unix it should be link to unique file

# the only loop statement in qmake is a for
# currently max for 20 levels of subdirs

MY_TOP_PRI_FILE_NAME = top.pri

defineReplace(goToTop) {
	dirs = 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0
	top = .
	endfor = no
	for(dir, dirs) {
		#message("$$dir")
		file = $${top}/$$MY_TOP_PRI_FILE_NAME
		contains(endfor, no) {
			exists($$file) {
				#message("BINGO")
				endfor = yes
				return($${top})
			}
			#message("$$file")
			top = $${top}/..
		}
    }
	error("GoToTop(): cann't find project top dir definition file $$MY_TOP_PRI_FILE_NAME")
}

MY_PATH_TO_TOP = $$goToTop() 

include($$MY_PATH_TO_TOP/$$MY_TOP_PRI_FILE_NAME)
