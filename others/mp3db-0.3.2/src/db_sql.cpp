#include "db_sql.h"

extern char *DBUSERNAME, *DBPASSWD, *DBNAME, *DBTABLENAME, *DBHOST;
extern int DEBUG_LEVEL;

bool sql_setup()
{
	ofstream sqlfile;
      sqlfile.open(DBHOST, ios:: trunc | ios::app);
	if (sqlfile.good()) {;
		sqlfile << "BEGIN" << endl;
		sqlfile.close();
	} else
		return false;
	return true;
}

bool sql_finish()
{
	ofstream sqlfile;
      sqlfile.open(DBHOST, ios::app);
	if (sqlfile.good()) {
		sqlfile << "COMMIT" << endl;
		sqlfile.close();
	} else
		return false;
	return true;
}


bool sql_commit_mp3(const struct mp3data * mp3)
{
	ofstream sqlfile;
      sqlfile.open(DBHOST, ios::app);
	if (sqlfile.good()) {
		string cmd;
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
		sqlfile << cmd << endl;
		sqlfile.close();
	} else
		return false;
	return true;
}
