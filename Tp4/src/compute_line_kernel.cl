__kernel void compute_line(__global double *inputMat, __global double *outputMat)
{
    int i = get_global_id(0);
    int j = get_global_id(1);
    int rows = get_global_size(0); // NDRange(0) == lignes
    int cols = get_global_size(1); //NDRange(1) == colonnes
    // traiter chaque rangée
    for (int idx_ligne = 0; idx_ligne < rows; ++idx_ligne)
    {
        // trouver l'index pivot du plus grand pivot de la colonne idx_ligne en valeur absolue
        // (pour une meilleure stabilité numérique).
        int threadId=get_global_id(0);
        int localThreadId=get_local_id(0);
        int localSize=get_local_size(0);
        int col_pivot[get_global_size(0)];
        __local int idx_pivot;

        if (get_global_id(0) == 0) {
            __global double val_pivot = fabs(inputMat[idx_ligne*cols+idx_ligne]);  //element sur la diagonale
            __global int idx_pivot = 0;
            for(int i= idx_ligne; i<rows; i++)
                if inputMat[(idx_ligne+threadId)*cols] < val_pivot{
                    val_pivot = inputMat[(idx_ligne+threadId)*cols];
                    idx_pivot= i ;
                }
        }


        // vérifier que la matrice n'est pas singulière
        if (inputMat[idx_pivot*cols+idx_ligne] == 0)
            return -1;
        // PIvot
        inputMat[idx_ligne*cols+get_local_id(0)] /=  inputMat[idx_ligne*cols+idx_ligne];
        // AUTRES
        inputMat[get_global_id(0)*get_global_id(1)+get_local_id(0)] -= inputMat[get_global_id(0)*get_global_id(1)+get_local_id(0)] *  inputMat[idx_ligne*cols+idx_ligne];
    }
} ;
