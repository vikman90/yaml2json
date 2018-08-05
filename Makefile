TARGET = bin/yaml2json
CFLAGS = -Wall -Wextra -pipe -O2 -Iinclude -Iexternal/libyaml/include -Iexternal/cJSON
LDFLAGS = -Lexternal/libyaml/src/.libs -Lexternal/cJSON
LDLIBS = -lyaml -lcjson
DEPS = external/cJSON external/libyaml

HEADERS = $(wildcard include/*.h)
SOURCES = $(wildcard src/*.c)
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean deps clean-deps

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(OBJECTS): $(HEADERS)

deps: $(DEPS)

external/cJSON:
	git clone https://github.com/DaveGamble/cJSON.git $@
	$(MAKE) -C $@ static

external/libyaml:
	git clone https://github.com/yaml/libyaml.git $@
	cd $@ && ./bootstrap && ./configure --enable-shared=no
	$(MAKE) -C $@

clean-deps:
	$(RM) -r $(DEPS)

clean:
	$(RM) $(TARGET) $(OBJECTS)
