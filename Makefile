CC ?= cc
CFLAGS ?= -std=c11 -O2 -Wall -Wextra

UNAME_S := $(shell uname -s)

SDL_CFLAGS := $(shell pkg-config --cflags sdl2 SDL2_ttf)
SDL_LIBS := $(shell pkg-config --libs sdl2 SDL2_ttf)

YAML_CFLAGS := $(shell pkg-config --cflags yaml-0.1 2>/dev/null)
YAML_LIBS := $(shell pkg-config --libs yaml-0.1 2>/dev/null)
ifeq ($(strip $(YAML_LIBS)),)
  YAML_LIBS := -lyaml
endif

ifeq ($(UNAME_S),Darwin)
  GL_LIBS := -framework OpenGL
else
  GL_LIBS := -lGL -lm
endif

BIN := pzdc_dungeon_2_gl

all: $(BIN)

$(BIN): main.c
	$(CC) $(CFLAGS) $(SDL_CFLAGS) $(YAML_CFLAGS) -o $@ $< $(SDL_LIBS) $(YAML_LIBS) $(GL_LIBS)

clean:
	rm -f $(BIN)
