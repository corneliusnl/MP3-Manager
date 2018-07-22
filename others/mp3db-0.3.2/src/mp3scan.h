
#ifndef _H_MP3SCAN_
#define _H_MP3SCAN_

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "stacker.h"
#include "mp3scan.h"

struct mp3data *scanmp3(string *);
string getstring(ifstream *, int);
bool suck(ifstream *);
bool IsDir(string *);
int ListFilesUnder(string, Stacker *);
char *ltoa(unsigned long);
string escape_string(const string *, bool strip_tabs = false);

struct mp3data {
	// required
	bool tag;
	string filename;
	unsigned long size;
	// optional
	string artist;
	string title;
	string genre;
	string album;
	string comment;
	unsigned int year;
};

#endif
