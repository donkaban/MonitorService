TARGET	= ./monitor

SOURCES = main.cpp
HEADERS = logger.h

################################################################

CXX          = g++
CXX_FLAGS    = -c -Wall -Wextra -std=c++11  -O3 
LINK_FLAGS   = -pthread
LIBS         = 

OBJECTS=$(SOURCES:.cpp=.o)

all: $(SOURCES) $(HEADERS) $(TARGET) Makefile
	rm -f $(OBJECTS)

$(TARGET): $(OBJECTS) $(HEADERS)  Makefile
	$(CXX) $(OBJECTS) $(LINK_FLAGS) $(LIBS) -o $@

.cpp.o: $(SOURCES)  $(HEADERS)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET)
	rm -f $(OBJECTS)



