
CC := g++ # This is the main compiler
# CC := clang --analyze # and comment out the linker last line for sanity
SRCDIR := src
BUILDDIR := build
TARGET1 := bin/chat_client
TARGET2 := bin/chat_server

SRCEXT := cpp
OBJECTS1 := $(BUILDDIR)/chat_client.o
OBJECTS2 := $(BUILDDIR)/chat_server.o
CFLAGS := -std=c++11
LIB1 := -lboost_system -lboost_thread -lboost_serialization -lpthread
LIB2 := -lboost_system -lboost_thread -lboost_serialization
INC := -I /usr/include/boost

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

all: $(TARGET1) $(TARGET2)

$(TARGET1): $(OBJECTS1)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(TARGET1) $(LIB1)"; $(CC) $^ -o $(TARGET1) $(LIB1)

$(TARGET2): $(OBJECTS2)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(TARGET2) $(LIB2)"; $(CC) $^ -o $(TARGET2) $(LIB2)

clean:
	@echo " Cleaning..."
	@echo " $(RM) -r $(BUILDDIR) $(TARGET1) $(TARGET2)"; $(RM) -r $(BUILDDIR) $(TARGET1) $(TARGET2)

