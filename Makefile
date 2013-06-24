all: lshape-nauty lshape-plantri
.PHONY: clean all get-nauty

NAUTY_VER=nauty24r2

COPTS=-O3 -Wall -I/home/gavento/local/include/ -L/home/gavento/local/lib/

lshape-solution.pb.cc: lshape-solution.proto
	protoc lshape-solution.proto --cpp_out=.

lshape-solution.pb.o: lshape-solution.pb.cc
	gcc -c lshape-solution.pb.cc $(COPTS)

# NAUTY sources needed for graph6 file reading
NAUTY_OPTS= -I${NAUTY_VER}/ ${NAUTY_VER}/gtools.o ${NAUTY_VER}/nauty1.o ${NAUTY_VER}/nautil1.o ${NAUTY_VER}/naugraph1.o -DMAXN=32 -DWITH_NAUTY

lshape-nauty: lshape-solution.pb.o lshape.cpp
	if [ ! -e ${NAUTY_VER}/nautil1.o ]; then echo "* ERROR: Run 'make get-nauty' before 'make lshape-nauty'."; exit 1; fi
	gcc lshape-solution.pb.o lshape.cpp -o $@ \
	  -lstdc++ -lprotobuf -lpthread $(COPTS) \
	  $(NAUTY_OPTS)

lshape-plantri: lshape-solution.pb.o lshape.cpp
	gcc lshape-solution.pb.o lshape.cpp -o $@ \
	  -lstdc++ -lprotobuf -lpthread $(COPTS)

clean:
	rm -f lshape-nauty lshape-plantri lshape*.o lshape-solution.pb.cc lshape-solution.pb.h
	rm -f ${NAUTY_VER}.tar.gz
	rm -rf ${NAUTY_VER}/

get-nauty:
	rm -rf ${NAUTY_VER}/
	wget -c http://cs.anu.edu.au/people/bdm/nauty/${NAUTY_VER}.tar.gz
	tar -xzvvf ${NAUTY_VER}.tar.gz
	cd ${NAUTY_VER}/ && ./configure && make
