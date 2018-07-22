#include "/usr/include/pgsql/libpq-fe.h"
#include <unistd.h>
#include <iostream>
#include <string>
using namespace std;

PGconn *pg_setup(char *pghost, char *username, char *password, char *dbname);
bool pg_finish(PGconn *conn);
int commit_mp3(PGconn *conn, char *dbname, char *tmp);

int main()
{
	char host[]="bofh.evil.malign.net";
	char user[]="bojay";
	char pass[]="bdb4tous";
	char dbname[]="mp3";
	char tablename[]="mp3data";
	PGconn *conn = pg_setup(&host[0],&user[0],&pass[0],&dbname[0]);
	PGresult *res;
	//int nFields,i,j;
	
	// DO DB STUFF
	char mp3[]="/export/music/mp3/rap/NWA - Straight Outta Compton/02 - Fuck Tha Police.mp3";
	commit_mp3(conn, &tablename[0], &mp3[0]);

	//pg_finish(conn);

}

PGconn *pg_setup(char *pghost, char *username, char *password, char *dbname)
{
	char *pgport, *pgoptions, *pgtty;
	int nFields;

	PGconn *conn;
	PGresult *res;

	pgport = NULL;		// port of the backend server
	pgoptions = NULL;	// special options to start up the backend server
	pgtty = NULL;		// debugging tty for the backend server

	//make a connection to the database
	conn = PQsetdbLogin(&pghost[0], pgport, pgoptions, pgtty, &dbname[0], &username[0], &password[0]);

	if (PQstatus(conn) == CONNECTION_BAD) {
		cout << "Connection to database " <<  dbname << " failed." << endl;
		cerr << PQerrorMessage(conn) << endl;
		PQclear(res);
		PQfinish(conn);
	}

	return conn;
}

bool pg_finish(PGconn *conn)
{
	PGresult *res;
	res = PQexec(conn, "COMMIT");
	// commit the transaction
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		cout << "Couldn't commit session!!" << endl;
		PQclear(res);
		PQfinish(conn);
		return false;
	}
	PQclear(res);
	return true;
}


int commit_mp3(PGconn *conn, char *tablename, char *tmp)
{
	// modify the string to escape ' characters - 
	// try to prevent security problems...
	string filename;
	for(int i=0; i < strlen(tmp); i++){
		if(tmp[i]=='\'')
			tmp += '\'';
		filename += tmp[i];
	}

	PGresult *res;
	
	// start a transaction block
	res = PQexec(conn, "BEGIN");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		cerr << "Can't start a transaction block!" << endl;
		PQclear(res);
		PQfinish(conn);
	}
	PQclear(res);
	
	// let's see if the file is already in the db
	string selectcmd;
	selectcmd = "DECLARE myfile CURSOR FOR SELECT * from mp3data";
	//selectcmd = "DECLARE myfile CURSOR FOR ";
	//selectcmd += "SELECT filename FROM ";
	//selectcmd += tablename;
	//selectcmd += " WHERE filename = '";
	//selectcmd += "*"; // was filename
	//selectcmd += "'";
	cout << selectcmd << endl;
	
	res = PQexec(conn, selectcmd.c_str());
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		cout << "Couldn't create a new cursor." << endl;
		PQclear(res);
		PQfinish(conn);
		return 0;
	}
	PQclear(res);
	
	res = PQexec(conn, "FETCH ALL in myfile");
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		cout << "Couldn't fetch record(s)!" << endl;
		PQclear(res);
		PQfinish(conn);
		return 0;
	}
	
	cout << "Data: " << endl;
	for (int i = 0; i < PQntuples(res); i++) {
			cout << ":" << PQgetvalue(res, i, 1) << endl;
	}
	
	PQclear(res);
	res = PQexec(conn, "CLOSE myfile");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		cout << "Couldn't close query." << endl;
		PQclear(res);
		PQfinish(conn);
		return 0;
	}
	PQclear(res);

}
//
//
//
//
//	to compile: 
//	g++ postgres_test1.C /usr/lib/libpq.so /usr/lib/libcrypt.so  -o ptest1
//
//
//
//
