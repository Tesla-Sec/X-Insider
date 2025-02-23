/** PROGRAMA: X-MetaTracker
*   DESCRIÇÃO: Monitora e calcula a média de requisições por segundo, registrando as requisições bem-sucedidas para manter e atualizar metadados de CNPJ
*
*   ÚLTIMA ATUALIZAÇÃO: 22/10/2024
*   RESPONSÁVEL: Nícolas Herculano Pires
*   CODIFICAÇÃO DO EDITOR DE TEXTO: ISO 8859-1 (LATIN 1)
*   LINHA DE COMPILAÇÃO: gcc -o X-MetaTracker.exe X-MetaTracker.c -I ".\curl\include" -L ".\curl\lib" -lcurl
*   VERSÃO GCC: 4.7.2 x86_32
*   VERSÃO libCURL: 8.10.1 x86_32
*
* DEPENDÊNCIAS EXPLÍCITAS: <stdbool.h>, <stdio.h>, <stdlib.h>, <string.h>, <time.h>
*
* DEPENDÊNCIAS IMPLÍCITAS: ".\curl\include\curl\curl.h", "cabecalho.h"
*
* NOTAS:
*   1- Certificar de referenciar corretamente no código e na linha de compilação as pastas que contém a curl.h, libcurl.a e libcurl.exe;
*   2- Qualquer dúvida sobre o código, favor me contatar via e-mail ou whatsapp;
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include ".\curl\include\curl\curl.h"
#include ".\Assets\cabecalho.h"

int main(void)
{
CURL *curl;
CURLcode res;
time_t start_time, current_time, last_5s_time;
FILE *logs;
ResponseData response_data;
double avg_per_second, avg_per_5_seconds, avg_per_minute;
double total_time, elapsed_time;
long http_code;
int successful_requests, failed_requests;
int total_requests, requests_5s;
int failed_urls[TOTAL_REQUESTS];
int failed_count;
int request_id;
char url[URL_SIZE];

    successful_requests = failed_requests = total_requests = requests_5s = failed_count = 0;

    logs = fopen(FILENAME, TYPE_OPEN);
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl)
    {
        time(&start_time);
        last_5s_time = start_time;

        while (successful_requests < TOTAL_REQUESTS)
        {
            if (failed_count > 0)
            {
                request_id = failed_urls[--failed_count];
            }
            else
            {
                request_id = REQUEST_BASE + successful_requests + failed_requests;
            }

            snprintf(url, sizeof(url), URL_FORMAT, request_id);

            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(curl, CURLOPT_URL, url);

            response_data = (ResponseData){.size = 0};
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

            res = curl_easy_perform(curl);

            if (res == CURLE_OK)
            {
                http_code = 0;
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

                if (http_code == SUCESSO)
                {
                    printf("||++ requisicao %s foi bem sucedida\n", url);
                    successful_requests++;
                    fprintf(logs, "%s\n", processar_requisicao(request_id, response_data.buffer));
                }
                else
                {
                    printf("//-- requisicao %s falhou com codigo HTTP: %ld. reagendando...\n", url, http_code);
                    failed_urls[failed_count++] = request_id;
                    failed_requests++;
                }
            }
            else
            {
                printf("//-- erro na requisicao %s: %s. reagendando...\n", url, curl_easy_strerror(res));
                failed_urls[failed_count++] = request_id;
                failed_requests++;
            }

            total_requests++;
            requests_5s++;

            time(&current_time);
            elapsed_time = difftime(current_time, start_time);
            if (elapsed_time >= 1.0)
            {
                avg_per_second = total_requests / elapsed_time;
                printf("\n=================================================================\n");
                printf("media atual de requisicoes por segundo: %.2f\n", avg_per_second);
                printf("=================================================================\n");
            }

            if (difftime(current_time, last_5s_time) >= 5.0)
            {
                avg_per_5_seconds = requests_5s / 5.0;
                printf("\n=================================================================\n");
                printf("media de requisicoes nos ultimos 5 segundos: %.2f\n", avg_per_5_seconds);
                printf("=================================================================\n");
                requests_5s = 0;
                last_5s_time = current_time;
            }
        }

        total_time = difftime(current_time, start_time);
        avg_per_minute = (total_requests / total_time) * 60.0;
        printf("\n=================================================================\n");
        printf("\n+-+- Media final de requisicoes por minuto: %.2f\n", avg_per_minute);
        printf("\n=================================================================\n");
        printf("\n+Total de requisicoes: %d\n", total_requests);
        printf("+Total de requisicoes bem-sucedidas: %d\n", successful_requests);
        printf("+Total de falhas: %d", failed_requests);
        printf("\n=================================================================\n");
        curl_easy_cleanup(curl);
        fclose(logs);
    }

    curl_global_cleanup();
    return 0;
}

size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata)
{
size_t total_size;
ResponseData *response;

    total_size = size * nmemb;
    response = (ResponseData *)userdata;
    if (response->size + total_size < BUFFER_SIZE - 1)
    {
        memcpy(&(response->buffer[response->size]), ptr, total_size);
        response->size += total_size;
        response->buffer[response->size] = '\0';
    }
    return total_size;
}

char *processar_requisicao(int pk_partic, const char *html_response)
{
size_t length;
char *resultado;
char cnpj[30];
const char *start_tag;
char *start;
char *end;

    resultado = (char *)malloc(100 * sizeof(char));
    strcpy(cnpj,"nao consta");
    start_tag = "<span id=\"lbNrPfPj\">";
    start = strstr(html_response, start_tag);
    
    if (start != NULL)
    {
        start += strlen(start_tag);
        end = strstr(start, "</span>");

        if (end != NULL)
        {
            length = end - start;
            strncpy(cnpj, start, length);
            cnpj[length] = '\0';
        }
    }

    snprintf(resultado, 100, "PK_PARTIC %d ------- CNPJ: %s", pk_partic, cnpj);

    return resultado;
}
