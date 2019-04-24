import json
import numpy as np

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