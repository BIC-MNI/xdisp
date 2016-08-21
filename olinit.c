/*
 * 	olinit.c
 *
 *	Initialization stuff for olgx usage.
 *
 *	olgx_init()
 *
 *	Copyright (c) B. Pike, 1993-1997
 */

#include "xdisp.h"

/*------------------------ olgx_init --------------------------*/
void olgx_init(void)
{
  /* set up the Text fonts */
  if (font!=NULL) {
    if ((ol_it_text_finfo = ol_text_finfo =
	 XLoadQueryFont(theDisp,font))==NULL) {
      fprintf(stderr,"Warning: couldn't open specified font... attempting default.\n");
      if ((ol_it_text_finfo = ol_text_finfo =
	   XLoadQueryFont(theDisp, FALLBACK_FONT))==NULL) 
	FatalError("couldn't find fonts.\n");
    }
  }
  else {
    if ((ol_text_finfo =
	 XLoadQueryFont(theDisp, DEFAULT_FONT ))==NULL) 
      if ((ol_it_text_finfo = ol_text_finfo =
	   XLoadQueryFont(theDisp,FALLBACK_FONT))==NULL) 
	FatalError("couldn't find fonts.\n");
    if ((ol_it_text_finfo =
	 XLoadQueryFont(theDisp, DEFAULT_ITALIC_FONT))==NULL) 
      if ((ol_it_text_finfo = ol_text_finfo =
	   XLoadQueryFont(theDisp,FALLBACK_FONT))==NULL) 
	FatalError("couldn't find fonts.\n");
  }

  /* set up the Glyph font */
  if ((ol_glyph_finfo =
       XLoadQueryFont(theDisp, GLYPH_FONT))==NULL) 
    FatalError("couldn't open 'olgyph-12' font\n");
  if ((ol_it_glyph_finfo =
       XLoadQueryFont(theDisp,GLYPH_IT_FONT ))==NULL) 
    FatalError("couldn't open 'olglyph-12' font\n");

/* define the olgx colors */
  ol_fg = newC[fcol];
  ol_bg1 = newC[bcol];
  olgx_calculate_3Dcolors(&ol_fg,&ol_bg1,&ol_bg2,&ol_bg3,&ol_white);

  /* Allocate olgx colors */
  if (Selected_Visual_Class != TrueColor) {
    if (!XAllocColor(theDisp,NewCmap,&ol_fg) ||
	!XAllocColor(theDisp,NewCmap,&ol_white) ||
	!XAllocColor(theDisp,NewCmap,&ol_bg1) ||
	!XAllocColor(theDisp,NewCmap,&ol_bg2) ||
	!XAllocColor(theDisp,NewCmap,&ol_bg3)) {
      private_cmap = TRUE;
      ol_white = newC[wp];
      ol_bg2 = newC[bp];
      ol_bg3 = newC[bp];
    }
  }
  else {
    private_cmap = TRUE;
    ol_white = newC[wp];
    ol_bg2 = newC[bp];
    ol_bg3 = newC[bp];    	
  }
  ol_pixvals[OLGX_WHITE] = ol_white.pixel;
  ol_pixvals[OLGX_BLACK] = ol_fg.pixel;
  ol_pixvals[OLGX_BG1] = ol_bg1.pixel;
  ol_pixvals[OLGX_BG2] = ol_bg2.pixel;
  ol_pixvals[OLGX_BG3] = ol_bg3.pixel;

/* perform the olgx main initialization */
  ol_ginfo = olgx_main_initialize(theDisp, theScreen, theDepth, OLGX_3D_COLOR,
				  ol_glyph_finfo, ol_text_finfo,
				  ol_pixvals, NULL);
  ol_it_ginfo = olgx_main_initialize(theDisp, theScreen, theDepth, OLGX_3D_COLOR,
				     ol_it_glyph_finfo, ol_it_text_finfo,
				     ol_pixvals, NULL);
}
