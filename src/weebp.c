/*
 * free and open-source wallpaper engine. set any window or video as your
 * wallpaper
 * 
 * ![](https://i.imgur.com/Hqy1uX2.gif)
 * 
 * ![](https://i.imgur.com/VB1Glnu.gif)
 * 
 * github doesn't like having big gifs in the readme, so here's a
 * [imgur gallery](https://imgur.com/a/vkWOA)
 * 
 * this was put together in ~2 afternoons worth of coding. I can't maintain this
 * very regularly as I don't use windows and I have to either fire up a virtual
 * machine or install windows temporarily to my 2nd pc to work on it, so help
 * and testing are welcome
 * 
 * only tested on Windows 10 version 1709 build 16299.15, should work on all
 * versions of windows 10 and 8.1 as well as Windows 7
 * 
 * # roadmap
 * - initial focus is on the command-line interface just because that's what I
 *   would use, but a gui is not hard to implement later on. it's easy to
 *   write glue scripts to automate it or bind it to keys anyways
 * - forwarding mouse interactions to the captured windows should be easy and
 *   would allow interactive wallpapers, I'll implement it ASAP
 * 
 * # installation
 * download the latest binaries from
 * https://github.com/Francesco149/weebp/releases
 * 
 * extract the zip and place the folder anywhere you like
 * 
 * double-click ```0bootstrap```. this will run ```install.ps1```. it's
 * recommended that you check install.ps1 in notepad before running this just
 * to make sure that it hasn't been tampered with and doesn't do anything bad
 * 
 * this will add the folder to your ```Path``` environment variable and create
 * three shortcuts on your desktop to quickly add windows to your wallpaper
 * and control mpv without opening a shell
 *
 * type ```wp``` in a powershell or cmd window to check out the available
 * commands
 * 
 * # mpv
 * mpv is the ideal candidate to play video files or stream videos to your
 * your desktop when combined with weebp. it's an extremely powerful player
 * that combined with youtube-dl can stream videos and livestreams from almost
 * any url as well as conventional video files.
 * 
 * download the latest mpv and youtube-dl [here](https://mpv.srsfckn.biz/) 
 * 
 * I like throwing the mpv files and youtube-dl into the same folder as
 * weebp so they're available from everywhere
 * 
 * if you don't want to do this, just make sure mpv's path is in your ```Path```
 * environment variable
 * 
 * check out [mpv-install.bat][3] if you wish to make mpv your default player
 * 
 * # recipe: automatic wallpaper set-up
 * this example starts infinitely looping playback of
 * ```%USERPROFILE%\Videos\wallpapers``` using mpv, moves it to the right
 * monitor and sets it as the wallpaper on that monitor.
 * then it starts Google Chrome in kiosk mode (borderless) on a nice 3D
 * demo site then sets it as the wallpaper on the left monitor.
 * 
 * create a .bat file in the same folder as wp.exe and put the following in it
 * ```bat
 * @echo off
 * 
 * wp run mpv ^
 *     --player-operation-mode=pseudo-gui ^
 *     --force-window=yes ^
 *     --terminal=no ^
 *     --no-audio ^
 *     --loop=inf ^
 *     --loop-playlist=inf ^
 *     --input-ipc-server=\\.\pipe\mpvsocket ^
 *     %USERPROFILE%\Videos\wallpapers
 * 
 * wp mv --wait --class mpv -x 1920
 * wp add --wait --fullscreen --class mpv
 * 
 * wp run "C:\Program Files (x86)\Google\Chrome\Application\chrome.exe" ^
 *     --incognito ^
 *     --kiosk ^
 *     --no-default-browser-check ^
 *     https://awkure.github.io
 * 
 * wp add --wait --fullscreen --name "Purgatory | Home - Google Chrome"
 * ```
 * 
 * if you have youtube-dl in the same folder as mpv, you can plug any youtube,
 * twitch url or pretty much any streaming service instead of a video
 * file/folder. try http://openings.moe ! this will slow down the start-up time
 * as it needs to connect to a server
 * 
 * you'd set up a script like this to run on start-up if you want your
 * wallpaper to persist at all times
 * 
 * # recipe: controlling mpv while it's in the wallpaper
 *  another neat mpv feature is that you can control it through a named pipe.
 * this means that you can send commands to an instance of mpv running in your
 * wallpaper to skip through your playlist, display text and more.
 * all you have to do is start mpv with
 * ```--input-ipc-server=\\.\pipe\mpvsocket```
 * 
 * wp has a built-in shortcut to write commands to mpv like
 * ```wp mpv playlist-next``` 
 * 
 * by the way, there's a headless version of ```wp.exe``` called
 * ```wp-headless.exe``` that doesn't pop up a terminal. this is useful if you
 * want to create a shortcut that runs wp commands.
 * 
 * # rationale and how it works
 * on linux you can just ```mpv --wid 0``` to play a video on the wallpaper
 * window, but windows makes it a bit more obscure. at first I tried drawing
 * to ```GetDesktopWindow``` and a few other windows that seemed good candidates
 * but it would just draw over the desktop icons.
 * 
 * after googling around a bit and dicking around on a friend's PC over
 * teamviewer (I don't have windows installed) I found [this great comment][1]
 * on codeproject that points out an undocumented window message that spawns a
 * window behind the desktop icons. this is supposedly used to display the
 * animation when switching backgrounds
 * 
 * I was delighted by the simple 2-lines-of-code solution so I decided to make
 * my own lightweight wallpaper engine
 * 
 * the best part is that this transparently gives you the window handle so you
 * can plug it into virtually anything. mpv accepts a ```--wid``` parameter
 * on windows as well so that already allows you to play almost any video format
 * to the wallpaper. if you add youtube-dl, you can also stream videos and
 * livestreams from all major sites
 * 
 * it's as simple as ```mpv --wid=$(wp id) myvideo.mp4``` and it doesnt require
 * having steam open or anything
 * 
 * # building from source
 * this is for developers and people who just want to compile this from source
 * for any reason
 * 
 * install:
 * - Visual C++ Build Tools 2017
 * - Windows 10 SDK (8.1 should work as well)
 * - git
 * - 7zip
 * 
 * the preferred method of installing dependencies is [chocolatey][2]
 * 
 * ```choco install git 7zip visualcpp-build-tools windows-sdk-10 -y```
 * 
 * now open a powershell window, clone and build for 32 and 64 bit
 * 
 * ```
 * git clone https://github.com/Francesco149/weebp
 * cd weebp
 * Set-ExecutionPolicy Bypass -Scope Process -Force
 * .\vcvarsall17.ps1 x86; .\release.ps1
 * .\vcvarsall17.ps1 x64; .\release.ps1
 * ```
 * 
 * if all goes well, this will create two zip's correctly named after the
 * version and architecture
 * 
 * if you just want to build and not package, you can run ```cmd /c "build.bat"
 * instead of ```.\release.ps1``` and it will build ```wp.exe``` into the
 * current directory
 * 
 * # using it as a C library
 * ```weebp.c``` is a single header library, check the top of the file for
 * documentation
 * 
 * ```c
 * #define WP_IMPLEMENTATION
 * #include "weebp.c"
 * ```
 * 
 * if you prefer using it as an actual library, a static ```weebp.lib``` is
 * included and you can link it and omit the ```WP_IMPLEMENTATION``` define to
 * only include the header part of weebp
 * 
 * there's also a dynamic library ```weeb.dll```, it's completely untested but
 * it should work for FFI/Pinvoke in other languages
 * 
 * # license
 * this is free and unencumbered software released into the public domain
 * see the attached UNLICENSE or http://unlicense.org for details
 * 
 * [1]: https://codeproject.com/Messages/5478543/Re-Doesnt-Work-on-Windows.aspx
 * [2]: https://chocolatey.org/
 * [3]: https://github.com/rossy/mpv-install/blob/master/README.md
 */

#ifndef WEEBP_H
#define WEEBP_H

#include <Windows.h>
#include <stdio.h>

#ifdef WP_LIB
#ifdef WP_IMPLEMENTATION
#define WEEBAPI __declspec(dllexport)
#else
#define WEEBAPI __declspec(dllimport)
#endif
#else
#define WEEBAPI
#endif

#define WP_VERSION_MAJOR 1 /* non-backwards-compatible changes */
#define WP_VERSION_MINOR 3 /* backwards compatible api changes */
#define WP_VERSION_PATCH 2 /* backwards-compatible changes */

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)
#define WP_VERSION_STR \
    STRINGIFY(WP_VERSION_MAJOR) "." \
    STRINGIFY(WP_VERSION_MINOR) "." \
    STRINGIFY(WP_VERSION_PATCH)

#define wp_err(fmt, ...) fprintf(stderr, fmt "\n", __VA_ARGS__)
#define wp_err1(msg) fprintf(stderr, "%s\n", msg)

typedef HWND wnd_t;
typedef RECT rect_t;
typedef HANDLE handle_t;

/*
 * unless specified, all int-returning functions return 0 on success and
 * non-zero on failure
 */

/* prints prefix followed by information about wnd */
WEEBAPI void wp_print_window(char const* prefix, wnd_t wnd, FILE* f);

/*
 * spawn a wallpaper window if it doesn't already exists and return handle
 * returns 0 on failure
 */
WEEBAPI wnd_t wp_id();

/*
 * hangs and polls VK_* keycodes in keys every poll_ms milliseconds until
 * at least one of them is pressed
 * keys is a zero-terminated array (for example, { VK_LBUTTON, VK_RBUTTON, 0 })
 * returns the key that was pressed
 */
WEEBAPI int wp_wait(int* keys, int poll_ms);

/*
 * calls wp_wait with keys and returns the handle to the top-level window at the
 * cursor position
 * 
 * returns 0 if the pressed key is any of cancel_keys
 */
WEEBAPI wnd_t wp_pick_window(int* keys, int* cancel_keys, int poll_ms);

/* return wnd's position mapped from screen space to wallpaper space */
WEEBAPI int wp_map_rect(wnd_t wnd, rect_t* mapped);

/* embeds wnd into the wallpaper window */
WEEBAPI int wp_add(wnd_t wnd);

/*
 * try to convince wnd that it's focused
 * if ensure is non-zero, focus will be switched off first to ensure that
 * focus events are triggered more reliably
 */
WEEBAPI void wp_focus(wnd_t wnd, int ensure);

/* pops wnd out of the wallpaper window and attempts to restore the old style */
WEEBAPI int wp_del(wnd_t wnd);

/* kills the process associated with wnd */
WEEBAPI int wp_kill(wnd_t wnd);

/*
 * enumerates all child windows of parent (all windows if parent is 0) and
 * stores their handles in the result array.
 * 
 * returns the number of windows on succ, < 0 on failure
 * if there are more windows than nmax, result is filled up to nmax
 * the results are sorted by handle value
 */
WEEBAPI int wp_list(wnd_t parent, wnd_t* result, int nmax);

/* get window rectangle (position and size) */
WEEBAPI int wp_get_rect(wnd_t wnd, rect_t* rect);

/* move and resize window */
WEEBAPI int wp_move(wnd_t wnd, long left, long top, long right, long bottom);

/* checks whether the window is visible */
WEEBAPI int wp_visible(wnd_t wnd);

/* make wnd fill the monitor it's on */
WEEBAPI int wp_fullscreen(wnd_t wnd);

/* make the wnd fill all monitors (entire desktop window) */
WEEBAPI int wp_panoramic(wnd_t wnd);

/* execute a program in the background */
WEEBAPI int wp_exec(char const* exe, char const* params);

/*
 * create a named mutex.
 * this is currently used to ensure a single instance of the cli's daemon mode
 * */
WEEBAPI handle_t wp_mutex(char const* name);

/* block the current thread for milliseconds */
WEEBAPI void wp_sleep(int milliseconds);

#endif /* WEEBP_H */

/* ########################################################################## */
/* ########################################################################## */
/* ########################################################################## */

#ifndef WP_BUILDING
/* helps people with retarded syntax highlighting that greys out ifdefs */
#define WP_IMPLEMENTATION
#define WP_DEBUG
#endif

#ifdef WP_IMPLEMENTATION

#ifdef WP_DEBUG
#define log(fmt, ...) \
    fprintf(stderr, \
        "[%s:%d:%s] " fmt "\n", __FILE__, __LINE__, __func__, \
        __VA_ARGS__ \
    )
#define log1(msg) log("%s", msg)
#define log_window(prefix, wnd) wp_print_window(prefix, wnd, stderr)
#else
#define log(...)
#define log1(msg)
#define log_window(prefix, wnd)
#endif

WEEBAPI
void wp_print_window(char const* prefix, wnd_t wnd, FILE* f)
{
    char classname[512];
    char name[512];
    RECT rect;
    unsigned wndid;

    *classname = 0;
    *name = 0;
    memset(&rect, 0, sizeof(rect));

    /* TODO: can window handles go above the 32-bit range? I doubt it */
    wndid = (unsigned)((size_t)wnd & 0xFFFFFF);
    GetClassNameA(wnd, classname, sizeof(classname) - 1);
    GetWindowTextA(wnd, name, sizeof(name) - 1);
    GetWindowRect(wnd, &rect);

    if (!prefix) {
        prefix = "";
    }

    fprintf(f, "%s[%08X] %40s | %4d, %4d, %4d, %4d | %s\n", prefix,
        wndid, classname, rect.left, rect.top, rect.right, rect.bottom, name);
}

BOOL CALLBACK find_worker(wnd_t wnd, LPARAM lp)
{
    wnd_t *pworker = (wnd_t*)lp;

    if (!FindWindowExA(wnd, 0, "SHELLDLL_DefView", 0)) {
        return TRUE;
    }

    *pworker = FindWindowExA(0, wnd, "WorkerW", 0);
    if (*pworker) {
        log_window("wallpaper is ", *pworker);
        log_window("its parent is ", wnd);
        return FALSE;
    }

    return TRUE;
}

BOOL is_win8_or_later()
{
   OSVERSIONINFOEX osvi;
   DWORDLONG dwlConditionMask = 0;

   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
   osvi.dwMajorVersion = 6;
   osvi.dwMinorVersion = 2;

   VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
   VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);

   return VerifyVersionInfo(&osvi, VER_MAJORVERSION | VER_MINORVERSION,
      dwlConditionMask);
}

WEEBAPI
wnd_t wp_id()
{
    wnd_t progman;
    wnd_t worker;

    log1("scanning for Progman");
    progman = FindWindowA("Progman", 0);

    if (!progman)
    {
        log("failed to find Progman, GLE=%08X", GetLastError());
        return 0;
    }

    log1("spawning wallpaper");

    /*
     * this is basically all the magic. it's an undocumented window message that
     * forces windows to spawn a window with class "WorkerW" behind deskicons
     */

    SendMessageA(progman, 0x052C, 0xD, 0);
    SendMessageA(progman, 0x052C, 0xD, 1);

    log1("checking for wallpaper");
    EnumWindows(find_worker, (LPARAM)&worker);

    if (!worker)
    {
        wp_err("W: couldn't spawn WorkerW window, trying old method");
        SendMessageA(progman, 0x052C, 0, 0);

        log1("checking for wallpaper");
        EnumWindows(find_worker, (LPARAM)&worker);
    }

    /*
     * windows 7 with aero is almost the same as windows 10, except that we
     * have to hide the WorkerW window and render to Progman child windows
     * instead
     */

    if (worker && !is_win8_or_later())
    {
        log1("detected windows 7, hiding worker window");
        ShowWindow(worker, SW_HIDE);
        worker = progman;
    }

    if (!worker)
    {
        wp_err("W: couldnt spawn window behind icons, falling back to Progman");
        worker = progman;
    }

    return worker;
}

WEEBAPI
int wp_wait(int* keys, int poll_ms)
{
    while (1)
    {
        int* k;

        for (k = keys; *k; ++k)
        {
            if (GetAsyncKeyState(*k) & 0x8000) {
                return *k;
            }
        }

        Sleep(poll_ms);
    }

    return 0;
}

WEEBAPI
wnd_t wp_pick_window(int* keys, int* cancel_keys, int poll_ms)
{
    int* k;
    int key;
    wnd_t wnd;
    POINT pos;

    key = wp_wait(keys, poll_ms);

    for (k = cancel_keys; *k; ++k)
    {
        if (*k == key) {
            return 0;
        }
    }

    if (!GetCursorPos(&pos))
    {
        wp_err("GetCursorPos failed, GLE=%08X", GetLastError());
        return 0;
    }

    wnd = WindowFromPoint(pos);
    return GetAncestor(wnd, GA_ROOT);
}

int update_window_styles(wnd_t wnd, long and, long ex_and, long or, long ex_or)
{
    unsigned gle;
    long style = 0, exstyle = 0;

    SetLastError(0);

    style = GetWindowLongA(wnd, GWL_STYLE);
    if (!style) goto errcheck;
    exstyle = GetWindowLongA(wnd, GWL_EXSTYLE);
errcheck:
    gle = GetLastError();
    if ((!style || !exstyle) && gle) {
        wp_err("GetWindowLongA failed, GLE=%08X", GetLastError());
        return 1;
    }

    style &= and;
    exstyle &= ex_and;
    style |= or;
    style |= ex_or;

    SetLastError(0);

    if (!SetWindowLongA(wnd, GWL_STYLE, style) ||
        !SetWindowLongA(wnd, GWL_EXSTYLE, exstyle))
    {
        gle = GetLastError();
        if (gle) {
            wp_err("SetWindowLongA failed, GLE=%08X\n", gle);
            return 1;
        }
    }

    return 0;
}

WEEBAPI
int wp_map_rect(wnd_t wnd, rect_t* mapped)
{
    if (wp_get_rect(wnd, mapped)) {
        return 1;
    }

    MapWindowPoints(0, wp_id(), (LPPOINT)mapped, 2);
    return 0;
}

WEEBAPI
int wp_add(wnd_t wnd)
{
    char wndclass[512];
    wnd_t wallpaper = wp_id();
    long and, ex_and;
    rect_t r;

    *wndclass = 0;
    GetClassNameA(wnd, wndclass, sizeof(wndclass) - 1);

    if (wallpaper == wnd || !strcmp(wndclass, "Shell_TrayWnd")) {
        wp_err1("can't add this window\n");
        return 1;
    }

    if (IsChild(wallpaper, wnd)) {
        wp_err1("already added\n");
        return 1;
    }

    log_window("adding ", wnd);

    /*
     * styles blacklist taken from https://github.com/Codeusa/Borderless-Gaming/
     * blob/2fef4ccc121412f215cd7f185c4351fd634cab8b/BorderlessGaming.Logic/
     * Windows/Manipulation.cs#L70
     */

    /* TODO: somehow save old styles so we can restore them */

    and = ~(
        WS_CAPTION |
        WS_THICKFRAME |
        WS_SYSMENU |
        WS_MAXIMIZEBOX |
        WS_MINIMIZEBOX
    );

    ex_and = ~(
        WS_EX_DLGMODALFRAME |
        WS_EX_COMPOSITED |
        WS_EX_WINDOWEDGE |
        WS_EX_CLIENTEDGE |
        WS_EX_LAYERED |
        WS_EX_STATICEDGE |
        WS_EX_TOOLWINDOW |
        WS_EX_APPWINDOW
    );

    if (update_window_styles(wnd, and, ex_and, WS_CHILD, 0)) {
        return 1;
    }

    /* window retains screen coordinates so we need to adjust them */
    wp_map_rect(wnd, &r);

    if (!SetParent(wnd, wallpaper)) {
        wp_err("SetParent failed, GLE=%08X", GetLastError());
        return 1;
    }

    ShowWindow(wnd, SW_SHOW);
    wp_move(wnd, r.left, r.top, r.right, r.bottom);

    return 0;
}

WEEBAPI
void wp_focus(wnd_t wnd, int ensure)
{
    /*
    * by focusing progman first we ensure that we trigger focus events even
    * if the window already has focus (this avoids loss of focus when clicking
    * stuff right after capturing a window)
    */
    if (ensure)
    {
        wnd_t progman;

        progman = FindWindowA("Progman", 0);
        SendMessageA(progman, WM_ACTIVATE, WA_CLICKACTIVE, (LPARAM)progman);
        SendMessageA(progman, WM_SETFOCUS, (WPARAM)progman, 0);
    }

    SendMessageA(wnd, WM_ACTIVATE, WA_CLICKACTIVE, (LPARAM)wnd);
    SendMessageA(wnd, WM_SETFOCUS, (WPARAM)wnd, 0);
}

WEEBAPI
int wp_del(wnd_t wnd)
{
    wnd_t wallpaper = wp_id();
    long or;
    long ex_or;

    if (wallpaper == wnd) {
        wp_err1("can't del this window\n");
        return 1;
    }

    if (!IsChild(wallpaper, wnd)) {
        wp_err1("already removed\n");
        return 1;
    }

    log_window("removing ", wnd);

    if (!SetParent(wnd, GetDesktopWindow())) {
        wp_err("SetParent failed, GLE=%08X", GetLastError());
        return 1;
    }

    or = WS_OVERLAPPEDWINDOW;
    ex_or = WS_EX_APPWINDOW;

    if (update_window_styles(wnd, -1, -1, or, ex_or)) {
        return 1;
    }

    SetWindowPos(wnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE |
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);

    InvalidateRect(wallpaper, 0, 1);
    wp_id(); /* can sometimes fix leftover unrefreshed portions */

    return 0;
}

WEEBAPI
int wp_kill(wnd_t wnd)
{
    DWORD pid = 0;
    HANDLE process;

    GetWindowThreadProcessId(wnd, &pid);
    if (!pid) {
        wp_err("no process found");
        return 1;
    }

    process = OpenProcess(PROCESS_TERMINATE, 0, pid);
    if (!process) {
        wp_err("OpenProcess failed, GLE=%08X", GetLastError());
        return 1;
    }

    if (!TerminateProcess(process, 0)) {
        wp_err("TerminateProcess failed, GLE=%08X", GetLastError());
        return 1;
    }

    wp_id(); /* can sometimes fix leftover unrefreshed portions */

    return 0;
}

/* -------------------------------------------------------------------------- */

int compar_windows(void const* a, void const* b)
{
    if (*(int*)a >= *(int*)b) return 1;
    if (*(int*)a == *(int*)b) return 0;
    return -1;
}

struct enum_window_state
{
    wnd_t* array;
    int n;
    int nmax;
    wnd_t parent;
};

typedef struct enum_window_state enum_window_state_t;

BOOL CALLBACK enum_window(wnd_t wnd, LPARAM lp)
{
    enum_window_state_t* s = (enum_window_state_t*)lp;

    if (!s->parent && GetAncestor(wnd, GA_ROOT) != wnd) {
        return TRUE;
    }

    if (s->n >= s->nmax) {
        log("enum_window exceeded nmax of %d", s->nmax);
        return FALSE;
    }

    s->array[s->n++] = wnd;

    return TRUE;
}

WEEBAPI int wp_list(wnd_t parent, wnd_t* result, int nmax)
{
    unsigned gle;
    BOOL succ;
    enum_window_state_t s;

    s.array = result;
    s.n = 0;
    s.nmax = nmax;
    s.parent = parent;

    succ = EnumChildWindows(parent, enum_window, (LPARAM)&s);
    gle = GetLastError();
    if (!succ && gle) {
        wp_err("EnumChildWindows failed, GLE=%08X\n", gle);
        return -1;
    }

    qsort(s.array, s.n, sizeof(wnd_t), compar_windows);
    return s.n;
}

/* -------------------------------------------------------------------------- */

WEEBAPI
int wp_get_rect(wnd_t wnd, rect_t* rect)
{
    if (!GetWindowRect(wnd, (RECT*)rect)) {
        wp_err("GetWindowRect failed, GLE=%08X", GetLastError());
        return 1;
    }
    return 0;
}

WEEBAPI
int wp_move(wnd_t wnd, long left, long top, long right, long bottom)
{
    BOOL succ = SetWindowPos(wnd, 0, left, top, right - left, bottom - top, 0);
    if (!succ) {
        wp_err("SetWindowPos failed, GLE=%08X", GetLastError());
        return 1;
    }
    return 0;
}

WEEBAPI
int wp_visible(wnd_t wnd)
{
    return IsWindowVisible(wnd) == TRUE;
}

WEEBAPI
int wp_fullscreen(wnd_t wnd)
{
    HMONITOR mon;
    MONITORINFO mi;
    rect_t current_rect;

    if (wp_get_rect(wnd, &current_rect)) {
        return 1;
    }

    mon = MonitorFromPoint(*(POINT*)&current_rect, MONITOR_DEFAULTTONEAREST);
    if (!mon) {
        wp_err("MonitorFromWindow failed, GLE=%08X", GetLastError());
        return 1;
    }

    mi.cbSize = sizeof(mi);

    if (!GetMonitorInfoA(mon, &mi)) {
        wp_err("GetMonitorInfoA failed, GLE=%08X", GetLastError());
        return 1;
    }

    MapWindowPoints(0, wp_id(), (LPPOINT)&mi.rcMonitor, 2);
    return wp_move(wnd, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right,
        mi.rcMonitor.bottom);
}

WEEBAPI
int wp_panoramic(wnd_t wnd)
{
    rect_t r;

    if (wp_get_rect(wp_id(), &r)) {
        return 1;
    }

    MapWindowPoints(0, wp_id(), (LPPOINT)&r, 2);
    return wp_move(wnd, r.left, r.top, r.right, r.bottom);
}

WEEBAPI
int wp_exec(char const* file, char const* params)
{
    if (!ShellExecuteA(0, "open", file, params, 0, 0)) {
        wp_err("ShellExecuteA filed, GLE=%08X", GetLastError());
        return 1;
    }

    return 0;
}

WEEBAPI
handle_t wp_mutex(char const* name)
{
    char full_name[512];
    handle_t res;

    snprintf(full_name, sizeof(full_name) - 1,
        "Global\\{1CAC44DF-F52B-49BB-991D-89D1E7E7A9A6}_%s", name);

    res = CreateMutexA(0, 0, full_name);
    if (!res) {
        wp_err("CreateMutexA failed, GLE=%08X", GetLastError());
    }

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        res = 0;
    }

    return res;
}

WEEBAPI
void wp_sleep(int milliseconds)
{
    Sleep(milliseconds);
}

#endif /* WP_IMPLEMENTATION */
