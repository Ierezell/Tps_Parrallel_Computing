#define __CL_ENABLE_EXCEPTIONS
#define CL_HPP_ENABLE_EXCEPTIONS
#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include "Matrix.hpp"
#include "Chrono.hpp"

using namespace std;


// Multiplier deux matrices.
Matrix multiplyMatrix(const Matrix &iMat1, const Matrix &iMat2)
{

    // vérifier la compatibilité des matrices
    assert(iMat1.cols() == iMat2.rows());
    // effectuer le produit matriciel
    Matrix lRes(iMat1.rows(), iMat2.cols());
    // traiter chaque rangée
    for (size_t i = 0; i < lRes.rows(); ++i)
    {
        // traiter chaque colonne
        for (size_t j = 0; j < lRes.cols(); ++j)
        {
            lRes(i, j) = (iMat1.getRowCopy(i) * iMat2.getColumnCopy(j)).sum();
        }
    }
    return lRes;
}

int main(int argc, char **argv)
{

    srand((unsigned)time(NULL));
    Chrono chron = Chrono();

    unsigned int taille_mat = 5;
    if (argc == 2)
    {
        taille_mat = atoi(argv[1]);
    }

    MatrixRandom matrice(taille_mat, taille_mat);
    MatrixIdentity matriceInverse(taille_mat);


    MatrixConcatCols matrice_et_id(matrice, MatrixIdentity(matrice.rows()));
    // cout << "Matrice d'entrée : " << endl
    //           << matrice_et_id << endl;
    // Pour les erreurs possibles
    cl_int err = CL_SUCCESS;
    float tic = chron.get();
    try
    {
        ////////////////////////////////////////
        // Liste les plateformes disponibles  //
        ////////////////////////////////////////
        vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        if (platforms.size() == 0)
        {
            cout << "Pas de plateformes disponibles ! Vérifier vos installations\n";
            return -1;
        }
        cerr << platforms.size() << " Platform(s) found " << endl;
        // Affiche les informations sur les plateformes disponibles
        for (int i = 0; i < platforms.size(); i++)
        {
            cout << "\t Platform : " << i << endl;
            string infos;
            platforms[i].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &infos);
            cout << "\t\t Platform is by: " << infos << endl;
            platforms[i].getInfo((cl_platform_info)CL_PLATFORM_NAME, &infos);
            cout << "\t\t Platform driver is : " << infos << endl;
            platforms[i].getInfo((cl_platform_info)CL_PLATFORM_VERSION, &infos);
            cout << "\t\t Platform version is: " << infos << endl;
        }
        cout << "Using platform 0 " << endl
                  << endl;

        ////////////////////////////////////////
        //          Crée le contexte          //
        ////////////////////////////////////////
        cl_context_properties contex_props[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0};
        cl::Context context(CL_DEVICE_TYPE_GPU, contex_props);

        ////////////////////////////////////////
        //    Liste les devices disponibles   //
        ////////////////////////////////////////
        vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
        // Affiche les infos sur les devices disponibles
        cerr << devices.size() << " Device(s) found " << endl;
        for (int i = 0; i < devices.size(); i++)
        {
            cout << "\t Device : " << i << endl;
            string infos;
            devices[i].getInfo((cl_device_info)CL_DEVICE_NAME, &infos);
            cout << "\t\t  Device name : " << infos << endl;
            devices[i].getInfo((cl_device_info)CL_DEVICE_TYPE, &infos);
            cout << "\t\t  Device type : " << infos << endl;
            devices[i].getInfo((cl_device_info)CL_DEVICE_VENDOR, &infos);
            cout << "\t\t  Device vendor is: " << infos << endl;
        }
        cout << "Using device 0 " << endl
                  << endl;

        ////////////////////////////////////////
        // Charge le fichier des kernels gpu  //
        ////////////////////////////////////////
        ifstream file("./src/compute_line_kernel.cl");
        if (file.is_open() == false)
        {
            cout << "Le fichier de kernel n'est pas loadé !";
            return -1;
        }
        string prog(istreambuf_iterator<char>(file), (istreambuf_iterator<char>()));
        cout << "File ok " << endl;

        ////////////////////////////////////////////////////////////
        // Transforme le fichier en source et crée le programme   //
        ////////////////////////////////////////////////////////////
        cl::Program::Sources source(1, make_pair(prog.c_str(), prog.length()));
        cl::Program program = cl::Program(context, source);

        ////////////////////////////////////////////////////////
        // Build le programme sur les devices (et get le log) //
        ////////////////////////////////////////////////////////
        try
        {
            program.build(devices);
        }
        // Get le log d'erreur si jamais problème dans le kernel
        catch (cl::Error &err)
        {
            if (err.err() == CL_BUILD_PROGRAM_FAILURE)
            {
                for (cl::Device dev : devices)
                {
                    // Check the build status
                    cl_build_status status = program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(dev);
                    if (status != CL_BUILD_ERROR)
                        continue;
                    // Get the build log
                    string name = dev.getInfo<CL_DEVICE_NAME>();
                    string buildlog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(dev);
                    cerr << "Build log for " << name << ":" << endl
                              << buildlog << endl;
                }
            }
            else
                throw err;
        }
        //////////////////////////////////////////////////////////////
        // Crée le noyeau associé à notre programme et notre device //
        //////////////////////////////////////////////////////////////
        cl::Kernel kernelProg(program, "compute_line", &err);

        ////////////////////////////////////////
        //           Crée la queue            //
        ////////////////////////////////////////
        cl::CommandQueue queue(context, devices[0]);

        ////////////////////////////////////////
        //   Crée le buffer pour le device    //
        ////////////////////////////////////////
        double *buff_mat = &matrice_et_id.getDataArray()[0];
        int taille_cols = matrice_et_id.cols();
        int *buff_nb_cols = &taille_cols;
        cl::Buffer inputMatriceBuffer(context, CL_MEM_READ_ONLY, matrice_et_id.rows() * matrice_et_id.cols() * sizeof(double));
        cl::Buffer outputMatriceBuffer(context, CL_MEM_READ_WRITE, matrice_et_id.rows() * matrice_et_id.cols() * sizeof(double));
        cl::Buffer nb_cols_buffer(context, CL_MEM_READ_WRITE, sizeof(int));
        cl::Buffer fact_elims(context, CL_MEM_READ_WRITE, taille_cols * sizeof(double));

        // Pour les flemmard idem que ci dessus mais le c++ fait tout pour nous
        // cl::Buffer outputMatriceBuffer(begin(matrice_et_id.getDataArray()), end(matrice_et_id.getDataArray()), true);
        // cl::Buffer inputMatriceBuffer(begin(matrice_et_id.getDataArray()), end(matrice_et_id.getDataArray()), true);

        ////////////////////////////////////////
        //  Ecrit les buffer dans la queue    //
        ////////////////////////////////////////
        queue.enqueueWriteBuffer(inputMatriceBuffer, CL_TRUE, 0, matrice_et_id.rows() * matrice_et_id.cols() * sizeof(double), buff_mat);
        queue.enqueueWriteBuffer(nb_cols_buffer, CL_TRUE, 0, sizeof(int), buff_nb_cols);
        cout << "Buffer ok " << endl;

        ////////////////////////////////////////
        //    Charge le kernel et ses args    //
        ////////////////////////////////////////
        err = kernelProg.setArg(0, nb_cols_buffer);
        err |= kernelProg.setArg(1, fact_elims);
        err |= kernelProg.setArg(2, inputMatriceBuffer);
        err |= kernelProg.setArg(3, outputMatriceBuffer);
        if (err != CL_SUCCESS)
        {
            cout << "Set args : Kernel panic ! " << endl;
            return -1;
        }
        cout << "Kernel ok " << endl;

        ////////////////////////////////////////
        // Defini la taille de notre problème //
        ////////////////////////////////////////
        cl_uint MAX_COMPUTE_UNITS;
        devices[0].getInfo((cl_device_info)CL_DEVICE_MAX_COMPUTE_UNITS, &MAX_COMPUTE_UNITS);
        // cout << "1111111  " << MAX_COMPUTE_UNITS << endl;

        cl_uint MAX_WORK_ITEM_DIMENSIONS;
        devices[0].getInfo((cl_device_info)CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, &MAX_WORK_ITEM_DIMENSIONS);
        // cout << "2222222  " << MAX_WORK_ITEM_DIMENSIONS << endl;

        size_t MAX_WORK_GROUP_SIZE;
        devices[0].getInfo((cl_device_info)CL_DEVICE_MAX_WORK_GROUP_SIZE, &MAX_WORK_GROUP_SIZE);
        // cout << "3333333  " << MAX_WORK_GROUP_SIZE << endl;

        size_t MAX_WORK_ITEM_SIZES[100];
        devices[0].getInfo((cl_device_info)CL_DEVICE_MAX_WORK_ITEM_SIZES, &MAX_WORK_ITEM_SIZES);
        // cout << "4444444  ";
        // for (auto i : MAX_WORK_ITEM_SIZES)
        //     cout << i << "    ";
        // cout << endl;
        int globalSize = MAX_COMPUTE_UNITS * MAX_WORK_GROUP_SIZE;
        int localSize = MAX_WORK_GROUP_SIZE;
        cl::NDRange globalProblemSize(MAX_COMPUTE_UNITS * MAX_WORK_GROUP_SIZE, 1);
        cl::NDRange localProblemSize(MAX_WORK_GROUP_SIZE, 1);
        printf("globalSize = %d", globalSize);
        printf("localSize = %d", localSize);
        // cl::NDRange globalProblemSize(16);
        // cl::NDRange localProblemSize(8);

        // queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, cl::NullRange);
        // queue.finish();

        cl::Event event;
        queue.enqueueNDRangeKernel(
            kernelProg,
            cl::NullRange,
            globalProblemSize,
            localProblemSize,
            NULL,
            &event);
        event.wait();
        cout << "Fini GPU " << endl;

        ////////////////////////////////////////
        //        Build le programme          //
        ////////////////////////////////////////
        // // recuperer les donnees calculees dans la memoire du device
        // queue.enqueueReadBuffer(outputMatriceBuffer, CL_TRUE, 0,
        //                         matrice_et_id.rows() * matrice_et_id.cols() * sizeof(cl_double), outputMatriceBuffer);
        double *matriceOutput = (double *)calloc(matrice_et_id.rows() * matrice_et_id.cols(), sizeof(double));
        queue.enqueueReadBuffer(outputMatriceBuffer, CL_TRUE, 0, matrice_et_id.rows() * matrice_et_id.cols() * sizeof(double), matriceOutput);

        Matrix matOut(matrice_et_id.rows(), matrice_et_id.cols());
        for (int i = 0; i < matrice_et_id.rows(); i++)
        {
            for (int j = 0; j < matrice_et_id.cols(); j++)
            {
                int index = (i * matrice_et_id.cols()) + j;
                matOut(i, j) = matriceOutput[index];
            }
        }
        float tac = chron.get();
        // for (int idx_lignes = 0; idx_lignes < matOut.rows(); idx_lignes++)
        // {
        //     for (int idx_lignes_rest = idx_lignes; idx_lignes_rest < matOut.rows(); idx_lignes_rest++)
        //     {
        //         if (matOut(idx_lignes_rest, idx_lignes) > 0)
        //             matOut = matOut.swapRows(idx_lignes_rest, idx_lignes);
        //     }
        // }
        
        // On copie la partie droite de la matrice AI ainsi transformée
        // dans la matrice courante (this).
        for (unsigned int i = 0; i < matriceInverse.rows(); ++i)
        {
            matriceInverse.getRowSlice(i) = matOut.getDataArray()[slice(i * matOut.cols() + matriceInverse.cols(), matriceInverse.cols(), 1)];
        }

        Matrix res = multiplyMatrix(matrice, matriceInverse);

        // cout << "Matrice d'entrée : " << endl
        //           << matrice_et_id << endl;
        // cout << endl;
        // cout << "Matrice de sortie : " << endl
        //           << matOut << endl;
        cout << "Erreur : " << res.getDataArray().sum() - taille_mat << endl
         << endl;
        cout << "Temps : " << tac - tic << "secondes" << endl;

        // ////////////////////////////////////////
        // //       Clean les allocations  pas possible en c++      //
        // ////////////////////////////////////////
        // clReleaseKernel(kernelProg);
        // clReleaseProgram(program);
        // clReleaseCommandQueue(queue);
        // clReleaseMemObject();
        // clReleaseMemObject();
        // clReleaseMemObject();
        // clReleaseContext(context);
        // clReleaseEvent(event);
        // free(platforms);
        // free(devices);
    }
    catch (cl::Error err)
    {
        cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << endl;
    }
}

//     // https://dournac.org/info/gpu_sum_reduction
//     // https://www.codeproject.com/Articles/92788/Introductory-Tutorial-to-OpenCL
//     https://www.khronos.org/registry/OpenCL/specs/2.2/html/OpenCL_Cxx.html#vector-utilities-library

// __kernel void maxping(__global __read_only float * a, __global __write_only float *b){
//                         int threadId=get_global_id(0);
//                         int localThreadId=get_local_id(0);
//                         int localSize=get_local_size(0);
//                         __local float fastMem[256];
//                         fastMem[localThreadId]=a[threadId];
//                         barrier(CLK_GLOBAL_MEM_FENCE|CLK_LOCAL_MEM_FENCE);

//                         for(int i=localSize/2;i>=1;i/=2)
//                         {
//                             if(localThreadId<i)
//                             {
//                                 if(fastMem[localThreadId]<fastMem[localThreadId+i])
//                                     fastMem[localThreadId]=fastMem[localThreadId+i];
//                             }
//                             barrier(CLK_LOCAL_MEM_FENCE);
//                         }
//                         if(localThreadId==0)
//                             b[threadId]=fastMem[localThreadId];
// }

// void floatMatrixMultLocals(__global float *MR, __global float *M1, __global float *M2, __global int *q)
// {
//     //Identification of this workgroup
//     int i = get_group_id(0);
//     int j = get_group_id(1);
//     //Identification of work-item
//     int idX = get_local_id(0);
//     int idY = get_local_id(1);
//     //matrixes dimensions
//     int p = get_global_size(0);
//     int r = get_global_size(1);
//     int Q = *q;
//     //Number of submatrixes to be processed by each worker (Q dimension)
//     int numSubMat = Q / BLOCK_SIZE;
//     float4 resp = (float4)(0, 0, 0, 0);
//     __local float A[BLOCK_SIZE][BLOCK_SIZE];
//     __local float B[BLOCK_SIZE][BLOCK_SIZE];
//     for (int k = 0; k < numSubMat; k++)
//     {
//         //Copy submatrixes to local memory. Each worker copies one element
//         //Notice that A[i,k] accesses elements starting from M[BLOCK_SIZE*i, BLOCK_SIZE*j]
//         A[idX][idY] = M1[BLOCK_SIZE * i + idX + p * (BLOCK_SIZE * k + idY)];
//         B[idX][idY] = M2[BLOCK_SIZE * k + idX + Q * (BLOCK_SIZE * j + idY)];
//         barrier(CLK_LOCAL_MEM_FENCE);
//         for (int k2 = 0; k2 < BLOCK_SIZE; k2 += 4)
//         {
//             float4 temp1 = (float4)(A[idX][k2], A[idX][k2 + 1], A[idX][k2 + 2], A[idX][k2 + 3]);
//             float4 temp2 = (float4)(B[k2][idY], B[k2 + 1][idY], B[k2 + 2][idY], B[k2 + 3][idY]);
//             resp += temp1 * temp2;
//         }
//         barrier(CLK_LOCAL_MEM_FENCE);
//     }
//     MR[BLOCK_SIZE * i + idX + p * (BLOCK_SIZE * j + idY)] = resp.x + resp.y + resp.z + resp.w;
// }
