CC=gcc -g

MPICC=mpicc -g

UTIL_OBJ=util.o

SERIAL_SORT_OBJ=${UTIL_OBJ} serial_sort.o

MAIN_SERIAL_OBJ=main_serial.o

MAIN_PTHREAD_OBJ=main_pthread.o

CREATE_DATA_OBJ=create_data.o

MAIN_MPI_FILE=main_mpi

OUTPUT_FOLDER=./output

all: main_mpi main_serial main_pthread create_data

main_serial: ${MAIN_SERIAL_OBJ} ${SERIAL_SORT_OBJ}
	${CC} $^ -o ${OUTPUT_FOLDER}/$@

main_mpi: ${MAIN_MPI_FILE}.o ${UTIL_OBJ}
	${MPICC} $^ -o ${OUTPUT_FOLDER}/$@

main_pthread: ${MAIN_PTHREAD_OBJ} ${UTIL_OBJ}
	${CC} $^ -o ${OUTPUT_FOLDER}/$@ -lpthread

create_data: ${CREATE_DATA_OBJ} ${UTIL_OBJ}
	${CC} $^ -o ${OUTPUT_FOLDER}/$@

%.o: %.c
	${CC} $^ -c

${MAIN_MPI_FILE}.o: ${MAIN_MPI_FILE}.c
	${MPICC} $^ -c

.PHONY clean:
	rm *.o
	rm ${OUTPUT_FOLDER}/*
	