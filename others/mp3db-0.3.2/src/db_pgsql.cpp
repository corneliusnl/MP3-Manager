#include "db_pgsql.h"

extern char *DBUSERNAME, *DBPASSWD, *DBNAME, *DBTABLENAME, *DBHOST;
extern PGconn *conn;
extern int DEBUG_LEVEL;

bool pg_setup()
{
	char *pgport, *pgoptions, *pgtty;
	int nFields;

	PGresult *res;

	pgport = NULL;		// port of the backend server

	pgoptions = NULL;	// special options to start up the backend server

	pgtty = NULL;		// debugging tty for the backend server

	//make a connection to the database
	conn = PQsetdbLogin(&DBHOST[0], pgport, pgoptions, pgtty, &DBNAME[0], &DBUSERNAME[0], &DBPASSWD[0]);

	if (PQstatus(conn) == CONNECTION_BAD) {
		cout << "failed." << endl;
		cerr << PQerrorMessage(conn) << endl;
		PQfinish(conn);
		return false;
	}
	// start a transaction block
	if (DEBUG_LEVEL > 2)
		cout << "BEGIN" << endl;

	res = PQexec(conn, "BEGIN");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		cerr << "Can't start a transaction block!" << endl;
		PQclear(res);
		PQfinish(conn);
	}
	PQclear(res);

	return true;
}

bool pg_finish()
{
	PGresult *res;
	if (DEBUG_LEVEL > 2)
		cout << "COMMIT" << endl;

	res = PQexec(conn, "COMMIT");
	// commit the transaction
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		PQclear(res);
		PQfinish(conn);
		return false;
	}
	PQclear(res);
	return true;
}


bool pg_commit_mp3(const struct mp3data * mp3)
{
	PGresult *res;

	// let's see if the file is already in the db
	string cmd;
	cmd = "DECLARE myfile CURSOR FOR SELECT filename FROM ";
	cmd += &DBTABLENAME[0];
	cmd += " WHERE filename = '";
	cmd += escape_string(&mp3->filename);
	cmd += "'";
	if (DEBUG_LEVEL > 2)
		cout << cmd << endl;

	res = PQexec(conn, cmd.c_str());
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		cerr << "Couldn't create a new cursor." << endl;
		PQclear(res);
		PQfinish(conn);
		return 0;
	}
	PQclear(res);

	if (DEBUG_LEVEL > 2)
		cout << "FETCH ALL in myfile" << endl;

	res = PQexec(conn, "FETCH ALL in myfile");
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		cerr << "Couldn't fetch record(s)!" << endl;
		PQclear(res);
		PQfinish(conn);
		return 0;
	}
	if (!PQntuples(res)) {
		// Nothing in db - commit it!
		cmd = "INSERT INTO ";
		cmd += DBTABLENAME;
		cmd += " (tag,filename,size";
		if (mp3->tag) {
			if (mp3->artist != "null")
				cmd += ",artist";
			if (mp3->title != "null")
				cmd += ",title";
			if (mp3->genre != "null")
				cmd += ",genre";
			if (mp3->album != "null")
				cmd += ",album";
			if (mp3->comment != "null")
				cmd += ",comment";
			if (mp3->year != 0)
				cmd += ",year";
		}
		cmd += ") VALUES (\'";
		cmd += ltoa(mp3->tag);
		cmd += "\', \'";
		cmd += escape_string(&mp3->filename);
		cmd += "\', \'";
		cmd += ltoa(mp3->size);
		cmd += '\'';
		if (mp3->tag) {
			if (mp3->artist != "null") {
				cmd += ", \'";
				cmd += escape_string(&mp3->artist);
				cmd += "\'";
			}
			if (mp3->title != "null") {
				cmd += ", \'";
				cmd += escape_string(&mp3->title);
				cmd += "\'";
			}
			if (mp3->genre != "null") {
				cmd += ", \'";
				cmd += escape_string(&mp3->genre);
				cmd += "\'";
			}
			if (mp3->album != "null") {
				cmd += ", \'";
				cmd += escape_string(&mp3->album);
				cmd += "\'";
			}
			if (mp3->comment != "null") {
				cmd += ", \'";
				cmd += escape_string(&mp3->comment);
				cmd += "\'";
			}
			if (mp3->year != 0) {
				cmd += ", \'";
				cmd += ltoa(mp3->year);
				cmd += "\'";
			}
		}
		cmd += ')';

		if (DEBUG_LEVEL > 2)
			cout << cmd << endl;

		PQclear(res);

		res = PQexec(conn, cmd.c_str());
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
			cerr << "Couldn't update database! (insert failed)" << endl;
			PQclear(res);
			PQfinish(conn);
			return 0;

		}
	} else if (DEBUG_LEVEL > 0)
		cout << mp3->filename << " is already in db.  Continuing..." << endl;

	PQclear(res);

	if (DEBUG_LEVEL > 2)
		cout << "CLOSE myfile" << endl;

	res = PQexec(conn, "CLOSE myfile");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		cerr << "Couldn't close query." << endl;
		PQclear(res);
		PQfinish(conn);
		return 0;
	}
	PQclear(res);

	if (DEBUG_LEVEL > 2)
		cout << cmd << endl;

	return 1;
}
