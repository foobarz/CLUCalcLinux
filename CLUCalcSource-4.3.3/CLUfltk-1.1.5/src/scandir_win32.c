/*
 * "$Id: scandir_win32.c,v 1.1 2004/02/18 16:10:43 chp Exp $"
 *
 * WIN32 scandir function for the Fast Light Tool Kit (FLTK).
 *
 * Copyright 1998-2003 by Bill Spitzak and others.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA.
 *
 * Please report all bugs and problems to "fltk-bugs@fltk.org".
 */

#ifndef __CYGWIN__
/* Emulation of posix scandir() call */

#include <FL/filename.H>
#include "flstring.h"
#include <windows.h>
#include <stdlib.h>

int fl_scandir(const char *dirname, struct dirent ***namelist,
	       int (*select)(struct dirent *),
	       int (*compar)(struct dirent **, struct dirent **)) {
  int len;
  char *findIn, *d;
  WIN32_FIND_DATA find;
  HANDLE h;
  int nDir = 0, NDir = 0;
  struct dirent **dir = 0, *selectDir;
  unsigned long ret;

  len    = strlen(dirname);
  findIn = malloc(len+5);

  if (!findIn) return 0;

  strcpy(findIn, dirname);
  for (d = findIn; *d; d++) if (*d=='/') *d='\\';
  if ((len==0)) { strcpy(findIn, ".\\*"); }
  if ((len==1)&& (d[-1]=='.')) { strcpy(findIn, ".\\*"); }
  if ((len>0) && (d[-1]=='\\')) { *d++ = '*'; *d = 0; }
  if ((len>1) && (d[-1]=='.') && (d[-2]=='\\')) { d[-1] = '*'; }
  
  if ((h=FindFirstFile(findIn, &find))==INVALID_HANDLE_VALUE) {
    free(findIn);
    ret = GetLastError();
    if (ret != ERROR_NO_MORE_FILES) {
      /* TODO: return some error code */
    }
    *namelist = dir;
    return nDir;
  }
  do {
    selectDir=(struct dirent*)malloc(sizeof(struct dirent)+strlen(find.cFileName)+1);
    strcpy(selectDir->d_name, find.cFileName);
    if (find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      // Append a trailing slash to directory names...
      strcat(selectDir->d_name, "/");
    }
    if (!select || (*select)(selectDir)) {
      if (nDir==NDir) {
	struct dirent **tempDir = calloc(sizeof(struct dirent*), NDir+33);
	if (NDir) memcpy(tempDir, dir, sizeof(struct dirent*)*NDir);
	if (dir) free(dir);
	dir = tempDir;
	NDir += 32;
      }
      dir[nDir] = selectDir;
      nDir++;
      dir[nDir] = 0;
    } else {
      free(selectDir);
    }
  } while (FindNextFile(h, &find));
  ret = GetLastError();
  if (ret != ERROR_NO_MORE_FILES) {
    /* TODO: return some error code */
  }
  FindClose(h);

  free (findIn);

  if (compar) qsort (dir, nDir, sizeof(*dir),
		     (int(*)(const void*, const void*))compar);

  *namelist = dir;
  return nDir;
}

#endif

/*
 * End of "$Id: scandir_win32.c,v 1.1 2004/02/18 16:10:43 chp Exp $".
 */
