

/*
 * testlibpq.c Test the C version of Libpq, the Postgres frontend
 * library.
 *
 *
 */
#include <stdio.h>
#include "/usr/include/pgsql/libpq-fe.h"
#include <unistd.h>

void exit_nicely(PGconn * conn)
{
	PQfinish(conn);
	exit(1);
}

main()
{
	char pghost[]="bofh.evil.malign.net";
	char *pgport, *pgoptions, *pgtty;
	char username[]="bojay";
	char password[]="bdb4tous";
	char dbName[]="mp3";
	int nFields;
	int i, j;

	/* FILE *debug; */

	PGconn *conn;
	PGresult *res;

	/*
	 * begin, by setting the parameters for a backend connection if the
	 * parameters are null, then the system will try to use reasonable
	 * defaults by looking up environment variables or, failing that,
	 * using hardwired constants
	 */
	// pghost = NULL;		/* host name of the backend server */
	pgport = NULL;		/* port of the backend server */
	pgoptions = NULL;	/* special options to start up the backend
				 * server */
	pgtty = NULL;		/* debugging tty for the backend server */
	//dbName = NULL;

	/* make a connection to the database */
	conn = PQsetdbLogin(&pghost[0], pgport, pgoptions, pgtty, &dbName[0], &username[0], &password[0]);

	/*
	 * check to see that the backend connection was successfully made
	 */
	if (PQstatus(conn) == CONNECTION_BAD) {
		fprintf(stderr, "Connection to database '%s' failed.\n", dbName);
		fprintf(stderr, "%s", PQerrorMessage(conn));
		exit_nicely(conn);
	}
	/* debug = fopen("/tmp/trace.out","w"); */
	/* PQtrace(conn, debug);  */

	/* start a transaction block */
	res = PQexec(conn, "BEGIN");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "BEGIN command failed\n");
		PQclear(res);
		exit_nicely(conn);
	}
	/*
	 * should PQclear PGresult whenever it is no longer needed to avoid
	 * memory leaks
	 */
	PQclear(res);

	/*
	 * fetch instances from the pg_database, the system catalog of
	 * databases
	 */
	res = PQexec(conn, "DECLARE mycursor CURSOR FOR select * from pg_database");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "DECLARE CURSOR command failed\n");
		PQclear(res);
		exit_nicely(conn);
	}
	PQclear(res);
	res = PQexec(conn, "FETCH ALL in mycursor");
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "FETCH ALL command didn't return tuples properly\n");
		PQclear(res);
		exit_nicely(conn);
	}
	/* first, print out the attribute names */
	nFields = PQnfields(res);
	for (i = 0; i < nFields; i++)
		printf("%-15s", PQfname(res, i));
	printf("\n\n");

	/* next, print out the instances */
	for (i = 0; i < PQntuples(res); i++) {
		for (j = 0; j < nFields; j++)
			printf("%-15s", PQgetvalue(res, i, j));
		printf("\n");
	}
	PQclear(res);

	/* close the cursor */
	res = PQexec(conn, "CLOSE mycursor");
	PQclear(res);

	/* commit the transaction */
	res = PQexec(conn, "COMMIT");
	PQclear(res);

	/* close the connection to the database and cleanup */
	PQfinish(conn);

	/* fclose(debug); */
}





/*
to compile: 

gcc postgres_test1.C /usr/lib/libpq.so /usr/lib/libcrypt.so  -o ptest1

*/
