CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
QMAKE_CFLAGS_WARN_ON += -W4

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        src/auxiliary.cpp \
        src/color.cpp \
        src/editor.cpp \
        src/game.cpp \
        src/gamestate.cpp \
        src/graphics.cpp \
        src/levelset.cpp \
        src/log.cpp \
        src/object.cpp \
        src/position.cpp \
        src/raygen.cpp \
        src/stack.cpp

LIBS += -L/usr/lib -lsfml-audio -lsfml-graphics -lsfml-network -lsfml-system -lsfml-window
OBJECTS_DIR = obj

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
	main.hpp \
	src/auxiliary.hpp \
	src/color.hpp \
	src/constants.hpp \
	src/drag.hpp \
	src/game.hpp \
	src/gamestate.hpp \
	src/graphics.hpp \
	src/log.hpp \
	src/object.hpp \
	src/position.hpp \
	src/raygen.hpp \
	src/stack.hpp
