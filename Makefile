OS_NAME=$(shell uname -s)
ifeq (${OS_NAME},Darwin)
	PLATFORM_LIBS=-framework CoreFoundation -framework CoreServices 
	CXXFLAGS = -std=c++11 -stdlib=libc++ -g -O0 -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
else
	PLATFORM_LIBS=-lrt
	CXXFLAGS = -std=c++11 -g -O0 -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
endif

NODE_NATIVE_PATH=vendor/node.native
HTTP_PARSER_PATH=$(NODE_NATIVE_PATH)/http-parser
LIBUV_PATH=$(NODE_NATIVE_PATH)/libuv
LIBUV_NAME=libuv.a
NODE_NATIVE_INCLUDES=-I$(LIBUV_PATH)/include -I$(HTTP_PARSER_PATH) -I$(NODE_NATIVE_PATH) 
NODE_NATIVE_LIBS=$(HTTP_PARSER_PATH)/http_parser.o $(LIBUV_PATH)/$(LIBUV_NAME) $(RTLIB) -lm -lpthread

JSON11_PATH=vendor/json11
JSON11_INCLUDES=-I$(JSON11_PATH)
JSON11_LIBS=$(JSON11_PATH)/json11.o

INCLUDES=$(NODE_NATIVE_INCLUDES) $(JSON11_INCLUDES)
LIBS=$(NODE_NATIVE_LIBS) $(PLATFORM_LIBS) $(JSON11_LIBS)

SOURCES=regex_tester.cpp
OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=regex_tester

build: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) node.native json11
	clang++ $(CXXFLAGS) -o $@ $(OBJECTS) $(LIBS)

%.o: %.cpp
	clang++ $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

json11: $(JSON11_PATH)/json11.o

node.native:
	$(MAKE) -C $(LIBUV_PATH) >/dev/null 2>/dev/null
	$(MAKE) -C $(HTTP_PARSER_PATH) http_parser.o >/dev/null 2>/dev/null

clean:
	$(MAKE) -C $(LIBUV_PATH) clean
	$(MAKE) -C $(HTTP_PARSER_PATH) clean
	rm -f $(LIBUV_PATH)/$(LIBUV_NAME)
	rm -f $(HTTP_PARSER_PATH)/http_parser.o
	rm -f $(JSON11_PATH)/json11.o
	rm -f $(OBJECTS) $(EXECUTABLE)
