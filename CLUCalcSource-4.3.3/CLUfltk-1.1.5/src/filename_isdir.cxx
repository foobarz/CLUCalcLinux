//
// "$Id: filename_isdir.cxx,v 1.1 2004/02/18 16:10:36 chp Exp $"
//
// Directory detection routines for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2003 by Bill Spitzak and others.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems to "fltk-bugs@fltk.org".
//

// Used by fl_file_chooser

#include "flstring.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <FL/filename.H>


#if defined(WIN32) || defined(__EMX__) && !defined(__CYGWIN__)
static inline int isdirsep(char c) {return c=='/' || c=='\\';}
#else
#define isdirsep(c) ((c)=='/')
#endif

int fl_filename_isdir(const char* n) {
  struct stat	s;

#ifdef WIN32
  char		fn[1024];
  int		length;
  // This workaround brought to you by the fine folks at Microsoft!
  // (read lots of sarcasm in that...)
  length = strlen(n);
  if (length < (int)(sizeof(fn) - 1)) {
    if (length < 4 && isalpha(n[0]) && n[1] == ':' &&
        (isdirsep(n[2]) || !n[2])) {
      // Always use D:/ for drive letters
      fn[0] = n[0];
      strcpy(fn + 1, ":/");
      n = fn;
    } else if (length > 0 && isdirsep(n[length - 1])) {
      // Strip trailing slash from name...
      length --;
      memcpy(fn, n, length);
      fn[length] = '\0';
      n = fn;
    }
  }
#endif

  return !stat(n, &s) && (s.st_mode&0170000)==0040000;
}

//
// End of "$Id: filename_isdir.cxx,v 1.1 2004/02/18 16:10:36 chp Exp $".
//
