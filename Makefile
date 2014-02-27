PREFIX                  = /usr
INCLUDE_DIR             = ${PREFIX}/include
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
		mkdir -p ${INCLUDE_DIR}/mysqlcpp
		mkdir -p ${LIBRARY_DIR}
		cp -f mysqlcpp.h ${INCLUDE_DIR}
		cp -fr include/* ${INCLUDE_DIR}/mysqlcpp
		cp -f src/libmysqlcpp.so ${LIBRARY_DIR}
		cp -f src/libmysqlcpp.a ${LIBRARY_DIR}
