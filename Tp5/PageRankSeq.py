import numpy as np
import json
import time
import datetime
from tools import load_json,create_matrix_from_json, create_matrix_from_json_seq, printTime, getDictIdNeigh
import sys

def pagerank(M, numIterations=100, d=0.85):
    N = M.shape[1]
    v = np.ones((N, 1))*1/N
    for _ in range(numIterations):
        v = d * np.matmul(M, v) + (1 - d)/N
    return v

if __name__ == '__main__':

    d = 0.85    
    n_iterations = 100

    if(len(sys.argv)<2):
        print("Usage: python PageRankPar.py file.json")
        exit()    

    tac = datetime.datetime.now()

    idToUrl ,UrlToId = getDictIdNeigh(sys.argv[1])

    v = pagerank(create_matrix_from_json_seq(sys.argv[1]), n_iterations, d)
    v = v.reshape(-1)
    for i in range(len(v)):
        print(f"{idToUrl[i]} {v[i]}")
    print(v)
    #v = sorted(v, reverse = True)
    tic = datetime.datetime.now()
    printTime(tac,tic)

