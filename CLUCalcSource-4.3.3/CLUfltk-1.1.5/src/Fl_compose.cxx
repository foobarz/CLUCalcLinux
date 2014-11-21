//
// "$Id: Fl_compose.cxx,v 1.3 2005/10/18 08:23:35 chp Exp $"
//
// Character compose processing for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl.H>

//
// MRS: Uncomment the following define to get the original (pre-1.1.2)
//      dead key support code.  The original code apparently did not
//      work on Belgian keyboards.
//

//#define OLD_DEAD_KEY_CODE


static const char* const compose_pairs =
"  ! % # $ y=| & : c a <<~ - r _ * +-2 3 ' u p . , 1 o >>141234? "
"`A'A^A~A:A*AAE,C`E'E^E:E`I'I^I:I-D~N`O'O^O~O:Ox O/`U'U^U:U'YTHss"
"`a'a^a~a:a*aae,c`e'e^e:e`i'i^i:i-d~n`o'o^o~o:o-:o/`u'u^u:u'yth:y";

#if !defined(WIN32) && defined(OLD_DEAD_KEY_CODE) // X only
// X dead-key lookup table.  This turns a dead-key keysym into the
// first of two characters for one of the compose sequences.  These
// keysyms start at 0xFE50.
// Win32 handles the dead keys before FLTK can see them.  This is
// unfortunate, because you don't get the preview effect.
static char dead_keys[] = {
  '`',	// XK_dead_grave
  '\'',	// XK_dead_acute
  '^',	// XK_dead_circumflex
  '~',	// XK_dead_tilde
  '_',	// XK_dead_macron
  0,	// XK_dead_breve
  '.',	// XK_dead_abovedot
  ':',	// XK_dead_diaeresis
  '*',	// XK_dead_abovering
  0,	// XK_dead_doubleacute
  'v',	// XK_dead_caron
  ','	// XK_dead_cedilla
//   0,	// XK_dead_ogonek
//   0,	// XK_dead_iota
//   0,	// XK_dead_voiced_sound
//   0,	// XK_dead_semivoiced_sound
//   0	// XK_dead_belowdot
};
#endif // !WIN32 && OLD_DEAD_KEY_CODE

int Fl::compose_state = 0;

int Fl::compose(int& del) {

  del = 0;
  unsigned char ascii = (unsigned)e_text[0];

  // Alt+letters are reserved for shortcuts.  But alt+foreign letters
  // has to be allowed, because some key layouts require alt to be held
  // down in order to type them...
  if (e_state & (FL_ALT|FL_META) && !(ascii & 128)) return 0;

  if (compose_state == 1) { // after the compose key

    if (ascii == ' ') { // space turns into nbsp
      e_text[0] = char(0xA0);
      compose_state = 0;
      return 1;
    }

    // see if it is either character of any pair:
    for (const char *p = compose_pairs; *p; p += 2) 
      if (p[0] == ascii || p[1] == ascii) {
	if (p[1] == ' ') e_text[0] = (p-compose_pairs)/2+0xA0;
	compose_state = ascii;
	return 1;
      }

    if (e_length) { // compose key also "quotes" control characters
      compose_state = 0;
      return 1;
    }

  } else if (compose_state) { // second character of compose

    char c1 = char(compose_state); // retrieve first character
    // now search for the pair in either order:
    for (const char *p = compose_pairs; *p; p += 2) {
      if (p[0] == ascii && p[1] == c1 || p[1] == ascii && p[0] == c1) {
	e_text[0] = (p-compose_pairs)/2+0xA0;
	del = 1; // delete the old character and insert new one
	compose_state = 0;
	return 1;
      }
    }

  }

  int i = e_keysym;

  // See if they type the compose prefix key:
//  if (i == FL_Control_R || i == 0xff20/* Multi-Key */) {
  // I don't want right ctrl to be special. Changed by C.Perwass 23.8.04
  if (i == 0xff20/* Multi-Key */) {
    compose_state = 1;
    return 1;
  }

#ifndef WIN32 // X only
  // See if they typed a dead key.  This gets it into the same state as
  // typing prefix+accent:
  if (i >= 0xfe50 && i <= 0xfe5b) {
#  ifdef OLD_DEAD_KEY_CODE
    ascii = dead_keys[i-0xfe50];
    for (const char *p = compose_pairs; *p; p += 2)
      if (p[0] == ascii) {
	compose_state = ascii;
	return 1;
      }
#  else
    ascii = e_text[0];
    for (const char *p = compose_pairs; *p; p += 2)
      if (p[0] == ascii ||
          (p[1] == ' ' && (p - compose_pairs) / 2 + 0xA0 == ascii)) {
        compose_state = p[0];
        return 1;
      }
#  endif // OLD_DEAD_KEY_CODE
    compose_state = 0;
    return 1;
  }
#endif

  // Only insert non-control characters:
  if (e_length 
	  && (!(e_keysym & 0xff00) || (e_state & FL_NUM_LOCK)) /* added by C.Perwass 18.10.05 to allow KeyPad to work */
	  && (ascii & ~31 && ascii!=127)) 
  {
	  compose_state = 0; 
	  return 1;
  }

  return 0;
}

