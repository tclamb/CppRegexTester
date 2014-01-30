OS_NAME=$(shell uname -s)
ifeq (${OS_NAME},Darwin)
	RTLIB=
	CXXFLAGS = -framework CoreFoundation -framework CoreServices -std=c++11 -stdlib=libc++ -g -O0 -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
else
	RTLIB=-lrt
	CXXFLAGS = -std=c++11 -g -O0 -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
endif

NODE_NATIVE_PATH=vendor/node.native
HTTP_PARSER_PATH=$(NODE_NATIVE_PATH)/http-parser
LIBUV_PATH=$(NODE_NATIVE_PATH)/libuv
LIBUV_NAME=libuv.a
NODE_NATIVE_INCLUDES=-I$(LIBUV_PATH)/include -I$(HTTP_PARSER_PATH) -I$(NODE_NATIVE_PATH) 
NODE_NATIVE_LIBS=$(HTTP_PARSER_PATH)/http_parser.o $(LIBUV_PATH)/$(LIBUV_NAME) $(RTLIB) -lm -lpthread

INCLUDES=$(NODE_NATIVE_INCLUDES)
LIBS=$(NODE_NATIVE_LIBS)

SOURCES=regex_tester.cpp
OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=regex_tester

build: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) node.native
	clang++ $(CXXFLAGS) -o $@ $(OBJECTS) $(LIBS)

%.o: %.cpp
	clang++ $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

node.native:
	$(MAKE) -C $(LIBUV_PATH)
	$(MAKE) -C $(HTTP_PARSER_PATH) http_parser.o

clean:
	$(MAKE) -C $(LIBUV_PATH) clean
	$(MAKE) -C $(HTTP_PARSER_PATH) clean
	rm -f $(LIBUV_PATH)/$(LIBUV_NAME)
	rm -f $(HTTP_PARSER_PATH)/http_parser.o
	rm -f $(OBJECTS) $(EXECUTABLE)
