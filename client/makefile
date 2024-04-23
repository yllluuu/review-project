PREFIX ?=`pwd`
LIBNAME=mycli
APP_NAME=client
DATABASE_NAME=cli_dtbase.db
CFLAGS +=-I ./include/ 
LDFLAGS +=-L ./lib/ 
ARGUMENTS=-i 127.0.0.1 -p 78965 -t 3 -d

all:install
	@gcc main.c -g -o ${APP_NAME} ${CFLAGS} -l${LIBNAME} ${LDFLAGS} -lsqlite3 -I /home/iot24/yanglu/sqlite/include/ -L /home/iot24/yanglu/sqlite/lib/ 

shared_lib:
	@gcc -shared -fPIC -o lib${LIBNAME}.so ./src/*.c -I ./include/ -I /home/iot24/yanglu/sqlite/include/ 

static_lib:
	@gcc -c ./src/*.c -I ./include/ -I /home/iot24/yanglu/sqlite/include/
	@ar -rcs lib${LIBNAME}.a *.o 

clean:
	@rm -f *.o

distclean:
	@rm -f ${PREFIX}/lib/*.so
	@rm -f ${PREFIX}/lib/*.a
	@rm ${DATABASE_NAME}
	@rm ${APP_NAME}

install:shared_lib static_lib
	@mkdir -p ${PREFIX}/lib
	@mv lib*.* ${PREFIX}/lib

uninstall:
	@rm -f ${PREFIX}/lib/lib${LIBNAME}.*

run:
	@export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${PREFIX}/lib&&./${APP_NAME} ${ARGUMENTS}
