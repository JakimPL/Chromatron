CONFIG += c++11 console
CONFIG -= app_bundle
QMAKE_CFLAGS_WARN_ON += -W4

HEADERS += \
	main.hpp \
	src/auxiliary.hpp \
	src/color.hpp \
	src/constants.hpp \
	src/drag.hpp \
	src/editor.hpp \
	src/game.hpp \
	src/gamestate.hpp \
	src/graphics.hpp \
	src/level.hpp \
	src/levelset.hpp \
	src/log.hpp \
	src/object.hpp \
	src/position.hpp \
	src/ray.hpp \
	src/stack.hpp


SOURCES += \
	main.cpp \
	src/auxiliary.cpp \
	src/color.cpp \
	src/editor.cpp \
	src/game.cpp \
	src/gamestate.cpp \
	src/graphics.cpp \
	src/level.cpp \
	src/levelset.cpp \
	src/log.cpp \
	src/object.cpp \
	src/position.cpp \
	src/ray.cpp \
	src/stack.cpp

LIBS += -L/usr/lib -lsfml-audio -lsfml-graphics -lsfml-network -lsfml-system -lsfml-window
OBJECTS_DIR = obj

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target