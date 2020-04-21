#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

int line_width = 2;
int speed = 400;
char default_color_name[] = "Tomato";

void draw_line(char *display_name, char *color_name, int x1, int y1, int x2, int y2);

main(int argc, char *argv[]) {
    char *display_name = getenv("DISPLAY");
    char *color_name = default_color_name;

    if (!display_name) {
        fprintf(stderr, "%s: cannot connect to X server '%s'\n", argv[0], display_name);
        exit(1);
    }

    if(argc<5) {
       printf("usage: \n\tdraw_line x1 y1 x2 y2 [color_name | color_hex]\n");
       printf("\n\tcolor_name can be one of https://www.w3schools.com/colors/colors_names.asp\n");
       exit(1);
    }

    if (argc >= 5) {
        color_name = argv[5];
        printf("using custom color name: %s\n", color_name);
    }

    int x1 = atoi(argv[1]);
    int y1 = atoi(argv[2]);
    int x2 = atoi(argv[3]);
    int y2 = atoi(argv[4]);

    draw_line(display_name, color_name, x1, y1, x2, y2);
}

void draw_line(char *display_name, char *color_name, int x1, int y1, int x2, int y2) {
    printf("drawing line in %s(%d, %d) (%d, %d)\n", display_name, x1, y1, x2, y2);

    Display *display = XOpenDisplay(display_name);
    int screen = DefaultScreen(display);

    //Create a window at line position
    XSetWindowAttributes window_attr;
    window_attr.override_redirect = 1;
    Window window = XCreateWindow(display, XRootWindow(display, screen),
        x1, y1,                             // x, y position
        x2-x1, y2-y1,                       // width, height
        0,                                  // border width
        DefaultDepth(display, screen),      // depth
        CopyFromParent,                     // class
        DefaultVisual(display, screen),     // visual
        CWOverrideRedirect,                 // valuemask
        &window_attr                        // attributes
    );
    XMapWindow(display, window);
    XStoreName(display, window, "draw-line");

    XClassHint *xclass = XAllocClassHint();
    xclass->res_name = "draw-line";
    xclass->res_class = "draw-line";
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

    // Prepare to draw on this window
    XGCValues values;
    values.graphics_exposures = False;
    unsigned long valuemask = 0;
    GC gc = XCreateGC(display, window, valuemask, &values);

    Colormap colormap = DefaultColormap(display, screen);
    XColor color;
    XAllocNamedColor(display, colormap, color_name, &color, &color);
    XSetForeground(display, gc, color.pixel);
    XSetLineAttributes(display, gc, line_width, LineSolid, CapButt, JoinBevel);

    // Draw the line
    XDrawLine(display, window, gc, 0, 0, x2-x1, y2-y1);
    XSync(display, False);

    // free resources
    usleep(speed * 100 * 200);
    XFreeGC(display, gc);
    XCloseDisplay(display);

}