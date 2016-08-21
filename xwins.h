/* xwins.h */

#ifndef XWINS_H
#define XWINS_H

#include <stdlib.h>
#include <X11/Xresource.h>

typedef struct XWIN
{
    Window	xid;
    Window	parent;
    void	*data;
    void	(*event_handler)();
} XWIN;

typedef struct D_BUTTON
{
    char	*label;
    int		x, y, width, height;
    int		enabled;
    void	(*action)(void *);
    void        *action_args;
} D_BUTTON;

typedef struct D_SLIDER
{
    char	*right_label, *left_label;
    int		old_value, new_value;
    int		x, y;
    int		slider_width, slider_height;
    int		right_label_width, left_label_width;
    void	(*action)(void *);
    void *	action_args;
} D_SLIDER;

/* Function prototypes */
void xwin_init(void);

XWIN *MakeXButton(int x, int y,
		  unsigned width, unsigned height, unsigned bdwidth,
		  ulong bdcolor, ulong bgcolor,
		  Window parent, char *text, void (*button_action)(void *),
		  void * action_data, int state);

XWIN *MakeXSlider(int x, int y,
                  unsigned slider_width, unsigned slider_height,
                  unsigned left_label_width, unsigned right_label_width,
                  unsigned value, ulong bdcolor, ulong bgcolor,
                  Window parent, char *right_label, char *left_label,
                  void (*slider_action)(void *), void * action_data);

/* Global Variables */
#ifdef XWIN_UTIL
XContext	xwin_context;
#else
extern XContext	xwin_context;
#endif
#endif		  
