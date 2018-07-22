/*GPL*START*
 * crc engine - for use with audio mpeg streams
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

#include "ttypes.h"

// 1998:
// 21 Jan  started
// 22 Jan  first usable version: modified and tested for use with audio mpeg
// 23 Jan  bit reversal in init removed
// 27 Jan  doc and init c(0) in cons


class CRC16 {
 public:
   enum {
      // CRC-16 polynom: x^16 + x^15 + x^2 + x^0
      CRC_16 = 0x18005, // works for audio mpeg 
      // CCITT  polynom: x^16 + x^12 + x^5 + x^0
      CCITT  = 0x11021, // (untested)
   };
   
   // create crc engine and reset to 0 (build shift table)
   CRC16(uint polynom): c(0) {
      for(uint i=0; i < 256; ++i) {
	 uint x = i<<9;
	 for(int j=0; j < 8; ++j, x<<=1) if(x&0x10000) x ^= polynom;
	 tab[i] = x>>1;
      }
   }
   
   void reset(ushort init = 0) {c = init;}         // reset engine to init
   void add(uchar b) {c = tab[(c>>8)^b] ^ (c<<8);} // add 8 bits
   ushort crc() const {return c;}                  // get current crc value
      
 private:            // private data
   ushort tab[256];  // shift table
   ushort c;         // current crc value
};


