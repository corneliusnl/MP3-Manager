#include <iostream>
#include <vector>
#include <string>
using namespace std;

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include "stacker.cpp"

bool IsDir(string *);
int ListFilesUnder(string, Stacker *);

int main(int argc, char **argv)
{
	string path;
	cout << "Enter path: ";
	cin >> path;

	cerr << "Scanning " << path << "...";

	Stacker fstack(8192);
	if(ListFilesUnder(path, &fstack) != 0){
		cerr << "failed.  (Stack is full)" << endl;
		return EXIT_FAILURE;
	}
	
	cout << "done." << endl;
	
	while(fstack.count()){
		string tmp;
		fstack.pop(tmp);
		cout << tmp << endl;
	}
	return EXIT_SUCCESS;
}

bool IsDir(string *suspect)
{
	DIR *tmp;
	if ((tmp = opendir(suspect->c_str())) != NULL) {
		closedir(tmp);
		return true;
	}
	return false;
}

int ListFilesUnder(string path, Stacker *fstack)
{
	struct dirent **namelist;
	string fullpath;
	int i = scandir(path.c_str(), &namelist, 0, alphasort);

	if (i < 1)
		return 1;

	while(i-- > 0){
		fullpath = path;
		
		if (strcmp(namelist[i]->d_name,".") != 0){
			if (path[path.size() - 1] != '/'){
				path += "/";
			}
			fullpath += namelist[i]->d_name;
		}
		
		if (IsDir(&fullpath)){
			if (strcmp(namelist[i]->d_name, ".") != 0 && strcmp(namelist[i]->d_name, "..") != 0){
				struct stat info;
				lstat(fullpath.c_str(), &info);
				if (!(info.st_mode & S_IFLNK)){	// don't follow symbolic links
					if(ListFilesUnder(fullpath, fstack) == 2)
						return 2;
				}
			}
		} else {
			if(fullpath.find(".mp3") != string::npos || fullpath.find(".MP3") != string::npos)
				if(! fstack->push(fullpath)){
					return 2;
				}
		}
	}
	return 0;
}
