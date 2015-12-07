@set PATH=
@set ROOT=%~d0/Developer/gtk

@set PATH=%~d0/Developer/vim/vim74;%PATH%
@set PATH=%ROOT%/MinGW/bin;%PATH%
@set PATH=%ROOT%/gtktool/bin;%PATH%
@set PATH=%ROOT%/gsl/bin;%PATH%
@set PATH=%~d0/Developer/gtk/Project/ImageRecog/dist;%PATH%
@set PATH=%~d0/Developer/winvs/CapImage/dist;%PATH%

@set COPTION=-g -c -std=gnu99 -mms-bitfields -I%ROOT%/gtktool/include/gtk-3.0 -I%ROOT%/gtktool/include/cairo -I%ROOT%/gtktool/include/pango-1.0 -I%ROOT%/gtktool/include/atk-1.0 -I%ROOT%/gtktool/include/cairo -I%ROOT%/gtktool/include/pixman-1 -I%ROOT%/gtktool/include -I%ROOT%/gtktool/include/freetype2 -I%ROOT%/gtktool/include -I%ROOT%/gtktool/include/libpng15 -I%ROOT%/gtktool/include/gdk-pixbuf-2.0 -I%ROOT%/gtktool/include/libpng15 -I%ROOT%/gtktool/include/glib-2.0 -I%ROOT%/gtktool/lib/glib-2.0/include -I%ROOT%/gtktool/x86_64-w64-mingw32/include -I%ROOT%/mariadb/include -I%ROOT%/gsl/include

@set CLIBS=-L%ROOT%/gtktool/lib -L%ROOT%/gsl/lib -lgtk-3 -lgdk-3 -lgdi32 -limm32 -lshell32 -lole32 -Wl,-luuid -lpangocairo-1.0 -lpangoft2-1.0 -lfreetype -lfontconfig -lpangowin32-1.0 -lgdi32 -lpango-1.0 -lm -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0 -lintl
