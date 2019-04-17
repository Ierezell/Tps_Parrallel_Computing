# Parameter M adjacency matrix where M_i,j represents the link from 'j' to 'i', such that for all 'j'
# sum(i, M_i,j) = 1
# Parameter d damping factor (default value 0.85)
# Parameter eps quadratic error for v (default value 1.0e-8)
# Return v, a vector of ranks such that v_i is the i-th rank from [0, 1]

import numpy as np
import json


graph = [{'id': 0, 'neighbors': [4]},
         {'id': 1, 'neighbors': [0]},
         {'id': 2, 'neighbors': [0]},
         {'id': 3, 'neighbors': [1, 2]},
         {'id': 4, 'neighbors': [2, 3]}]


def load_json(path_json: str) -> dict:
    with open('python.org.json') as json_file:
        graph = json.load(json_file)
    return graph


def create_matrix_from_json(path_json: str) -> np.array:
    graph = load_json(path_json)
    M = np.zeros((len(graph), len(graph)))
    for site in graph:
        for neighbors in site['neighbors']:
            M[site['id'], neighbors] += 1
    M /= M.sum(axis=0)
    return M


def create_matrix_from_graph(graph: dict) -> np.array:
    M = np.zeros((len(graph), len(graph)))
    for site in graph:
        for neighbors in site['neighbors']:
            M[site['id'], neighbors] += 1
    M /= M.sum(axis=0)
    return M


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


v = pagerank(create_matrix_from_graph(graph), 0.001, 0.85)
v2 = pagerank2(create_matrix_from_graph(graph), 0.001, 0.85)
print(v.reshape(-1))
print(v2.reshape(-1))
