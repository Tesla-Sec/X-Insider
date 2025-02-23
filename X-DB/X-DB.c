#include "X-DB.h"


/* Executa uma query simples e retorna um DBResult */
DBResult DBQuery(DB *baseDados, const char *query) {
    DBResult res;
    char *errMsg = NULL;
    int rc = sqlite3_get_table(baseDados->db, query, &res.results, &res.nrows, &res.ncols, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error (DBQuery): %s\n", errMsg);
        sqlite3_free(errMsg);
        res.results = NULL;
        res.nrows = res.ncols = 0;
    }
    return res;
}

/* Versão com formatação (varargs) para executar queries */
DBResult DBQueryf(DB *baseDados, const char *query, ...) {
    char query_buffer[1024];
    va_list args;
    va_start(args, query);
    vsnprintf(query_buffer, sizeof(query_buffer), query, args);
    va_end(args);
    return DBQuery(baseDados, query_buffer);
}

/* Cria uma tabela. Os argumentos variáveis são as definições das colunas, terminando com NULL.
   Exemplo de uso:
       CreateTable(db, "usuarios", "id INTEGER PRIMARY KEY", "nome TEXT", "idade INTEGER", NULL);
*/
int CreateTable(DB *baseDados, const char *name, ...) {
    char sql[2048];
    snprintf(sql, sizeof(sql), "CREATE TABLE IF NOT EXISTS %s (", name);

    va_list args;
    va_start(args, name);
    const char *col;
    int first = 1;
    while ((col = va_arg(args, const char *)) != NULL) {
        if (!first) {
            strncat(sql, ", ", sizeof(sql) - strlen(sql) - 1);
        }
        strncat(sql, col, sizeof(sql) - strlen(sql) - 1);
        first = 0;
    }
    va_end(args);
    strncat(sql, ");", sizeof(sql) - strlen(sql) - 1);

    char *errMsg = NULL;
    int rc = sqlite3_exec(baseDados->db, sql, NULL, NULL, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error (CreateTable): %s\n", errMsg);
        sqlite3_free(errMsg);
    }
    return rc;
}

/* Se o parâmetro 'where' for fornecido (não vazio), executa um DELETE da tabela;
   caso contrário, executa um DROP TABLE IF EXISTS. */
int DropTable(DB *baseDados, const char *name, const char *where) {
    char sql[1024];
    if (where && strlen(where) > 0) {
        snprintf(sql, sizeof(sql), "DELETE FROM %s WHERE %s;", name, where);
    } else {
        snprintf(sql, sizeof(sql), "DROP TABLE IF EXISTS %s;", name);
    }
    char *errMsg = NULL;
    int rc = sqlite3_exec(baseDados->db, sql, NULL, NULL, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error (DropTable): %s\n", errMsg);
        sqlite3_free(errMsg);
    }
    return rc;
}

/* Atualiza registros da tabela.
   set_clause: por exemplo, "idade = 30"
   where_clause: por exemplo, "nome = 'João'" (pode ser NULL para atualizar todos)
*/
int UpdateTable(DB *baseDados, const char *table, const char *set_clause, const char *where_clause) {
    char sql[1024];
    if (where_clause && strlen(where_clause) > 0) {
        snprintf(sql, sizeof(sql), "UPDATE %s SET %s WHERE %s;", table, set_clause, where_clause);
    } else {
        snprintf(sql, sizeof(sql), "UPDATE %s SET %s;", table, set_clause);
    }
    char *errMsg = NULL;
    int rc = sqlite3_exec(baseDados->db, sql, NULL, NULL, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error (UpdateTable): %s\n", errMsg);
        sqlite3_free(errMsg);
    }
    return rc;
}

/* Insere valores na tabela.
   columns: lista de colunas, por exemplo, "nome, idade"
   values: lista de valores, por exemplo, "'Maria', 25"
*/
int InsertValues(DB *baseDados, const char *table, const char *columns, const char *values) {
    char sql[1024];
    snprintf(sql, sizeof(sql), "INSERT INTO %s (%s) VALUES (%s);", table, columns, values);
    char *errMsg = NULL;
    int rc = sqlite3_exec(baseDados->db, sql, NULL, NULL, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error (InsertValues): %s\n", errMsg);
        sqlite3_free(errMsg);
    }
    return rc;
}

/* Abre (ou cria) um banco de dados SQLite e retorna um ponteiro para a estrutura DB */
DB *OpenDB(const char *filename) {
    DB *baseDados = malloc(sizeof(DB));
    if (!baseDados) {
        fprintf(stderr, "Erro de alocação de memória.\n");
        return NULL;
    }
    int rc = sqlite3_open(filename, &baseDados->db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Não foi possível abrir o banco de dados: %s\n", sqlite3_errmsg(baseDados->db));
        free(baseDados);
        return NULL;
    }
    return baseDados;
}

void CloseDB(DB *baseDados) {
    if (baseDados) {
        sqlite3_close(baseDados->db);
        free(baseDados);
    }
}

void FreeDBResult(DBResult res) {
    if (res.results) {
        sqlite3_free_table(res.results);
    }
}

char *strfmt(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int size = vsnprintf(0, 0, fmt, args) + 1;
    va_end(args);
    char *str = malloc(size);
    va_start(args, fmt);
    vsnprintf(str, size, fmt, args);
    va_end(args);
    return str;
}