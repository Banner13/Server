CXXFLAGS += -g -Wall -fPIC
OUTPUT = build

SRC_INC = \
	abs_server.h \
	tcp_server.h \
	test_server.h

SRC_LIB = \
	tcp_server.cpp \
	test_server.cpp

SRC_DEMO = \
	demo.cpp 

OBJ_LIB := $(addprefix $(OUTPUT)/, $(SRC_LIB:.cpp=.o))
OBJ_DEMO := $(addprefix $(OUTPUT)/, $(SRC_DEMO:.cpp=.o))

STATIC_LIB := $(OUTPUT)/libserver.a
DYNAMIC_LIB := $(OUTPUT)/libserver.so
DEMO := $(OUTPUT)/demo

.PHONY: all clean

all: build lib demo
	@echo "Build all done!"

lib: $(STATIC_LIB) $(DYNAMIC_LIB)
	@echo "Lib build OK!"

# 生成静态库
$(STATIC_LIB): $(OBJ_LIB)
	ar rcs $@ $^

# 生成动态库
$(DYNAMIC_LIB): $(OBJ_LIB)
	$(CXX) -shared $^ -o $@

# 编译源文件生成目标文件
$(OUTPUT)/%.o: %.cpp $(SRC_INC) | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build:
	mkdir -p $(OUTPUT)

demo: $(OBJ_DEMO) lib
	$(CXX) $(CXXFLAGS) $(OBJ_DEMO) -L$(OUTPUT) -lserver -lpthread -o $(OUTPUT)/$@

clean:
	rm -rf $(OUTPUT)
