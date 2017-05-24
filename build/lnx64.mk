
CC = icpc

FLG = -O3 -std=c++11 -fPIC

# Add -DFFMPEG for ffmpeg library
DEF = -DLINUX -DTETLIBRARY -DNDEBUG -D__STDC_CONSTANT_MACROS

#FLTK = $(FLTK_DIR)
#FLTK_INC = -I$(FLTK)
#FLTK_LIB = -L$(FLTK)/$(PLAT) -lfltk_gl -lfltk_images -lfltk -lfltk_jpeg -lfltk_png
#FLTK_LIB = -L$(FLTK)/$(PLAT) -lfltk_z

GLEW_DIR = /home/sci/rawlins/Applications/glew-2.0.0
GLEW_INC = -I$(GLEW_DIR)/include/GL
GLEW_LIB = -L$(GLEW_DIR)/lib -lGLEW

QT_DIR = /home/sci/rawlins/Applications/Qt/Qt5.8/5.8/gcc_64
QT_INC = -I$(QT_DIR)/include -I$(QT_DIR)/include/QtGui -I$(QT_DIR)/include/QtWidgets
QT_LIB = -Wl,-rpath-link=$(QT_DIR)/lib -lQt5Core -lQt5Gui -lQt5Widgets

#LINKX11 = -L/usr/lib64 -lGLU -lGL -lXext -lX11 -lXft -lfontconfig -lXinerama -lXfixes -lXcursor -lXrender -ldl
LINKX11 = -L/usr/lib64 -lGLU -lGL

FLEX_INC = -I$(FLEX_DIR)/Flex
FLEX_LIB = -L$(FLEX_DIR)/build/lib -lflex_$(PLAT)

TETLIB = -L$(TETGEN_DIR)/lib -ltet64
TETINC = -I$(TETGEN_DIR)

FFMPEG_INC = -I$(FFMPEG_DIR)/include
FFMPEG_LIB = -L$(FFMPEG_DIR)/lib64 -lavcodec -lavdevice -lavfilter -lswresample -lavformat -lavutil -lswscale -lbz2


INC = -I$(PSTDIR) $(TETINC) $(GLEW_INC) $(QT_INC)
LIBS = $(GLEW_LIB) $(QT_LIB) $(LINKX11) $(TETLIB) -lz -lm -lpthread -Wl,-rpath='$$ORIGIN/../lib'
