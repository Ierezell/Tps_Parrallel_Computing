# Parameter M adjacency matrix where M_i,j represents the link from 'j' to 'i', such that for all 'j'
# sum(i, M_i,j) = 1
# Parameter d damping factor (default value 0.85)
# Parameter eps quadratic error for v (default value 1.0e-8)
# Return v, a vector of ranks such that v_i is the i-th rank from [0, 1]

import numpy as np
import json
import time
from tools import *

graph = [{'id': 0, 'neighbors': [4]},
         {'id': 1, 'neighbors': [0]},
         {'id': 2, 'neighbors': [0]},
         {'id': 3, 'neighbors': [1, 2]},
         {'id': 4, 'neighbors': [2, 3]}]



print(create_matrix_from_graph(graph))


def pagerank(M, eps=1.0e-8, d=0.85):
    N = M.shape[1]
    v = np.random.rand(N, 1)
    v = v / np.linalg.norm(v, 1)
    last_v = np.ones((N, 1), dtype=np.float32) * 100

    while np.linalg.norm(v - last_v, 2) > eps:
        last_v = v
        v = d * np.matmul(M, v) + (1 - d) / N
    return v


def pagerank2(M, eps=1.0e-8, d=0.85):
    N = M.shape[1]
    v = np.ones((N, 1))*1/N
    last_v = np.ones((N, 1), dtype=np.float32) * np.inf
    while np.linalg.norm(v - last_v, 2) > eps:
        last_v = v
        v = d * np.matmul(M, v) + (1 - d) / N
    return v

def pagerank3(M, numIterations=100, d=0.85):
    N = M.shape[1]
    v = np.ones((N, 1))*1/N
    for i in range(numIterations):
        last_v = v
        v = d * np.matmul(M, v) + (1 - d) / N
    return v

tic = time.clock()
v = pagerank3(create_matrix_from_json("./python.org.json"), 100, 0.85)
v = v.reshape(-1)
v = sorted(v, reverse = True)
tac = time.clock()
temps_execution = tac-tic
print(v)
print("temps d'execution: "+str(temps_execution)+" secondes")


v2 = pagerank2(create_matrix_from_graph(graph), 0.0001, 0.85)

print(v2.reshape(-1))

