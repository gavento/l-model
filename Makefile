all: lshape-graph6 lshape-plantri
.PHONY: clean all get-nauty

# customise output, see README.md
WRITE_OPTS=
WRITE_OPTS+=-DWRITE_GRAPH
#WRITE_OPTS+=-DWRITE_PROTOBUF
#WRITE_OPTS+=-DWRITE_DEPTHS
#WRITE_OPTS+=-DWRITE_SOLUTION
WRITE_OPTS+=-DWRITE_GNUPLOT


NAUTY_VER=nauty24r2

COPTS=-O3 -Wall

lshape-solution.pb.cc: lshape-solution.proto
	protoc lshape-solution.proto --cpp_out=.

lshape-solution.pb.o: lshape-solution.pb.cc
	gcc -c lshape-solution.pb.cc $(COPTS)

# NAUTY sources needed for graph6 file reading
NAUTY_OPTS= -I${NAUTY_VER}/ ${NAUTY_VER}/gtools.o ${NAUTY_VER}/nauty1.o ${NAUTY_VER}/nautil1.o ${NAUTY_VER}/naugraph1.o -DMAXN=32 -DWITH_NAUTY

lshape-graph6: lshape-solution.pb.o lshape.cpp
	@if [ ! -e ${NAUTY_VER}/nautil1.o ]; then echo "* ERROR: Run 'make get-nauty' before 'make lshape-nauty'."; exit 1; fi
	gcc lshape-solution.pb.o lshape.cpp -o $@ \
	  -lstdc++ -lprotobuf -lpthread $(COPTS) \
	  $(NAUTY_OPTS) $(WRITE_OPTS) -Wno-unused-variable

lshape-plantri: lshape-solution.pb.o lshape.cpp
	gcc lshape-solution.pb.o lshape.cpp -o $@ \
	  -lstdc++ -lprotobuf -lpthread $(COPTS) \
	  $(WRITE_OPTS)

clean:
	rm -f lshape-graph6 lshape-plantri lshape*.o lshape-solution.pb.cc lshape-solution.pb.h

clean-nauty:
	rm -f ${NAUTY_VER}.tar.gz
	rm -rf ${NAUTY_VER}/

get-nauty:
	rm -rf ${NAUTY_VER}/
	wget -c http://cs.anu.edu.au/people/bdm/nauty/${NAUTY_VER}.tar.gz
	tar -xzvvf ${NAUTY_VER}.tar.gz
	cd ${NAUTY_VER}/ && ./configure && make
