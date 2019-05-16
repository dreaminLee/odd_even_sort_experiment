CC=gcc -g

MPICC=mpicc -g

SRC_FOLDER=./src

OUTPUT_FOLDER=./output

UTIL_OBJ=${SRC_FOLDER}/util/util.o

SERIAL_SORT_OBJ=${UTIL_OBJ} ${SRC_FOLDER}/serial/serial_sort.o

MAIN_SERIAL_OBJ=${SRC_FOLDER}/serial/main_serial.o

MAIN_PTHREAD_OBJ=${SRC_FOLDER}/pthread/main_pthread.o

PTHREAD_SORT_OBJ=${SRC_FOLDER}/pthread/pthread_sort.o

MAIN_OPENMP_FILE=${SRC_FOLDER}/openmp/main_openmp

CREATE_DATA_OBJ=${SRC_FOLDER}/util/create_data.o

MAIN_MPI_FILE=${SRC_FOLDER}/mpi/main_mpi

all: main_mpi main_serial main_pthread main_openmp create_data

main_serial: ${MAIN_SERIAL_OBJ} ${SERIAL_SORT_OBJ}
	${CC} $^ -o ${OUTPUT_FOLDER}/$@

main_mpi: ${MAIN_MPI_FILE}.o ${UTIL_OBJ}
	${MPICC} $^ -o ${OUTPUT_FOLDER}/$@

main_pthread: ${MAIN_PTHREAD_OBJ} ${UTIL_OBJ} ${PTHREAD_SORT_OBJ}
	${CC} $^ -o ${OUTPUT_FOLDER}/$@ -lpthread

main_openmp: ${MAIN_OPENMP_FILE}.o ${UTIL_OBJ}
	${CC} $^ -fopenmp -o ${OUTPUT_FOLDER}/$@ 

create_data: ${CREATE_DATA_OBJ} ${UTIL_OBJ}
	${CC} $^ -o ${OUTPUT_FOLDER}/$@

%.o: %.c
	${CC} $^ -c -o $@

${MAIN_MPI_FILE}.o: ${MAIN_MPI_FILE}.c
	${MPICC} $^ -c -o $@

${MAIN_OPENMP_FILE}.o: ${MAIN_OPENMP_FILE}.c
	${CC} $^ -fopenmp -c -o $@

.PHONY clean:
	rm ${SRC_FOLDER}/*/*.o
	rm ${OUTPUT_FOLDER}/*
	