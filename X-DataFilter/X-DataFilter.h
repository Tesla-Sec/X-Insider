#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include ".\curl\include\curl\curl.h"
#include ".\Assets\cabecalho.h"



/** Informacoes dos Ativos */
typedef struct ATIVO
{
    char *typeAplic;
    char *typeAtiv;
    char *codISIN;
    char *desc;
    char *codSELIC;
    char *emission;
    char *vencimento;
    
}ATIVO;

/** Posicao Final */
typedef struct PF
{
    int     qtd;
    float   valoresCoust; /* Valores*/
    float   valoresMarket; /* Valores*/
    float   patrimonioLiquidoPer; /* Percentual*/
    
}PF;

/** Negocios Realizados no Mes */
typedef struct NRM
{
    /* Coluna de Vendas*/
    int     *vendasQtd;
    float   *vendasValue;

    /* Coluna de Aquisicoes (compra)*/
    int     *aquisicoesQtd;
    float   *aquisicoesValue;
    
}NRM;

/** Lista de Aplicacoes */
typedef struct L_APLIC
{
    /* Ativos*/
    ATIVO      *ativo;

    char        *classificacao;
    char        *empresaLigada;

    NRM     *negRealMes; /* Negocios Realizados no Mes*/
    PF      *posicaoFinal;

    struct L_APLIC      *prox;
    struct L_APLIC      *ant;

    /* Metodos*/
        void (*add)(struct L_APLIC *, struct L_APLIC *);
        void (*rm)(struct L_APLIC *, struct L_APLIC *);
}L_APLIC;

/** Lista de Dados do CNPJ */
typedef struct DATA
{
    int id;

    char    *competencia;
    char    *nomeFundo;
    char    *cnpjFundo;

    float   *patrimonioLiq;
    char    *nomeAdm;
    char    *cnpjAdm;

    char    *dateReceive;
    char    *version;

    L_APLIC *aplicacoes;

    struct DATA     *prox;
    struct DATA     *ant;

    /* Metodos*/
        void (*add)(struct DATA *, struct DATA *);
        void (*rm)(struct DATA *, struct DATA *);
        
}DATA, *PDATA;

