#include <cstdio>

#define N 64

int main(int argc, char **argv)
{
    int a[N];
    int i;

    #pragma acc parallel loop
    for(i=0;i<N;i++){
        a[i] = i;
    }

    #pragma acc parallel loop
    for(i=0;i<N;i++){
        a[i] = 2*a[i];
    }

    for(i=0;i<N;i++){
        printf("%d\n",a[i]);
    }


    
    return 0;
}