#include "mp3scan.h"

bool IsDir(string * suspect)
{
	DIR *tmp;
	if ((tmp = opendir(suspect->c_str())) != NULL) {
		closedir(tmp);
		return true;
	}
	return false;
}

int ListFilesUnder(string path, Stacker * fstack)
{
	struct dirent **namelist;
	string fullpath;
	int i = scandir(path.c_str(), &namelist, 0, alphasort);

	if (i < 1)
		return 1;

	while (i-- > 0) {
		fullpath = path;
		
		if (strcmp(namelist[i]->d_name, ".") != 0){
			if(path[path.size()-1] != '/'){
				fullpath += "/";
			}
			fullpath += namelist[i]->d_name;
		}
		if (IsDir(&fullpath)) {
			if (strcmp(namelist[i]->d_name, ".") != 0 && strcmp(namelist[i]->d_name, "..") != 0) {
				struct stat info;
				lstat(fullpath.c_str(), &info);
				if (!(info.st_mode & S_IFLNK)) {	// don't follow symbolic links

					if (ListFilesUnder(fullpath, fstack) == 2)
						return 2;
				}
			}
		} else {
		      if (fullpath.find(".mp3") != string:: npos || fullpath.find(".MP3") != string::npos)
				if (!fstack->push(fullpath)) {
					return 2;
				}
			// pedantic programmers may wish to modify the if statement to only look at the last 4 
			// characters of the string.
		}
	}
	return 0;
}

struct mp3data *scanmp3(string * fname)
{
	struct mp3data *mp3 = new struct mp3data;
	// let's zero everything before going any farther.
	mp3->tag = false;
	mp3->filename = "";
	mp3->size = 0;
	mp3->artist = "";
	mp3->title = "";
	mp3->genre = "";
	mp3->album = "";
	mp3->comment = "";
	mp3->year = 0;

	mp3->filename = fname->c_str();

	ifstream mp3file;
	mp3file.open(fname->c_str());
	if (!mp3file.is_open()) {
		return NULL;
	}
      mp3file.seekg(0, ios::end);
	mp3->size = mp3file.tellg();
      mp3file.seekg(-128, ios::end);

	int i = 0;
	char tag[4] =
	{'\0'};
	while (i < 3 && mp3file.get(tag[i])) {
		i++;
	}

	// Does this file have an mp3 tag?
	if (strcmp(tag, "TAG") == 0) {
		mp3->tag = true;
	} else {
		mp3file.close();
		mp3->tag = false;
		return mp3;
	}

	// WARNING: these MUST be called in this exact order!
	mp3->title = getstring(&mp3file, 30);
	mp3->artist = getstring(&mp3file, 30);
	mp3->album = getstring(&mp3file, 30);
	mp3->year = atoi(getstring(&mp3file, 4).c_str());	// evil! :)

	mp3->comment = getstring(&mp3file, 30);
	mp3->genre = getstring(&mp3file, 1);

	mp3file.close();
	return mp3;
}

// this is a somewhat dangerous function.  Be sure you call it correctly, hmm?
string getstring(ifstream * mp3file, int i)
{
	// WARNING: do not modify this table!
	char *genres[125] =
	{
	    "Blues", "Classic Rock", "Country", "Dance", "Disco", "Funk",
		"Grunge", "Hip-Hop", "Jazz", "Metal", "New Age", "Oldies",
		"Other", "Pop", "R&B", "Rap", "Reggae", "Rock", "Techno",
	     "Industrial", "Alternative", "Ska", "Death Metal", "Pranks",
	     "Soundtrack", "Euro-Techno", "Ambient", "Trip-Hop", "Vocal",
	    "Jazz+Funk", "Fusion", "Trance", "Classical", "Instrumental",
		"Acid", "House", "Game", "Sound Clip", "Gospel", "Noise",
	     "AlternRock", "Bass", "Soul", "Punk", "Space", "Meditative",
	     "Instrumental Pop", "Instrumental Rock", "Ethnic", "Gothic",
		"Darkwave", "Techno-Industrial", "Electronic", "Pop-Folk",
		"Eurodance", "Dream", "Southern Rock", "Comedy", "Cult", "Gangsta",
		"Top 40", "Christian Rap", "Pop/Funk", "Jungle", "Native American",
		"Cabaret", "New Wave", "Psychadelic", "Rave", "Showtunes", "Trailer",
	   "Lo-Fi", "Tribal", "Acid Punk", "Acid Jazz", "Polka", "Retro",
	      "Musical", "Rock & Roll", "Hard Rock", "Folk", "Folk/Rock",
	 "National Folk", "Swing", "Bebob", "Latin", "Revival", "Celtic",
	    "Bluegrass", "Avantgarde", "Gothic Rock", "Progressive Rock",
	   "Psychedelic Rock", "Symphonic Rock", "Slow Rock", "Big Band",
		"Chorus", "Easy Listening", "Acoustic", "Humour", "Speech", "Chanson",
		"Opera", "Chamber Music", "Sonata", "Symphony", "Booty Bass", "Primus",
	   "Porn Groove", "Satire", "Slow Jam", "Club", "Tango", "Samba",
		"Folklore", "Ballad", "Power Ballad", "Rhythmic Soul", "Freestyle",
	     "Duet", "Punk Rock", "Drum Solo", "A capella", "Euro-House",
		"Dance Hall"
	};

	string data;
	char ch;

	switch (i) {
	case 30:		// default

		while (i--) {
			mp3file->get(ch);
			data += ch;
		}
		return data;
		break;

	case 4:		// year

		while (i--) {
			mp3file->get(ch);
			data += ch;
		}
		return data;
		break;

	case 1:		// genre

		mp3file->get(ch);
		if ((unsigned int) ch < 125)
			data = genres[(unsigned int) ch];
		return data;
		break;

	default:
		cout << "*";
		return data;
	}
}


char *ltoa(unsigned long rval)
{
	int n = 2;		// magic number, don't change.

	if (rval / 10 <= 0) {
		char *x = new char[n];
		if (snprintf(x, n, "%ul", rval) == -1)
			return NULL;
		return x;
	}
	int tmp = rval;
	while (tmp / 10 > 0) {
		tmp = tmp / 10;
		n++;
	}

	char *x = new char[n];
	if (snprintf(x, n, "%ul", rval) == -1)
		return NULL;
	return x;
}

string escape_string(const string * tmp, bool strip_tabs = false)
{
	// modify the string to escape ' characters -
	// try to prevent security problems...
	string rval;
	for (int i = 0; i < strlen(tmp->c_str()); i++) {
		if (tmp->c_str()[i] == '\'')
			rval += '\'';
		rval += tmp->c_str()[i];
	}
	if (strip_tabs) {
		for (int i = 0; i < strlen(tmp->c_str()); i++) {
			if (tmp->c_str()[i] != 9)
				rval += tmp->c_str()[i];
		}
	}
	return rval;
}
