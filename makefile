# directory that the makefile was called from
PROJECT_DIR_TEMP:=$(patsubst %/makefile,%, $(abspath $(lastword ${MAKEFILE_LIST})))
#! DONT EDIT ABOVE THIS

# the number is how many tasks are going to be used for compiling (should be the number of threads your cpu has for best performance)
MAKEFLAGS=-j16
# exe name
PROJECT:=main
# the directory in which all .o and .d files will be made
OBJ_O_DIR:=bin
# the include flags for compilation by default includes the project directory and include directory
INCLUDE_DIRS=/VSCodeFolder/Libraries/SFML-3.0.0/include /VSCodeFolder/Libraries/TGUI-1.7/include \VSCodeFolder\Libraries\box2c-3.0\include /Git_projects/cpp-Utilities/include /VSCodeFolder/Libraries/thread-pool-4.1.0/include /Git_projects/cpp-Networking-Library/include /VSCodeFolder/Libraries/box2c-3.0
# extra include flags
INCLUDE_FLAGS=-D SFML_STATIC
# the paths to libs for linking
LIB_DIRS=/VSCodeFolder/Libraries/SFML-3.0.0/lib /VSCodeFolder/Libraries/TGUI-1.7/lib /Git_projects/cpp-Utilities/lib \VSCodeFolder\Libraries\box2c-3.0\src /Git_projects/cpp-Networking-Library/lib
# source files directory (the project directory is automatically added)
SRC:=src
# the directory for lib files that are made with "make lib"
# this should be the full path
# this can NOT be "libs"
LIB_DIR:=lib
# the directory where all the source files that you want in the lib are
LIB_SOURCE:=src
LIB_NAME:=libgameFramework

# compiler command
CPPC:=g++
CPP_EXT:=cpp
CC:=gcc
C_EXT:=c
# linker flags for compilation
# add "-mwindows" to disable the terminal
LINKERFLAGS:=-lNetworking -lutils -ltgui-s -lsfml-graphics-s -lsfml-window-s \
			-lsfml-system-s -lsfml-audio-s -lsfml-network-s \
			-lws2_32 -lflac -lvorbisenc -lvorbisfile -lvorbis \
			-logg -lopengl32 -lwinmm -lgdi32 -lfreetype \
			-lstdc++ -lbox2d \
			# -mwindows
# flags to generate dependencies for all .o files
DEPFLAGS:=-MP -MD
DEBUG_FLAGS = -g -D _DEBUG
RELEASE_FLAGS = -O3
# just so you dont have to change all occurrences of the flags var
CURRENT_FLAGS =	${DEBUG_FLAGS}
# any compiler options
# put -Werror for warnings to be treated as errors
# use -Wextra -Wall every so often to find possible bugs
CPP_COMPILE_OPTIONS=-std=c++20 -static ${CURRENT_FLAGS}
C_COMPILE_OPTIONS=-std=c11 -static ${CURRENT_FLAGS}

#! DONT EDIT ANYTHING FROM HERE DOWN

ifeq ($(OS),Windows_NT)
	EXE:=.exe
	FIXPATH=$(subst /,\,$1)
	PROJECT_DIR:=$(call FIXPATH,${PROJECT_DIR_TEMP})
	SOURCEDIRS:=$(call FIXPATH,$(shell dir ${SRC} /s /b /ad)) $(call FIXPATH,${PROJECT_DIR}/${SRC}) $(call FIXPATH,${PROJECT_DIR})
	LIBSOURCEDIRS:=$(call FIXPATH,$(shell dir "${LIB_SOURCE}" /s /b /ad)) $(call FIXPATH,${PROJECT_DIR}/${LIB_SOURCE})
	RM=del /q /f
	RMDIR=rmdir /S /Q
	MD:=mkdir
# if you are using wsl or something which requires a more exact path add it here (this path will be added in front of all lib_dirs and include_dirs)
	DRIVEPATH:=
else
	EXE:=
	FIXPATH=$(subst \,/,$1)
	PROJECT_DIR:=$(call FIXPATH,${PROJECT_DIR_TEMP})
	SOURCEDIRS:=$(shell find "$$PWD" -type d -path "*${PROJECT_DIR}/${SRC}*") ${PROJECT_DIR}
	RM=rm -r --preserve-root
	RMDIR=rm -r --preserve-root
	MD:=mkdir -p
# if you are using wsl or something which requires a more exact path add it here (this path will be added in front of all lib_dirs and include_dirs)
	DRIVEPATH:=/mnt/v
endif

INCLUDES:=$(addprefix -I ${DRIVEPATH},$(call FIXPATH,${INCLUDE_DIRS})) ${INCLUDE_FLAGS} -I ${PROJECT_DIR} -I ${PROJECT_DIR}/include
LIBS:=$(addprefix -L ${DRIVEPATH},$(call FIXPATH,${LIB_DIRS}))

# all .cpp file paths
SRC:=$(foreach D,${SOURCEDIRS},$(wildcard ${D}/*.${CPP_EXT})) $(foreach D,${SOURCEDIRS},$(wildcard ${D}/*.${C_EXT}))
LIBSRC:=$(foreach D,${LIBSOURCEDIRS},$(wildcard ${D}/*.${CPP_EXT})) $(foreach D,${LIBSOURCEDIRS},$(wildcard ${D}/*.${C_EXT}))
# Create an object file of every cpp file
CPP_OBJECTS:=$(patsubst ${PROJECT_DIR}%,${PROJECT_DIR}/${OBJ_O_DIR}%,$(call FIXPATH,$(patsubst %.${CPP_EXT},%.o,$(filter %.${CPP_EXT},${SRC}))))
C_OBJECTS:=$(patsubst ${PROJECT_DIR}%,${PROJECT_DIR}/${OBJ_O_DIR}%,$(call FIXPATH,$(patsubst %.${C_EXT},%.o,$(filter %.${C_EXT},${SRC}))))
OBJECTS:=${CPP_OBJECTS} ${C_OBJECTS}
CPP_LIB_OBJECTS:=$(patsubst ${PROJECT_DIR}%,${PROJECT_DIR}/${OBJ_O_DIR}%,$(call FIXPATH,$(patsubst %.${CPP_EXT},%.o,$(filter %.${CPP_EXT},${LIBSRC}))))
C_LIB_OBJECTS:=$(patsubst ${PROJECT_DIR}%,${PROJECT_DIR}/${OBJ_O_DIR}%,$(call FIXPATH,$(patsubst %.${C_EXT},%.o,$(filter %.${C_EXT},${LIBSRC}))))
LIB_OBJECTS:=${CPP_LIB_OBJECTS} ${C_LIB_OBJECTS}
# Creating dependency files
DEPFILES=$(patsubst %.o,%.d,${OBJECTS})
# All bin directories
BIN_DIRS=$(foreach dir,$(call FIXPATH,$(SOURCEDIRS)),$(patsubst $(call FIXPATH,$(PROJECT_DIR)%),$(call FIXPATH,$(PROJECT_DIR)/$(OBJ_O_DIR)%),$(dir)))

# so there is no file that gets mistaked with the tasks listed
.PHONY = all info clean libs libs-r libs-d run clean_object clean_exe clean_libs
.NOTPARALLEL: libs

all: ${BIN_DIRS} ${PROJECT}

${PROJECT}: ${OBJECTS}
	${CPPC} ${CPP_COMPILE_OPTIONS} ${INCLUDES} -o ${@} ${^} ${LIBS} ${LINKERFLAGS}

${PROJECT_DIR}/${OBJ_O_DIR}%.o:${PROJECT_DIR}%.${CPP_EXT}
	${CPPC} ${CPP_COMPILE_OPTIONS} ${INCLUDES} ${DEPFLAGS} -c -o ${@} ${<}

${PROJECT_DIR}/${OBJ_O_DIR}%.o:${PROJECT_DIR}%.${C_EXT}
	${CC} ${C_COMPILE_OPTIONS} ${INCLUDES} ${DEPFLAGS} -c -o ${@} ${<}

libs: 
	make libs-r 
	make libs-d

# build the lib with the same compile options
# do this as a clean build unless sure that previous builds where release builds
libs-r: CURRENT_FLAGS = ${RELEASE_FLAGS}
libs-r: clean_object ${BIN_DIRS} ${LIB_DIR} ${LIB_OBJECTS}
	ar rcs $(call FIXPATH,${PROJECT_DIR}/${LIB_DIR}/${LIB_NAME}.a) ${LIB_OBJECTS}
	@echo Release Libs created

# do this as a clean build unless sure that previous builds where release builds
libs-d: CURRENT_FLAGS = ${DEBUG_FLAGS}
libs-d: clean_object ${BIN_DIRS} ${LIB_DIR} ${LIB_OBJECTS}
	ar rcs $(call FIXPATH,${PROJECT_DIR}/${LIB_DIR}/${LIB_NAME}-d.a) ${LIB_OBJECTS}
	@echo Debug Libs created

# include the dependencies
-include ${DEPFILES}

# on linux it confuses the create directory target with dependency files
$(call FIXPATH,${PROJECT_DIR}/${OBJ_O_DIR})%.d:
	@echo -n ""

$(call FIXPATH,${PROJECT_DIR}/${OBJ_O_DIR})%:
	$(call makeDir,$(call FIXPATH,${@}))

${LIB_DIR}:
	$(call makeDir,$(call FIXPATH,${PROJECT_DIR}/${LIB_DIR}))

clean: clean_object clean_exe clean_libs	

clean_object:
	$(shell ${RMDIR} ${OBJ_O_DIR})

clean_exe:
	$(shell ${RM} ${PROJECT}${EXE})

clean_libs:
	$(shell ${RMDIR} $(call FIXPATH,${PROJECT_DIR}/${LIB_DIR}))

# builds and runs the program
run: ${BIN_DIRS} ${PROJECT}
	./${PROJECT}${EXE}

# makes a dir for the given path
makeDir=$(shell ${MD} $(call FIXPATH,$1))
# make bin directories 
# makeBinDir=$(foreach dir,${BIN_DIRS},$(call makeDir,${dir}))

info:
	@echo Project Directory: ${PROJECT_DIR}
	@echo Source Directories: ${SOURCEDIRS} 
	@echo Lib Source Directories: ${LIBSOURCEDIRS} 
	@echo Object Files: ${OBJECTS}
	@echo Dependencies: ${DEPFILES}
