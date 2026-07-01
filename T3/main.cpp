//
// Created by gonzalo on 6/30/26.
//
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h> // Cabecera esencial para el mapeo GPU-GPU
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "implementations/common.h"
#include "implementations/CudaGol/cudaGol.h"

// Dimensiones de la simulación y ventana
const uint WIDTH = 1024;
const uint HEIGHT = 768;
const int BLOCK_SIZE = 256;

// Variables globales para la ventana y objetos gráficos
GLFWwindow* window = nullptr;
GLuint pbo = 0;
GLuint textureID = 0;
cudaGraphicsResource* cuda_pbo_resource = nullptr;

// Punteros de memoria para el estado del Juego de la Vida en la GPU
ubyte* d_data = nullptr;
ubyte* d_resultData = nullptr;

// Función de inicialización de entornos gráficos
void initGraphics() {
    if (!glfwInit()) {
        std::cerr << "Error: No se pudo inicializar GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);

    window = glfwCreateWindow(WIDTH, HEIGHT, "CC7515 - Tarea 3: Interop CUDA-OpenGL", NULL, NULL);
    if (!window) {
        std::cerr << "Error: No se pudo crear la ventana con GLFW" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    // Inicializar GLEW para cargar los entrypoints de extensiones modernas de OpenGL
    glewExperimental = GL_TRUE;

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cerr << "Error al inicializar GLEW: " << glewGetErrorString(err) << std::endl;
        exit(EXIT_FAILURE);
    }

    // 1. Crear el Pixel Buffer Object (PBO)
    glGenBuffers(1, &pbo);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
    // Reservar tamaño en VRAM para contener los colores RGBA (4 bytes por píxel)
    glBufferData(GL_PIXEL_UNPACK_BUFFER, WIDTH * HEIGHT * sizeof(uchar4), NULL, GL_DYNAMIC_DRAW);

    // 2. REGISTRO CRÍTICO: Vincular el buffer de OpenGL al runtime de CUDA
    cudaError_t cudaStat = cudaGraphicsGLRegisterBuffer(&cuda_pbo_resource, pbo, cudaGraphicsRegisterFlagsWriteDiscard);
    if (cudaStat != cudaSuccess) {
        std::cerr << "Error al registrar el PBO en CUDA: " << cudaGetErrorString(cudaStat) << std::endl;
        exit(EXIT_FAILURE);
    }

    // 3. Crear y configurar la textura encargada de mapear el PBO a la pantalla
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    // Filtros NEAREST para que los píxeles (células) se vean nítidos al escalar
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Desenlazar buffers por seguridad
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

int main() {
    // Inicializar subsistemas gráficos
    initGraphics();

    // Semilla aleatoria
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    size_t dataLength = WIDTH * HEIGHT;

    // Reservar memoria física persistente en la GPU para la simulación interna
    cudaMalloc(&d_data, dataLength * sizeof(ubyte));
    cudaMalloc(&d_resultData, dataLength * sizeof(ubyte));

    // Generar estado inicial aleatorio en el Host (20% de probabilidad de vida)
    std::vector<ubyte> hostInit(dataLength);
    for (size_t i = 0; i < dataLength; ++i) {
        hostInit[i] = (std::rand() % 100 < 20) ? 1 : 0;
    }

    // Transferir la semilla inicial a la GPU (ÚNICA transferencia síncrona del programa)
    cudaMemcpy(d_data, hostInit.data(), dataLength * sizeof(ubyte), cudaMemcpyHostToDevice);

    std::cout << "Inicialización completada con éxito. Ejecutando bucle de interop..." << std::endl;

    // Bucle principal de Renderizado e Interoperabilidad (Cero uso de CPU / Cero copias a RAM)
    while (!glfwWindowShouldClose(window)) {

        // --- ETAPA 1: CÓMPUTO EN GPU ---
        // Avanzar una iteración del autómata celular en la memoria interna de CUDA
        runGameOfLifeStepCuda(d_data, d_resultData, WIDTH, HEIGHT, BLOCK_SIZE);
        std::swap(d_data, d_resultData); // Intercambio de punteros lógico en GPU

        // --- ETAPA 2: INTEROPERABILIDAD (Mapeo directo) ---
        // Bloquear el PBO de OpenGL para uso exclusivo de CUDA
        cudaGraphicsMapResources(1, &cuda_pbo_resource, 0);

        void* pboDevicePtr = nullptr;
        size_t mappedSize = 0;
        // Obtener el puntero nativo de memoria de la GPU correspondiente al PBO
        cudaGraphicsResourceGetMappedPointer(&pboDevicePtr, &mappedSize, cuda_pbo_resource);

        // Lanzar el nuevo kernel que escribe los colores (blanco/negro) directo en el PBO
        renderSimulationToPBO(d_data, pboDevicePtr, WIDTH, HEIGHT);

        // Sincronizar hilos de CUDA y liberar el recurso para que OpenGL recupere el control
        cudaDeviceSynchronize();
        cudaGraphicsUnmapResources(1, &cuda_pbo_resource, 0);

        // --- ETAPA 3: RENDERIZADO CON OPENGL ---
        glClear(GL_COLOR_BUFFER_BIT);

        // Asociar el PBO a la textura de forma interna en la GPU
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
        glBindTexture(GL_TEXTURE_2D, textureID);
        // Transfiere los datos del PBO a la textura (operación interna de VRAM a VRAM)
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

        // Dibujar un plano ortogonal a pantalla completa conteniendo la textura mapeada
        glEnable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, -1.0f);
            glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f, -1.0f);
            glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f,  1.0f);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f,  1.0f);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Intercambiar buffers de pantalla físicos y procesar eventos de entrada
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // --- ETAPA 4: LIMPIEZA DE RECURSOS ---
    std::cout << "Cerrando aplicación y liberando recursos..." << std::endl;

    cudaGraphicsUnregisterResource(cuda_pbo_resource);
    glDeleteBuffers(1, &pbo);
    glDeleteTextures(1, &textureID);

    cudaFree(d_data);
    cudaFree(d_resultData);

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
