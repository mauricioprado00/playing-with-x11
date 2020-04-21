compile:

```
gcc prog-1.cc $(pkg-config --cflags --libs x11) -o prog-1
gcc prog-2.cc $(pkg-config --cflags --libs x11) -o prog-2
gcc prog-3.cc $(pkg-config --cflags --libs x11) -o prog-3
gcc highlight.cc $(pkg-config --cflags --libs x11) -o highlight
```