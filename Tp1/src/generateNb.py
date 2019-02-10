import random

nombre = "0123456789"
with open('./nombres.txt', 'w') as fichier_nombre:
    fichier_nombre.write("")
for _ in range(10):  #nombre d'intervalles
    intervalle_bas = ""
    intervalle_haut = ""
    for i in range(5):  #nombres de chiffres maximum par borne d'intervalle
        intervalle_bas += nombre[random.randint(0, 9)]
        intervalle_haut += nombre[random.randint(0, 9)]
    with open('./nombres.txt', 'a') as fichier_nombre:
        fichier_nombre.write(intervalle_bas+" "+intervalle_haut+"\n")
