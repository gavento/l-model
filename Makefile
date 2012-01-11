all: lshape

lshape-solution.pb.cc: lshape-solution.proto
	protoc lshape-solution.proto --cpp_out=.

lshape-solution.pb.o: lshape-solution.pb.cc
	gcc -c lshape-solution.pb.cc -O2 -I/home/gavento/local/include/ -g

lshape: lshape-solution.pb.o lshape.cpp
	gcc lshape-solution.pb.o lshape.cpp -o lshape \
	  -O2 -I/home/gavento/local/include/ -g \
	  -lstdc++ -lprotobuf -L /home/gavento/local/lib/ -lpthread

clean:
	rm -f lshape lshape*.o lshape-solution.pb.cc lshape-solution.pb.h

.PHONY: clean all
