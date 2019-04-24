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
    M = M.transpose()
    M /= M.sum(axis=0)
    return M

def create_matrix_from_json_seq(path_json: str) -> np.array:
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
    M = M.transpose()
    M /= M.sum(axis=0)
    return M

def printTime(debut, fin):
    timeDiff = str(fin-debut)
    seconds = int(timeDiff.split('.')[0].split(':')[-1])
    milliseconds = int(timeDiff.split('.')[-1][:-3])
    print(f"{seconds}s {milliseconds}ms")


def computeContribs(urls, rank):
    num_urls = len(urls)
    for url in urls:
        yield (url, rank / num_urls)

def parseJson(jsonPath):
    listGraph = []
    with open(jsonPath) as json_file:
        graph = json.load(json_file)
        for node in graph:
            listGraph.append((node['id'],node['neighbors']))
    return listGraph

def getDictIdNeigh(jsonPath):
    idToUrl = {}
    UrlToId = {}
    with open(jsonPath) as json_file:
        graph = json.load(json_file)
    for node in graph:
        idToUrl[node['id']] = node['url'] 
        UrlToId[node['url']] = node['id']
    return idToUrl ,UrlToId