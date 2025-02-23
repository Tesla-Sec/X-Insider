/** Defines necessárias para requisicao*/
/*
 *   URL da consulta: https://cvmweb.cvm.gov.br/SWB/Sistemas/SCW/CPublica/CDA/CPublicaCDA.aspx?PK_PARTIC=%d
 *   Total de requisicoes: 100
 *   Codigo HTTP sinoniom de sucesso: 200
 *   PK_PARTIC utilizada como base: 160375
 *
 */
#define URL_FORMAT "https://cvmweb.cvm.gov.br/SWB/Sistemas/SCW/CPublica/CDA/CPublicaCDA.aspx?PK_PARTIC=%d"
#define TOTAL_REQUESTS 100
#define SUCESSO 200
#define REQUEST_BASE 160375

/** Defines para configurar a libCURL */
/*
 *   Tamanho do buffer: 2^(14)
 *   Tamanho da url: 2^(8)
 */
#define BUFFER_SIZE 16384
#define URL_SIZE 256

/** Defines para configurar o arquivo */
/*
 *   Nome do arquivo: pkpartic.txt
 *   Tipo de abertura: w
 */
#define FILENAME "pkpartic.txt"
#define TYPE_OPEN "w"

char *processar_requisicao(int, const char *);
size_t write_callback(void *, size_t, size_t, void *);

typedef struct
{
    char buffer[BUFFER_SIZE];
    size_t size;
} ResponseData;
