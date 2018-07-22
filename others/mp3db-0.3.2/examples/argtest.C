// program notes 
// --------------
// the argtable library is replete with buffer overflows.
// The author states on his homepage that he plans on having this
// fixed in the next release of his library.  In the meantime,
// I have done my best to eliminate the conditions which cause 
// this library to overflow.  


#include <iostream>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include "../argtable/argtable.h"

using namespace std;

char *DBUSERNAME,*DBPASSWD,*DBNAME,*DBTABLENAME,*DBHOST;
unsigned short int DEBUG_LEVEL;

bool parsecmdline(int argc, char **argv);
char *getusername();

int main(int argc, char **argv)
{
	
	if(parsecmdline(argc,argv)){
		cout << "Command line parsing successful." << endl;
		cout << "user\t"   	<< DBUSERNAME << endl
		     << "dbname\t" 	<< DBNAME << endl
		     << "dbtname\t" 	<< DBTABLENAME << endl
		     << "host\t"	<< DBHOST << endl;
	} 
	
	return 0;
	
} 

bool parsecmdline(int argc, char **argv)
{
int csize=0; // largest string length in argv
	{
		int i=argc;
		while (i-- > 0){
			int tmp=strlen(argv[i]);
			if(csize < tmp){
				csize = tmp;
			}
		}
	}
	csize++; // needed to make sure a \0 is at the end of the largest string.
	
	char user[csize],passwd[csize],dbname[csize],dbtablename[csize],dbhost[csize];
	int help;
	
	arg_rec argtable[] = {
		{ "-u", " <username>"	, arg_str, user, getusername(), "\t\t\tusername to access db" },
		{ "-p", " <passwd>"	, arg_str, passwd, "", "\t\t\tpassword to access db" },
		{ "-d", " <db name>"	, arg_str, dbname, "mp3", "\t\t\tDatabase to access (\'mp3\' is the default)"},
		{ "--table-name", " <db table>", arg_str, dbtablename, "mp3data", "\tTable to update (\'mp3data\' is the default)"},
		{ "-H", " <host:port>"	, arg_str, dbhost, "localhost", "\t\thost or IP address:port (default is to access localhost)"},
		{ "-h", NULL		, arg_lit, &help, "0","\t\t\tDisplay this help message."},
		{ "-D", " <level>"	, arg_int, &DEBUG_LEVEL, "0", "\t\t\tDebug level [0-9]" }
		
	};
	const size_t narg = sizeof(argtable)/sizeof(arg_rec);
	if(argc >= 96){ // argtable library overflows if argc > 96 for some reason.  
		cout << "Usage: " << argv[0] << " " << arg_syntax(argtable,narg) << endl << endl;
		cout << arg_glossary(argtable,narg,"  ") << endl;
		return false;
	} else {
		int addon=0;
		if(csize < 200)
			addon=200-csize;
		char cmdline[csize+addon], errmsg[csize+addon], errmark[csize+addon];
		if (!arg_scanargv(argc, argv, argtable, narg, cmdline, errmsg, errmark)){
			cerr << "Error: " << cmdline << endl;
			cerr << "       " << errmark << " " << errmsg << endl;
			return false;
		}
	}
	
	// parsing successful, let's assign the values to the global variables.
	if(help){
		cout << "Usage: " << argv[0] << " " << arg_syntax(argtable,narg) << endl << endl;
		cout << arg_glossary(argtable,narg,"  ") << endl;
		return false;
	}
	DBUSERNAME=new char[strlen(user)];
	strcpy(DBUSERNAME,user);
	
	if(strcmp(passwd,"")==0)
		DBPASSWD=NULL;
	else {
		DBPASSWD=new char[strlen(passwd)];
		strcpy(DBPASSWD,passwd);
	}
	
	DBNAME=new char[strlen(dbname)];
	strcpy(DBNAME,dbname);
	
	DBTABLENAME=new char[strlen(dbtablename)];
	strcpy(DBTABLENAME,dbtablename);
	
	DBHOST=new char[strlen(dbhost)];
	strcpy(DBHOST,dbhost);
	
	return true;
}

char *getusername()
{
	struct passwd *tmp=getpwuid(getuid());
	char *foo=new char[strlen(tmp->pw_name)];
	strcpy(foo,tmp->pw_name);
	return foo;
}
