# crée des intervalles de tests.
# le fichier nombresN.txt contient 100 intervalles de largeur 10**N
import random

nLargeur = 5  # intervalle de largeur 10^nLargeur


with open("./nombres"+".txt", 'w') as fichier_nombre:
    fichier_nombre.write("")
for _ in range(500):  # nombre d'intervalles
    intervalle_bas = random.randint(1, 9) * 10**(random.randint(10, 100))
    intervalle_haut = intervalle_bas + 10**nLargeur  # largeur des intervalles
    with open("./nombres"+".txt", 'a') as fichier_nombre:
        fichier_nombre.write(str(intervalle_bas)+" "+str(intervalle_haut)+"\n")
