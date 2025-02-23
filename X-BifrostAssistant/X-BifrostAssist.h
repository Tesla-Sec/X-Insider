/* Assistant calculos*/
#ifndef X_BIFROST_ASSIST_H
#define X_BIFROST_ASSIST_H

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

//flags
    #define USE_NEURAL_GLOBAL 1 //recomendado 1 para usar rede neural

    #define extrapolador(...) extrapolador1(__VA_ARGS__, -1.0)
    #define extrapoladorNeural(...) extrapoladorNeural1(__VA_ARGS__, -1.0)


//funcoes
float extrapolador1(float primeiro, ...);
float extrapoladorNeural1(float primeiro, ...);
float ativacao(float x);
float ativacao_derivada(float x);
float neural_network_predict(float *input, int input_size);
float extrapoladorNeural1(float primeiro, ...);

#if USE_NEURAL_GLOBAL
    float hidden_weights[3][2] = {
        {1.0f, 0.0f},
        {0.0f, 1.0f},
        {0.0f, 0.0f}
    };
    float hidden_biases[3] = {0.0f, 0.0f, 0.0f};
    float output_weights[3] = {1.0f, 1.0f, 0.0f};
    float output_bias = 0.0f;

    void treinar_neural_network(float input[2], float target, float margem_erro, float learning_rate, int max_iter);
    float forward_propagation(float input[2], float hidden_out[3]);
#endif

#endif
