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
    int		(*event_handler)();
} XWIN;

typedef struct D_BUTTON
{
    char	*label;
    int		x, y, width, height;
    int		enabled;
    int		(*action)(caddr_t);
    caddr_t	action_args;
} D_BUTTON;

typedef struct D_SLIDER
{
    char	*right_label, *left_label;
    int		old_value, new_value;
    int		x, y;
    int		slider_width, slider_height;
    int		right_label_width, left_label_width;
    int		(*action)(caddr_t);
    caddr_t	action_args;
} D_SLIDER;

/* Function prototypes */
void xwin_init(void);
static int button_handler(XWIN *);
static int slider_handler(XWIN *);

XWIN *MakeXButton(int x, int y,
		  unsigned width, unsigned height, unsigned bdwidth,
		  unsigned long bdcolor, unsigned long bgcolor,
		  Window parent, char *text, int (*button_action)(),
		  caddr_t action_data, int state);

XWIN *MakeXSlider(int x, int y,
                  unsigned slider_width, unsigned slider_height,
                  unsigned left_label_width, unsigned right_label_width,
                  unsigned value, unsigned long bdcolor, unsigned long bgcolor,
                  Window parent, char *right_label, char *left_label,
                  int(*slider_action)(), caddr_t action_data);

/* Global Variables */
#ifdef XWIN_UTIL
XContext	xwin_context;
#else
extern XContext	xwin_context;
#endif
#endif		  
