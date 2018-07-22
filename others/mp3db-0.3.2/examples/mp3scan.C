#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

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

struct mp3data *scanmp3(string *);
string getstring(ifstream *,int);
bool suck(ifstream *);

int main()
{
	cout << "Enter filename: ";
	char tmp[1024];
	cin.getline(tmp,1024);
	string fname = tmp;
	cerr << "Scanning " << fname << "...";
	struct mp3data *mp3;
	if((mp3=scanmp3(&fname))== NULL){
		cerr << "failed." << endl;
		return 1;
	}
	cerr << "success." << endl;
	
	cout << "Filename\t" << mp3->filename << endl;
	cout << "Size\t\t" << mp3->size << endl;
	if(mp3->tag){
		cout << "Title\t\t"   << mp3->title   << endl;
		cout << "Artist\t\t"  << mp3->artist  << endl;
		cout << "Album\t\t"   << mp3->album   << endl;
		cout << "Year\t\t"    << mp3->year    << endl;
		cout << "Comment\t\t" << mp3->comment << endl;
		cout << "Genre\t\t"   << mp3->genre   << endl;
	}
	return 0;
}

struct mp3data *scanmp3(string *fname)
{
	struct mp3data *mp3=new struct mp3data;
	mp3->filename = fname->c_str();
	
	ifstream mp3file;
	mp3file.open(fname->c_str());
	if(! mp3file.is_open()){
		return NULL;
	}
	
	mp3file.seekg(0,ios::end);
	mp3->size=mp3file.tellg();
	mp3file.seekg(-128, ios::end);
	
	int i=0;
	char tag[4]={'\0'};
	while(i < 3 && mp3file.get(tag[i])){
		i++;
	}
	
	// Does this file have an mp3 tag?
	if(strcmp(tag,"TAG")==0){
		mp3->tag=true;
	} else {
		mp3file.close();
		return mp3;
	}
	
	// WARNING: these MUST be called in this exact order!
	mp3->title=getstring(&mp3file,30);
	mp3->artist=getstring(&mp3file,30);
	mp3->album=getstring(&mp3file,30);
	mp3->year=atoi(getstring(&mp3file,4).c_str());  // evil! :)
	mp3->comment=getstring(&mp3file,30);
	mp3->genre=getstring(&mp3file,1);
	
	mp3file.close();
	return mp3;
} 

// this is a somewhat dangerous function.  Be sure you call it correctly, hmm?
string getstring(ifstream *mp3file, int i)
{
	// WARNING: do not modify this table!
	char *genres[125]={
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
	if(i != 1){
		while(i--){
			mp3file->get(ch);
			data += ch;
		}
		return data;
	} 

	mp3file->get(ch);
	data=genres[(int)ch];
	return data;
}
