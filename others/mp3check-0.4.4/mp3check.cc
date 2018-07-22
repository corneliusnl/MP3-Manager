/*GPL*START*
 * mp3check - check mp3 file for consistency and print infos
 * 
 * Copyright (C) 1998 by Johannes Overmann <overmann@iname.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * *GPL*END*/  

#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "tappconfig.h"
#include "tfiletools.h"
#include "crc16.h"


// 1998:
// 20 Jan: mp3check started and first header seek check (v0.0)
// 21 Jan: v0.1 colorization, error and anomaly check implen, crc started
// 23 Jan: v0.2 advanced formatting
// 24 Jan: v0.3 disable errors/anomalies
// 27 Jan: v0.3a '-c only first frame' bug fixed, --dump-header added
// 27 Jan: v0.3b starting mpeg 2.0 support, cut-junk-start added
// 28 Jan: v0.3c added some data for mpeg 2.0
// 09 Jul: v0.3e mpeg2.5 does not work :(
// 18 Sep: 0.3.1 prepare for sunsite
// 22 Oct: 0.4.1 prepare for use with printcdcover (raw output format)
// 1999:
// 25 Aug: 0.4.2 cerr changed to cerror (conflict with c++ libs with gcc2.95.1)
// 15 Sep 19:21 0.4.3 cut-junk-end added (thanks to Pollyanna Lindgren <jtlindgr@cs.helsinki.fi>)
// 19 Sep 18:13 0.4.4 fix-headers added (no succes on trying to fix madonna-mix)

// update VER in Makefile!

// todo:
// layer 1 and 2: add crc check support
// add mpeg 2.5 support


const char *options[] = {
   "#usage='Usage: [OPTIONS and FILES] [--] [FILES]\n\n"					 
     "this program checks audio mpeg layer 1,2 and 3 (*.mp3) files for\n"       
     "consistency (headers and crc) and anomalies'",
   "#trailer='\n%n version %v *** (C) 1997-99 by Johannes Overmann\ncomments, bugs and suggestions welcome: %e\n%gpl'",
   "#stopat--",
   "#onlycl",					 
   // options
   "name=list             , type=switch, char=l, help='list parameters by examining the first valid header and size', headline=mode:",
   "name=compact-list     , type=switch, char=c, help='list parameters of one file per line in a very compact format: "
     "version (l=1.0, L=2.0), layer, sampling frequency [kHz] (44=44.1), bitrate [kBit/s], mode (js=joint stereo, st=stereo, sc=single channel, dc=dual channel), "
     "emphasis (n=none, 5=50/15 usecs, J=CCITT J.17), COY (has [C]rc, [O]riginal, cop[Y]right), length [min:sec], filename (poss. truncated)'",
   "name=error-check      , type=switch, char=e, help='check crc and headers for consistency and print several error messages'",
   "name=anomaly-check    , type=switch, char=a, help='report all differences from these parameters: layer 3, 44.1kHz, 112kB or 128kB, joint stereo, no emphasis, has crc'",
   "name=dump-header      , type=switch, char=d, help='dump all possible header with sync=0xfff'",
   "name=raw-list         , type=switch,       , help='list parameters in raw output format for use with external programms'",
   "name=raw-elem-sep     , type=string,       , default=0x09, param=NUM, help='separate elements in one line by char NUM (numerical ascii code)'",
   "name=raw-line-sep     , type=string,       , default=0x0a, param=NUM, help='separate lines by char NUM (numerical ascii code)'",
				       
   "name=cut-junk-start   , type=switch,       , help='remove junk before first frame', headline='fix errors:'",
   "name=cut-junk-end     , type=switch,       , help='remove junk after last frame'",
   "name=fix-headers      , type=switch,       , help='fix invlid headers (prevent constant parameter switching), implies -e, use with care'",
   
   "name=ign-tag128       , type=switch, char=G, help='ignore 128 byte TAG after last frame', headline='disable error messages for -e --error-check:'",
   "name=ign-resync       , type=switch, char=R, help='ignore invalid frame header'",
   "name=ign-junk-end     , type=switch, char=E, help='ignore junk after last frame'",
   "name=ign-crc-error    , type=switch, char=Z, help='ignore crc errors'",
   "name=ign-non-ampeg    , type=switch, char=N, help='ignore non audio mpeg streams'",
   "name=ign-truncated    , type=switch, char=T, help='ignore truncated last frames'",
   "name=ign-junk-start   , type=switch, char=S, help='ignore junk before first frame'",
   "name=ign-bitrate-sw   , type=switch, char=B, help='ignore bitrate switching'",
   "name=ign-constant-sw  , type=switch, char=W, help='ignore switching of constant parameters, such as sampling frequency'",
					 
   "name=any-crc          , type=switch, char=C, help='ignore crc anomalies', headline='disable anomaly messages for -a --anomaly-check'",
   "name=any-mode         , type=switch, char=M, help='ignore mode anomalies'",
   "name=any-layer        , type=switch, char=L, help='ignore layer anomalies'",
   "name=any-bitrate      , type=switch, char=K, help='ignore bitrate anomalies'",
   "name=any-version      , type=switch, char=I, help='ignore version anomalies'",
   "name=any-sampling     , type=switch, char=F, help='ignore sampling frequency anomalies'",
   "name=any-emphasis     , type=switch, char=P, help='ignore emphasis anomalies'",
   
   
   "name=single-line      , type=switch, char=s, help='print one line per file and message instead of splitting into several lines', headline='output options:'",    
   "name=log-file         , type=string, char=g, param=FILE, help='print names of erroneous files to FILE, one per line'",
   "name=quiet            , type=switch, char=q, help='quiet mode, hide messages about direcories, non-regular or non-existing files'",
   "name=no-color         , type=switch, char=n, help='do not colorize output with ansi sequences'",
   "name=alt-color        , type=switch, char=b, help='do not use bold ansi sequences'",
   "name=progress         , type=switch, char=p, help='show progress information on stderr'", 
   "name=dummy            , type=switch, char=0, help='do not write/modify anything other than the logfile', headline=common options:",
   "EOL" // end of list     
};
					 
					 

// color config:
// 30 black
// 31 red
// 32 green
// 33 yellow
// 34 blue
// 35 magenta
// 36 cyan
// 37 white
const char *cfil = "\033[37;01m";
const char *cano = "\033[33;01m";
const char *cerror = "\033[31;01m";
const char *cval = "\033[34;01m";
const char *cok  = "\033[00;32m";
const char *cnor = "\033[00m";

const char *c_fil = "\033[37m";
const char *c_ano = "\033[33m";
const char *c_err = "\033[31m";
const char *c_val = "\033[34m";
const char *c_ok  = "\033[32m";
const char *c_nor = "\033[00m";

// minimum number of sequential valid and constant header to validate header
const int min_valid = 6;


// global data
bool progress = false;
bool single_line = false;
bool dummy = false;

bool ano_any_crc   = false;
bool ano_any_bit   = false;
bool ano_any_emp   = false;
bool ano_any_rate  = false;
bool ano_any_mode  = false;
bool ano_any_layer = false;
bool ano_any_ver   = false;

bool ign_crc   = false;
bool ign_start = false;
bool ign_end   = false;
bool ign_tag   = false;
bool ign_bit   = false;
bool ign_const = false;
bool ign_trunc = false;
bool ign_noamp = false;
bool ign_sync  = false;


// header info
					 
int layer_tab[4]= {0, 3, 2, 1};

const int FREEFORMAT = 0;
const int FORBIDDEN = -1;
int bitrate1_tab[16][3] = {   
   {FREEFORMAT, FREEFORMAT, FREEFORMAT},
   {32, 32, 32},
   {64, 48, 40},
   {96, 56, 48},
   {128, 64, 56},
   {160, 80, 64},
   {192, 96, 80},
   {224, 112, 96},
   {256, 128, 112},
   {288, 160, 128},
   {320, 192, 160},
   {352, 224, 192},
   {384, 256, 224},
   {416, 320, 256},
   {448, 384, 320},
   {FORBIDDEN, FORBIDDEN, FORBIDDEN}
};
// 
int bitrate2_tab[16][3] = {   
   {FREEFORMAT, FREEFORMAT, FREEFORMAT},
   { 32,  8,  8},
   { 48, 16, 16},
   { 56, 24, 24},
   { 64, 32, 32},
   { 80, 40, 40},
   { 96, 48, 48},
   {112, 56, 56},
   {128, 64, 64},
   {144, 80, 80},
   {160, 96, 96},
   {176,112,112},
   {192,128,128},
   {224,144,144},
   {256,160,160},
   {FORBIDDEN, FORBIDDEN, FORBIDDEN}
};


double sampd1_tab[4]={44.1, 48.0, 32.0, 0.0};
int samp_1_tab[4]={44100, 48000, 32000, 50000};
double sampd2_tab[4]={22.05, 24.0, 16.0, 0.0};
int samp_2_tab[4]={22050, 24000, 16000, 50000};

const uint CONST_MASK = 0xffffffff;
struct Header {
   uint
   emphasis: 2,          // fix 2 reserved
   original: 1,          // fix
   copyright: 1,         // fix
   mode_extension: 2,    //      (not fix!)
   mode: 2,              // fix
   private_bit: 1,       //
   padding_bit: 1,       // 
   sampling_frequency: 2,// fix 3 reserved
   bitrate_index: 4,     //    15 forbidden
   protection_bit: 1,    // fix
   layer_index: 2,       // fix 0 reserved
   ID: 1,                // fix 1==mpeg1.0 0==mpeg2.0
   syncword: 12;         // fix must 0xfff

   bool isValid() const {
      if(syncword!=0xfff) return false;
      if(ID==1) { // mpeg 1.0
	 if((layer_index!=0) && (bitrate_index!=15) &&
	    (sampling_frequency!=3) && (emphasis!=2)) return true;
	 return false;
      } else {    // mpeg 2.0
	 if((layer_index!=0) && (bitrate_index!=15) &&
	    (sampling_frequency!=3) && (emphasis!=2)) return true;
	 return false;
      }
   }
   
   bool sameConstant(Header h) const {
      if((*(uint*)this) == (*(uint*)&h)) return true;
      if((syncword          ==h.syncword          ) &&
	 (ID                ==h.ID                ) &&
	 (layer_index       ==h.layer_index       ) &&
	 (protection_bit    ==h.protection_bit    ) &&
//	 (bitrate_index     ==h.bitrate_index     ) &&
	 (sampling_frequency==h.sampling_frequency) &&
	 (mode              ==h.mode              ) &&
//	 (mode_extension    ==h.mode_extension    ) &&
	 (copyright         ==h.copyright         ) &&
	 (original          ==h.original          ) &&
	 (emphasis          ==h.emphasis          ) &&
	 1) return true;
      else return false;
   }
   
   int bitrate() const {
      if(ID)
	return bitrate1_tab[bitrate_index][layer()-1];
      else
	return bitrate2_tab[bitrate_index][layer()-1];
   }
   int layer() const {return layer_tab[layer_index];}
   
   TString print() const {
      TString s;
      
      s.sprintf("(%03x,ID%d,l%d,prot%d,%2d,%4.1fkHz,pad%d,priv%d,mode%d,ext%d,copy%d,orig%d,emp%d)", 
		syncword, ID, layer(), protection_bit, bitrate_index, 
		samp_rate(), padding_bit, private_bit, mode,
		mode_extension, copyright, original, emphasis);
      return s;
   }
   double version() const {
      if(ID) return 1.0;
      else   return 2.0;
   }
   enum {STEREO, JOINT_STEREO, DUAL_CHANNEL, SINGLE_CHANNEL};
   const char *mode_str() const {
      switch(mode) {
       case 0: return "stereo";
       case 1: return "joint stereo";
       case 2: return "dual channel";
       case 3: return "single chann";
      }
      return 0;
   }
   const char *short_mode_str() const {
      switch(mode) {
       case 0: return "st";
       case 1: return "js";
       case 2: return "dc";
       case 3: return "sc";
      }
      return 0;
   }
   enum {emp_NONE, emp_50_15_MICROSECONDS, emp_RESERVED, emp_CCITT_J_17};
   const char *emphasis_str() const {
      switch(emphasis) {
       case 0: return "no emph";
       case 1: return "50/15us";
       case 2: return "reservd";
       case 3: return "C. J.17";
      }
      return 0;
   }
   const char *short_emphasis_str() const {
      switch(emphasis) {
       case 0: return "n";
       case 1: return "5";
       case 2: return "!";
       case 3: return "J";
      }
      return 0;
   }
   double samp_rate() const {
      if(ID)
	return sampd1_tab[sampling_frequency];
      else
 	return sampd2_tab[sampling_frequency];
   }
   int samp_int_rate() const {
      if(ID)
	return samp_1_tab[sampling_frequency];
      else
 	return samp_2_tab[sampling_frequency];
   }
   int get_int() const {return *((const int *)this);}
};


// get header from pointer
inline Header get_header(const uchar *p) {
   Header h;
   uchar *q = (uchar *)&h;
   q[0]=p[3];
   q[1]=p[2];
   q[2]=p[1];
   q[3]=p[0];   
   return h;
}


// set header to pointer
inline void set_header(uchar *p, Header h) {
   uchar *q = (uchar *)&h;
   p[0]=q[3];
   p[1]=q[2];
   p[2]=q[1];
   p[3]=q[0];   
}


// return lenght of frame in bytes
inline int frame_length(Header h) {
   if(h.ID) {
      return ((((h.layer()==1)?48000:144000)*h.bitrate())/
	      h.samp_int_rate())+h.padding_bit;
   } else {
      return ((((h.layer()==1)?24000:72000)*h.bitrate())/
	      h.samp_int_rate())+h.padding_bit;
   }
}

					 
// return next pos of min_valid sequential valid and constant header 
// or -1 if not found
inline int find_next_header(const uchar *p, int len, int min_valid) {
   int i;
   const uchar *q = p;
   const uchar *t;   
   int rest, k, l;
   Header h, h2;
   
   for(i=0; i < len-3; i++, q++) {
      if(*q==255) {
	 h = get_header(q);
	 l = frame_length(h);
	 if(h.isValid() && (l>=21)) {
	    t = q + l;
	    rest = len - i - l;
	    for(k=1; (k < min_valid) && (rest >= 4); k++) {
	       h2 = get_header(t);
	       if(!h2.isValid()) break;
	       if(!h2.sameConstant(h)) break;
	       l = frame_length(h2);
	       if(l < 21) break;
	       t += l;
	       rest -= l;
	    }
	    if(k == min_valid) return i;
	 }
      }
   }
   
   return -1;  // not found
}



void fmes(const char *name, const char *format, ...) __attribute__ ((format(printf,2,3)));

void fmes(const char *name, const char *format, ...) {
   va_list ap;
   static TString lastname;
   
   if(single_line) {
      printf("%s%s%s: ", cfil, name, cnor);
   } else {
      if(name != lastname) {
	 lastname = name;	 
	 TString s = TString(name).shortFilename(79);
	 printf("%s%s%s:\n", cfil, *s, cnor);	 
      }
   }
   va_start(ap, format);
   vprintf(format, ap);
   va_end(ap);
}


// returns true on error
bool error_check(const char *name, const uchar *p, int len, CRC16& crc, bool fix_headers) {
   bool had_error = false;
   int start = find_next_header(p, len, min_valid);
   int rest = len;
   int frame=0;
   int l,s;
   if(start<0) {
      if(!ign_noamp) {
	 fmes(name, "%snot an audio mpeg 1.0/2.0 stream%s\n", cerror, cnor);
	 had_error = true;
      }
   } else {
      
      // check for junk at beginning
      if(start>0) {
	 if(!ign_start) {
	    fmes(name, "%s%d%s %sbytes of junk before first frame header%s\n", 
		 cval, start, cnor, cerror, cnor);	    
	    had_error = true;
	 }
      }
      
      // check for TAG trailer
      if((rest>=128)&&(p[rest-128]=='T')&&(p[rest-127]=='A')&&(p[rest-126]=='G')) {
	 if(!ign_tag) {
	    if(progress) putc('\r', stderr);
	    fmes(name, "%sTAG trailer found%s '%s%-59.59s%s'\n", 
		 cerror, cnor, cval, p+(rest-128+3), cnor); 
	    had_error = true;
	 }
	 rest-=128;
      } 
      
      // check whole file
      rest -= start;
      p += start;
      Header head = get_header(p);
      l = frame_length(head);
      p += l;
      rest -= l;
      start += l;
      while(rest>=4) {
	 Header h = get_header(p);
	 frame++;
	 if(progress) {
	    if((frame%1000)==0) {
	       putc('.', stderr);
	       fflush(stderr);
	    }
	 }
	 if(!(h.isValid()&&(frame_length(h)>=21))) {
	    // invalid header 
	    
	    // search for next valid header
	    s = find_next_header(p, rest, min_valid);
	    
	    if(s<0) break; // error: junk at eof
	    
	    // skip s invalid bytes
	    p += s;
	    rest -= s;
	    start += s;
	    if(!ign_sync) {
	       if(progress) putc('\r', stderr);
	       fmes(name, "frame %s%5d%s: %sinvalid header at%s %s0x%08x%s (%s0x%08x%s), skipping %s%d%s bytes\n", 
		    cval, frame, cnor, cerror, cnor, cval, start, cnor, cval, 
		    h.get_int(), cnor, cval, s, cnor);  
	       had_error = true;
	    }
	    
	 } else {
	    // valid header
	    
	    // check for constant parameters
	    if(!head.sameConstant(h)) {
	       if(!ign_const) {
		  had_error = true;
		  if(progress) putc('\r', stderr);
		  fmes(name, "frame %s%5d%s: %sconstant parameter switching%s (%s0x%08x%s!=%s0x%08x%s)\n", 
		       cval, frame, cnor, cerror, cnor, cval, head.get_int()&CONST_MASK, 
		       cnor, cval, h.get_int()&CONST_MASK, cnor);
	       }
	       if(fix_headers) {
		  fmes(name, "frame %s%5d%s: %sfixing header%s\n", 
		       cval, frame, cnor, cerror, cnor);
		  set_header((uchar *)p, head);
		  h = head;
	       } 
	    }
	    if(head.bitrate_index != h.bitrate_index) {
	       if(!ign_bit) {
		  had_error = true;
		  if(progress) putc('\r', stderr);
		  fmes(name, "frame %s%5d%s: %sbitrate switching from %s%d%s to %s%d%s\n", 
		       cval, frame, cnor, cerror, cval, 
		       head.bitrate(), cerror, cval, h.bitrate(), cnor);  		     
	       }
	    }
	    head = h;
	    	    
	    // check crc16
	    if((!ign_crc)&&(h.protection_bit==0)&&(rest>=32+6)) {
	       // reset crc checker
	       crc.reset(0xffff);
	       // bits 16..31 of header
	       crc.add(p[2]);
	       crc.add(p[3]);
	       if(h.version()==1.0) {
		  if(h.layer()==3) {
		     if(h.mode==Header::SINGLE_CHANNEL) s = 17;
		     else s = 32;
		     for(int i=0; i < s; i++) crc.add(p[i+6]);
		  } else {
		     // layer 1 and 2: crc check not yet supported
		  }
	       } else {
		  if(h.layer()==3) {
		     if(h.mode==Header::SINGLE_CHANNEL) s = 9; 
		     else s = 17;
		     for(int i=0; i < s; i++) crc.add(p[i+6]);
		  } else {
		     // layer 1 and 2: crc check not yet supported
		  }
	       }
	       // compare crc
	       ushort c = p[5] | (ushort(p[4])<<8);
	       if(c != crc.crc()) {
		  if(progress) putc('\r', stderr);
		  fmes(name, "frame %s%5d%s: %scrc error%s (%s0x%04x%s!=%s0x%04x%s)\n", 
		       cval, frame, cnor, cerror, cnor, 
		       cval, c, cnor, cval, crc.crc(), cnor);  		     		  
		  had_error = true;
	       }
	    }
	    
	    // skip to next frame
	    l = frame_length(h);
	    p += l;
	    rest -= l;
	    start += l;
	 }
      }
      
      // check for truncated file
      if(rest < 0) {
	 if(!ign_trunc) {
	    if(progress) putc('\r', stderr);
	    fmes(name, "frame %s%5d%s: %sfile truncated%s, %s%d%s bytes missing for last frame (%s0x%08x%s)\n", 
		 cval, frame, cnor, cerror, cnor, cval,-rest, cnor, cval, start, cnor);	    
	    had_error = true;
	 }
      }
      
      // check for trailing junk
      if(rest > 0) {
	 if(!ign_end) {
	    if(progress) putc('\r', stderr);
	    fmes(name, "frame %s%5d%s: %s%d%s %sbytes of junk after last frame%s at %s0x%08x%s\n", 
		 cval, frame, cnor, cval, rest, cnor, cerror, cnor, cval, start, cnor); 
	    had_error = true;
	 }
      }      
   }   
   if(progress) {
      fputs("\r                                                                              \r", stderr);
      fflush(stderr);
   }
   return had_error;
}


// returns true on anomaly
bool anomaly_check(const char *name, const uchar *p, int len, bool err_check, int& err) {
   bool had_ano = false;
   int start = find_next_header(p, len, min_valid);
   if(start>=0) {
      Header h = get_header(p+start);
      if(!ano_any_ver) {
	 if(h.version()!=1.0) {
	    fmes(name, "%sanomaly%s: audio mpeg version %s%3.1f%s stream\n", 
		 cano, cnor, cval, h.version(), cnor);	 
	    had_ano = true;
	 } 
      }
      if(!ano_any_layer) {
	 if(h.layer()!=3) {
	    fmes(name, "%sanomaly%s: audio mpeg %slayer %d%s stream\n", 
		 cano, cnor, cval, h.layer(), cnor);	 
	    had_ano = true;
	 } 
      }
      if(!ano_any_rate) {
	 if(h.samp_rate()!=44.1) {
	    fmes(name, "%sanomaly%s: sampling rate %s%4.1fkHz%s\n", 
		 cano, cnor, cval, h.samp_rate(), cnor); 
	    had_ano = true;
	 }
      }
      if(!ano_any_bit) {
	 if((h.bitrate()!=112) && (h.bitrate()!=128)) {
	    fmes(name, "%sanomaly%s: bitrate %s%3dkBit/s%s\n", 
		 cano, cnor, cval, h.bitrate(), cnor); 
	    had_ano = true;
	 }
      }
      if(!ano_any_mode) {
	 if(h.mode!=Header::JOINT_STEREO) {
	    fmes(name, "%sanomaly%s: mode %s%s%s\n", 
		 cano, cnor, cval, h.mode_str(), cnor);  
	    had_ano = true;
	 }
      }
      if(!ano_any_crc) {
	 if(h.protection_bit==1) {
	    fmes(name, "%sanomaly%s: %sno crc%s\n", 
		 cano, cnor, cval, cnor);	    
	    had_ano = true;
	 }
      }
      if(!ano_any_emp) {
	 if(h.emphasis!=Header::emp_NONE) {
	    fmes(name, "%sanomaly%s: emphasis %s%s%s\n", 
		 cano, cnor, cval, h.emphasis_str(), cnor); 
	    had_ano = true;
	 }
      }
   } else {
      if((!err_check)&&(!ign_noamp)) {
	 fmes(name, "%snot an audio mpeg 1.0/2.0 stream%s\n", cerror, cnor);
	 ++err;
      }
   }
   return had_ano;
}
					
					 
// return true if junk was found and cut
bool cut_junk_end(const char *name, const uchar *p, int len, const uchar *free_p, int fd, int& err) {
// this is basically the error_check routine which treats only the trainling junk case
// (implemented by Pollyanna Lindgren <jtlindgr@cs.helsinki.fi>)
   int start = find_next_header(p, len, min_valid);
   int rest = len;
   const uchar *alku=p;
   int frame=0;
   int l,s;
   int free_len = len;

   if(start<0) {
      if(!ign_noamp) {
	 fmes(name, "%snot an audio mpeg 1.0/2.0 stream%s\n", cerror, cnor);
	 err++;
      }
      return false;
   } else {
      // check whole file
      rest -= start;
      p += start;
      Header head = get_header(p);
      l = frame_length(head);
      p += l;
      rest -= l;
      start += l;
      while(rest>=4) {
	 Header h = get_header(p);
	 frame++;
	 if(progress) {
	    if((frame%1000)==0) {
	       putc('.', stderr);
	       fflush(stderr);
	    }
	 }
	 if(!(h.isValid()&&(frame_length(h)>=21))) {
	    // invalid header 
	    
	    // search for next valid header
	    s = find_next_header(p, rest, min_valid);
	    
	    if(s<0) break; // error: junk at eof
	    
	    // skip s invalid bytes
	    p += s;
	    rest -= s;
	    start += s;
	 } else {
	    // valid header
	    
	    head = h;
	    
	    // skip to next frame
	    l = frame_length(h);
	    p += l;
	    rest -= l;
	    start += l;
	 }
      }
      
      // remove trailing junk
      if(rest > 0) {
	 printf("%s%s%s: %scut-junk-end: removing last %s%d%s bytes, retrying%s\n",
		cfil, name, cnor, cok, cval, rest, cok, cnor);	    
	 if(!dummy) {
	    // create new file
	    len-=rest;
	    TString fn(name);
	    fn+="~mp3ctf~";
	    FILE *f=fopen(fn, "w");
	    if(f==0) 
	      userError("error while opening file '%s' for writing!\n", *fn);
	    if(fwrite(alku, 1, len, f)!=(uint)len)
	      userError("error while writing %d bytes to file '%s'! (disk full?)\n", len, *fn);		 
	    fclose(f);
	    
	    // unmap file and close
	    if(munmap((char*)free_p, free_len)) {
	       perror("munmap");
	       userError("can't unmap file '%s'!\n", name);
	    }
	    close(fd);      
	    
	    // move file
	    if(rename(fn, name))
	      userError("error while renaming file '%s' as '%s'!\n", *fn, name);
	 }
	 return true;
      } else {
	 printf("%s%s%s: %scut-junk-end: no junk found%s\n",
		cfil, name, cnor, cok, cnor);	    
	 return false;
      }
   }
}
					 
   
int main(int argc, char *argv[]) {
   // get parameters
   TAppConfig ac(options, "options", argc, argv, 0, 0, VERSION);
   
   // setup options
   bool quiet = ac("quiet");
   dummy = ac("dummy");
   progress = ac("progress");
   int opt=0;
   // alt mode
   if(ac("error-check")) opt=1; 
   if(ac("fix-headers")) opt=1;
   if(ac("anomaly-check")) opt=1; 
   if(ac("cut-junk-start")) opt=1; 
   if(ac("cut-junk-end")) opt=1;
   // main mode
   if(ac("dump-header")) opt++; 
   if(ac("list")) opt++; 
   if(ac("compact-list")) opt++; 
   if(ac("raw-list")) opt++; 
   // check for mode
   if(opt==0) 
     userError("you must specify the mode of operation!  (try --help for more info)\n");
   if(opt>1) 
     userError("incompatible modes specified!  (try --help for more info)\n");
   // color
   if(ac("no-color")) 
     cval = cnor = cano = cerror = cfil = "";
   if(ac("alt-color")) {
      cval = c_val;
      cano = c_ano;
      cerror = c_err;
      cfil = c_fil;
      cnor = c_nor;
      cok = c_ok;
   }
   TString rawsepstr = ac.getString("raw-elem-sep");
   char rawsep = strtol(rawsepstr, 0, 0);
   if(rawsepstr) if(!isdigit(rawsepstr[0])) rawsep = rawsepstr[0];
   TString rawlinesepstr = ac.getString("raw-line-sep");
   char rawlinesep = strtol(rawlinesepstr, 0, 0);     
   if(rawlinesepstr) if(!isdigit(rawlinesepstr[0])) rawsep = rawlinesepstr[0];
   if(ac("raw-list")) {
      quiet=true;
      progress=false;
   }
   
   
   if(ac.numParam()==0) 
     userError("need at least one file! (try --help for more info)\n");
   single_line = ac("single-line");
   
   // setup ignores/anys
   ano_any_crc   = ac("any-crc");
   ano_any_mode  = ac("any-mode");
   ano_any_layer = ac("any-layer");
   ano_any_bit   = ac("any-bitrate");
   ano_any_emp   = ac("any-emphasis");
   ano_any_rate  = ac("any-sampling");
   ano_any_ver   = ac("any-version");
   ign_crc   = ac("ign-crc-error");
   ign_start = ac("ign-junk-start");
   ign_end   = ac("ign-junk-end");
   ign_tag   = ac("ign-tag128");
   ign_bit   = ac("ign-bitrate-sw");
   ign_const = ac("ign-constant-sw");
   ign_trunc = ac("ign-truncated");
   ign_noamp = ac("ign-non-ampeg");
   ign_sync  = ac("ign-resync");
   
   
   // check all files
   int err=0;
   int checked=0;
   int num_ano=0;
   CRC16 crc(CRC16::CRC_16);   
   FILE *log = 0;
   if(ac.getString("log-file")) {
      log = fopen(ac.getString("log-file"), "a");
      if(log==0)
	userError("can't open logfile '%s'!\n", *ac.getString("log-file"));
   }
   for(int i=0; i < ac.numParam(); i++) {
      
      // check for file
      const char *name = ac.param(i);
      struct stat buf;
      if(stat(name, &buf)) {
	 if(!quiet) printf("%s: can't stat file (dangling symbolic link?)\n", name);
	 continue;
      }
      if(S_ISDIR(buf.st_mode)) {
	 if(!quiet) printf("%s: ignoring directory\n", name);
	 continue;
      }
      if(!S_ISREG(buf.st_mode)) {
	 if(!quiet) printf("%s: ignoring non regular file\n", name);
	 continue;
      }
      int len = buf.st_size;
      
      // open file
      int flags = O_RDONLY;
      if(ac("fix-headers")) flags = O_RDWR;
      int fd = open(name, flags);
      if(fd==-1) {
	 perror("open");
	 userError("can't open file '%s' for reading!\n", name);      
      }
      // mmap file
      int prot = PROT_READ;
      if(ac("fix-headers")) prot |= PROT_WRITE;
      const uchar *p = (const uchar *) mmap(0, len, prot, MAP_SHARED, fd, 0);
      const uchar *free_p = p;
      if(p==(const uchar *)-1) {
	 perror("mmap");
	 userError("can't map file '%s'!\n", name);
      }

      // list
      if(ac("list")||ac("compact-list")||ac("raw-list")) {
	 // speed up list of very large files (like *.wav)
	 int maxl = 128*1024; // search max 128k
	 int start = find_next_header(p, len<=maxl?len:maxl, min_valid);
	 if(start<0) {
	    if(!ign_noamp) {
	       if(ac("raw-list")) {
		  printf("invalid_stream%c%s%c%c", rawsep, name, rawsep, rawlinesep);
	       } else {
		  printf("%s%s%s: %snot an audio mpeg 1.0/2.0 stream%s\n", 
			 cfil, name, cnor, cerror, cnor);
	       }
	       err++;
	    }
	 } else {
	    Header h = get_header(p+start);
	    int l_min = (len*8)/h.bitrate();
	    int l_mil = l_min%1000;
	    l_min/=1000;
	    int l_sec = l_min%60;
	    l_min/=60;
	    if(ac("list")) {
	       TString n = single_line?TString(name):TString(name).shortFilename(79);
	       printf("%s%s%s:%cmpeg %s%3.1f%s layer %s%d%s %s%2.1f%skHz %s%3d%skBit/s %s%-12.12s%s %s%-7.7s%s %s%s%s %s%s%s %s%s%s %s%2d:%02d.%02d%s\n", 
		      cfil, *n, cnor, single_line?' ':'\n',
		      h.version()==1.0?cval:cano, h.version(), cnor, 
		      h.layer()==3?cval:cano, h.layer(), cnor, 
		      h.samp_rate()==44.1?cval:cano, h.samp_rate(), cnor, 
		      (h.bitrate()==112)||(h.bitrate()==128)?cval:cano, h.bitrate(), cnor,
		      h.mode==Header::JOINT_STEREO?cval:cano, h.mode_str(), cnor,
		      h.emphasis==Header::emp_NONE?cval:cano, h.emphasis_str(), cnor,
		      h.protection_bit?cano:cval, h.protection_bit?"---":"crc", cnor, 
		      h.original?cval:cano, h.original?"orig":"copy", cnor,
		      h.copyright?cano:cval, h.copyright?"cprgt":"-----", cnor,
		      cval, l_min, l_sec, l_mil/10, cnor);  
	    } else if(ac("compact-list")) {
	       TString n = TString(name).shortFilename(79-25);
	       printf("%s%c%s%s%d%s %s%2.0f%s %s%3d%s %s%s%s %s%s%s %s%s%s%s%s%s%s%s%s %s%2d:%02d%s %s%s%s\n", 
		      h.version()==1.0?cval:cano, h.version()==1.0?'l':'L', cnor, 
		      h.layer()==3?cval:cano, h.layer(), cnor, 
		      h.samp_rate()==44.1?cval:cano, h.samp_rate(), cnor, 
		      (h.bitrate()==112)||(h.bitrate()==128)?cval:cano, h.bitrate(), cnor,
		      h.mode==Header::JOINT_STEREO?cval:cano, h.short_mode_str(), cnor,
		      h.emphasis==Header::emp_NONE?cval:cano, h.short_emphasis_str(), cnor,
		      h.protection_bit?cano:cval, h.protection_bit?"-":"C", cnor, 
		      h.original?cval:cano, h.original?"O":"-", cnor,
		      h.copyright?cano:cval, h.copyright?"Y":"-", cnor,
		      cval, l_min, l_sec, cnor, cfil, *n, cnor);  
	    } else if(ac("raw-list")) {	       
	       printf("valid_stream%c%.1f%c%d%c%.1f%c%d%c%s%c%s%c%s%c%s%c%s%c%d%c%d%c%d%c%s%c%c",
		      rawsep,
		      h.version(), rawsep,
		      h.layer(), rawsep, 
		      h.samp_rate(), rawsep,
		      h.bitrate(), rawsep,
		      h.mode_str(), rawsep,
		      h.emphasis_str(), rawsep,
		      h.protection_bit?"---":"crc", rawsep,
		      h.original?"orig":"copy", rawsep,
		      h.copyright?"cprgt":"-----", rawsep,
		      l_min, rawsep, 
		      l_sec, rawsep, 
		      l_mil, rawsep,
		      name, rawsep, rawlinesep);
	    }
	 }
      }
      
      // cut-junk-start
      if(ac("cut-junk-start")) {
	 int start = find_next_header(p, len, min_valid);
	 if(start<0) {
	    fmes(name, "%snot an audio mpeg 1.0/2.0 stream%s\n", cerror, cnor);
	    err++;
	 } else if(start==0) {
	    printf("%s%s%s: %scut-junk-start: no junk found%s\n", 
		   cfil, name, cnor, cok, cnor);	    
	 } else {
	    printf("%s%s%s: %scut-junk-start: removing first %s%d%s bytes, retrying%s\n",
		   cfil, name, cnor, cok, cval, start, cok, cnor);	    
	    if(!dummy) {
	       // create new file
	       p+=start;
	       len-=start;
	       TString fn(name);
	       fn+="~mp3ctf~";
	       FILE *f=fopen(fn, "w");
	       if(f==0) 
		 userError("error while opening file '%s' for writing!\n", *fn);
	       if(fwrite(p, 1, len, f)!=(uint)len)
		 userError("error while writing %d bytes to file '%s'! (disk full?)\n", len, *fn);		 
	       fclose(f);
	       
	       // unmap file and close
	       if(munmap((char*)free_p, len)) {
		  perror("munmap");
		  userError("can't unmap file '%s'!\n", name);
	       }
	       close(fd);      
	       
	       // move file
	       if(rename(fn, name))
		 userError("error while renaming file '%s' as '%s'!\n", *fn, name);
	       
	       // retry this file 
	       --i;
	       continue;
	    }
	 }
      }

      // cut-junk-end
      if(ac("cut-junk-end")) {
	 // lots of side effects: perhaps unmaps closes file
	 if(cut_junk_end(name, p, len, free_p, fd, err)) {
	    // retry this file 
	    --i;
	    continue;	   
	 }	   
      }
      
      // check for errors
      if(ac("error-check") || ac("fix-headers")) {
	 if(progress) {
	    TString s = TString(name).shortFilename(79);
	    fprintf(stderr, "%-79.79s\r", *s);
	    fflush(stderr);
	 }
	 if(error_check(name, p, len, crc, ac("fix-headers"))) {
	    if(log) fprintf(log, "%s\n", name);
	    ++err;
	 }
      }

      // check for anomalies
      if(ac("anomaly-check")) {
	 if(progress) {
	    TString s = TString(name).shortFilename(79);
	    fprintf(stderr, "%-79.79s\r", *s);
	    fflush(stderr);
	 }
	 if(anomaly_check(name, p, len, ac("error-check"), err)) ++num_ano;
      }      
            
      // dump header
      if(ac("dump-header")) {
	 printf("%s%s%s:\n", cfil, name, cnor);	 
	 for(int k=0; k<len-3; p++, k++) {
	    if(*p==255) {
	       Header h;	       
	       h = get_header(p);
	       if(h.syncword==0xfff) {
		  TString s=h.print();
		  printf("%7d %s\n", k, *s);
		  int l = frame_length(h);
		  if(l>=21) {
		     p+=l;
		     p--;
		     k+=l;
		     k--;
		  }
	       }
	    }
	 }
      }      
      
      
      // unmap file and close
      if(munmap((char*)free_p, len)) {
	 perror("munmap");
	 userError("can't unmap file '%s'!\n", name);
      }
      close(fd);      
      ++checked;
   }

   // print final statistics
   if((ac.numParam()>1)&&(!ac("raw-list"))) {
      printf("--                                                                             \n%s%d%s file%s %s, %s%d%s erroneous file%s found\n", 
	     cval, checked, cnor, checked==1?"":"s", 
	     ac("list")?"listed":"checked", cval, err, cnor, err==1?"":"s");
      if(num_ano) printf("(%s%d%s %sanomal%s%s found)\n", 
			 cval, num_ano, cnor, cano, num_ano==1?"y":"ies", cnor);      
   }
   
   // end
   if(log) fclose(log);
   
   return 0;
}

