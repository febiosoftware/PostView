
CC = clang

FLG = -O3 -fPIC

# Add -DFFMPEG for ffmpeg library
DEF = -DNDEBUG

GLEW_DIR = /Applications/glew-2.0.0
GLEW_INC = -I$(GLEW_DIR)/include/GL
GLEW_LIB = -L$(GLEW_DIR)/lib -lGLEW

QT_DIR = /Applications/Qt5.6.1/5.6/clang_64/lib
QT_INC = -I$(QT_DIR)/QtCore.framework/Headers -I$(QT_DIR)/QtGui.framework/Headers -I$(QT_DIR)/QtWidgets.framework/Headers
QT_FRW = -F$(QT_DIR) -framework QtCore -framework QtGui -framework QtWidgets

INC = -I$(PSTDIR) $(GLEW_INC) $(QT_INC)
LIBS = $(GLEW_LIB) $(QT_FRW) -lz -lm -lpthread
