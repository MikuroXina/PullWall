COMPILER  = clang++
CFLAGS    = -std=c++14 -g -MMD -MP -Wall -Wextra -Winit-self -Wno-missing-field-initializers -v
ifeq "$(shell getconf LONG_BIT)" "64"
  LDFLAGS = -v
else
  LDFLAGS = -v
endif
LIBS      = /usr/local/Cellar/ftgl/2.1.3-rc5/lib/libftgl.a /usr/local/Cellar/freetype/2.8/lib/libfreetype.a  /usr/local/lib/libSDL2.dylib /System/Library/Frameworks/OpenGL.framework/Libraries/libGL.dylib
INCLUDE   = -I./include -framework SDL2 -I/Library/Frameworks/SDL2.framework/Headers/ -I/usr/local/include/freetype2/
TARGET    = ./bin.out
SRCDIR    = ./
ifeq "$(strip $(SRCDIR))" ""
  SRCDIR  = .
endif
SOURCES   = $(wildcard $(SRCDIR)/*.cpp)
OBJDIR    = ./obj
ifeq "$(strip $(OBJDIR))" ""
  OBJDIR  = .
endif
OBJECTS   = $(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.cpp=.o)))
DEPENDS   = $(OBJECTS:.o=.d)

$(TARGET): $(OBJECTS) $(LIBS)
	$(COMPILER) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	-mkdir -p $(OBJDIR)
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

all: clean $(TARGET)

clean:
	-rm -f $(OBJECTS) $(DEPENDS) $(TARGET)

-include $(DEPENDS)
