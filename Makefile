DEBUG=
#Uncomment the below line to dispaly the runner debug
#DEBUG+= -DDEBUG_RUNNER
#Template for other debug flags to be added later
#DEBUG+=  -DDEBUG_DAY_14

#If adding another include directory, be sure to add it here
CFLAGS=-g ${DEBUG} -Iinclude/common -Iinclude/runner -Iinclude/screen -Iinclude/solutions

.DEFAULT_GOAL := all

# Runner library; contains a lot of common code for file parsing and handling test files
build/runner/file_utils.o: src/runner/file_utils.c  \
	include/runner/file_utils.h \
	include/common/constants.h
	gcc ${CFLAGS} -o build/runner/file_utils.o -c src/runner/file_utils.c

build/runner/aoc_test.o: src/runner/aoc_test.c  \
	include/runner/aoc_test.h \
	include/common/constants.h
	gcc ${CFLAGS} -o build/runner/aoc_test.o -c src/runner/aoc_test.c

build/runner/aoc_tests.o: src/runner/aoc_tests.c  \
	include/runner/aoc_tests.h \
	include/runner/aoc_test.h \
	include/common/constants.h \
	include/runner/file_utils.h
	gcc ${CFLAGS} -o build/runner/aoc_tests.o -c src/runner/aoc_tests.c

bin/lib/librunner.a: build/runner/aoc_test.o  \
	build/runner/aoc_tests.o  \
	build/runner/file_utils.o
	ar rcs bin/lib/librunner.a build/runner/aoc_test.o build/runner/aoc_tests.o build/runner/file_utils.o

# Screen libary - contains the screen functionality for game-of-life like problems
build/screen/aoc_screen.o: src/screen/aoc_screen.c  \
	include/screen/aoc_screen.h
	gcc ${CFLAGS} -o build/screen/aoc_screen.o -c src/screen/aoc_screen.c

bin/lib/libscreen.a: build/screen/aoc_screen.o
	ar rcs bin/lib/libscreen.a build/screen/aoc_screen.o

## Solutions - These are the programs for the daily solutions
build/solutions/aoc_solutions.o: src/solutions/aoc_solutions.c  \
	include/solutions/aoc_solutions.h \
	include/common/constants.h
	gcc ${CFLAGS} -o build/solutions/aoc_solutions.o -c src/solutions/aoc_solutions.c

##Generic line to compile a daily solution.
##Be sure to add the .o file to the libsoluations.a target
build/solutions/aoc_day_0.o: src/solutions/aoc_day_0.c  \
	include/solutions/aoc_day_0.h \
	include/common/constants.h
	gcc ${CFLAGS} -o build/solutions/aoc_day_0.o -c src/solutions/aoc_day_0.c

build/solutions/aoc_day_1.o: src/solutions/aoc_day_1.c  \
	include/solutions/aoc_day_1.h \
	include/common/constants.h
	gcc ${CFLAGS} -o build/solutions/aoc_day_1.o -c src/solutions/aoc_day_1.c

build/solutions/aoc_day_2.o: src/solutions/aoc_day_2.c  \
	include/solutions/aoc_day_2.h \
	include/common/constants.h
	gcc ${CFLAGS} -o build/solutions/aoc_day_2.o -c src/solutions/aoc_day_2.c

build/solutions/aoc_day_3.o: src/solutions/aoc_day_3.c  \
	include/solutions/aoc_day_3.h \
	include/common/constants.h
	gcc ${CFLAGS} -o build/solutions/aoc_day_3.o -c src/solutions/aoc_day_3.c

build/solutions/aoc_day_4.o: src/solutions/aoc_day_4.c  \
	include/solutions/aoc_day_4.h \
	include/common/constants.h
	gcc ${CFLAGS} -o build/solutions/aoc_day_4.o -c src/solutions/aoc_day_4.c

build/solutions/aoc_day_5.o: src/solutions/aoc_day_5.c  \
	include/solutions/aoc_day_5.h \
	include/common/constants.h
	gcc ${CFLAGS} -o build/solutions/aoc_day_5.o -c src/solutions/aoc_day_5.c

build/solutions/aoc_day_6.o: src/solutions/aoc_day_6.c  \
	include/solutions/aoc_day_6.h \
	include/common/constants.h
	gcc ${CFLAGS} -o build/solutions/aoc_day_6.o -c src/solutions/aoc_day_6.c

build/solutions/aoc_day_7.o: src/solutions/aoc_day_7.c  \
	include/solutions/aoc_day_7.h \
	include/common/constants.h
	gcc ${CFLAGS} -o build/solutions/aoc_day_7.o -c src/solutions/aoc_day_7.c

build/solutions/aoc_day_8.o: src/solutions/aoc_day_8.c  \
	include/solutions/aoc_day_8.h \
	include/common/constants.h
	gcc ${CFLAGS} -o build/solutions/aoc_day_8.o -c src/solutions/aoc_day_8.c

build/solutions/aoc_day_9.o: src/solutions/aoc_day_9.c  \
	include/solutions/aoc_day_9.h \
	include/screen/aoc_screen.h \
	include/common/constants.h
	gcc ${CFLAGS} -o build/solutions/aoc_day_9.o -c src/solutions/aoc_day_9.c

build/solutions/aoc_day_10.o: src/solutions/aoc_day_10.c  \
	include/solutions/aoc_day_10.h \
	include/screen/aoc_screen.h \
	include/common/constants.h
	gcc ${CFLAGS} -o build/solutions/aoc_day_10.o -c src/solutions/aoc_day_10.c

build/solutions/aoc_day_11.o: src/solutions/aoc_day_11.c  \
	include/solutions/aoc_day_11.h \
	include/common/constants.h
	gcc ${CFLAGS} -o build/solutions/aoc_day_11.o -c src/solutions/aoc_day_11.c

build/solutions/aoc_day_12.o: src/solutions/aoc_day_12.c  \
	include/solutions/aoc_day_12.h \
	include/common/constants.h
	gcc ${CFLAGS} -o build/solutions/aoc_day_12.o -c src/solutions/aoc_day_12.c

build/solutions/aoc_day_13.o: src/solutions/aoc_day_13.c  \
	include/solutions/aoc_day_13.h \
	include/common/constants.h
	gcc ${CFLAGS} -o build/solutions/aoc_day_13.o -c src/solutions/aoc_day_13.c

build/solutions/aoc_day_14.o: src/solutions/aoc_day_14.c  \
	include/solutions/aoc_day_14.h \
	include/screen/aoc_screen.h \
	include/common/constants.h
	gcc ${CFLAGS} -o build/solutions/aoc_day_14.o -c src/solutions/aoc_day_14.c

bin/lib/libsolutions.a: build/solutions/aoc_solutions.o  \
	build/solutions/aoc_day_1.o \
	build/solutions/aoc_day_2.o \
	build/solutions/aoc_day_3.o \
	build/solutions/aoc_day_4.o \
	build/solutions/aoc_day_5.o \
	build/solutions/aoc_day_6.o \
	build/solutions/aoc_day_7.o \
	build/solutions/aoc_day_8.o \
	build/solutions/aoc_day_9.o \
	build/solutions/aoc_day_10.o \
	build/solutions/aoc_day_11.o \
	build/solutions/aoc_day_12.o \
	build/solutions/aoc_day_13.o \
	build/solutions/aoc_day_14.o \
	build/solutions/aoc_day_0.o
	ar rcs bin/lib/libsolutions.a $^

# The aoc executable
build/aoc.o: src/aoc.c  \
	include/solutions/aoc_solutions.h  \
	include/runner/aoc_tests.h  \
	include/runner/file_utils.h \
	include/common/constants.h
	gcc ${CFLAGS} -o build/aoc.o -c src/aoc.c

##If adding a new library, be sure to add it in the correct order in the compile line
bin/aoc: build/aoc.o  \
	bin/lib/librunner.a \
	bin/lib/libscreen.a \
	bin/lib/libsolutions.a 
	gcc ${CFLAGS} -o bin/aoc build/aoc.o -Lbin/lib -lsolutions -lscreen -lrunner


clean:
	rm -f build/runner/file_utils.o  \
	build/runner/aoc_test.o  \
	build/runner/aoc_tests.o  \
	build/screen/aoc_screen.o  \
	build/solutions/aoc_solutions.o  \
	build/solutions/aoc_day_0.o  \
	build/solutions/aoc_day_1.o  \
	build/solutions/aoc_day_2.o  \
	build/solutions/aoc_day_3.o  \
	build/solutions/aoc_day_4.o  \
	build/solutions/aoc_day_5.o \
	build/solutions/aoc_day_6.o \
	build/solutions/aoc_day_7.o \
	build/solutions/aoc_day_8.o \
	build/solutions/aoc_day_9.o \
	build/solutions/aoc_day_10.o \
	build/solutions/aoc_day_11.o \
	build/solutions/aoc_day_12.o \
	build/solutions/aoc_day_13.o \
	build/solutions/aoc_day_14.o \
	build/aoc.o  \
	bin/lib/librunner.a  \
	bin/lib/libscreen.a  \
	bin/lib/libsolutions.a  \
	bin/aoc


all: build/runner/file_utils.o  \
	build/runner/aoc_test.o  \
	build/runner/aoc_tests.o  \
	build/screen/aoc_screen.o  \
	build/solutions/aoc_solutions.o  \
	build/solutions/aoc_day_0.o  \
	build/solutions/aoc_day_1.o  \
	build/solutions/aoc_day_2.o  \
	build/solutions/aoc_day_3.o  \
	build/solutions/aoc_day_4.o  \
	build/solutions/aoc_day_5.o \
	build/solutions/aoc_day_6.o \
	build/solutions/aoc_day_7.o \
	build/solutions/aoc_day_8.o \
	build/solutions/aoc_day_9.o \
	build/solutions/aoc_day_10.o \
	build/solutions/aoc_day_11.o \
	build/solutions/aoc_day_12.o \
	build/solutions/aoc_day_13.o \
	build/solutions/aoc_day_14.o \
	build/aoc.o  \
	bin/lib/librunner.a  \
	bin/lib/libscreen.a  \
	bin/lib/libsolutions.a  \
	bin/aoc

