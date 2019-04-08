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

// // pick up device type from compiler command line or from the default type
// #ifndef DEVICE
// #define DEVICE CL_DEVICE_TYPE_DEFAULT
// #endif

int main(int argc, char **argv)
{

    srand((unsigned)time(NULL));

    unsigned int taille_mat = 5;
    if (argc == 2)
    {
        taille_mat = atoi(argv[1]);
    }

    MatrixRandom matrice(taille_mat, taille_mat);

    matrice(0, 0) = 3;
    matrice(0, 1) = 2;
    matrice(0, 2) = 4;
    matrice(0, 3) = 12;
    matrice(0, 4) = 5;
    matrice(1, 0) = 6;
    matrice(1, 1) = 8;
    matrice(1, 2) = 7;
    matrice(1, 3) = 9;
    matrice(1, 4) = 11;
    matrice(2, 0) = 10;
    matrice(2, 1) = 13;
    matrice(2, 2) = 14;
    matrice(2, 3) = 15;
    matrice(2, 4) = 16;
    matrice(3, 0) = 17;
    matrice(3, 1) = 18;
    matrice(3, 2) = 19;
    matrice(3, 3) = 20;
    matrice(3, 4) = 21;
    matrice(4, 0) = 22;
    matrice(4, 1) = 23;
    matrice(4, 2) = 24;
    matrice(4, 3) = 25;
    matrice(4, 4) = 26;
    MatrixConcatCols matrice_et_id(matrice, MatrixIdentity(matrice.rows()));
    std::cout << "Matrice d'entrée : " << std::endl
              << matrice_et_id << std::endl;
    // Pour les erreurs possibles
    cl_int err = CL_SUCCESS;
    try
    {
        ////////////////////////////////////////
        // Liste les plateformes disponibles  //
        ////////////////////////////////////////
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        if (platforms.size() == 0)
        {
            std::cout << "Pas de plateformes disponibles ! Vérifier vos installations\n";
            return -1;
        }
        std::cerr << platforms.size() << " Platform(s) found " << std::endl;
        // Affiche les informations sur les plateformes disponibles
        for (int i = 0; i < platforms.size(); i++)
        {
            std::cout << "\t Platform : " << i << std::endl;
            std::string infos;
            platforms[i].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &infos);
            std::cout << "\t\t Platform is by: " << infos << std::endl;
            platforms[i].getInfo((cl_platform_info)CL_PLATFORM_NAME, &infos);
            std::cout << "\t\t Platform driver is : " << infos << std::endl;
            platforms[i].getInfo((cl_platform_info)CL_PLATFORM_VERSION, &infos);
            std::cout << "\t\t Platform version is: " << infos << std::endl;
        }
        std::cout << "Using platform 0 " << std::endl
                  << std::endl;

        ////////////////////////////////////////
        //          Crée le contexte          //
        ////////////////////////////////////////
        cl_context_properties contex_props[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0};
        cl::Context context(CL_DEVICE_TYPE_GPU, contex_props);

        ////////////////////////////////////////
        //    Liste les devices disponibles   //
        ////////////////////////////////////////
        std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
        // Affiche les infos sur les devices disponibles
        std::cerr << devices.size() << " Device(s) found " << std::endl;
        for (int i = 0; i < devices.size(); i++)
        {
            std::cout << "\t Device : " << i << std::endl;
            std::string infos;
            devices[i].getInfo((cl_device_info)CL_DEVICE_NAME, &infos);
            std::cout << "\t\t  Device name : " << infos << std::endl;
            devices[i].getInfo((cl_device_info)CL_DEVICE_TYPE, &infos);
            std::cout << "\t\t  Device type : " << infos << std::endl;
            devices[i].getInfo((cl_device_info)CL_DEVICE_VENDOR, &infos);
            std::cout << "\t\t  Device vendor is: " << infos << std::endl;
        }
        std::cout << "Using device 0 " << std::endl
                  << std::endl;

        ////////////////////////////////////////
        // Charge le fichier des kernels gpu  //
        ////////////////////////////////////////
        std::ifstream file("./src/compute_line_kernel.cl");
        if (file.is_open() == false)
        {
            std::cout << "Le fichier de kernel n'est pas loadé !";
            return -1;
        }
        std::string prog(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()));
        std::cout << "File ok " << std::endl;

        ////////////////////////////////////////////////////////////
        // Transforme le fichier en source et crée le programme   //
        ////////////////////////////////////////////////////////////
        cl::Program::Sources source(1, std::make_pair(prog.c_str(), prog.length()));
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
                    std::string name = dev.getInfo<CL_DEVICE_NAME>();
                    std::string buildlog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(dev);
                    std::cerr << "Build log for " << name << ":" << std::endl
                              << buildlog << std::endl;
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
        // Pour les flemmard idem que ci dessus mais le c++ fait tout pour nous
        // cl::Buffer outputMatriceBuffer(std::begin(matrice_et_id.getDataArray()), std::end(matrice_et_id.getDataArray()), true);
        // cl::Buffer inputMatriceBuffer(std::begin(matrice_et_id.getDataArray()), std::end(matrice_et_id.getDataArray()), true);

        ////////////////////////////////////////
        //  Ecrit les buffer dans la queue    //
        ////////////////////////////////////////
        queue.enqueueWriteBuffer(inputMatriceBuffer, CL_TRUE, 0, matrice_et_id.rows() * matrice_et_id.cols() * sizeof(double), buff_mat);
        queue.enqueueWriteBuffer(nb_cols_buffer, CL_TRUE, 0, sizeof(int), buff_nb_cols);
        std::cout << "Buffer ok " << std::endl;

        ////////////////////////////////////////
        //    Charge le kernel et ses args    //
        ////////////////////////////////////////
        err = kernelProg.setArg(0, nb_cols_buffer);
        err |= kernelProg.setArg(1, inputMatriceBuffer);
        err |= kernelProg.setArg(2, outputMatriceBuffer);
        if (err != CL_SUCCESS)
        {
            std::cout << "Set args : Kernel panic ! " << std::endl;
            return -1;
        }
        std::cout << "Kernel ok " << std::endl;

        ////////////////////////////////////////
        // Defini la taille de notre problème //
        ////////////////////////////////////////
        cl::NDRange problemSize(matrice_et_id.rows());
        // queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, cl::NullRange);
        // queue.finish();

        cl::Event event;
        queue.enqueueNDRangeKernel(
            kernelProg,
            cl::NullRange,
            problemSize,
            cl::NullRange,
            NULL,
            &event);
        event.wait();
        std::cout << "Fini GPU " << std::endl;

        ////////////////////////////////////////
        //        Build le programme          //
        ////////////////////////////////////////
        // // recuperer les donnees calculees dans la memoire du device
        // queue.enqueueReadBuffer(outputMatriceBuffer, CL_TRUE, 0,
        //                         matrice_et_id.rows() * matrice_et_id.cols() * sizeof(cl_double), outputMatriceBuffer);
        double *matriceOutput = (double *)malloc(matrice_et_id.rows() * matrice_et_id.cols() * sizeof(double));
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

        // for (int idx_lignes = 0; idx_lignes < matOut.rows(); idx_lignes++)
        // {
        //     for (int idx_lignes_rest = idx_lignes; idx_lignes_rest < matOut.rows(); idx_lignes_rest++)
        //     {
        //         if (matOut(idx_lignes_rest, idx_lignes) > 0)
        //             matOut = matOut.swapRows(idx_lignes_rest, idx_lignes);
        //     }
        // }

        std::cout << "Matrice d'entrée : " << std::endl
                  << matrice_et_id << std::endl;
        std::cout << std::endl;
        std::cout << "Matrice de sortie : " << std::endl
                  << matOut << std::endl;

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
        std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
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
