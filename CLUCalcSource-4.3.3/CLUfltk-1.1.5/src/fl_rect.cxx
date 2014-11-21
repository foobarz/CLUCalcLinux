//
// "$Id: fl_rect.cxx,v 1.1 2004/02/18 16:10:40 chp Exp $"
//
// Rectangle drawing routines for the Fast Light Tool Kit (FLTK).
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

// These routines from fl_draw.H are used by the standard boxtypes
// and thus are always linked into an fltk program.
// Also all fl_clip routines, since they are always linked in so
// that minimal update works.

#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>
#include <FL/x.H>

void fl_rect(int x, int y, int w, int h) {
  if (w<=0 || h<=0) return;
#ifdef WIN32
  MoveToEx(fl_gc, x, y, 0L); 
  LineTo(fl_gc, x+w-1, y);
  LineTo(fl_gc, x+w-1, y+h-1);
  LineTo(fl_gc, x, y+h-1);
  LineTo(fl_gc, x, y);
#elif defined(__APPLE__)
  Rect rect;
  SetRect(&rect, x, y, x+w, y+h);
  FrameRect(&rect);
#else
  XDrawRectangle(fl_display, fl_window, fl_gc, x, y, w-1, h-1);
#endif
}

void fl_rectf(int x, int y, int w, int h) {
  if (w<=0 || h<=0) return;
#ifdef WIN32
  RECT rect;
  rect.left = x; rect.top = y;  
  rect.right = x + w; rect.bottom = y + h;
  FillRect(fl_gc, &rect, fl_brush());
#elif defined(__APPLE__)
  Rect rect;
  SetRect(&rect, x, y, x+w, y+h);
  PaintRect(&rect);
#else
  if (w && h) XFillRectangle(fl_display, fl_window, fl_gc, x, y, w, h);
#endif
}

void fl_xyline(int x, int y, int x1) {
#ifdef WIN32
  MoveToEx(fl_gc, x, y, 0L); LineTo(fl_gc, x1+1, y);
#elif defined(__APPLE__)
  MoveTo(x, y); LineTo(x1, y);
#else
  XDrawLine(fl_display, fl_window, fl_gc, x, y, x1, y);
#endif
}

void fl_xyline(int x, int y, int x1, int y2) {
#ifdef WIN32
  if (y2 < y) y2--;
  else y2++;
  MoveToEx(fl_gc, x, y, 0L); 
  LineTo(fl_gc, x1, y);
  LineTo(fl_gc, x1, y2);
#elif defined(__APPLE__)
  MoveTo(x, y); 
  LineTo(x1, y);
  LineTo(x1, y2);
#else
  XPoint p[3];
  p[0].x = x;  p[0].y = p[1].y = y;
  p[1].x = p[2].x = x1; p[2].y = y2;
  XDrawLines(fl_display, fl_window, fl_gc, p, 3, 0);
#endif
}

void fl_xyline(int x, int y, int x1, int y2, int x3) {
#ifdef WIN32
  if(x3 < x1) x3--;
  else x3++;
  MoveToEx(fl_gc, x, y, 0L); 
  LineTo(fl_gc, x1, y);
  LineTo(fl_gc, x1, y2);
  LineTo(fl_gc, x3, y2);
#elif defined(__APPLE__)
  MoveTo(x, y); 
  LineTo(x1, y);
  LineTo(x1, y2);
  LineTo(x3, y2);
#else
  XPoint p[4];
  p[0].x = x;  p[0].y = p[1].y = y;
  p[1].x = p[2].x = x1; p[2].y = p[3].y = y2;
  p[3].x = x3;
  XDrawLines(fl_display, fl_window, fl_gc, p, 4, 0);
#endif
}

void fl_yxline(int x, int y, int y1) {
#ifdef WIN32
  if (y1 < y) y1--;
  else y1++;
  MoveToEx(fl_gc, x, y, 0L); LineTo(fl_gc, x, y1);
#elif defined(__APPLE__)
  MoveTo(x, y); LineTo(x, y1);
#else
  XDrawLine(fl_display, fl_window, fl_gc, x, y, x, y1);
#endif
}

void fl_yxline(int x, int y, int y1, int x2) {
#ifdef WIN32
  if (x2 > x) x2++;
  else x2--;
  MoveToEx(fl_gc, x, y, 0L); 
  LineTo(fl_gc, x, y1);
  LineTo(fl_gc, x2, y1);
#elif defined(__APPLE__)
  MoveTo(x, y); 
  LineTo(x, y1);
  LineTo(x2, y1);
#else
  XPoint p[3];
  p[0].x = p[1].x = x;  p[0].y = y;
  p[1].y = p[2].y = y1; p[2].x = x2;
  XDrawLines(fl_display, fl_window, fl_gc, p, 3, 0);
#endif
}

void fl_yxline(int x, int y, int y1, int x2, int y3) {
#ifdef WIN32
  if(y3<y1) y3--;
  else y3++;
  MoveToEx(fl_gc, x, y, 0L); 
  LineTo(fl_gc, x, y1);
  LineTo(fl_gc, x2, y1);
  LineTo(fl_gc, x2, y3);
#elif defined(__APPLE__)
  MoveTo(x, y); 
  LineTo(x, y1);
  LineTo(x2, y1);
  LineTo(x2, y3);
#else
  XPoint p[4];
  p[0].x = p[1].x = x;  p[0].y = y;
  p[1].y = p[2].y = y1; p[2].x = p[3].x = x2;
  p[3].y = y3;
  XDrawLines(fl_display, fl_window, fl_gc, p, 4, 0);
#endif
}

void fl_line(int x, int y, int x1, int y1) {
#ifdef WIN32
  MoveToEx(fl_gc, x, y, 0L); 
  LineTo(fl_gc, x1, y1);
  // Draw the last point *again* because the GDI line drawing
  // functions will not draw the last point ("it's a feature!"...)
  SetPixel(fl_gc, x1, y1, fl_RGB());
#elif defined(__APPLE__)
  MoveTo(x, y); 
  LineTo(x1, y1);
#else
  XDrawLine(fl_display, fl_window, fl_gc, x, y, x1, y1);
#endif
}

void fl_line(int x, int y, int x1, int y1, int x2, int y2) {
#ifdef WIN32
  MoveToEx(fl_gc, x, y, 0L); 
  LineTo(fl_gc, x1, y1);
  LineTo(fl_gc, x2, y2);
  // Draw the last point *again* because the GDI line drawing
  // functions will not draw the last point ("it's a feature!"...)
  SetPixel(fl_gc, x2, y2, fl_RGB());
#elif defined(__APPLE__)
  MoveTo(x, y); 
  LineTo(x1, y1);
  LineTo(x2, y2);
#else
  XPoint p[3];
  p[0].x = x;  p[0].y = y;
  p[1].x = x1; p[1].y = y1;
  p[2].x = x2; p[2].y = y2;
  XDrawLines(fl_display, fl_window, fl_gc, p, 3, 0);
#endif
}

void fl_loop(int x, int y, int x1, int y1, int x2, int y2) {
#ifdef WIN32
  MoveToEx(fl_gc, x, y, 0L); 
  LineTo(fl_gc, x1, y1);
  LineTo(fl_gc, x2, y2);
  LineTo(fl_gc, x, y);
#elif defined(__APPLE__)
  MoveTo(x, y); 
  LineTo(x1, y1);
  LineTo(x2, y2);
  LineTo(x, y);
#else
  XPoint p[4];
  p[0].x = x;  p[0].y = y;
  p[1].x = x1; p[1].y = y1;
  p[2].x = x2; p[2].y = y2;
  p[3].x = x;  p[3].y = y;
  XDrawLines(fl_display, fl_window, fl_gc, p, 4, 0);
#endif
}

void fl_loop(int x, int y, int x1, int y1, int x2, int y2, int x3, int y3) {
#ifdef WIN32
  MoveToEx(fl_gc, x, y, 0L); 
  LineTo(fl_gc, x1, y1);
  LineTo(fl_gc, x2, y2);
  LineTo(fl_gc, x3, y3);
  LineTo(fl_gc, x, y);
#elif defined(__APPLE__)
  MoveTo(x, y); 
  LineTo(x1, y1);
  LineTo(x2, y2);
  LineTo(x3, y3);
  LineTo(x, y);
#else
  XPoint p[5];
  p[0].x = x;  p[0].y = y;
  p[1].x = x1; p[1].y = y1;
  p[2].x = x2; p[2].y = y2;
  p[3].x = x3; p[3].y = y3;
  p[4].x = x;  p[4].y = y;
  XDrawLines(fl_display, fl_window, fl_gc, p, 5, 0);
#endif
}

void fl_polygon(int x, int y, int x1, int y1, int x2, int y2) {
  XPoint p[4];
  p[0].x = x;  p[0].y = y;
  p[1].x = x1; p[1].y = y1;
  p[2].x = x2; p[2].y = y2;
#ifdef WIN32
  SelectObject(fl_gc, fl_brush());
  Polygon(fl_gc, p, 3);
#elif defined(__APPLE__)
  PolyHandle poly = OpenPoly();
  MoveTo(x, y);
  LineTo(x1, y1);
  LineTo(x2, y2);
  ClosePoly();
  PaintPoly(poly);
  KillPoly(poly);
#else
  p[3].x = x;  p[3].y = y;
  XFillPolygon(fl_display, fl_window, fl_gc, p, 3, Convex, 0);
  XDrawLines(fl_display, fl_window, fl_gc, p, 4, 0);
#endif
}

void fl_polygon(int x, int y, int x1, int y1, int x2, int y2, int x3, int y3) {
  XPoint p[5];
  p[0].x = x;  p[0].y = y;
  p[1].x = x1; p[1].y = y1;
  p[2].x = x2; p[2].y = y2;
  p[3].x = x3; p[3].y = y3;
#ifdef WIN32
  SelectObject(fl_gc, fl_brush());
  Polygon(fl_gc, p, 4);
#elif defined(__APPLE__)
  PolyHandle poly = OpenPoly();
  MoveTo(x, y);
  LineTo(x1, y1);
  LineTo(x2, y2);
  LineTo(x3, y3);
  ClosePoly();
  PaintPoly(poly);
  KillPoly(poly);
#else
  p[4].x = x;  p[4].y = y;
  XFillPolygon(fl_display, fl_window, fl_gc, p, 4, Convex, 0);
  XDrawLines(fl_display, fl_window, fl_gc, p, 5, 0);
#endif
}

void fl_point(int x, int y) {
#ifdef WIN32
  SetPixel(fl_gc, x, y, fl_RGB());
#elif defined(__APPLE__)
  MoveTo(x, y); Line(0, 0); 
#else
  XDrawPoint(fl_display, fl_window, fl_gc, x, y);
#endif
}

////////////////////////////////////////////////////////////////

#define STACK_SIZE 10
#define STACK_MAX (STACK_SIZE - 1)
static Fl_Region rstack[STACK_SIZE];
static int rstackptr=0;
int fl_clip_state_number=0; // used by gl_begin.cxx to update GL clip

#if !defined(WIN32) && !defined(__APPLE__)
// Missing X call: (is this the fastest way to init a 1-rectangle region?)
// MSWindows equivalent exists, implemented inline in win32.H
Fl_Region XRectangleRegion(int x, int y, int w, int h) {
  XRectangle R;
  R.x = x; R.y = y; R.width = w; R.height = h;
  Fl_Region r = XCreateRegion();
  XUnionRectWithRegion(&R, r, r);
  return r;
}
#endif

#ifdef __APPLE__
extern Fl_Region fl_window_region;
#endif

// undo any clobbering of clip done by your program:
void fl_restore_clip() {
  fl_clip_state_number++;
  Fl_Region r = rstack[rstackptr];
#ifdef WIN32
  SelectClipRgn(fl_gc, r); //if r is NULL, clip is automatically cleared
#elif defined(__APPLE__)
#  if 1
  if ( fl_window ) 
  {
    GrafPtr port = GetWindowPort( fl_window );
    if ( port ) { // port will be NULL if we are using a GWorld (and fl_window_region is invalid)
      RgnHandle portClip = NewRgn();
      CopyRgn( fl_window_region, portClip ); // changed
      if ( r ) 
        SectRgn( portClip, r, portClip );
      SetPortClipRegion( port, portClip );
      DisposeRgn( portClip );
    }
  }
#  else
  if (r) SetClip(r);
  else {
    Rect rect; rect.left=0; rect.top=0; rect.right=0x7fff; rect.bottom=0x7fff;
    ClipRect(&rect);
  }
#  endif
#else
  if (r) XSetRegion(fl_display, fl_gc, r);
  else XSetClipMask(fl_display, fl_gc, 0);
#endif
}

// Replace the top of the clip stack:
void fl_clip_region(Fl_Region r) {
  Fl_Region oldr = rstack[rstackptr];
  if (oldr) XDestroyRegion(oldr);
  rstack[rstackptr] = r;
  fl_restore_clip();
}

// Return the current clip region...
Fl_Region fl_clip_region() {
  return rstack[rstackptr];
}

// Intersect & push a new clip rectangle:
void fl_push_clip(int x, int y, int w, int h) {
  Fl_Region r;
  if (w > 0 && h > 0) {
    r = XRectangleRegion(x,y,w,h);
    Fl_Region current = rstack[rstackptr];
    if (current) {
#ifdef WIN32
      CombineRgn(r,r,current,RGN_AND);
#elif defined(__APPLE__)
      SectRgn(r, current, r); 
#else
      Fl_Region temp = XCreateRegion();
      XIntersectRegion(current, r, temp);
      XDestroyRegion(r);
      r = temp;
#endif
    }
  } else { // make empty clip region:
#ifdef WIN32
    r = CreateRectRgn(0,0,0,0);
#elif defined(__APPLE__)
    r = NewRgn(); 
    SetEmptyRgn(r);
#else
    r = XCreateRegion();
#endif
  }
  if (rstackptr < STACK_MAX) rstack[++rstackptr] = r;
  fl_restore_clip();
}

// make there be no clip (used by fl_begin_offscreen() only!)
void fl_push_no_clip() {
  if (rstackptr < STACK_MAX) rstack[++rstackptr] = 0;
  fl_restore_clip();
}

// pop back to previous clip:
void fl_pop_clip() {
  if (rstackptr > 0) {
    Fl_Region oldr = rstack[rstackptr--];
    if (oldr) XDestroyRegion(oldr);
  }
  fl_restore_clip();
}

// does this rectangle intersect current clip?
int fl_not_clipped(int x, int y, int w, int h) {
  if (x+w <= 0 || y+h <= 0 || x > Fl_Window::current()->w()
      || y > Fl_Window::current()->h()) return 0;
  Fl_Region r = rstack[rstackptr];
#ifdef WIN32
  if (!r) return 1;
  RECT rect;
  rect.left = x; rect.top = y; rect.right = x+w; rect.bottom = y+h;
  return RectInRegion(r,&rect);
#elif defined(__APPLE__)
  if (!r) return 1;
  Rect rect;
  rect.left = x; rect.top = y; rect.right = x+w; rect.bottom = y+h;
  return RectInRgn(&rect, r);
#else
  return r ? XRectInRegion(r, x, y, w, h) : 1;
#endif
}

// return rectangle surrounding intersection of this rectangle and clip:
int fl_clip_box(int x, int y, int w, int h, int& X, int& Y, int& W, int& H){
  X = x; Y = y; W = w; H = h;
  Fl_Region r = rstack[rstackptr];
  if (!r) return 0;
#ifdef WIN32
// The win32 API makes no distinction between partial and complete
// intersection, so we have to check for partial intersection ourselves.
// However, given that the regions may be composite, we have to do
// some voodoo stuff...
  Fl_Region rr = XRectangleRegion(x,y,w,h);
  Fl_Region temp = CreateRectRgn(0,0,0,0);
  int ret;
  if (CombineRgn(temp, rr, r, RGN_AND) == NULLREGION) { // disjoint
    W = H = 0;
    ret = 2;
  } else if (EqualRgn(temp, rr)) { // complete
    ret = 0;
  } else {	// parital intersection
    RECT rect;
    GetRgnBox(temp, &rect);
    X = rect.left; Y = rect.top; W = rect.right - X; H = rect.bottom - Y;
    ret = 1;
  }
  DeleteObject(temp);
  DeleteObject(rr);
  return ret;
#elif defined(__APPLE__)
  RgnHandle rr = NewRgn();
  SetRectRgn( rr, x, y, x+w, y+h );
  SectRgn( r, rr, rr );
  Rect rp; GetRegionBounds(rr, &rp);
  X = rp.left;
  Y = rp.top;
  W = rp.right - X;
  H = rp.bottom - Y;
  DisposeRgn( rr );
  if ( H==0 ) return 2;
  if ( h==H && w==W ) return 0;
  return 0;
#else
  switch (XRectInRegion(r, x, y, w, h)) {
  case 0: // completely outside
    W = H = 0;
    return 2;
  case 1: // completely inside:
    return 0;
  default: // partial:
    break;
  }
  Fl_Region rr = XRectangleRegion(x,y,w,h);
  Fl_Region temp = XCreateRegion();
  XIntersectRegion(r, rr, temp);
  XRectangle rect;
  XClipBox(temp, &rect);
  X = rect.x; Y = rect.y; W = rect.width; H = rect.height;
  XDestroyRegion(temp);
  XDestroyRegion(rr);
  return 1;
#endif
}

//
// End of "$Id: fl_rect.cxx,v 1.1 2004/02/18 16:10:40 chp Exp $".
//
