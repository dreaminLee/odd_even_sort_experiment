import os
import decimal
import numpy
import sys

'''
    usage: python3 get_result.py <sort program you want to run, see below for details> 
                                 <num_of_exec> 
                                 <init_data_size> <max_data_size> <data_size increase step>
    |serial_odd_even_sort, qsort, mpi, pthreads, openmp|
'''

if len(sys.argv) != 6 or len(sys.argv[1]) != 5:
    print("usage: python3 get_result.py <sort program you want to run, see below for details> <num_of_exec> <init_data_size> <max_data_size> <data_size increase step>")
    exit(0)

num_of_exec = int(sys.argv[2])
data_size = int(sys.argv[3])
data_incr_step = int(sys.argv[5])
max_data_size = int(sys.argv[4])
run_serial_odd_even = sys.argv[1][0] == '1'
run_serial_qsort = sys.argv[1][1] == '1'
run_mpi = sys.argv[1][2] == '1'
run_pthreads = sys.argv[1][3] == '1'
run_openmp = sys.argv[1][4] == '1'

output_folder = "../output/"
exec_create_data = "create_data"
exec_serial = "main_serial"
exec_mpi = "main_mpi"
exec_pthread = "main_pthread"
exec_openmp = "main_openmp"
input_data = "data.txt"


def add_output_folder(filename):
    return output_folder + filename


def get_exec_time(command):
    times = []
    for i in range(num_of_exec):
        times.append(decimal.Decimal(os.popen(command).read().strip()))
    return numpy.mean(times)


def get_serial_odd_even_sort_exec_time():
    return get_exec_time(add_output_folder(exec_serial) + " " + add_output_folder(input_data) + " 1")


def get_serial_qsort_time():
    return get_exec_time(add_output_folder(exec_serial) + " " + add_output_folder(input_data) + " 2")


def get_mpi_exec_time(proc_num):
    return get_exec_time("mpiexec -n " + str(proc_num) + " " + add_output_folder(exec_mpi) + " " + add_output_folder(input_data))


def get_pthreads_exec_time(thread_num):
    return get_exec_time(add_output_folder(exec_pthread) + " " + str(thread_num) + " " + add_output_folder(input_data))


def get_openmp_exec_time(thread_num):
    return get_exec_time(add_output_folder(exec_openmp) + " " + str(thread_num) + " " + add_output_folder(input_data))


# using this list to customize the number of processes/threads when running parallel programs.
proc_nums = [2, 4, 8]

result_filename = "result.csv"
result_file = open(add_output_folder(result_filename), "w")
for i in range(data_size, max_data_size, data_incr_step):
    result_file.write(str(i) + ",")
result_file.write(str(max_data_size) + "\n")

'''
serial_odd_even
serial_qsort
mpi(proc_nums[...])
pthreads(proc_nums[...])
openmp(proc_nums[...])
'''

# var for saving test time
exec_times = []
for i in range((run_serial_odd_even + run_serial_qsort) + (run_mpi + run_pthreads + run_openmp) * len(proc_nums)):
    exec_times.append([])

# run program and get test time
while data_size <= max_data_size:
    os.system(add_output_folder(exec_create_data) + ' ' +
              str(data_size) + ' > ' + add_output_folder(input_data))

    if run_serial_odd_even:
        serial_exec_time = get_serial_odd_even_sort_exec_time()
        exec_times[0].append(serial_exec_time)
    if run_serial_qsort:
        serial_exec_time = get_serial_qsort_time()
        exec_times[run_serial_odd_even].append(serial_exec_time)

    os.putenv("OMP_SCHEDULE", "auto")

    for i in range(len(proc_nums)):
        if run_mpi:
            mpi_exec_time = get_mpi_exec_time(proc_nums[i])
            exec_times[(run_serial_odd_even + run_serial_qsort) +
                       i].append(mpi_exec_time)
        if run_pthreads:
            pthreads_exec_time = get_pthreads_exec_time(proc_nums[i])
            exec_times[(run_serial_odd_even + run_serial_qsort) +
                       run_mpi*len(proc_nums) + i].append(pthreads_exec_time)
        if run_openmp:
            openmp_exec_time = get_openmp_exec_time(proc_nums[i])
            exec_times[(run_serial_odd_even + run_serial_qsort) + (run_mpi +
                                                                   run_pthreads)*len(proc_nums) + i].append(openmp_exec_time)

    print("finished data size " + str(data_size))
    data_size += data_incr_step

# write result to file
for i in exec_times:
    for j in range(len(i) - 1):
        result_file.write(str(i[j]) + ",")
    result_file.write(str(i[-1]) + "\n")

result_file.close()
