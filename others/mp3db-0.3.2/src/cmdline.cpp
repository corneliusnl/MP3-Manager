#include "cmdline.h"

extern char *DBUSERNAME, *DBPASSWD, *DBNAME, *DBTABLENAME, *DBHOST,
*SCANPATH;
extern int DEBUG_LEVEL;
extern int DBDRIVER;
extern char *VERSION;

bool parsecmdline(int argc, char **argv)
{
	int csize = 0;		// largest string length in argv
	 {
		int i = argc;
		while (i-- > 0) {
			int tmp = strlen(argv[i]);
			if (csize < tmp) {
				csize = tmp;
			}
		}
	}
	csize++;		// needed to make sure a \0 is at the end of the largest string.
	
	char *foo = getusername();
	if(strlen(foo) > csize){
		csize = strlen(foo);
	}
	
	if(csize < 10){
		csize = 10;
	}
	delete foo;
	
	char user[csize], passwd[csize], dbname[csize], dbtablename[csize],
	 dbhost[csize], scanpath[csize], dbdriver[csize];
	int help = 0;

	arg_rec argtable[] =
	{
		{"-u", " <username>", arg_str, user, getusername(), "\t\t\tusername to access db"},
		{"-p", " <passwd>", arg_str, passwd, "", "\t\t\tpassword to access db"},
		{"-n", " <db name>", arg_str, dbname, "mp3", "\t\t\tDatabase to access (\'mp3\' is the default)"},
		{"-t", " <db table>", arg_str, dbtablename, "mp3data", "\t\t\tTable to update (\'mp3data\' is the default)"},
		{"-H", " <host:port>", arg_str, dbhost, "localhost", "\t\thost or IP address:port (default is to access localhost)"},
		{"-h", NULL, arg_lit, &help, "0", "\t\t\t\tDisplay this help message."},
		{"-D", " <level>", arg_int, &DEBUG_LEVEL, "0", "\t\t\tDebug level [0-9]"},
		{"-d", " <driver>", arg_str, dbdriver, "pgsql", "\t\t\tdatabase driver to use [pgsql|sql|raw]"},
	  {NULL, "<path>", arg_str, scanpath, NULL, "\t\t\tPath to scan"}
	};
	const size_t narg = sizeof(argtable) / sizeof(arg_rec);

	if (argc >= 96) {	// argtable library overflows if argc > 96 for some reason.  

		cout << "Usage: " << argv[0] << " " << arg_syntax(argtable, narg) << endl << endl;
		cout << arg_glossary(argtable, narg, "  ") << endl;
		return false;
	}
	int addon = 0;
	if (csize < 200)
		addon = 200 - csize;
	char cmdline[csize + addon], errmsg[csize + addon], errmark[csize + addon];
	if (!arg_scanargv(argc, argv, argtable, narg, cmdline, errmsg, errmark)) {
		if (help) {
			cout << "mp3db version " << VERSION << endl;
			cout << "Usage: " << argv[0] << " " << arg_syntax(argtable, narg) << endl << endl;
			cout << arg_glossary(argtable, narg, "  ") << endl;
			cout << "Examples: " << argv[0] << " -u guest -p guest -n dbserver.somewhere.org ~/mp3s/" << endl;
			cout << "          " << argv[0] << " -d raw -H exportfile.txt ~/mp3s/" << endl;
			return false;
		}
		cerr << "Error: " << cmdline << endl;
		cerr << "       " << errmark << " " << errmsg << endl;
		return false;
	}
	// parsing successful, let's assign the values to the global variables.
	DBUSERNAME = new char[strlen(user) + 1];
	strcpy(DBUSERNAME, user);

	if (strcmp(passwd, "") == 0)
		DBPASSWD = NULL;
	else {
		DBPASSWD = new char[strlen(passwd) + 1];
		strcpy(DBPASSWD, passwd);
	}

	DBNAME = new char[strlen(dbname) + 1];
	strcpy(DBNAME, dbname);

	DBTABLENAME = new char[strlen(dbtablename) + 1];
	strcpy(DBTABLENAME, dbtablename);
	
	DBHOST = new char[strlen(dbhost) + 1];
	strcpy(DBHOST, dbhost);

	SCANPATH = new char[strlen(scanpath) + 1];
	strcpy(SCANPATH, scanpath);

	if (strcasecmp(dbdriver, "pgsql") == 0)
		DBDRIVER = 1;
	else if (strcasecmp(dbdriver, "raw") == 0)
		DBDRIVER = 2;
	else if (strcasecmp(dbdriver, "sql") == 0)
		DBDRIVER = 3;
	else
		DBDRIVER = 0;

	return true;
}

char *getusername()
{
	struct passwd *tmp = getpwuid(getuid());
	char *foo = new char[strlen(tmp->pw_name)];
	strcpy(foo, tmp->pw_name);
	return foo;
}
