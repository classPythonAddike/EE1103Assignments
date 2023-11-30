import os

os.system("gcc -o main main.c -lm;")

f = open("graph.dat", "w")
num_points = 100
step = 1 / num_points
for i in range(num_points):
    os.system(f"./main 1000 {i * step}")
    with open("temp.txt", "r") as f_temp:
        R_2 = f_temp.readline().split()[-1]
        f.write(f"{i * step:.3f} {R_2}\n")
f.close()
