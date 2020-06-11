######################################################################
# Automatically generated by qmake (3.1) Tue Jan 23 16:20:13 2018
######################################################################

TEMPLATE = app
DESTDIR = ../build/bin
TARGET = PostView2
CONFIG += debug c++14 warn_off 
CONFIG += qt opengl qtwidgets qtcharts
QMAKE_CXX = g++
QMAKE_CXXFLAGS += -DLINUX -DTETLIBRARY -DNDEBUG -DFFMPEG
QMAKE_CXXFLAGS += -MMD -fPIC
QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_LFLAGS_RELEASE += -O1
QMAKE_RPATHDIR += $ORIGIN/../lib
#MOC_DIR = .
INCLUDEPATH += ../
INCLUDEPATH += /home/mherron/Projects/FEBioStudio/
#INCLUDEPATH += /home/mherron/Resources/glew-2.0.0/include/GL
INCLUDEPATH += /usr/include/ffmpeg
QT += gui widgets charts

#LIBS += /home/sci/mherron/Resources/glew-2.0.0/lib/libGLEW.a
LIBS += -L/home/mherron/Resources/tetgen1.5.1/build -ltet
LIBS += -L/home/mherron/Projects/FEBioStudio/build/lib
LIBS += -Wl,--start-group 
LIBS += -lglwlib -lpostgl -lxpltlib -lpostlib -lcuilib -limagelib -lfscore -lmeshtools -lgeomlib -lmeshlib -lxml -lgllib -lmathlib
LIBS += -Wl,--end-group
LIBS += -L/usr/lib64 -lGLU -lGL
LIBS += -lavformat -lavcodec -lavresample -lavutil -lswresample -lswscale
LIBS += -lz -lm
#LIBS += -lpng16 -licui18n -licuuc -licudata -lpcre16 -lglew2

RESOURCES = ../postview.qrc

# Input
HEADERS += *.h
SOURCES = $$files(*.cpp)
SOURCES -= $$files(moc_*.cpp)
SOURCES -= $$files(qrc_*)
