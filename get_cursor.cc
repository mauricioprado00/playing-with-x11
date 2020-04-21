
#include <stdlib.h>
#include <stdio.h>

#include <X11/Xlib.h>

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

    printf("%u %u\n", root_x, root_y);

    XCloseDisplay(display);
}