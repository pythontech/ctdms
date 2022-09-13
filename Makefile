CXXFLAGS = -g -Wall

all:	ctdms_errors.h ctdms.o ctdms_errors.o

ctdms_errors.h ctdms_errors.c:	errors.def generrors
	./generrors $<

t1:	t1.o ctdms.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^
