
free and open-source wallpaper engine. set any window or video as your
wallpaper

![](https://i.imgur.com/Hqy1uX2.gif)

![](https://i.imgur.com/tZ14Znr.gif)

![](https://i.imgur.com/VB1Glnu.gif)

github doesn't like having big gifs in the readme, so here's a
[imgur gallery](https://imgur.com/a/vkWOA)

this was put together in ~2 afternoons worth of coding. I can't maintain this
very regularly as I don't use windows and I have to either fire up a virtual
machine or install windows temporarily to my 2nd pc to work on it, so help
and testing are welcome

only tested on Windows 10 version 1709 build 16299.15, should work on all
versions of windows 10 and 8.1 as well as Windows 7

# Windows 7 prerequisites
go to start -> search for
"Adjust the appearance and performance of windows" and click it, then
make sure at least these are checked
* Animate controls and elements inside windows
* Enable desktop composition
* Use visual styles on windows and buttons

if you don't enable these settings, weebp won't be able to put your
animated wallpaper behind the icons. it will still work, but it will
cover your desktop icons

# roadmap
- initial focus is on the command-line interface just because that's what I
would use, but a gui is not hard to implement later on. it's easy to
write glue scripts to automate it or bind it to keys anyways
- forwarding mouse interactions to the captured windows should be easy and
would allow interactive wallpapers, I'll implement it ASAP

# installation
download the latest binaries from
https://github.com/Francesco149/weebp/releases

extract the zip and place the folder anywhere you like

double-click ```0bootstrap```. this will run ```install.ps1```. it's
recommended that you check install.ps1 in notepad before running this just
to make sure that it hasn't been tampered with and doesn't do anything bad

this will add the folder to your ```Path``` environment variable and create
three shortcuts on your desktop to quickly add windows to your wallpaper
and control mpv without opening a shell

type ```wp``` in a powershell or cmd window to check out the available
commands

# mpv
mpv is the ideal candidate to play video files or stream videos to your
your desktop when combined with weebp. it's an extremely powerful player
that combined with youtube-dl can stream videos and livestreams from almost
any url as well as conventional video files.

download the latest mpv and youtube-dl [here](https://mpv.srsfckn.biz/) 

I like throwing the mpv files and youtube-dl into the same folder as
weebp so they're available from everywhere

if you don't want to do this, just make sure mpv's path is in your ```Path```
environment variable

check out [mpv-install.bat][3] if you wish to make mpv your default player

# recipe: automatic wallpaper set-up
this example starts infinitely looping playback of
```%USERPROFILE%\Videos\wallpapers``` using mpv, moves it to the right
monitor and sets it as the wallpaper on that monitor.
then it starts Google Chrome in kiosk mode (borderless) on a nice 3D
demo site then sets it as the wallpaper on the left monitor.

create a .bat file in the same folder as wp.exe and put the following in it
```bat
@echo off

wp run mpv ^
--player-operation-mode=pseudo-gui ^
--force-window=yes ^
--terminal=no ^
--no-audio ^
--loop=inf ^
--loop-playlist=inf ^
--input-ipc-server=\\.\pipe\mpvsocket ^
%USERPROFILE%\Videos\wallpapers

wp mv --wait --class mpv -x 1920
wp add --wait --fullscreen --class mpv

wp run "C:\Program Files (x86)\Google\Chrome\Application\chrome.exe" ^
--incognito ^
--kiosk ^
--no-default-browser-check ^
https://awkure.github.io

wp add --wait --fullscreen --name "Purgatory | Home - Google Chrome"
```

if you have youtube-dl in the same folder as mpv, you can plug any youtube,
twitch url or pretty much any streaming service instead of a video
file/folder. try http://openings.moe ! this will slow down the start-up time
as it needs to connect to a server

you'd set up a script like this to run on start-up if you want your
wallpaper to persist at all times

# recipe: controlling mpv while it's in the wallpaper
another neat mpv feature is that you can control it through a named pipe.
this means that you can send commands to an instance of mpv running in your
wallpaper to skip through your playlist, display text and more.
all you have to do is start mpv with
```--input-ipc-server=\\.\pipe\mpvsocket```

wp has a built-in shortcut to write commands to mpv like
```wp mpv playlist-next``` 

by the way, there's a headless version of ```wp.exe``` called
```wp-headless.exe``` that doesn't pop up a terminal. this is useful if you
want to create a shortcut that runs wp commands.

# rationale and how it works
on linux you can just ```mpv --wid 0``` to play a video on the wallpaper
window, but windows makes it a bit more obscure. at first I tried drawing
to ```GetDesktopWindow``` and a few other windows that seemed good candidates
but it would just draw over the desktop icons.

after googling around a bit and dicking around on a friend's PC over
teamviewer (I don't have windows installed) I found [this great comment][1]
on codeproject that points out an undocumented window message that spawns a
window behind the desktop icons. this is supposedly used to display the
animation when switching backgrounds

I was delighted by the simple 2-lines-of-code solution so I decided to make
my own lightweight wallpaper engine

the best part is that this transparently gives you the window handle so you
can plug it into virtually anything. mpv accepts a ```--wid``` parameter
on windows as well so that already allows you to play almost any video format
to the wallpaper. if you add youtube-dl, you can also stream videos and
livestreams from all major sites

it's as simple as ```mpv --wid=$(wp id) myvideo.mp4``` and it doesnt require
having steam open or anything

# building from source
this is for developers and people who just want to compile this from source
for any reason

install:
- Visual C++ Build Tools 2017
- Windows 10 SDK (8.1 should work as well)
- git
- 7zip

the preferred method of installing dependencies is [chocolatey][2]

```choco install git 7zip visualcpp-build-tools windows-sdk-10 -y```

now open a powershell window, clone and build for 32 and 64 bit

```
git clone https://github.com/Francesco149/weebp
cd weebp
Set-ExecutionPolicy Bypass -Scope Process -Force
.\vcvarsall17.ps1 x86; .\release.ps1
.\vcvarsall17.ps1 x64; .\release.ps1
```

if all goes well, this will create two zip's correctly named after the
version and architecture

if you just want to build and not package, you can run ```cmd /c "build.bat"
instead of ```.\release.ps1``` and it will build ```wp.exe``` into the
current directory

# using it as a C library
```weebp.c``` is a single header library, check the top of the file for
documentation

```c
#define WP_IMPLEMENTATION
#include "weebp.c"
```

if you prefer using it as an actual library, a static ```weebp.lib``` is
included and you can link it and omit the ```WP_IMPLEMENTATION``` define to
only include the header part of weebp

there's also a dynamic library ```weeb.dll```, it's completely untested but
it should work for FFI/Pinvoke in other languages

# license
this is free and unencumbered software released into the public domain
see the attached UNLICENSE or http://unlicense.org for details

[1]: https://codeproject.com/Messages/5478543/Re-Doesnt-Work-on-Windows.aspx
[2]: https://chocolatey.org/
[3]: https://github.com/rossy/mpv-install/blob/master/README.md

