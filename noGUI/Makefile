src = $(wildcard src/*.cc)
obj = $(src: .c=.o)


CXXFLAGS = -std=c++11 -m64 -Wall -s
LDFLAGS = -lngspice -lpthread -ldl

simulator: $(obj)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)