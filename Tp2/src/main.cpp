#include <stdio.h>
#include <omp.h>

int main(int argc, char const *argv[])
{
#pragma omp parallel
    {
        int ID = omp_get_thread_num();
        printf("Coucou toi :  %d", ID);
    }
    return 0;
}
