import os
import decimal

output_folder = "../output/"
exec_create_data = "create_data"
exec_serial = "main_serial"
exec_mpi = "main_mpi"
exec_pthread = "main_pthread"
exec_openmp = "main_openmp"
input_data = "data.txt"

num_of_exec = 10

def add_output_folder(filename):
    return output_folder + filename

def get_exec_time(command):
    times = []
    for i in range(num_of_exec):
        times.append(decimal.Decimal(os.popen(command).read().strip()))
    times.sort()
    return times[int((num_of_exec - 1) / 2)] if num_of_exec % 2 == 1 else (times[int(num_of_exec / 2)] + times[int(num_of_exec / 2 - 1)]) / 2

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

data_size = 10000
data_incr_step = 10000
max_data_size = 100000

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
exec_times = []
for i in range(2 + 3 * len(proc_nums)):
    exec_times.append([])

while data_size <= max_data_size:
    os.system(add_output_folder(exec_create_data) + ' ' +
              str(data_size) + ' > ' + add_output_folder(input_data))

    serial_exec_time = get_serial_odd_even_sort_exec_time()
    exec_times[0].append(serial_exec_time)
    serial_exec_time = get_serial_qsort_time()
    exec_times[1].append(serial_exec_time)

    os.putenv("OMP_SCHEDULE", "auto")

    for i in range(len(proc_nums)):
        mpi_exec_time = get_mpi_exec_time(proc_nums[i])
        pthreads_exec_time = get_pthreads_exec_time(proc_nums[i])
        openmp_exec_time = get_openmp_exec_time(proc_nums[i])
        exec_times[2 + i].append(mpi_exec_time)
        exec_times[2 + len(proc_nums) + i].append(pthreads_exec_time)
        exec_times[2 + 2*len(proc_nums) + i].append(openmp_exec_time)

    print("finished data size " + str(data_size))
    data_size += data_incr_step

for i in exec_times:
    for j in range(len(i) - 1):
        result_file.write(str(i[j]) + ",")
    result_file.write(str(i[-1]) + "\n")

result_file.close()
