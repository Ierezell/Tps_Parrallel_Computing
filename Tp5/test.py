from random import random
import pyspark
from time import time
 
NUM_SAMPLE = 100000000
 
def sample(p):
    x, y = random(), random()
    return 1 if x**2+y**2 < 1 else 0
 
def reduce_func(a, b):
    return a+b
 
tic = time()
sc = pyspark.SparkContext()
plop = [1,2,3,4,5,6]
ddd = sc.parallelize(plop)
print(ddd)
# trials = sc.range(0, NUM_SAMPLE)
# in_circle = trials.map(sample)
# pi = in_circle.reduce(reduce_func)*4/NUM_SAMPLE
# tac = time()
# print(f"pi1 = {pi} en {tac-tic}s")
# tic = time()
# count = 0
# for _ in range(NUM_SAMPLE):
#     x, y = random(), random()
#     count += 1 if x**2+y**2 < 1 else 0
# tac = time()
# print(f"pi2 = {count*4/NUM_SAMPLE} en {tac-tic}s")