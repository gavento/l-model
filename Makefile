all: lshape

lshape-solution.pb.cc: lshape-solution.proto
	protoc lshape-solution.proto --cpp_out=.

lshape: lshape-solution.pb.cc lshape.cpp
	gcc lshape-solution.pb.cc lshape.cpp -o lshape -I/home/gavento/local/include/ -lstdc++ -lprotobuf -L /home/gavento/local/lib/ -lpthread -g


clean:
	rm -f lshape lshape*.o lshape-solution.pb.cc lshape-solution.pb.h

.PHONY: clean all
