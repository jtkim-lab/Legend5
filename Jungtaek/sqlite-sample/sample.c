#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	int i;

	for(i = 0; i < argc; i++)
	{
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}

	printf("\n");

	return 0;
}

int main(int argc, char* argv[])
{
	sqlite3 *db;
	int ret;
	char* sql;
	char* err = 0;

	ret = sqlite3_open("sample.db", &db);

	if(ret)
	{
		exit(0);
	}
	else
	{
		printf("I am here!\n");
	}

	sql = "CREATE TABLE SENTENCE(" \
		  "ID      INTEGER PRIMARY KEY   AUTOINCREMENT NOT NULL," \
		  "TIME    TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL," \
	      "IDARRAY CHAR(100)                           NOT NULL," \
		  "ETC     TEXT);";

	ret = sqlite3_exec(db, sql, callback, 0, &err);

	if(ret == SQLITE_OK)
	{
		printf("I have already been here!\n");
	}
	else
	{
		printf("I am not here!\n");
		sqlite3_free(err);
	}

	sql = "INSERT INTO SENTENCE (IDARRAY)"  \
		  "VALUES ('123');";

	ret = sqlite3_exec(db, sql, callback, 0, &err);

	if(ret == SQLITE_OK)
	{
		printf("I said I was here!\n");
	}
	else
	{
		printf("I lost my way..\n");
	}

	sqlite3_close(db);
	return 0;
}
