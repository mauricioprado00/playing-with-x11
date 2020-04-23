/*
 * Takes a screenshot on the location of the mouse cursor
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xcomposite.h>

// Some variables you can play with :-)
int size = 220;
int step = 40;

/**
 * Save image to a PPM image file format
 * @see https://en.wikipedia.org/wiki/Netpbm#File_formats
 */
void save_ppm(XImage *pImage, char * filename) {
    FILE *fp;
    int i, j;
    char *pointer;

    fp = fopen(filename, "wb");

    if(fp == NULL)
        return;

    fprintf(fp, "P3\n");
    fprintf(fp, "%d %d\n", pImage->width, pImage->height);
    fprintf(fp, "255\n");

    pointer = pImage->data;
    for (i = 0; i < pImage->width; i++) {
        for (j = 0; j < pImage->height; j++) {
            fprintf(fp, "%hhu %hhu %hhu\n", *(char *)(pointer+2), *(char *)(pointer + 1), *(char *)(pointer + 0));
            // move to next pixel
            pointer += 4;
        }
    }
}

XImage * image_grayscale_colorimetric(XImage *pImage)
{
    XImage *grayscale = (XImage *)malloc(sizeof(XImage));
    grayscale->width = pImage->width;
    grayscale->height = pImage->height;
    grayscale->data = (char *) malloc(pImage->width * pImage->height * 4);
    char * source = pImage->data;
    char * target = grayscale->data;
    unsigned int offset = 0;
    unsigned char grayvalue;
    int i,j;
    for (i = 0; i < pImage->width; i++) {
        for (j = 0; j < pImage->height; j++) {
            grayvalue = source[offset + 2] * 0.2126 + source[offset +1] * 0.7172 + source[offset +0] * 0.0722;
            target[offset+0] = grayvalue;
            target[offset+1] = grayvalue;
            target[offset+2] = grayvalue;
            // move to next pixel
            offset += 4;
        }
    }

    return grayscale;
}

int main(int argc, char* argv[]) {
    // Setup display and such
    char *display_name = getenv("DISPLAY");
    if (!display_name) {
        fprintf(stderr, "%s: cannot connect to X server '%s'\n", argv[0], display_name);
        exit(1);
    }

    Display *display = XOpenDisplay(display_name);
    int screen = DefaultScreen(display);

    // Get the mouse cursor position
    int win_x, win_y, root_x, root_y = 0;
    unsigned int mask = 0;
    Window child_win, root_win;
    XQueryPointer(display, XRootWindow(display, screen),
        &child_win, &root_win,
        &root_x, &root_y, &win_x, &win_y, &mask);

    // Create a window at the mouse position
    XSetWindowAttributes window_attr;
    window_attr.override_redirect = 1;
    Window window = XCreateWindow(display, XRootWindow(display, screen),
        root_x - size/2, root_y - size/2,   // x, y position
        size, size,                         // width, height
        0,                                  // border width
        DefaultDepth(display, screen),      // depth
        CopyFromParent,                     // class
        DefaultVisual(display, screen),     // visual
        CWOverrideRedirect,                 // valuemask
        &window_attr                        // attributes
    );
    XMapWindow(display, window);
    XStoreName(display, window, "find-cursor");

    XClassHint *xclass = XAllocClassHint();
    xclass->res_name = "find-cursor";
    xclass->res_class = "find-cursor";
    XSetClassHint(display, window, xclass);
    XFree(xclass);

    // Keep the window on top
    XEvent e;
    memset(&e, 0, sizeof(e));
    e.xclient.type = ClientMessage;
    e.xclient.message_type = XInternAtom(display, "_NET_WM_STATE", False);
    e.xclient.display = display;
    e.xclient.window = window;
    e.xclient.format = 32;
    e.xclient.data.l[0] = 1;
    e.xclient.data.l[1] = XInternAtom(display, "_NET_WM_STATE_STAYS_ON_TOP", False);
    XSendEvent(display, XRootWindow(display, screen), False, SubstructureRedirectMask, &e);

    XRaiseWindow(display, window);
    XFlush(display);

    // get window image
    XImage *image;
    XWindowAttributes attr;
    XGetWindowAttributes(display, window, &attr);
    image = XGetImage(display,window,0,0,attr.width,attr.height,AllPlanes,ZPixmap);
    save_ppm(image, "screenshot.ppm");
    XImage *grayscale = image_grayscale_colorimetric(image);
    save_ppm(grayscale, "screenshot-grayscale.ppm");

    XCloseDisplay(display);
}

