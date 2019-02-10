import random

with open('./nombres.txt', 'w') as fichier_nombre:
    fichier_nombre.write("")
for _ in range(50):  #nombre d'intervalles
    intervalle_bas = random.randint(1,9) * 10**(random.randint(10,100))
    intervalle_haut = intervalle_bas + 10**6    #largeur des intervalles
    with open('./nombres.txt', 'a') as fichier_nombre:
        fichier_nombre.write(str(intervalle_bas)+" "+str(intervalle_haut)+"\n")
