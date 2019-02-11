#crée des intervalles de tests.
#le fichier nombresN.txt contient 100 intervalles de largeur 10**N
import random

for j in range(2,11):
    with open("./nombres"+str(j)+".txt", 'w') as fichier_nombre:
        fichier_nombre.write("")
    for _ in range(500):  #nombre d'intervalles
        intervalle_bas = random.randint(1,9) * 10**(random.randint(10,100))
        intervalle_haut = intervalle_bas + 10**j   #largeur des intervalles
        with open("./nombres"+str(j)+".txt", 'a') as fichier_nombre:
            fichier_nombre.write(str(intervalle_bas)+" "+str(intervalle_haut)+"\n")
