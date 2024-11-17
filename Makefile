
CXXFLAGS += -g -lpthread -Wall -fPIC 

SRC_LIB = \
	tcp_server.cpp

SRC_DEMO = \
	demo.cpp 

OBJ_LIB := $(SRC_LIB:.cpp=.o)
OBJ_DEMO := $(SRC_DEMO:.cpp=.o)

STATIC_LIB := libserver.a
DYNAMIC_LIB := libserver.so
DEMO := demo

.PHONY: all clean

all: lib demo
	echo "Build all done!"

lib: $(STATIC_LIB) $(DYNAMIC_LIB)
	echo "Lib build OK!"

# 生成静态库
$(STATIC_LIB): $(OBJ_LIB)
	ar rcs $@ $^

# 生成动态库
$(DYNAMIC_LIB): $(OBJ_LIB)
	$(CXX) -shared -o $@ $^

demo: $(OBJ_DEMO) lib
	$(CXX) $(OBJ_DEMO) -L. -lserver -lpthread -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o *.so *.a demo

#$CXX -g++ main.cpp tcp_server.cpp -lpthread -o test