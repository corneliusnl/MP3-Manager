#include "db_raw.h"

extern char *DBUSERNAME, *DBPASSWD, *DBNAME, *DBTABLENAME, *DBHOST;
extern int DEBUG_LEVEL;

bool raw_setup()
{
	ofstream sqlfile;
      sqlfile.open(DBHOST, ios:: trunc | ios::app);
	if (sqlfile.good()) {;
		sqlfile.close();
	} else
		return false;
	return true;
}

bool raw_finish()
{
	return true;
}


bool raw_commit_mp3(const struct mp3data * mp3)
{
	ofstream sqlfile;
	sqlfile.open(DBHOST, ios::app);
	if (sqlfile.good()) {
		string cmd;
		cmd += ltoa(mp3->tag);
		cmd += "\t";
		cmd += escape_string(&mp3->filename, 1);
		cmd += "\t";
		cmd += ltoa(mp3->size);
		if (mp3->tag) {
			if (mp3->artist != "null") {
				cmd += "\t";
				cmd += escape_string(&mp3->artist, 1);
			} else
				cmd += "\t";
			if (mp3->title != "null") {
				cmd += "\t";
				cmd += escape_string(&mp3->title, 1);
			} else
				cmd += "\t";
			if (mp3->genre != "null") {
				cmd += "\t";
				cmd += escape_string(&mp3->genre, 1);
			} else
				cmd += "\t";
			if (mp3->album != "null") {
				cmd += "\t";
				cmd += escape_string(&mp3->album, 1);
			} else
				cmd += "\t";
			if (mp3->comment != "null") {
				cmd += "\t";
				cmd += escape_string(&mp3->comment, 1);
			} else
				cmd += "\t";
			if (mp3->year != 0) {
				cmd += "\t";
				cmd += ltoa(mp3->year);
			} else
				cmd += "\t";
		}
		sqlfile << cmd << endl;
		sqlfile.close();
	} else
		return false;
	return true;
}
