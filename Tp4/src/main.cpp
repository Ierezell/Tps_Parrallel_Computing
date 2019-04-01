#define __CL_ENABLE_EXCEPTIONS
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
const char *helloStr = "__kernel void "
                       "hello(void) "
                       "{ "
                       "  "
                       "} ";

//     "__kernel void "
//    "compute_line(__global float* line_pivot,__global float* line,__global float* coeff_ligne) "
//    "{ "
//    "int i = get_global_id(0);"
//    "int j = get_global_id(1);"
//    "line[i][j] = line[i][j] - line_pivot[j] * coeff_ligne;"
//    "}";

int main(int argc, char **argv)
{

    srand((unsigned)time(NULL));

    unsigned int taille_mat = 5;
    if (argc == 2)
    {
        taille_mat = atoi(argv[1]);
    }

    MatrixRandom matrice(taille_mat, taille_mat);
    MatrixConcatCols matrice_et_id(matrice, MatrixIdentity(matrice.rows()));
    cl_int err = CL_SUCCESS;
    try
    {
        // Pour les erreurs possibles
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
        cl_context_properties contex_props[] =
            {CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0};
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
        std::ifstream file("./src/hello_kernel.cl");
        if (file.is_open() == false)
        {
            std::cout << "Le fichier de kernel n'est pas loadé !";
            return -1;
        }
        std::string prog(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()));
        std::cout << "File ok " << std::endl;

        ////////////////////////////////////////
        //  Crée et build le programme        //
        ////////////////////////////////////////
        //cl::Program::Sources source(1, std::make_pair(helloStr, strlen(helloStr)));
        cl::Program::Sources source(1, std::make_pair(prog.c_str(), prog.length()));

        cl::Program program_ = cl::Program(context, source);
        std::cout << "Prog ok " << std::endl;

        try
        {
            cl::Error err = program_.build(devices);
            std::cout << "Build ok " << std::endl;
        }
        catch (cl::Error err)
        {
            cl::STRING_CLASS buildlog;
            program_.getBuildInfo((const cl::Device &)devices, (cl_program_build_info)CL_PROGRAM_BUILD_LOG, &buildlog);
            std::cout << "Build infos : " << buildlog << std::endl;
        }
        ////////////////////////////////////////
        //   Crée le buffer pour le device    //
        ////////////////////////////////////////
        Matrix *buff_mat = &matrice_et_id;
        cl::Buffer outCL(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                         matrice_et_id.rows() * matrice_et_id.cols(), buff_mat, &err);
        std::cout << "Buffer ok " << std::endl;

        ////////////////////////////////////////
        //    Charge le kernel et ses args    //
        ////////////////////////////////////////
        cl::Kernel kernel(program_, "hello", &err);
        // err = kernel.setArg(0, outCL);
        //  kernel.setArg(0, inputBufferDevice);
        //  kernel.setArg(1, outputBufferDevice);
        // if (err != CL_SUCCESS)
        // {
        //     std::cout << "Kernel panic ! " << std::endl;
        //     return -1;
        // }
        std::cout << "Kernel ok " << std::endl;

        ////////////////////////////////////////
        //           Crée la queue            //
        ////////////////////////////////////////
        cl::Event event;
        cl::CommandQueue queue(context, devices[0], 0, &err);
        queue.enqueueNDRangeKernel(
            kernel,
            cl::NullRange,
            cl::NDRange(4, 4), // cl::NDRange(DATA_SIZE)
            cl::NullRange,
            NULL,
            &event);
        event.wait();
        std::cout << "Queue ok " << std::endl;

        ////////////////////////////////////////
        //        Build le programme          //
        ////////////////////////////////////////
        // recuperer les donnees calculees dans la memoire du device
        queue.enqueueReadBuffer(outputBufferDevice, CL_TRUE, 0,
                                DATA_SIZE * sizeof(float), outputDataHost);
        ////////////////////////////////////////
        //        Build le programme          //
        ////////////////////////////////////////
        ////////////////////////////////////////
        //        Build le programme          //
        ////////////////////////////////////////

        //         // Create the kernel functor
        //         cl::make_kernel<cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, int> compute_lines(program, "compute_line");

        //         d_a = cl::Buffer(context, h_a.begin(), h_a.end(), true);
        //         d_b = cl::Buffer(context, h_b.begin(), h_b.end(), true);
        //         d_c = cl::Buffer(context, h_c.begin(), h_c.end(), true);
        //         d_d = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * LENGTH);

        //         // allouer et initialiser la memoire du device
        //         // cl::Buffer inputBufferDevice(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * DATA_SIZE, inputDataHost);
        //         // cl::Buffer outputBufferDevice(context, CL_MEM_WRITE_ONLY,
        //         //                               sizeof(float) * DATA_SIZE);
        //         cl::
        //             for (size_t idx_ligne = 0; idx_ligne < matrice_et_id.rows(); ++idx_ligne)
        //         {
        //             // trouver l'index p du plus grand pivot de la colonne k en valeur absolue
        //             // (pour une meilleure stabilité numérique).
        //             size_t idx_pivot = idx_ligne;
        //             double lMax = fabs(matrice_et_id(idx_pivot, idx_pivot));
        //             for (size_t idx_ligne_rest = idx_ligne; idx_ligne_rest < matrice_et_id.rows(); ++idx_ligne_rest)
        //             {
        //                 if (fabs(matrice_et_id(idx_ligne_rest, idx_pivot)) > lMax)
        //                 {
        //                     lMax = fabs(matrice_et_id(idx_ligne_rest, idx_pivot));
        //                     idx_pivot = idx_ligne_rest;
        //                 }
        //             }
        //             // vérifier que la matrice n'est pas singulière
        //             if (matrice_et_id(idx_pivot, idx_ligne) == 0)
        //                 throw runtime_error("Matrix not invertible");

        //             // échanger la ligne courante avec celle du pivot
        //             if (idx_pivot != idx_ligne)
        //                 matrice_et_id.swapRows(idx_pivot, idx_ligne);

        //             // Normaliser le pivot
        //             // double val_pivot = matrice_et_id(idx_ligne, idx_ligne);
        //             // for (size_t j = 0; j < matrice_et_id.cols(); ++j)
        //             //     matrice_et_id(idx_ligne, j) /= val_pivot;
        //             normalise_pivot(
        //                 cl::EnqueueArgs(
        //                     queue,
        //                     cl::NDRange(size)),
        //                 line_pivot,
        //                 coeff_pivot,
        //                 size);
        //             // Faire toute les transformations linéaires sur GPU

        //             compute_lines(
        //                 cl::EnqueueArgs(
        //                     queue,
        //                     cl::NDRange(size)),
        //                 line_pivot,
        //                 line,
        //                 coeff_ligne,
        //                 size);

        //             cl::copy(queue, d_d, h_d.begin(), h_d.end());

        //             // Test the results
        //             Matrix res_Par = multiplyMatrix(matrice, mat_inv);
        //             // On copie la partie droite de la matrice [ A I ] qui est devenue [I A^-1] ainsi transformée dans la matrice courante.
        //             Matrix mat_inv = MatrixIdentity(matrice.rows());
        //             for (size_t i = 0; i < matrice_et_id.rows(); ++i)
        //             {
        //                 mat_inv.getRowSlice(i) = matrice_et_id.getDataArray()[slice(i * matrice_et_id.cols() + matrice.cols(), matrice.cols(), 1)];
        //             }
    }
    catch (cl::Error err)
    {
        std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
    }
}

//     // https://dournac.org/info/gpu_sum_reduction
//     // https://www.codeproject.com/Articles/92788/Introductory-Tutorial-to-OpenCL
//     https://www.khronos.org/registry/OpenCL/specs/2.2/html/OpenCL_Cxx.html#vector-utilities-library

/*
 * The following example shows a general use case for the C++
 * bindings, including support for the optional exception feature and
 * also the supplied vector and string classes, see following sections for
 * decriptions of these features.
 *
 * \code
 * #define __CL_ENABLE_EXCEPTIONS
 *
 * #if defined(__APPLE__) || defined(__MACOSX)
 * #include <OpenCL/cl.hpp>
 * #else
 * #include <CL/cl.hpp>
 * #endif
 * #include <cstdio>
 * #include <cstdlib>
 * #include <iostream>
 *
 *  const char * helloStr  = "__kernel void "
 *                           "hello(void) "
 *                           "{ "
 *                           "  "
 *                           "} ";
 *
 *  int
 *  main(void)
 *  {
 *     cl_int err = CL_SUCCESS;
 *     try {
 *
 *       std::vector<cl::Platform> platforms;
 *       cl::Platform::get(&platforms);
 *       if (platforms.size() == 0) {
 *           std::cout << "Platform size 0\n";
 *           return -1;
 *       }
 *
 *       cl_context_properties properties[] =
 *          { CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0};
 *       cl::Context context(CL_DEVICE_TYPE_CPU, properties);
 *
 *       std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
 *
 *       cl::Program::Sources source(1,
 *           std::make_pair(helloStr,strlen(helloStr)));
 *       cl::Program program_ = cl::Program(context, source);
 *       program_.build(devices);
 *
 *       cl::Kernel kernel(program_, "hello", &err);
 *
 *       cl::Event event;
 *       cl::CommandQueue queue(context, devices[0], 0, &err);
 *       queue.enqueueNDRangeKernel(
 *           kernel,
 *           cl::NullRange,
 *           cl::NDRange(4,4),
 *           cl::NullRange,
 *           NULL,
 *           &event);
 *
 *       event.wait();
 *     }
 *     catch (cl::Error err) {
 *        std::cerr
 *           << "ERROR: "
 *           << err.what()
 *           << "("
 *           << err.err()
 *           << ")"
 *           << std::endl;
 *     }
 *
 *    return EXIT_SUCCESS;
 *  }
 *
 * \endcode
 *
 */
