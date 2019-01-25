PKG_CONFIG_PATH="C:\gstreamer\1.0\x86_64\lib\pkgconfig C:\lib\gtk+-2\lib\pkgconfig"

all:
	gcc -std=c99 -export-dynamic -O1 -Wall -shared -fPIC *.c -o videoconf.so `pkg-config --cflags --libs gstreamer-1.0 gstreamer-video-1.0 farstream-0.2 gtk+-2.0`

install:
	cp videoconf.so %APPDATA%\HexChat\addons

deinstall:
	rm %APPDATA%\HexChat\addons\videoconf.so
