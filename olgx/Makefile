# To the point Makefile for libolgx.a
# (modified from xview-3.2p1.4.src.tar distribution)
#
# This makefile should not require modification.
#
# B Pike, Oct 1997
#

HDRS = olgx.h busy.h grey.h olgx_impl.h

SRCS = 	ol_button.c ol_color.c ol_draw.c ol_init.c ol_misc.c \
	ol_sb.c ol_slider.c

OBJS = 	ol_button.o ol_color.o ol_draw.o ol_init.o ol_misc.o \
	ol_sb.o ol_slider.o

all: libolgx.a

.c.o: 
	gcc -c -O3 -I. $<

libolgx.a: $(OBJS) 
	rm -f libolgx.a
	ar rc libolgx.a $(OBJS)
	ranlib libolgx.a

clean:
	rm -f $(OBJS) libolgx.a





