# L-Shape search

Searching for (planar/general) graphs without L-models (1-bend drawing with single bend type/direction)

WLOG assuming 1-connectivity (disconnected graphs can be drawn by components) and mindeg >= 2
(leaves can be always drawn).
First done for more restricted cases (less graphs, assumed to be good candidates),
but later for all graphs as well.

Large data can be found at the KAM computer kamenolom in /aux/gavento/lshape-data/

## Installation

You need the protocol-buffer compiler and libraries
(packages `protobuf-compiler` and `libprotobuf-dev` in Debian).
You also need a working C++ compiler.

For graph6 format support, you need nauty-24r2. Run `make get-nauty` to download and compile it.
This needs `wget`. Then run `make all`.

To only make the plantri version, run `make lshape-plantri`.

## Running


## Experimental results





### PLANAR GRAPHS, mindeg 3, 3 conn.

#### At 11 vertices (440564 graphs) @kamisama 4x

plantri45/plantri -m3 -c3 -p 11 -a
tot user 54m, 0.0073 s/graph, all have L-models

#### At 12 vertices (6384634 graphs) @kamenolom 16x

plantri45/plantri -m3 -c3 -p 12 -a
approx. 0.067s/g, estimated 119h total, all have L-models


### PLANAR GRAPHS, mindeg 2, 1 conn.

#### 10 vertices (2612178 graphs) @kamenolom 16x, nice 15

plantri45/plantri -m2 -c1 -p 10 -a
tot user 43m, all have L-models

#### 11 vertices (51286080 graphs), @kamenolom 16x, nice 15

plantri45/plantri -m2 -c1 -p 11 -a
tot user 40h, all have L-models


### ALL GRAPHS, mindeg 2, 2 conn

#### 8 vertices (7123 graphs) @kamenolom

./geng 8 -C -d2
4.798s, 0.00065 s/graph, all have L-models

#### 9 vertices (194066 graphs) @kamenolom

./geng 9 -C -d2
9m34.006s, 0.003 s/graph, all but: "HEhbtjK" have L-models

    Graph HEhbtjK:
    0 : 3 4 7 8;
    1 : 3 5 6 8;
    2 : 4 5 6 7;
    3 : 0 1 6 7;
    4 : 0 2 6 8;
    5 : 1 2 7 8;
    6 : 1 2 3 4;
    7 : 0 2 3 5;
    8 : 0 1 4 5;

#### 10 vertices (9743542 graphs) @kamenolom 16x, nice 15: (expect 28h)

./geng 10 -C -d2
total 22h, 0.008 s/graph, 23 graphs without L-model:

    I?qrbYyt_
    IEhbtjK}?
    IEhbtjK{_
    IEhbtjKu_
    IEhbtjK]_
    IEhbtjKx_
    IEhbtjK~?
    IEhbtjK}_
    IEhbtjK|_
    IEhbtjKmo
    IEhbtjK~_
    IEhbtjK}o
    IEhbtjK~o
    IEhbtjK~w
    I?qdUh{[o
    I?qbByyt_
    ICpelXwkg
    I?qduhk[o
    I?qduh{[o
    ICpddXit_
    ICR`upuj_
    I?rDrpulO
    IUZurzmmo


### ALL GRAPHS, mindeg 2, 1 conn

#### 8 vertices, 7442 graphs, all have L-models

./nauty24r2/geng -c -d2 8 -g

#### 9 vertices,  197772 graphs, @kamenolom 1x: 260s

./nauty24r2/geng -c -d2 9 -g
only one without L-model: HEhbtjK (see above)

#### 10 vertices, 9808209 graphs, NOT RUN

./nauty24r2/geng -c -d2 10 -u
est. 22h runtime
