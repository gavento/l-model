all: lshape

COPTS=-O0 -g -Wall -I/home/gavento/local/include/ -L/home/gavento/local/lib/ -Inauty24r2/

lshape-solution.pb.cc: lshape-solution.proto
	protoc lshape-solution.proto --cpp_out=.

lshape-solution.pb.o: lshape-solution.pb.cc
	gcc -c lshape-solution.pb.cc $(COPTS)

lshape: lshape-solution.pb.o lshape.cpp
	gcc lshape-solution.pb.o lshape.cpp -o lshape \
	  nauty24r2/gtools.o nauty24r2/nauty1.o nauty24r2/nautil1.o nauty24r2/naugraph1.o -DMAXN=32 \
	  -lstdc++ -lprotobuf -lpthread $(COPTS)

clean:
	rm -f lshape lshape*.o lshape-solution.pb.cc lshape-solution.pb.h

.PHONY: clean all
