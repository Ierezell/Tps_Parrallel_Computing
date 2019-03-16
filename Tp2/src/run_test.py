import subprocess
import matplotlib.pyplot as plt
import numpy as np
temps_intra = []
temps_extra = []
THREADS_LIST = [1, 2, 4, 8, 16]
for nb_thread in THREADS_LIST:
    cmd_for = f"../bin/Tp2_Sebastien_Pierre_main_for {nb_thread} nombres.txt"
    p_for = subprocess.Popen(cmd_for, stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE, shell=True,
                             close_fds=True)

    output_for, err_for = p_for.communicate()
    p_for_status = p_for.wait()
    err_for = err_for.decode("utf-8")
    temps_extra.append(float(err_for.split()[3]))
    p_for.kill()
    # ###############################################

    cmd_numbers = f"../bin/Tp2_Sebastien_Pierre_main_numbers {nb_thread} nombres.txt"
    p_numbers = subprocess.Popen(cmd_numbers, stdout=subprocess.PIPE,
                                 stderr=subprocess.PIPE, shell=True,
                                 close_fds=True)

    output_numbers, err_numbers = p_numbers.communicate()
    p_numbers_status = p_numbers.wait()
    err_numbers = err_numbers.decode("utf-8")
    temps_intra.append(float(err_numbers.split()[3]))
    p_numbers.kill()

fig, axes = plt.subplots(2, 1)
speed_up_intra = [temps_intra[0]/temps_intra[i]
                  for i in range(len(temps_intra))]
speed_up_extra = [temps_extra[0]/temps_extra[i]
                  for i in range(len(temps_extra))]

print(temps_extra)
print(temps_intra)
print()
axes[0].plot(THREADS_LIST, speed_up_intra)
axes[0].plot(THREADS_LIST, speed_up_extra)

axes[1].plot(THREADS_LIST,
             [speed_up_intra[i]/(t) for i, t in enumerate(THREADS_LIST)])
axes[1].plot(THREADS_LIST,
             [speed_up_extra[i]/(t) for i, t in enumerate(THREADS_LIST)])

plt.show()
