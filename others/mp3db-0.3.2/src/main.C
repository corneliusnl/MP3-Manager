#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "stacker.h"
#include "mp3scan.h"
#include "cmdline.h"
#include "db_pgsql.h"
#include "db_sql.h"
#include "db_raw.h"

// some constants for db plugins - don't change
#define DB_TYPE_NONE (0)

#define DB_TYPE_PGSQL (1)

#define DB_TYPE_RAW (2)

#define DB_TYPE_SQL (3)

#ifndef STACK_SIZE
#define STACK_SIZE (8192)
#endif
using namespace std;

const char *VERSION="0.3.2";

char *DBUSERNAME = NULL, *DBPASSWD = NULL, *DBNAME = NULL, *DBTABLENAME = NULL,
*DBHOST = NULL, *SCANPATH = NULL;
PGconn *conn;
int DEBUG_LEVEL = 0;
int DBDRIVER = 0;

int main(int argc, char **argv)
{
	if (!parsecmdline(argc, argv))
		return EXIT_FAILURE;
	cout << "mp3db v" << VERSION << endl;
	Stacker fstack(STACK_SIZE);

	string path = SCANPATH;
	
	// engage in questionable semantics to try to resolve
	// the absolute pathname for the path given.
	// we'd like to keep dup entries in the db to a minimum.

	if(SCANPATH[0] != '/')
		{
		string fullpath;
		char *tmp = new char[PATH_MAX];
		if(getcwd(tmp,PATH_MAX) && tmp == NULL){
			cerr << "Error attempting to read directory \"" << SCANPATH << "\"" <<endl;
			return EXIT_FAILURE;
		} else {
			fullpath += tmp;
			free(tmp);
			int slen = strlen(SCANPATH);
			int is_path=0;
			for(int i=-1; i < slen ; i++){
				if(is_path || SCANPATH[i] == '/'){
					fullpath += SCANPATH[i];	
					is_path=1;
				}
			}
			path = fullpath;
			if(! is_path){
				path += "/";
				path += SCANPATH;
			}
		}
	}
	
	delete SCANPATH;
	
	cout << "Scanning " << path << "..." << flush;
	
	if (ListFilesUnder(path, &fstack) != 0) {
		cerr << "failed. " << flush;
		if(fstack.count()== STACK_SIZE)
			cerr << "( stack is full )" << endl;
		else
			cerr << "( bad pathname? )" << endl;
		return EXIT_FAILURE;
	}
	cout << "done." << endl;
	cout << "Connecting to remote database..." << flush;
	switch (DBDRIVER) {
	case 1:
		if (pg_setup())
			cout << "done." << endl << flush;
		else
			return EXIT_FAILURE;
		break;
	case 2:
		if (raw_setup())
			cout << "done." << endl << flush;
		else
			return EXIT_FAILURE;
		break;
	case 3:
		if (sql_setup())
			cout << "done." << endl << flush;
		else
			return EXIT_FAILURE;
		break;
	default:
		cout << "Can't happen." << endl;
		return EXIT_FAILURE;
	}

	while (fstack.count()) {
		string tmp;
		fstack.pop(tmp);
		struct mp3data *mp3;
		if ((mp3 = scanmp3(&tmp)) == NULL) {
			cerr << "Can't read: " << tmp << endl;
		} else if (DEBUG_LEVEL > 0) {
			cout << mp3->filename << endl
			    << "Size:\t" << mp3->size << endl;
			if (mp3->tag) {
				cout << "Title\t" << mp3->title << endl
				    << "Artist\t" << mp3->artist << endl
				    << "Album:\t" << mp3->album << endl
				    << "Year:\t" << mp3->year << endl
				    << "Comment:\t" << mp3->comment << endl
				    << "Genre:\t" << mp3->genre << endl;
			}
		}
		switch (DBDRIVER) {
		case 1:
			if (!pg_commit_mp3(mp3)) {
				cout << "Couldn't add \"" << mp3->filename << "\" to database!" << endl;
				return EXIT_FAILURE;
			}
			break;
		case 2:
			if (!raw_commit_mp3(mp3)) {
				cout << "Couldn't add \"" << mp3->filename << "\" to database!" << endl;
				return EXIT_FAILURE;
			}
			break;
		case 3:
			if (!sql_commit_mp3(mp3)) {
				cout << "Couldn't add \"" << mp3->filename << "\" to database!" << endl;
				return EXIT_FAILURE;
			}
			break;
		default:
			cout << "Can't happen" << endl;
			return EXIT_FAILURE;
		}
	}

	switch (DBDRIVER) {
	case 1:
		if (pg_finish()) {
			cout << "Transaction completed.  Have a nice day!" << endl;
			return EXIT_SUCCESS;
		} else {
			cout << "Transaction failed.  Sorry." << endl;
			return EXIT_FAILURE;
		}
		break;
	case 2:
		if (raw_finish()) {
			cout << "Transaction completed.  Have a nice day!" << endl;
			return EXIT_SUCCESS;
		} else {
			cout << "I/O error writing to file?  Contents under pressure, open with care." << endl;
			return EXIT_FAILURE;
		}
		break;
	case 3:
		if (sql_finish()) {
			cout << "Transaction completed.  Have a nice day!" << endl;
			return EXIT_SUCCESS;
		} else {
			cout << "I/O error writing to file?  Contents under pressure, open with care." << endl;
			return EXIT_FAILURE;
		}
		break;
	default:
		cout << "Can't happen" << endl;
		return EXIT_FAILURE;
	}
}
