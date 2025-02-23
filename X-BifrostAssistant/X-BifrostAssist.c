#include "X-BifrostAssist.h"

/* parte de extrapolacao*/
float extrapolador1(float primeiro, ...)
{
    float valor_anterior, ultimo_valor, valor_atual;
    float soma_diferencas, diferenca_media;
    double prox;
    int count;

    va_list args;

    va_start(args, primeiro);

    valor_anterior = primeiro;
    ultimo_valor = primeiro;
    count = 1;
    soma_diferencas = 0.0f;

    while (1)
    {
        prox = va_arg(args, double);
        if (prox == -1.0)
        {
            break;
        }
        valor_atual = (float)prox;
        soma_diferencas += (valor_atual - valor_anterior);
        valor_anterior = valor_atual;
        ultimo_valor = valor_atual;
        count++;
    }
    va_end(args);

    if (count == 1)
    {
        return primeiro;
    }

    diferenca_media = soma_diferencas / (count - 1);
    return ultimo_valor + diferenca_media;
}

// Função de ativação (identidade) e sua derivada (sempre 1)
float ativacao(float x)
{
    return x;
}

float ativacao_derivada(float x)
{
    return 1.0f;
}

float neural_network_predict(float *input, int input_size)
{
    int i,j;
    float soma, output, output_bias;
    float hidden_output[3];
    float hidden_biases[3] = {0.0f, 0.0f, 0.0f};
    float output_weights[3] = {1.0f, 1.0f, 0.0f};//pesos
    float hidden_weights[3][2] = {
        {1.0f, 0.0f}, // Neurônio 1: usa apenas o último valor
        {0.0f, 1.0f}, // Neurônio 2: usa apenas a diferença
        {0.0f, 0.0f}  // Neurônio 3: neurônio dummy
    };

    if (input_size != 2)
    {
        return 0.0f;
    }


    // Cálculo da saída da camada oculta (ativação identidade)
    for ( i = 0; i < 3; i++)
    {
        soma = hidden_biases[i];
        for ( j = 0; j < 2; j++)
        {
            soma += hidden_weights[i][j] * input[j];
        }
        hidden_output[i] = soma; // Função identidade
    }

    output_bias = 0.0f;

    output = output_bias;
    for ( i = 0; i < 3; i++)
    {
        output += output_weights[i] * hidden_output[i];
    }

    return output;
}

float extrapoladorNeural1(float primeiro, ...)
{
    float ultimo_valor, penultimo_valor, diferenca, input[2];
    float previsto, valores[100];
    int count;
    double prox;
    va_list args;

    va_start(args, primeiro);

    // Para armazenar os valores passados (limite arbitrário de 100 elementos)
    valores[count++] = primeiro;

    count = 0;
    while (1)
    {
        prox = va_arg(args, double);
        if (prox == -1.0)
        {
            break;
        }
        if (count < 100)
        {
            valores[count++] = (float)prox;
        }
    }
    va_end(args);

    // Se houver apenas um valor retorna o valor recebido.
    if (count < 2)
    {
        return primeiro;
    }

    ultimo_valor = valores[count - 1];
    penultimo_valor = valores[count - 2];
    diferenca = ultimo_valor - penultimo_valor;

    input[0] = ultimo_valor;
    input[1] = diferenca;

    // Previsão da rede neural
    previsto = neural_network_predict(input, 2);
    return previsto;
}


#if USE_NEURAL_GLOBAL

    float forward_propagation(float input[2], float hidden_out[3]) {
        int i,j;
        float soma, output;
        // Camada oculta
        for ( i = 0; i < 3; i++) {
            soma = hidden_biases[i];
            for ( j = 0; j < 2; j++) {
                soma += hidden_weights[i][j] * input[j];
            }
            hidden_out[i] = ativacao(soma);
        }
        // Camada de saída
        output = output_bias;
        for ( i = 0; i < 3; i++) {
            output += output_weights[i] * hidden_out[i];
        }
        return ativacao(output);
    }

      // ==================== Treinamento da Rede ====================
    /*
    * Função: treinar_neural_network
    * -------------------------------
    * Treina a rede para que, dada a entrada (input de 2 elementos), o valor
    * previsto se aproxime do valor target dentro de uma margem de erro.
    *
    * Parâmetros:
    *    input       - vetor de 2 floats: [último valor, diferença entre último e penúltimo]
    *    target      - valor que a rede deve prever
    *    margem_erro - margem aceitável de erro (|target - previsão| <= margem_erro)
    *    learning_rate - taxa de aprendizado para a atualização dos pesos
    *    max_iter    - número máximo de iterações de treinamento
    */
    void treinar_neural_network(float input[2], float target, float margem_erro, float learning_rate, int max_iter) {
        int iter, i, j;
        float hidden_out[3];
        float output, erro, delta_output, delta_hidden;

        iter = 0;
        while (iter < max_iter) {
            // Propagação direta
            output = forward_propagation(input, hidden_out);
            erro = target - output;
            if (fabs(erro) <= margem_erro) {
                printf("Treinamento convergiu em %d iterações. Erro: %f\n", iter, erro);
                break;
            }
            
            // -------------------- Backpropagation --------------------
            // Atualização da camada de saída:
            // delta_output = erro * derivada da função de ativação (que é 1 para a identidade)
            delta_output = erro * ativacao_derivada(output);
            
            // Atualiza os pesos da camada de saída
            for ( i = 0; i < 3; i++) {
                output_weights[i] += learning_rate * delta_output * hidden_out[i];
            }
            // Atualiza o viés da saída
            output_bias += learning_rate * delta_output;
            
            // Atualização da camada oculta:
            // Para cada neurônio da camada oculta, calcula o delta:
            for ( i = 0; i < 3; i++) {
                // O erro do neurônio oculto é propagado pela ligação com a saída
                delta_hidden = delta_output * output_weights[i] * ativacao_derivada(hidden_out[i]);
                for ( j = 0; j < 2; j++) {
                    hidden_weights[i][j] += learning_rate * delta_hidden * input[j];
                }
                hidden_biases[i] += learning_rate * delta_hidden;
            }
            iter++;
        }
        if (iter == max_iter) {
            printf("Treinamento não convergiu em %d iterações. Erro final: %f\n", max_iter, target - output);
        }
    }
#endif