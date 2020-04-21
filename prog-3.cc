// Written by Ch. Tronche (http://tronche.lri.fr:8000/)
// Copyright by the author. This is unmaintained, no-warranty free software. 
// Please use freely. It is appreciated (but by no means mandatory) to
// acknowledge the author's contribution. Thank you.
// Started on Thu Jun 26 23:29:03 1997

//
// Xlib tutorial: 2nd program
// Make a window appear on the screen and draw a line inside.
// If you don't understand this program, go to
// http://tronche.lri.fr:8000/gui/x/xlib-tutorial/2nd-program-anatomy.html
//

#include <X11/Xlib.h> // Every Xlib program must include this
#include <assert.h>   // I include this to test return values the lazy way
#include <unistd.h>   // So we got the profile for 10 seconds
#include <stdio.h>
#include <X11/Xutil.h>

#define NIL (0)       // A name for the void pointer

main()
{
      // Open the display
      int microseconds_in_second = 1000000;
      Display *dpy = XOpenDisplay(NIL);
      assert(dpy);

      // Get some colors

      int blackColor = BlackPixel(dpy, DefaultScreen(dpy));
      int whiteColor = WhitePixel(dpy, DefaultScreen(dpy));

    // Get width and height of the display
    int windowHeight = XDisplayHeight (dpy, 0);
    int windowWidth = XDisplayWidth(dpy, 0);


      // Create the window

      // Window w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, 
				  //    200, 100, 0, blackColor, blackColor);
      Window w = XCreateWindow(dpy, DefaultRootWindow(dpy), 0, 0, 
                   windowHeight, windowWidth, 5, 
                   CopyFromParent, CopyFromParent, CopyFromParent,
                   NIL, 0);

      // We want to get MapNotify events

      XSelectInput(dpy, w, StructureNotifyMask);

      // "Map" the window (that is, make it appear on the screen)

      XMapWindow(dpy, w);

      // Create a "Graphics Context"

      GC gc = XCreateGC(dpy, w, 0, NIL);

      // Tell the GC we draw using the white color

      XSetForeground(dpy, gc, whiteColor);

      // Wait for the MapNotify event

      for(;;) {
	    XEvent e;
	    XNextEvent(dpy, &e);
	    if (e.type == MapNotify)
		  break;
      }

      // Draw the line
      usleep(10000);
      
      XDrawLine(dpy, w, gc, 0, 0, 180, 20);

      // Send the "DrawLine" request to the server

      XFlush(dpy);

      // see https://stackoverflow.com/questions/27977081/how-to-get-pixel-color-from-a-pixmap
      // see https://stackoverflow.com/questions/38554986/missing-xgetpixel-in-x11-xlib-h
    // Get the root of the display
      Window root = DefaultRootWindow(dpy);

    // Map the root window
    XMapWindow(dpy, root);


    // Get dump of screen
    XImage *image = XGetImage(dpy, root, 0, 0, windowWidth, windowHeight, AllPlanes, ZPixmap);


      unsigned long pixel = XGetPixel(image, 0, 200);

      printf("The pixel: %lx\n", pixel);
      //printf("%lu\n", pixel)

      // Wait for 10 seconds

      usleep(microseconds_in_second * 0.5);
}

