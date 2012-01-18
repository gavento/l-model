all: lshape

COPTS=-O3 -Wall -I/home/gavento/local/include/ -L/home/gavento/local/lib/

lshape-solution.pb.cc: lshape-solution.proto
	protoc lshape-solution.proto --cpp_out=.

lshape-solution.pb.o: lshape-solution.pb.cc
	gcc -c lshape-solution.pb.cc $(COPTS)

# For graph6 parsing
NAUTY_STUFF= -Inauty24r2/ nauty24r2/gtools.o nauty24r2/nauty1.o nauty24r2/nautil1.o nauty24r2/naugraph1.o -DMAXN=32

# If WITH_NAUTY is not defined, can be linked without $(NAUTY_STUFF)
lshape: lshape-solution.pb.o lshape.cpp
	gcc lshape-solution.pb.o lshape.cpp -o lshape \
	  -lstdc++ -lprotobuf -lpthread $(COPTS) \
	  $(NAUTY_STUFF)

clean:
	rm -f lshape lshape*.o lshape-solution.pb.cc lshape-solution.pb.h

.PHONY: clean all
