#include <sqlite3.h>
#include <stdio.h>

int callback(void *NotUsed, int argc, char **argv, char **azColName);

int main(void) {

    sqlite3 *db;
    sqlite3_stmt *res;

		char *err_msg = 0;

    int rc = sqlite3_open("inventory.sqlite3", &db);

    if (rc != SQLITE_OK) {

        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return 1;
    }

    rc = sqlite3_prepare_v2(db, "SELECT SQLITE_VERSION()", -1, &res, 0);

    if (rc != SQLITE_OK) {

        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return 1;
    }

    rc = sqlite3_step(res);

    if (rc == SQLITE_ROW) {
        printf("SQLITE_VERSION == %s\n\n", sqlite3_column_text(res, 0));
    }

    sqlite3_finalize(res);

		rc = sqlite3_db_config(db,SQLITE_DBCONFIG_ENABLE_LOAD_EXTENSION,1,NULL);

    if (rc != SQLITE_OK) {

        fprintf(stderr, "Cannot config database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return 1;
    }

		rc = sqlite3_load_extension(db,"sqlite3/csv.so",0, &err_msg);

	  if (rc != SQLITE_OK) {

        fprintf(stderr, "Failed to load extension: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return 1;
    }

    rc = sqlite3_exec(db, "CREATE VIRTUAL TABLE temp.t1 USING csv(filename='inventory.csv',header=true)", 0, 0, &err_msg);

    if (rc != SQLITE_OK) {

        fprintf(stderr, "Failed to create virtual table: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return 1;
    }

    rc = sqlite3_exec(db, "DROP TABLE IF EXISTS items", 0, 0, &err_msg);

    if (rc != SQLITE_OK) {

        fprintf(stderr, "Failed to drop  table: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return 1;
    }

    rc = sqlite3_exec(db, "CREATE TABLE items AS SELECT * FROM t1", 0, 0, &err_msg);

    if (rc != SQLITE_OK) {

        fprintf(stderr, "Failed to create virtual table: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return 1;
    }


		rc = sqlite3_exec(db, "SELECT * FROM items", callback, 0, &err_msg);

    if (rc != SQLITE_OK ) {

        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        return 1;
    }

    sqlite3_close(db);

    return 0;
}



int callback(void *NotUsed, int argc, char **argv, char **azColName) {

    NotUsed = 0;

    for (int i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");

    return 0;
}
