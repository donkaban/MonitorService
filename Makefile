SERVICE	= ./monitor 
TESTBIN = ./test

TST_SRC = test.cpp    
SRV_SRC = main.cpp packet.cpp transport.cpp
HEADERS = 

################################################################

CXX          = g++-4.9
CXX_FLAGS    = -Wall -Wextra -std=c++11  -O3  
LINK_FLAGS   = -pthread
LIBS         = 

SRV_OBJ  	 = $(SRV_SRC:.cpp=.o)
TST_OBJ      = $(TST_SRC:.cpp=.o)

all: $(SRV_SRC) $(TST_SRC) $(HEADERS) $(SERVICE) $(TESTBIN) Makefile
	rm -f $(TST_OBJ)
	rm -f $(SRV_OBJ)

$(SERVICE): $(SRV_OBJ) $(HEADERS)  Makefile
	$(CXX) $(SRV_OBJ) $(LINK_FLAGS) $(LIBS) -o $@

$(TESTBIN): $(TST_OBJ) $(HEADERS)  Makefile
	$(CXX) $(TST_OBJ) $(LINK_FLAGS) $(LIBS) -o $@	

.cpp.o: $(SRV_SRC) $(TST_SRC) $(HEADERS)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

clean:
	rm -f $(SERVICE)
	rm -f $(TESTBIN)
	rm -f $(SRV_OBJ)
	rm -f $(TST_OBJ)
	rm -f *.log



