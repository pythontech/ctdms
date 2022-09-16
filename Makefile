CXXFLAGS = -g -Wall -fpic
CFLAGS = -g -Wall -fpic

all:	libctdms.so

libctdms.so:	ctdms.o ctdms_errors.o
	$(CXX) -g -z defs -shared -o $@ $^

ctdms_errors.h ctdms_errors.c:	errors.def generrors
	./generrors $<

ctdms.o ctdms_errors.o:	ctdms.h

t1:	t1.o ctdms.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^
