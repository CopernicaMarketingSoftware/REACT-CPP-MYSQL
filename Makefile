PREFIX                  = /usr
INCLUDE_DIR             = ${PREFIX}/include/reactcpp
LIBRARY_DIR             = ${PREFIX}/lib

all:
		$(MAKE) -C src all

static:
		$(MAKE) -C src static

shared:
		$(MAKE) -C src shared

clean:
		$(MAKE) -C src clean

install:
		mkdir -p ${INCLUDE_DIR}/mysql
		mkdir -p ${LIBRARY_DIR}
		cp -f mysql.h ${INCLUDE_DIR}
		cp -fr include/* ${INCLUDE_DIR}/mysql
		cp -f src/libreactcpp-mysql.so ${LIBRARY_DIR}
		cp -f src/libreactcpp-mysql.a ${LIBRARY_DIR}
