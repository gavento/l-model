all: lshape

COPTS=-O3 -Wall -I/home/gavento/local/include/ -L/home/gavento/local/lib/

lshape-solution.pb.cc: lshape-solution.proto
	protoc lshape-solution.proto --cpp_out=.

lshape-solution.pb.o: lshape-solution.pb.cc
	gcc -c lshape-solution.pb.cc $(COPTS)

lshape: lshape-solution.pb.o lshape.cpp
	gcc lshape-solution.pb.o lshape.cpp -o lshape \
	  -lstdc++ -lprotobuf -lpthread $(COPTS)

clean:
	rm -f lshape lshape*.o lshape-solution.pb.cc lshape-solution.pb.h

.PHONY: clean all
