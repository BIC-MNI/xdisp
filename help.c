/*
 *      help.c
 *
 *      All help information for Xdisp.
 *
 *      Syntax()
 *      Help()
 *      online_Help()
 *      FatalError()
 *
 *      Copyright (c) B. Pike, 1993-2000
 */


#include "xdisp.h"
 
/*---------------------------- Syntax-----------------------------*/
void Syntax(char *cmd)
{
  fprintf(stderr,"Usage: %s filename ",cmd);
  fprintf(stderr,"[-geometry WxH+x+y|-geom WxH+x+y] ");
  fprintf(stderr,"[-font font|-fn font] ");
  fprintf(stderr,"[-foreground color|-fg color] ");
  fprintf(stderr,"[-background color|-bg color] ");
  fprintf(stderr,"[-grc graphics_color] ");
  fprintf(stderr,"[-PseudoColor|-DirectColor|-TrueColor] ");
  fprintf(stderr,"[-name window_name] ");
  fprintf(stderr,"[-noiconify] ");
  fprintf(stderr,"[-private] ");
  fprintf(stderr,"[-cmi] ");
  fprintf(stderr,"[-gery|-hot|-spectral] ");
  fprintf(stderr,"[-swap] ");
  fprintf(stderr,"[-color_bar] ");
  fprintf(stderr,"[-bilinear|-nn|-nearest_neighbour] ");
  fprintf(stderr,"[-all] ");
  fprintf(stderr,"[-image image_number] ");
  fprintf(stderr,"[-gamma] ");
  fprintf(stderr,"[-byte|-ubyte|-short|-ushort|-int|-uint|-long|-ulong|-float|-double] ");
  fprintf(stderr,"[-nc num_colors] ");
  fprintf(stderr,"[-wl] ");
  fprintf(stderr,"[-roi roi_filename] ");
  fprintf(stderr,"[-rescale] ");
  fprintf(stderr,"[-verbose|-v] ");
  fprintf(stderr,"[-height height|-h height] ");
  fprintf(stderr,"[-width width|-w width]  ");
  fprintf(stderr,"[-offset byte_offset|-o byte_offset] ");
  fprintf(stderr,"[-skip byte_skip] ");
  fprintf(stderr,"[-upper upper|-u upper] ");
  fprintf(stderr,"[-lower lower|-l lower] ");
  fprintf(stderr,"[-zoom zoom_factor|-z zoom_factor] ");
  fprintf(stderr,"[-version] ");    
  fprintf(stderr,"[-help] ");
  fprintf(stderr,"[--] \n");
  exit_xdisp(1);
}


/*----------------------------- Help -----------------------------*/
void Help(void)
{
  fprintf(stderr,"xdisp (%s): An image display utility for X systems.\n",version);
  fprintf(stderr,"Usage: xdisp filename [options]\n");
  fprintf(stderr,"     \n");
  fprintf(stderr,"Options: \n");
  fprintf(stderr,"-w width  \n");
  fprintf(stderr,"-width width ->      image width in pixels (default=256)\n");
  fprintf(stderr,"-h height \n");
  fprintf(stderr,"-h height ->         image height in pixels (256)\n");
  fprintf(stderr,"-o byte_offset \n");
  fprintf(stderr,"-offset offset ->    offset in bytes to image data (0)\n");
  fprintf(stderr,"-skip offset ->      offset in bytes between image elements (0)\n");
  fprintf(stderr,"-u upper  \n");
  fprintf(stderr,"-upper upper ->      initial colormap upper value (image max)\n");
  fprintf(stderr,"-l lower  \n");
  fprintf(stderr,"-lower lower ->      initial colormap lower value (image min)\n");
  fprintf(stderr,"-wl ->               always display window/level (False)\n");
  fprintf(stderr,"-all ->              load all images from file upon startup\n");
  fprintf(stderr,"-byte ->             data is signed byte \n");
  fprintf(stderr,"-ubyte ->            data is unsigned byte \n");
  fprintf(stderr,"-short ->            data is signed short integer (default)\n");
  fprintf(stderr,"-ushort ->           data is unsigned short integer \n");
  fprintf(stderr,"-int ->              data is signed integer \n");
  fprintf(stderr,"-uint ->             data is unsigned integer \n");
  fprintf(stderr,"-long ->             data is signed long integer \n");
  fprintf(stderr,"-ulong ->            data is unsigned long integer \n");
  fprintf(stderr,"-float ->            data is float \n");
  fprintf(stderr,"-double ->           data is double \n");
  fprintf(stderr,"-nc num_colors ->    number of colors to allocate\n");
  fprintf(stderr,"-cmi ->              image is in CMI format\n");
  fprintf(stderr,"-color_bar ->        display a color_scale at the side of the image\n");
  fprintf(stderr,"-bilinear ->         use bilinear interpolation for all resizes (default)\n");
  fprintf(stderr,"-nearest_neighbour \n");
  fprintf(stderr,"-nn ->               use nearest neighbour interpolation for resizes\n");
  fprintf(stderr,"-rescale ->          always rescale image (False)\n");
  fprintf(stderr,"-v \n");
  fprintf(stderr,"-verbose ->          verbose mode (False)\n");
  fprintf(stderr,"-z factor  \n");
  fprintf(stderr,"-zoom factor ->      initial zoom factor (1.0)\n");
  fprintf(stderr,"-gamma gamma ->      initial Gamma correction factor (1.0)\n");
  fprintf(stderr,"-grc color ->        graphics overlay color\n");
  fprintf(stderr,"-grey ->             use a grey scale color table (default)\n");
  fprintf(stderr,"-hot ->              use a hot metal color table\n");
  fprintf(stderr,"-spectral ->         use spectral color table \n");
  fprintf(stderr,"-image num ->        initial image to display\n");
  fprintf(stderr,"-name window_name -> name of windows and icons\n");
  fprintf(stderr,"-noiconify ->        initial command window state\n");
  fprintf(stderr,"-fg color \n");
  fprintf(stderr,"-foreground color -> foreground color\n");
  fprintf(stderr,"-bg color \n");
  fprintf(stderr,"-background color -> background color\n");
  fprintf(stderr,"-geom WxH+x+y \n");
  fprintf(stderr,"-geometry WxH+x+y -> main window geometry\n");
  fprintf(stderr,"-private ->          install a private color map\n");
  fprintf(stderr,"-roi filename ->     ROI file name\n");
  fprintf(stderr,"-swap ->             swap bytes of image data\n");    
  fprintf(stderr,"-PseudoColor ->      attempt to use a PseudoColor visual (default)\n");
  fprintf(stderr,"-DirectColor ->      attempt to use a DirectColor visual\n");
  fprintf(stderr,"-TrueColor ->        attempt to use a TrueColor visual\n");
  fprintf(stderr,"-version ->          display the version number\n");
  fprintf(stderr,"-help  ->            this help message\n");
  fprintf(stderr,"-- ->                read from standard in\n");
  fprintf(stderr,"Within the image window moving the mouse with the middle button\n");
  fprintf(stderr,"depressed changes the window/level.  Hold down the left button\n");
  fprintf(stderr,"to draw ROIs.  The right button (de)iconifies the command window.\n");
  fprintf(stderr,"Type 'h' within the image window for a complete help listing.\n");
  fprintf(stderr,"Example 1: display a 512x512 byte image and skip a 2048 byte header \n");
  fprintf(stderr,"xdisp my_image.byte -width 512 -height 512 -offset 2048 -byte -noiconify\n\n");
  fprintf(stderr,"Example 2: display a minc file, load the entire volume at startup and \n");
  fprintf(stderr,"           use a TrueColor visual, spectral colormap and show the colorbar \n");
  fprintf(stderr,"xdisp cool_stuff.mnc -all -TrueColor -spectral -color_bar \n\n");
}

/*----------------------------- online_Help -----------------------------*/
void online_Help(void)
{
  fprintf(stderr,"xdisp (%s): An image display utility for X systems.\n\n",version);
  fprintf(stderr,"Moving the mouse with the middle button depressed changes the window/level.\n");
  fprintf(stderr,"Hold down the left button to draw ROI's.  The right button opens/closes the \n");
  fprintf(stderr,"the command window. The following keys are also defined:\n");
  fprintf(stderr,"     q ->   quit\n");
  fprintf(stderr,"     a ->   auto-scale image\n");
  fprintf(stderr,"     b ->   toggle the color bar state\n");
  fprintf(stderr,"     B ->   save image to file in byte format\n");
  fprintf(stderr,"     c ->   crop image to current roi size\n");
  fprintf(stderr,"     C ->   cycle through color maps (grey, hot metal, spectral)\n");    
  fprintf(stderr,"     d ->   toggle current volume dimension\n");
  fprintf(stderr,"     f ->   output image to an ecapsulated postscript (EPSF) file\n");
  fprintf(stderr,"     G ->   create a GIF file\n");
  fprintf(stderr,"     h ->   this help text\n"); 
  fprintf(stderr,"     H ->   this help text\n"); 
  fprintf(stderr,"     i ->   toggle between bilinear and nearest neighbour interpolation\n"); 
  fprintf(stderr,"     l ->   load all images from file\n");
  fprintf(stderr,"     L ->   enter Lower value of color map\n");
  fprintf(stderr,"     m ->   calculate maximum intensity projection image\n");
  fprintf(stderr,"     M ->   save image to a file in matlab format\n");
  fprintf(stderr,"     n ->   reload image as a new file\n");
  fprintf(stderr,"     o ->   reorient entire volume\n");
  fprintf(stderr,"     p ->   print image\n");
  fprintf(stderr,"     P ->   suspend plotting\n");
  fprintf(stderr,"     r ->   rescale image to current window/level range\n");
  fprintf(stderr,"     R ->   refresh image (erase any overlays)\n");
  fprintf(stderr,"     s ->   image statistics\n");
  fprintf(stderr,"     S ->   save image to file in short integer format\n");    
  fprintf(stderr,"     t ->   toggle (invert) greyscale\n");
  fprintf(stderr,"     T ->   create a TIFF file\n");
  fprintf(stderr,"     u ->   unzoom image (restore original size)\n");
  fprintf(stderr,"     U ->   enter Upper value of color map\n");
  fprintf(stderr,"     v ->   print version number\n");
  fprintf(stderr,"     w ->   toggle window/level display\n");
  fprintf(stderr,"     + ->   increment image number by 1\n");
  fprintf(stderr,"     - ->   decrement image number by 1\n");
  fprintf(stderr,"     * ->   increment image number by 10\n");
  fprintf(stderr,"     / ->   decrement image number by 10\n");
  fprintf(stderr,"     ? ->   this help text\n"); 
  fprintf(stderr,"   x/X ->   shrink/enlarge image width by a factor of 2\n");
  fprintf(stderr,"   y/Y ->   shrink/enlarge image height by a factor of 2\n");
  fprintf(stderr,"   z/Z ->   shrink/enlarge image by a factor of 2\n");
  fprintf(stderr,"    ^T ->   toggle external roi display state\n");    
  fprintf(stderr," ^h/^v ->   horizontal/vertical profiles\n");
  fprintf(stderr," ^X/^Y ->   flip in x/y direction\n");
  fprintf(stderr,"    ^F ->   open file selector to load new image\n");
  fprintf(stderr,"    ^I ->   open minc header information window\n");
  fprintf(stderr,"    ^S ->   open file selector to spawn a new xdisp\n");
  fprintf(stderr,"    ^P ->   plot a time intensity curve for the current ROI\n");
  fprintf(stderr,"    ^R ->   rotate image clockwise\n\n");
}


/*----------------------------- FatalError -------------------------*/
void FatalError (char *identifier)
{
  fprintf(stderr, "%s: %s\n", cmd, identifier);
  exit_xdisp(-1);
}

