@set PATH=
@set ROOT=%~d0/Developer

@set PATH=%~d0/Developer/vim/vim74;%PATH%
@set PATH=%ROOT%/mingw/w32/bin;%PATH%
@set PATH=%ROOT%/Librarys/gtk/w32/bin;%PATH%
@set PATH=%ROOT%/Librarys/mariadb/w32/bin;%PATH%

@set COPTION=-g -c -std=gnu99 -mms-bitfields -I%ROOT%/Librarys/gtk/w32/include/gtk-3.0 -I%ROOT%/Librarys/gtk/w32/include/cairo -I%ROOT%/Librarys/gtk/w32/include/pango-1.0 -I%ROOT%/Librarys/gtk/w32/include/atk-1.0 -I%ROOT%/Librarys/gtk/w32/include/cairo -I%ROOT%/Librarys/gtk/w32/include/pixman-1 -I%ROOT%/Librarys/gtk/w32/include -I%ROOT%/Librarys/gtk/w32/include/freetype2 -I%ROOT%/Librarys/gtk/w32/include -I%ROOT%/Librarys/gtk/w32/include/libpng15 -I%ROOT%/Librarys/gtk/w32/include/gdk-pixbuf-2.0 -I%ROOT%/Librarys/gtk/w32/include/libpng15 -I%ROOT%/Librarys/gtk/w32/include/glib-2.0 -I%ROOT%/Librarys/gtk/w32/lib/glib-2.0/include -I%ROOT%/Librarys/mariadb/w32/include -I%ROOT%/Librarys/edna/include

@set CLIBS=-L%ROOT%/Librarys/gtk/w32/lib -lgtk-3 -lgdk-3 -lgdi32 -limm32 -lshell32 -lole32 -Wl,-luuid -lpangocairo-1.0 -lpangoft2-1.0 -lfreetype -lfontconfig -lpangowin32-1.0 -lgdi32 -lpango-1.0 -lm -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0 -lintl -L%ROOT%/Librarys/mariadb/w32/lib -lmariadb -L%ROOT%/Librarys/edna/lib -lezdnaapi
