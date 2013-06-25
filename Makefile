all: lmodel-graph6 lmodel-plantri
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

lmodel-solution.pb.cc: lmodel-solution.proto
	protoc lmodel-solution.proto --cpp_out=.

lmodel-solution.pb.o: lmodel-solution.pb.cc
	gcc -c lmodel-solution.pb.cc $(COPTS)

# NAUTY sources needed for graph6 file reading
NAUTY_OPTS= -I${NAUTY_VER}/ ${NAUTY_VER}/gtools.o ${NAUTY_VER}/nauty1.o ${NAUTY_VER}/nautil1.o ${NAUTY_VER}/naugraph1.o -DMAXN=32 -DWITH_NAUTY

lmodel-graph6: lmodel-solution.pb.o lmodel.cpp
	@if [ ! -e ${NAUTY_VER}/nautil1.o ]; then echo "* ERROR: Run 'make get-nauty' before 'make lmodel-nauty'."; exit 1; fi
	gcc lmodel-solution.pb.o lmodel.cpp -o $@ \
	  -lstdc++ -lprotobuf -lpthread $(COPTS) \
	  $(NAUTY_OPTS) $(WRITE_OPTS) -Wno-unused-variable

lmodel-plantri: lmodel-solution.pb.o lmodel.cpp
	gcc lmodel-solution.pb.o lmodel.cpp -o $@ \
	  -lstdc++ -lprotobuf -lpthread $(COPTS) \
	  $(WRITE_OPTS)

clean:
	rm -f lmodel-graph6 lmodel-plantri lmodel*.o lmodel-solution.pb.cc lmodel-solution.pb.h

clean-nauty:
	rm -f ${NAUTY_VER}.tar.gz
	rm -rf ${NAUTY_VER}/

get-nauty:
	rm -rf ${NAUTY_VER}/
	wget -c http://cs.anu.edu.au/people/bdm/nauty/${NAUTY_VER}.tar.gz
	tar -xzvvf ${NAUTY_VER}.tar.gz
	cd ${NAUTY_VER}/ && ./configure && make
