#include "/usr/include/pgsql/libpq-fe.h"
#include <unistd.h>
#include <iostream>
#include <string>
#include "../src/mp3scan.h"

using namespace std;

int VERBOSE=1;

PGconn *pg_setup(char *pghost, char *username, char *password, char *dbname);
bool pg_finish(PGconn *conn);
int commit_mp3(PGconn *conn, char *dbname, const struct mp3data *);
char *ltoa(unsigned long);

int main()
{
	char host[]="bofh.evil.malign.net";
	char user[]="bojay";
	char pass[]="bdb4tous";
	char dbname[]="mp3";
	char tablename[]="mp3data";
	PGconn *conn = pg_setup(&host[0],&user[0],&pass[0],&dbname[0]);
	PGresult *res;
	
	// DO DB STUFF
	struct mp3data mp3={
		true,
		"test_of_database.mp3",
		31337,
		"Signal 11",
		"A Simple Title",
		"Genre",
		"Album",
		"No Comment.",
		1999
	};
	if(commit_mp3(conn, &tablename[0], &mp3)){
		if( ! pg_finish(conn)){
			cerr << "Couldn't complete transaction. (Abort!)" << endl;
			return EXIT_FAILURE;
		}
	} else {
		cerr << "Couldn't update record" << endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
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


int commit_mp3(PGconn *conn, char *tablename, const struct mp3data *mp3)
{
	// modify the string to escape ' characters - 
	// try to prevent security problems...
	string filename;
	for(int i=0; i < strlen(mp3->filename.c_str()); i++){
		if(mp3->filename.c_str()[i]=='\'')
			filename += '\'';
		filename += mp3->filename.c_str()[i];
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
	string cmd;
	cmd = "DECLARE myfile CURSOR FOR SELECT filename FROM ";
	cmd += &tablename[0];
	cmd += " WHERE filename = '"; 
	cmd += filename;
	cmd += "'";

	res = PQexec(conn, cmd.c_str());
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		cerr << "Couldn't create a new cursor." << endl;
		PQclear(res);
		PQfinish(conn);
		return 0;
	}
	PQclear(res);
	
	res = PQexec(conn, "FETCH ALL in myfile");
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		cerr << "Couldn't fetch record(s)!" << endl;
		PQclear(res);
		PQfinish(conn);
		return 0;
	}
	
	if (! PQntuples(res)){
		// Nothing in db - commit it!
		cmd = "INSERT INTO ";
		cmd += tablename;
		cmd += " (tag,filename,size";
		if(mp3->tag){
			if(mp3->artist != "null")
				cmd += ",artist";
			if(mp3->title != "null")
				cmd += ",title";
			if(mp3->genre != "null")
				cmd += ",genre";
			if(mp3->album != "null")
				cmd += ",album";
			if(mp3->comment != "null")
				cmd += ",comment";
			if(mp3->year != 0)
				cmd += ",year";
		}
		cmd += ") VALUES (\'";
		cmd += ltoa(mp3->tag);
		cmd += "\', \'";
		cmd += mp3->filename; 
		cmd += "\', \'";
		cmd += ltoa(mp3->size);
		cmd += '\'';
		if(mp3->tag){
			if(mp3->artist != "null"){
				cmd += ", \'";
				cmd += mp3->artist;
				cmd += "\'";
			}
			if(mp3->title != "null"){
				cmd += ", \'";
				cmd += mp3->title;
				cmd += "\'";
			}
			if(mp3->genre != "null"){
				cmd += ", \'";
				cmd += mp3->genre;
				cmd += "\'";
			}
			if(mp3->album != "null"){
				cmd += ", \'";
				cmd += mp3->album;
				cmd += "\'";
			}
			if(mp3->comment != "null"){
				cmd += ", \'";
				cmd += mp3->comment;
				cmd += "\'";
			}
			if(mp3->year != 0){
				cmd += ", \'"; 
				cmd += ltoa(mp3->year);
				cmd += "\'";
			}
		}
		cmd += ')';
		
	} else if (VERBOSE > 0)
			cout << mp3->filename << " is already in db.  Continuing..." << endl;
		
	PQclear(res);
	
	res = PQexec(conn, "CLOSE myfile");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		cerr << "Couldn't close query." << endl;
		PQclear(res);
		PQfinish(conn);
		return 0;
	}
	PQclear(res);

	res = PQexec(conn, cmd.c_str());
	if(!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		cerr << "Couldn't update database! (insert failed)" << endl;
		PQclear(res);
		PQfinish(conn);
		return 0;
	}
	PQclear(res);

	return 1;	
}

char *ltoa(unsigned long rval)
{
	int n=2; // magic number, don't change.
	
	if(rval / 10 <= 0){
		char *x=new char[n];
		if(snprintf(x,n,"%ul",rval)==-1)
			return NULL;
		return x;
	} 
	
	int tmp=rval;
	while(tmp / 10 > 0){
		tmp = tmp / 10;
		n++;
	}
	
	char *x=new char[n];
	if(snprintf(x,n,"%ul",rval)==-1)
		return NULL;
	return x;
}
//
//
//
//
//	to compile: 
//	g++ ptest4.C /usr/lib/libpq.so /usr/lib/libcrypt.so  -o ptest4
//
//
//
//
