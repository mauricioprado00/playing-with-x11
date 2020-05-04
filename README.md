compile:

```
gcc prog-1.cc $(pkg-config --cflags --libs x11) -o bin/prog-1
gcc prog-2.cc $(pkg-config --cflags --libs x11) -o bin/prog-2
gcc prog-3.cc $(pkg-config --cflags --libs x11) -o bin/prog-3
gcc highlight.cc $(pkg-config --cflags --libs x11) -o bin/highlight
gcc cursorshot.cc $(pkg-config --cflags --libs x11) -o bin/cursorshot
gcc get_cursor.cc $(pkg-config --cflags --libs x11) -o bin/get_cursor
gcc overlay.cc $(pkg-config --cflags --libs x11) -o bin/overlay
gcc overlay2.cc $(pkg-config --cflags --libs x11) -o bin/overlay2
gcc rectangle.cc $(pkg-config --cflags --libs x11) -o bin/rectangle
gcc draw_line.cc $(pkg-config --cflags --libs x11) -o bin/draw_line
gcc highlight-point.cc $(pkg-config --cflags --libs x11) -o bin/highlight-point
gcc threshold.cc $(pkg-config --cflags --libs x11) -o bin/threshold
```