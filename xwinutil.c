/* xwinutil.c */
#define XWIN_UTIL
#include "xdisp.h"
void xwin_init(void)
{
  xwin_context=XUniqueContext();
}
