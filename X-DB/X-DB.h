#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "./sqlite-autoconf-3490100/sqlite3.h"
#include "./sqlite-autoconf-3490100/sqlite3ext.h"

/* Estrutura que representa a conexão com o banco de dados */
typedef struct {
    sqlite3 *db;
} DB;

/* Estrutura para armazenar o resultado de uma query (similar ao psql) */
typedef struct {
    int nrows;      // número de linhas (excluindo cabeçalho)
    int ncols;      // número de colunas
    char **results; // matriz de strings (primeira linha: nomes das colunas)
} DBResult;

DBResult DBQuery(DB *baseDados, const char *query);
DBResult DBQueryf(DB *baseDados, const char *query, ...);
int CreateTable(DB *baseDados, const char *name, ...); // varargs: cada coluna como string; último argumento NULL
int DropTable(DB *baseDados, const char *name, const char *where); // se where != NULL e não vazio, executa DELETE
int UpdateTable(DB *baseDados, const char *table, const char *set_clause, const char *where_clause);
int InsertValues(DB *baseDados, const char *table, const char *columns, const char *values);
DB *OpenDB(const char *filename);
void CloseDB(DB *baseDados);
void FreeDBResult(DBResult res);
char *strfmt(const char *, ...);