//
// "$Id: fl_color_mac.cxx,v 1.1 2004/02/18 16:10:37 chp Exp $"
//
// MacOS color functions for the Fast Light Tool Kit (FLTK).
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

// The fltk "colormap".  This allows ui colors to be stored in 8-bit
// locations, and provides a level of indirection so that global color
// changes can be made.  Not to be confused with the X colormap, which
// I try to hide completely.

// MacOS - matt: the macintosh port does not support colormaps

#include <config.h>
#include <FL/Fl.H>
#include <FL/x.H>
#include <FL/fl_draw.H>

static unsigned fl_cmap[256] = {
#include "fl_cmap.h" // this is a file produced by "cmap.cxx":
};

// Translations to mac data structures:
Fl_XMap fl_xmap[256];

Fl_XMap* fl_current_xmap;

Fl_Color fl_color_;

void fl_color(Fl_Color i) {
  fl_color_ = i;
  int index;
  uchar r, g, b;
  if (i & 0xFFFFFF00) {
    // translate rgb colors into color index
    r = i>>24;
    g = i>>16;
    b = i>> 8;
  } else {
    // translate index into rgb:
    index = i;
    unsigned c = fl_cmap[i];
    r = c>>24;
    g = c>>16;
    b = c>> 8;
  }
  RGBColor rgb; 
  rgb.red   = (r<<8)|r;
  rgb.green = (g<<8)|g;
  rgb.blue  = (b<<8)|b;
  RGBForeColor(&rgb);
}

void fl_color(uchar r, uchar g, uchar b) {
  RGBColor rgb; 
  fl_color_ = fl_rgb_color(r, g, b);
  rgb.red   = (r<<8)|r;
  rgb.green = (g<<8)|g;
  rgb.blue  = (b<<8)|b;
  RGBForeColor(&rgb);
}

void Fl::set_color(Fl_Color i, unsigned c) {
  if (fl_cmap[i] != c) {
    fl_cmap[i] = c;
  }
}

//
// End of "$Id: fl_color_mac.cxx,v 1.1 2004/02/18 16:10:37 chp Exp $".
//
