AUTOMAKE_OPTIONS = check-news
ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = xdisp
dist_man1_MANS = xdisp.man1

EXTRA_DIST = $(m4_files) \
   cross_hr.bit \
   xdcicon.bit \
   xdcmask.bit \
   epm-header.in

# SUBDIRS = olgx EZWGL-1.50
SUBDIRS = olgx

m4_files = m4/mni_REQUIRE_LIB.m4 \
    m4/mni_REQUIRE_MNILIBS.m4 \
    m4/mni_REQUIRE_OPENINVENTOR.m4 \
    m4/mni_cxx_have_koenig_lookup.m4 \
    m4/smr_CGAL_MAKEFILE.m4 \
    m4/smr_OPTIONAL_LIB.m4 \
    m4/smr_REQUIRED_LIB.m4 \
    m4/smr_WITH_BUILD_PATH.m4

xdisp_SOURCES = xdisp.c xdisp.h \
acr_nema.c acr_nema.h \
element.c element.h \
ext_roi.c ext_roi.h \
file_io.c file_io.h \
group.c group.h \
message.c message.h \
bilinear.c buttons.c color.c compress.c defcolor.c evprocs.c exit.c \
ezwerror.c ezwfs.c ezwhelp.c ezwinfo.c ezwinit.c ezwinput.c ezwplot.c \
files.c handler.c help.c image.c init.c keypress.c minc.c mip.c \
motion.c ms_roi.h nneighbour.c olinit.c profile.c read.c reshape.c roi.c \
roitomin.h roitominc.h scale.c selzoom.c setcicon.c setmicon.c spectral.h \
tic.c update.c winlev.c xbutton.c xwins.h xwinutil.c 

xdisp_CFLAGS = -Iolgx -IEZWGL-1.50/include $(X_CFLAGS)
xdisp_LDADD = olgx/libolgx.a \
	EZWGL-1.50/lib/libEZ.a \
	-lpng -ltiff -ljpeg \
	$(X_PRE_LIBS) $(X_LIBS) -lX11 -lXext $(X_EXTRA_LIBS)

