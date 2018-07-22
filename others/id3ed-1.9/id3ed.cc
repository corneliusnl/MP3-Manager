#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_LIBREADLINE
#ifdef OLD_READLINE
extern "C" {
#endif
#include <readline/readline.h>
#include <readline/history.h>
#ifdef OLD_READLINE
}
#endif
#ifdef HAVE_LIBCURSES
#include <curses.h>
#endif
#ifdef HAVE_LIBTERMCAP
#include <termcap.h>
#endif
#endif //HAVE_LIBREADLINE
#include "misc.h"

/* Field Length offsets
 *  Tag 3 0-2
 *  Songname 30 3-32
 *  Artist 30 33-62
 *  Album 30 63-92
 *  Year 4 93-96
 *  Comment 30 97-126
 *  or{
 *    Comment 28 97-124
 *    zero 1 125
 *    Tracknum 1 126
 *  }
 *  Genre 1 127
 */
struct s_v11sub {
	char comment[28];
	unsigned char empty;
	unsigned char tracknum;
};
struct s_id3{
   char tag[3];
   char songname[30];
   char artist[30];
   char album[30];
   char year[4];
   union{
	   char comment[30];
	   s_v11sub v11;
   };
//   char comment[28];
//   unsigned char empty;
//   unsigned char tracknum;
   unsigned char genre;
}id3;

#define NUM_GENRE 148

//char *ftp_cmdlist[FTP_LAST+1]={
//	        "NONE",
//		        "QUIT",
			
const char *genre_list[NUM_GENRE+1]={
	"Blues", "Classic Rock", "Country", "Dance", "Disco", "Funk",
	"Grunge", "Hip-Hop", "Jazz", "Metal", "New Age", "Oldies",
	"Other", "Pop", "R&B", "Rap", "Reggae", "Rock",
	"Techno", "Industrial", "Alternative", "Ska", "Death Metal", "Pranks",
	"Soundtrack", "Euro-Techno", "Ambient", "Trip-Hop", "Vocal", "Jazz+Funk",
	"Fusion", "Trance", "Classical", "Instrumental", "Acid", "House",
	"Game", "Sound Clip", "Gospel", "Noise", "AlternRock", "Bass",
	"Soul", "Punk", "Space", "Meditative", "Instrumental Pop", "Instrumental Rock",
	"Ethnic", "Gothic", "Darkwave", "Techno-Industrial", "Electronic", "Pop-Folk",
	"Eurodance", "Dream", "Southern Rock", "Comedy", "Cult", "Gangsta",
	"Top 40", "Christian Rap", "Pop/Funk", "Jungle", "Native American", "Cabaret",
	"New Wave", "Psychadelic", "Rave", "Showtunes", "Trailer", "Lo-Fi",
	"Tribal", "Acid Punk", "Acid Jazz", "Polka", "Retro", "Musical",
	"Rock & Roll", "Hard Rock", "Folk", "Folk/Rock", "National Folk", "Swing",
	"Fast-Fusion", "Bebob", "Latin", "Revival", "Celtic", "Bluegrass", "Avantgarde",
	"Gothic Rock", "Progressive Rock", "Psychedelic Rock", "Symphonic Rock", "Slow Rock", "Big Band",
	"Chorus", "Easy Listening", "Acoustic", "Humour", "Speech", "Chanson",
	"Opera", "Chamber Music", "Sonata", "Symphony", "Booty Bass", "Primus",
	"Porn Groove", "Satire", "Slow Jam", "Club", "Tango", "Samba",
	"Folklore", "Ballad", "Power Ballad", "Rhythmic Soul", "Freestyle", "Duet",
	"Punk Rock", "Drum Solo", "A capella", "Euro-House", "Dance Hall",
	"Goa", "Drum & Bass", "Club House", "Hardcore", "Terror",
	"Indie", "BritPop", "NegerPunk", "Polsk Punk", "Beat",
	"Christian Gangsta", "Heavy Metal", "Black Metal", "Crossover", "Contemporary C",
	"Christian Rock", "Merengue", "Salsa", "Thrash Metal", "Anime", "JPop",
	"SynthPop",
	NULL
};

void i3info(const char *file,int quiet);
void i3remove(const char *file,int quiet);
void i3edit(const char * file, const char * defsongname,
            const char * defartist, const char * defalbum,
            const char * defyear, const char * defcomment,
            const char * deftracknum, const char * defgenre,int quiet);
void stredit(const char * name, int maxlen, char * buf);
void print_genre_list(int mode);

void print_help(void){
      printf("id3ed v1.9 - mpeg layer 3 file information editor\n");
      printf("Copyright 1998-1999 Matt Mueller <donut@azstarnet.com>\n");
      printf("\n\
This program is free software; you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation; either version 2 of the License, or\n\
(at your option) any later version.\n\n\
This program is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
GNU General Public License for more details.\n\n\
You should have received a copy of the GNU General Public License\n\
along with this program; if not, write to the Free Software\n\
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n");	
      printf("\nUSAGE: id3ed [-s songname] [-n artist] [-a album] [-y year] [-c comment]\n"
               "             [-k tracknum] [-g genre] [-q] [-SNAYCKG] [-l/-L] [-r] [-i] <mp3files>\n\n"
	       "-q	quiet mode, set tags as specified on command line.  Use twice for total silence\n"
	       "-SNAYCKG prompt to edit only the specified tags.  Other tags can still be set with -[snaycg]\n"
	       "-l/-L	display list of genres\n"
	       "-r	remove id3 tag from files\n"
		   "-i  show current id3 tag, don't edit\n");
 }

int main(int argc,char ** argv){
   int c,quiet=0,q2=0,mode=0;
   char *songname = NULL, *artist = NULL, *album = NULL, *year = NULL,
        *comment = NULL, *tracknum = NULL, *genre = NULL;

   if (argc<2){
	   print_help();
   }
   else {
      while (EOF != (c = getopt(argc, argv, "s:n:a:y:c:k:g:qSNAYCKGlLri"))){
         switch (c){
            case 's':
               songname = optarg;
               break;
            case 'n':
               artist = optarg;
               break;
            case 'a':
               album = optarg;
               break;
            case 'y':
               year = optarg;
               break;
            case 'c':
               comment = optarg;
               break;
            case 'k':
               tracknum = optarg;
               break;
            case 'g':
               genre = optarg;
               break;
	    case 'q':
	       if(q2)
		       quiet|=1;
	       else{
		       quiet|=0xfffe;
		       q2=1;
	       }
	       break;
#define UNQUIET(b) {if (!q2) {quiet|=0xfffe;q2=1;} quiet &= ~b;}
	    case 'S':
	       UNQUIET(2);
	       break;
	    case 'N':
	       UNQUIET(4);
	       break;
	    case 'A':
	       UNQUIET(8);
	       break;
	    case 'Y':
	       UNQUIET(0x10);
	       break;
	    case 'C':
	       UNQUIET(0x20);
	       break;
	    case 'G':
	       UNQUIET(0x40);
	       break;
	    case 'K':
	       UNQUIET(0x80);
	       break;
	    case 'l':
	       print_genre_list(0);
	       break;
	    case 'L':
	       print_genre_list(1);
	       break;
	    case 'r':
	       mode=1;
	       break;
	    case 'i':
	       mode=2;
	       break;
            case ':':
            case '?':
//               fprintf(stderr, "Illegal parameters\n");
	       print_help();
               return 1;
         }
      }
//      printf("quiet=%i q2=%i\n",quiet,q2);
      for (int i=optind;i<argc;i++)
	      switch (mode){
			  case 2:
				  i3info(argv[i],quiet);
				  break;
			  case 1:
				  i3remove(argv[i],quiet);
				  break;
			  default:
				  i3edit(argv[i],songname,artist,album,year,comment,tracknum,genre,quiet);
		  }
   }
   return 0;
}

void print_genre_list(int mode){
	int c=0,w=0;
#ifdef HAVE_LIBREADLINE
	c=COLS;
#endif
	if (c<=0)c=80;
	w=(c/26);
	if (w<1)w=1;
//	printf("c=%i w=%i\n",c,w);
	//for (int x=0, y=0, i=0;genre_list[i];mode?i++:(i=y+x*((NUM_GENRE+w-1)/w))){
	for (int x=0, y=0, i=0;genre_list[i];mode?i++:(i=y+x*((NUM_GENRE)/w))){
		printf("%3i: %-20s",i,genre_list[i]);
		//			       printf("x%i,y%i,i%i\n",x,y,i);
		if (x>=w-1){
			x=0;y++;
		}else{
			x++;
		}
		if (x==0){
			if (i==NUM_GENRE-1)
				break;
			printf("\n");
		}
		else
			printf(" ");
	}
	printf("\n");
}
int agenretoi(const char * g){
	int i;
	char * errpos;
	for(i=0;i<NUM_GENRE;i++)
		if(strcasecmp(genre_list[i],g)==0)
			return i;
	i=strtol(g,&errpos,0);
	if (!*errpos)
		return i;
	printf("'%s' is not a valid genre name, nor a number between 0 and 255\n",g);
	return -1;
}

#ifdef HAVE_LIBREADLINE
char *my_rl_default;

int set_rl_default(void){
   rl_insert_text(my_rl_default);
//   rl_forced_update_display();
   return 0;
}
#endif
void genreedit(unsigned char &d){
	char def[40];
	int g=0;
	if (d<NUM_GENRE)
		strcpy(def,genre_list[d]);
	else
		sprintf(def,"%i",d);
#ifdef HAVE_LIBREADLINE
	char *str=NULL;
	my_rl_default=def;
	rl_startup_hook=(Function*)set_rl_default;
#else
	char str[40];
#endif

	do {
#ifdef HAVE_LIBREADLINE
		if (str){strcpy(def,str);free(str);}
		if((str=readline("genre[0-255/name]: "))){
			if (*str){
				add_history(str);
			}
		}
#else
		printf("genre[0-255/name def:%s]: ",def);
		fflush(stdout);
		fflush(stdin);
		if (fgets(str,40,stdin)){
			char *t;
			if ((t=strchr(str,'\n')))
				t[0]=0;
		}
#endif
	} while (!str || !str[0] || (g=agenretoi(str))<0);

#ifdef HAVE_LIBREADLINE
	free(str);
#endif
	d=g;
}
void stredit(const char * name, int maxlen, char * buf){
	char def[40];
	strncpy(def,buf,maxlen);
	def[maxlen]=0;
#ifdef HAVE_LIBREADLINE
	char prompt[40];
	char *str;
	sprintf(prompt,"%s[max:%i]: ",name,maxlen);
	my_rl_default=def;
	rl_startup_hook=(Function*)set_rl_default;
	if((str=readline(prompt))){
		if (*str){
			strncpy(buf,str,maxlen);
			add_history(str);
		}
		free(str);
	}
#else
	char str[40];
	printf("%s[max:%i def:%s]: ",name,maxlen,def);
	fflush(stdout);
	fflush(stdin);
	if (fgets(str,40,stdin)){
		char *t;
		if ((t=strchr(str,'\n')))
			t[0]=0;
		if (str[0])
			strncpy(buf,str,maxlen);
	}
#endif
}

void i3info(const char *file,int quiet){
	int f;
	off_t end;
	if (doopen(f,file,O_RDONLY))return;
	printf("%s: ",file);fflush(stdout);
	if ((end=lseek(f,-128,SEEK_END))<0){
		perror("lseek");return;
	}
	if (doread(f,&id3,sizeof(id3),"id3buf"))return;
	if (!strncmp(id3.tag,"TAG",3)){
		if (id3.v11.empty==0)
			printf("(tag v1.1%s)\n",id3.v11.tracknum?"":"?");
		else
			printf("(tag v1.0)\n");
#define PRINTINFO(f) printf("%s: %.*s\n",#f, (int)sizeof(id3. ## f), id3. ## f)
		PRINTINFO(songname);
		PRINTINFO(artist);
		PRINTINFO(album);
		PRINTINFO(year);
		PRINTINFO(comment);
		if (id3.v11.empty==0)
			printf("tracknum: %i\n", id3.v11.tracknum);
		printf("genre: %s(%i)\n\n",(id3.genre<NUM_GENRE)?genre_list[id3.genre]:"unknown",id3.genre);
		
	}else
		if ((!quiet))
			printf("(no tag)\n");
		
	close (f);
}

void i3remove(const char *file,int quiet){
	int f;
	off_t end;
	if (doopen(f,file,RDWRMODE))return;
	if ((end=lseek(f,-128,SEEK_END))<0){
		perror("lseek");return;
	}
	if (doread(f,&id3,sizeof(id3),"id3buf"))return;
	if (!strncmp(id3.tag,"TAG",3)){
		ftruncate(f,end);
		if ((!quiet))
			printf("tag removed from %s\n",file);
	}else
		if ((!quiet))
			printf("no tag in %s\n",file);
		
	close (f);
}

void i3edit(const char * file, const char * defsongname,
            const char * defartist, const char * defalbum,
            const char * defyear, const char * defcomment,
            const char * deftracknum, const char * defgenre, int quiet){
   int f,hasid3,commentsize=28;
   if (!(quiet&1))
	   printf("\nFile %s: ", file);
   if (doopen(f,file,RDWRMODE))return;
   if (lseek(f,-128,SEEK_END)<0){
      perror("lseek");return;
   }
   if (doread(f,&id3,sizeof(id3),"id3buf"))return;
   if (strncmp(id3.tag,"TAG",3)){
      memset(&id3,0,sizeof(id3));
      id3.tag[0]='T';id3.tag[1]='A';id3.tag[2]='G';
      id3.genre=255;
      hasid3=0;
	  if (!(quiet&1))
		  printf("(no tag)\n");
   } else {
		if (id3.v11.empty==0){
			if (!(quiet&1))
				printf("(tag v1.1)\n");
			hasid3=2;//id3 v1.1
		}else{
			if (!(quiet&1))
				printf("(tag v1.0");
			if (deftracknum){
				hasid3=2;
				if (!(quiet&1))
					printf("->1.1)\n");
			}else{
				hasid3=1;//id3 v1.0
				commentsize=30;
				if (!(quiet&1))
					printf(")\n");
			}
		}
   }
    if (defsongname){
       memset(id3.songname,0,sizeof(id3.songname));
       strncpy(id3.songname,defsongname,sizeof(id3.songname));
    }
    if (defartist){
       memset(id3.artist,0,sizeof(id3.artist));
       strncpy(id3.artist,defartist,sizeof(id3.artist));
    }
    if (defalbum){
       memset(id3.album,0,sizeof(id3.album));
       strncpy(id3.album,defalbum,sizeof(id3.album));
    }
    if (defyear){
       memset(id3.year,0,sizeof(id3.year));
       strncpy(id3.year,defyear,sizeof(id3.year));
    }
    if (defcomment){
       memset(id3.comment,0,commentsize);
       strncpy(id3.comment,defcomment,commentsize);
    }
    if (deftracknum){
       //memset(id3.tracknum,0,sizeof(id3.tracknum));
       id3.v11.empty=0;
       id3.v11.tracknum = atoi(deftracknum);
       //strncpy(id3.tracknum,deftracknum,sizeof(id3.tracknum));
    }
    if (defgenre){
	 id3.genre = agenretoi(defgenre);
//       unsigned tmp;
//       sscanf(defgenre,"%u",&tmp);
//       id3.genre = tmp;
    }
   if (!(quiet&2))
	   stredit("songname",30,id3.songname);
   if (!(quiet&4))
	   stredit("artist",30,id3.artist);
   if (!(quiet&8))
	   stredit("album",30,id3.album);
   if (!(quiet&0x10))
	   stredit("year",4,id3.year);
   if (!(quiet&0x20))
	   stredit("comment",commentsize,id3.comment);
   if (hasid3!=1 && !(quiet&0x80)) {
	   char tbuf[3];
	   sprintf(tbuf, "%i", id3.v11.tracknum);
	   stredit("tracknum", 3, tbuf);
	   id3.v11.empty=0;
	   id3.v11.tracknum=atoi(tbuf);
   }
   if (!(quiet&0x40)){
//	   char sbuf[4];
//	   sprintf(sbuf,"%i",id3.genre);
//	   stredit("genre",3,sbuf);
//	   id3.genre=atoi(sbuf);
	   genreedit(id3.genre);
   }
   if (hasid3){
      if (lseek(f,-128,SEEK_END)<0){
	 perror("lseek");return;
      }
   }else{
      if (lseek(f,0,SEEK_END)<0){
	 perror("lseek");return;
      }
   }
   if (dowrite(f,&id3,sizeof(id3),"id3buf"))return;
   close(f);
}
