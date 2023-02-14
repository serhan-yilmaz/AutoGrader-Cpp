CXXFLAGS=-Wall -O3 -g -Wno-deprecated -Wno-sign-compare -Wno-unused-result -fopenmp -fPIC -std=c++11
LDFLAGS=-lboost_program_options -lgomp
VER=1.0
SOURCEDIR=src/
OBJECTDIR=obj/
BINARYDIR=
BIN=Autograder

.mkdir: 
	mkdir -p ${OBJECTDIR} && touch .mkdir

.PHONY: install

install: .mkdir .make

all: $(BIN)

clean:
	rm -f $(BIN) ${OBJECTDIR}*.o .make .mkdir *.tgz

${OBJECTDIR}timereporter.o: ${SOURCEDIR}TimeReporter.cpp ${SOURCEDIR}TimeReporter.h
	g++ ${CXXFLAGS} -c ${SOURCEDIR}TimeReporter.cpp -o ${OBJECTDIR}TimeReporter.o
	
${OBJECTDIR}xmlexporter.o: ${SOURCEDIR}XMLexporter.cpp ${SOURCEDIR}XMLexporter.h
	g++ ${CXXFLAGS} -c ${SOURCEDIR}XMLexporter.cpp -o ${OBJECTDIR}xmlexporter.o
	
.make: ${SOURCEDIR}main.cpp ${SOURCEDIR}Util.h ${SOURCEDIR}TestResult.h ${SOURCEDIR}XMLexporter.h ${OBJECTDIR}xmlexporter.o ${OBJECTDIR}timereporter.o ${SOURCEDIR}TimeReporter.h
	g++ ${CXXFLAGS} ${SOURCEDIR}main.cpp ${OBJECTDIR}xmlexporter.o ${OBJECTDIR}timereporter.o -o ${BINARYDIR}$(BIN) \
	&& touch .make







