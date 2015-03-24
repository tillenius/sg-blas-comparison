ACML6_ROOT=${HOME}/acml6.1.0.31
ACML6_INC=-isystem $(ACML6_ROOT)/gfortran64/include
ACML6_LIBDIR=$(ACML6_ROOT)/gfortran64/lib
ACML6_LIB=-L$(ACML6_LIBDIR) -lacml -lgfortran
ACML6MP_INC=-isystem $(ACML6_ROOT)/gfortran64_mp/include
ACML6MP_LIBDIR=$(ACML6_ROOT)/gfortran64_mp/lib
ACML6MP_LIB=-L$(ACML6MP_LIBDIR) -lacml_mp -lgfortran

ACML5_ROOT=${HOME}/acml5.3.1
ACML5_INC=-isystem $(ACML5_ROOT)/open64_64_fma4/include
ACML5_LIBDIR=$(ACML5_ROOT)/open64_64_fma4/lib
ACML5_LIB=-L$(ACML5_LIBDIR) -lacml -lfortran -lffio
ACML5MP_INC=-isystem $(ACML5_ROOT)/open64_64_fma4_mp/include
ACML5MP_LIBDIR=$(ACML5_ROOT)/open64_64_fma4_mp/lib
ACML5MP_LIB=-L$(ACML5MP_LIBDIR) -lacml_mp -lfortran -lffio

all5: bin/cholesky5 bin/cholesky5-log bin/cholesky5-acml bin/cholesky5-sacml
all6: bin/cholesky6 bin/cholesky6-log bin/cholesky6-acml bin/cholesky6-sacml

bin/cholesky5: src/cholesky.cpp src/cholesky.hpp src/choleskysg.hpp Makefile
	mkdir -p bin
	$(CXX) $< -O3 -o $@ ${ACML5_INC} -I src/superglue ${ACML5_LIB} -pthread

bin/cholesky5-log: src/cholesky-log.cpp src/cholesky.hpp src/choleskysg.hpp Makefile
	mkdir -p bin
	$(CXX) $< -O3 -o $@ ${ACML5_INC} -I src/superglue ${ACML5_LIB} -pthread

bin/cholesky5-acml: src/cholesky-acml.cpp src/cholesky.hpp Makefile
	mkdir -p bin
	$(CXX) $< -O3 -openmp -o $@ ${ACML5MP_INC} ${ACML5MP_LIB}

bin/cholesky5-sacml: src/cholesky-acml.cpp src/cholesky.hpp Makefile
	mkdir -p bin
	$(CXX) $< -O3 -o $@ ${ACML5_INC} ${ACML5_LIB}

bin/cholesky6: src/cholesky.cpp src/cholesky.hpp src/choleskysg.hpp Makefile
	mkdir -p bin
	$(CXX) $< -O3 -o $@ ${ACML6_INC} -I src/superglue ${ACML6_LIB} -pthread

bin/cholesky6-log: src/cholesky-log.cpp src/cholesky.hpp src/choleskysg.hpp Makefile
	mkdir -p bin
	$(CXX) $< -O3 -o $@ ${ACML6_INC} -I src/superglue ${ACML6_LIB} -pthread

bin/cholesky6-acml: src/cholesky-acml.cpp src/cholesky.hpp Makefile
	mkdir -p bin
	$(CXX) $< -O3 -openmp -o $@ ${ACML6MP_INC} ${ACML6MP_LIB}

bin/cholesky6-sacml: src/cholesky-acml.cpp src/cholesky.hpp Makefile
	mkdir -p bin
	$(CXX) $< -O3 -o $@ ${ACML6_INC} ${ACML6_LIB}


run-chol5: bin/cholesky5
	LD_LIBRARY_PATH=${ACML5_LIBDIR} ./$<

run-chol5-log: bin/cholesky5-log
	LD_LIBRARY_PATH=${ACML5_LIBDIR} ./$<

run-chol5-acml: bin/cholesky5-acml
	LD_LIBRARY_PATH=${ACML5MP_LIBDIR} ./$<

run-chol5-sacml: bin/cholesky5-sacml
	LD_LIBRARY_PATH=${ACML5_LIBDIR} ./$<

run-chol6: bin/cholesky6
	LD_LIBRARY_PATH=${ACML6_LIBDIR} ./$<

run-chol6-log: bin/cholesky6-log
	LD_LIBRARY_PATH=${ACML6_LIBDIR} ./$<

run-chol6-acml: bin/cholesky6-acml
	LD_LIBRARY_PATH=${ACML6MP_LIBDIR} ./$<

run-chol6-sacml: bin/cholesky6-sacml
	LD_LIBRARY_PATH=${ACML6_LIBDIR} ./$<

clean:
	rm bin/*

.PHONY: all5 all6 \
  run-chol5 run-chol5-log run-chol5-acml run-chol5-sacml \
  run-chol6 run-chol6-log run-chol6-acml run-chol6-sacml clean
