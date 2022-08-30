CXXFLAGS = -g -Wall

t1:	t1.o ctdms.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^
