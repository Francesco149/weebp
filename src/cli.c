/*
 * a basic command line interface for weebp, for documentation see weebp.c
 * 
 * major todo's
 * - make generic argument parser and unclutter argument parsing code
 * - add option to hide the console (like when running from shortcuts)
 * 
 * this is free and unencumbered software released into the public domain
 * see the attached UNLICENSE or http://unlicense.org for details
 */

#include "weebp.c"

#define VERSION_MAJOR 0 /* non-backwards-compatible changes */
#define VERSION_MINOR 5 /* backwards compatible api changes */
#define VERSION_PATCH 2 /* backwards-compatible changes */

#define VERSION_STR \
    STRINGIFY(VERSION_MAJOR) "." \
    STRINGIFY(VERSION_MINOR) "." \
    STRINGIFY(VERSION_PATCH)

#include <stdio.h>

int picker_keys[] = { VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_ESCAPE, 0 };
int picker_cancel_keys[] = { VK_ESCAPE, 0 };

void print_usage(FILE* f);

int version(int argc, char* argv[])
{
    (void)argc; (void)argv;

    printf("%s\n", VERSION_STR);
    return 0;
}

int help(int argc, char* argv[])
{
    (void)argc; (void)argv;

    print_usage(stdout);
    return 0;
}

int id(int argc, char* argv[])
{
    int i;
    wnd_t worker;
    char const* fmt = "%u";

    for (i = 0; i < argc; ++i)
    {
        if (!strcmp(argv[i], "-x") || !strcmp(argv[i], "--hex")) {
            fmt = "%08X";
        }
    }

    while (1)
    {
        worker = wp_id();
        if (worker) {
            break;
        }

        wp_err("retrying in 0.5s...");
        wp_sleep(500);
    }
    
    printf(fmt, (unsigned)((size_t)worker & 0xFFFFFFFF));

    return 0;
}

int run(int argc, char* argv[])
{
    char cmdline[4096];
    char* p = cmdline;
    char* end = cmdline + sizeof(cmdline);
    int i;

    if (argc < 1) {
        return 1;
    }

    *p = 0;

    for (i = 1; i < argc; ++i) {
        p += snprintf(p, end - p, "\"%s\" ", argv[i]);
    }

    return wp_exec(argv[0], cmdline);
}

int info(int argc, char* argv[])
{
    wnd_t wnd;

    (void)argc; (void)argv;

    wp_err("click on a window...");
    wnd = wp_pick_window(picker_keys, picker_cancel_keys, 10);
    if (!wnd) {
        return 1;
    }

    wp_print_window("", wnd, stdout);

    return 0;
}

int parse_window_params(wnd_t parent, int argc, char* argv[], int interactive,
    wnd_t* result)
{
    static const int FL_WAIT = 1<<0;
    static const int FL_WAIT_VISIBLE = 1<<1;

    int i;
    int flags = 0;

    *result = 0;

parseargs:
    i = 0;

    for (; !*result && i < argc; ++i)
    {
        if (!strcmp(argv[i], "-a") || !strcmp(argv[i], "--handle")) {
            if (i >= argc - 1) return 1;
            *result = (wnd_t)(UINT_PTR)strtol(argv[++i], 0, 16);
        }

        else if (!strcmp(argv[i], "-c") || !strcmp(argv[i], "--class")) {
            if (i >= argc - 1) return 1;
            *result = FindWindowA(argv[++i], 0);
        }

        else if (!strcmp(argv[i], "-n") || !strcmp(argv[i], "--name")) {
            if (i >= argc - 1) return 1;
            *result = FindWindowA(0, argv[++i]);
        }

        else if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "--index"))
        {
            int n;
            int index;
            wnd_t windows[512];

            if (i >= argc - 1) return 1;

            n = wp_list(parent, windows, 512);
            if (n < 0) {
                continue;
            }

            index = atoi(argv[++i]);
            if (!n) {
                wp_err("no windows left");
                return 1;
            }
            if (index < 0 || index >= n) {
                wp_err("out of range window index %d/%d", index, n);
                return 1;
            }

            *result = windows[index];
        }

        else if (!strcmp(argv[i], "-t") || !strcmp(argv[i], "--wait")) {
            flags |= FL_WAIT;
        }

        else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--wait-visible")) {
            flags |= FL_WAIT_VISIBLE;
        }
    }

    if (!*result)
    {
        if (flags & FL_WAIT)
        {
            wp_sleep(100);
            goto parseargs; /* less ugly than more nesting or recursion */
        }

        while ((flags & FL_WAIT_VISIBLE) && !wp_visible(*result))
        {
            wp_sleep(100);
        }

        if (!interactive) {
            wp_err1("no matching window found");
            return 1;
        }

        wp_err1("click on a window...");
        *result = wp_pick_window(picker_keys, picker_cancel_keys, 10);
        if (!*result) {
            return 1;
        }
    }

    if (!IsWindow(*result)) {
        wp_err1("invalid window");
        return 1;
    }

    wp_print_window("", *result, stderr);

    return 0;
}

int add(int argc, char* argv[])
{
    static const int FL_FULLSCREEN = 1<<0;
    static const int FL_NOFOCUS = 1<<1;

    int i;
    wnd_t wnd = 0;
    int flags = 0;

    if (parse_window_params(0, argc, argv, 1, &wnd)) {
        return 1;
    }

    for (i = 0; i < argc; ++i)
    {
        if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--fullscreen")) {
            flags |= FL_FULLSCREEN;
        }

        if (!strcmp(argv[i], "--no-focus")) {
            flags |= FL_NOFOCUS;
        }
    }

    if (wp_add(wnd)) {
        return 1;
    }

    if ((flags & FL_FULLSCREEN) && wp_fullscreen(wnd)) {
        return 1;
    }

    if (!(flags & FL_NOFOCUS)) {
        wp_focus(wnd, 1);
    }

    return 0;
}

int focus(int argc, char* argv[])
{
    wnd_t wnd;

    if (parse_window_params(wp_id(), argc, argv, 0, &wnd)) {
        return 1;
    }

    wp_focus(wnd, 1);
    return 0;
}

int daemon(int argc, char* argv[])
{
    int i;
    int poll_ms = 16;

    if (!wp_mutex("weebpd"))
    {
        puts(
            "the weebp daemon appears to be already running. if you "
            "think this is a mistake, kill all instances of wp.exe and "
            "wp-headless.exe from the task manager"
        );

        return 1;
    }

    for (i = 0; i < argc; ++i)
    {
        if (!strcmp(argv[i], "-r") || !strcmp(argv[i], "--rate")) {
            if (i >= argc - 1) return 1;
            poll_ms = atoi(argv[++i]);
        }
    }

    puts("running as a daemon");

    while (1)
    {
        int n;
        wnd_t wnds[512];

        n = wp_list(wp_id(), wnds, 512);
        for (i = 0; i < n; ++i) {
            wp_focus(wnds[i], 0);
        }

        wp_sleep(poll_ms);
    }

    return 0;
}

int del(int argc, char* argv[])
{
    wnd_t wnd = 0;

    if (parse_window_params(wp_id(), argc, argv, 1, &wnd)) {
        return 1;
    }

    return wp_del(wnd);
}

int kill(int argc, char* argv[])
{
    wnd_t wnd = 0;

    if (parse_window_params(wp_id(), argc, argv, 0, &wnd)) {
        return 1;
    }

    return wp_kill(wnd);
}

int killall(int argc, char* argv[])
{
    wnd_t wnd = 0;

    do
    {
        if (parse_window_params(wp_id(), argc, argv, 0, &wnd)) {
            return 0;
        }

        wp_kill(wnd);
    }
    while (wnd);

    return 0;
}

int mv(int argc, char* argv[])
{
    int i;
    wnd_t wnd = 0;
    rect_t rect;

    if (parse_window_params(wp_id(), argc, argv, 1, &wnd)) {
        return 1;
    }

    if (!GetWindowRect(wnd, &rect)) {
        wp_err("GetWindowRect failed, GLE=%08X", GetLastError());
        return 1;
    }

    for (i = 0; i < argc; ++i)
    {
        if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--fullscreen")) {
            return wp_fullscreen(wnd);
        }

        else if (!strcmp(argv[i], "-x"))
        {
            long dx;

            if (i >= argc - 1) return 1;
            dx = atol(argv[++i]) - rect.left;
            rect.left += dx;
            rect.right += dx;
        }

        else if (!strcmp(argv[i], "-y"))
        {
            long dy;

            if (i >= argc - 1) return 1;
            dy = atol(argv[++i]) - rect.top;
            rect.top += dy;
            rect.bottom += dy;
        }

        else if (!strcmp(argv[i], "-w") || !strcmp(argv[i], "--width")) {
            if (i >= argc - 1) return 1;
            rect.right = rect.left + atol(argv[++i]);
        }

        else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--height")) {
            if (i >= argc - 1) return 1;
            rect.bottom = rect.top + atol(argv[++i]);
        }

        else if (!strcmp(argv[i], "--left")) {
            if (i >= argc - 1) return 1;
            rect.left = atol(argv[++i]);
        }

        else if (!strcmp(argv[i], "--top")) {
            if (i >= argc - 1) return 1;
            rect.top = atol(argv[++i]);
        }

        else if (!strcmp(argv[i], "--right")) {
            if (i >= argc - 1) return 1;
            rect.right = atol(argv[++i]);
        }

        else if (!strcmp(argv[i], "--bottom")) {
            if (i >= argc - 1) return 1;
            rect.bottom = atol(argv[++i]);
        }
    }

    return wp_move(wnd, rect.left, rect.top, rect.right, rect.bottom);
}

int ls(int argc, char* argv[])
{
    wnd_t parent = wp_id();
    int i;
    wnd_t wnds[512];
    int n;

    for (i = 0; i < argc; ++i)
    {
        if (!strcmp(argv[i], "-a") || !strcmp(argv[i], "--all")) {
            parent = 0;
        }
    }

    n = wp_list(parent, wnds, 512);
    if (n < 0) {
        return 1;
    }

    if (n >= 512) {
        wp_err("W: there are too many windows, only the first 512 "
            "will be listed");
    }

    for (i = 0; i < n; ++i)
    {
        printf("%3d: ", i);
        wp_print_window("", wnds[i], stdout);
    }

    return 0;
}

int mpv(int argc, char* argv[])
{
    char cmdline[4096];
    char* p = cmdline;
    char* end = cmdline + sizeof(cmdline);
    int i;
    FILE* f;
    char const* fname = "\\\\.\\pipe\\mpvsocket";

    while (*argv[0] == '-' && argc > 0)
    {
        if (!strcmp(argv[0], "--file"))
        {
            if (argc < 2) {
                return 1;
            }

            fname = argv[1];
            argv += 2;
            argc -= 2;
        }
    }

    *p = 0;

    for (i = 0; i < argc; ++i)
    {
        char const* fmt = "%s ";

        if (strstr(argv[i], " ")) {
            fmt = "\"%s\" ";
        }

        p += snprintf(p, end - p, fmt, argv[i]);
    }

    wp_err("'%s' -> %s\n", cmdline, fname);

    f = fopen(fname, "w");
    if (!f) {
        perror("fopen");
        return 1;
    }

    fprintf(f, "%s\n", cmdline);
    fclose(f);

    return 0;
}

/* -------------------------------------------------------------------------- */

struct command
{
    char const* name;
    int (* run)(int argc, char* argv[]);
    char const* help;
};

typedef struct command command_t;

command_t commands[] = 
{
    {
        "id", id,
        "spawns a window behind desktop icons if not already present and "
        "prints its handle/id\n"
        "    -x|--hex: print handle in hexadecimal (defaults to decimal)"
    },
    { "run", run, "run a program in the background, pass through parameters" },
    { "info", info, "interactively click a window and print info" },
    {
        "add", add,
        "embed a window into the wallpaper. parameters are evaluated from left "
        "to right until a valid window is found. if no valid window is found "
        "or no parameters are given the user is prompted to click on a window\n"
        "    -i|--index 123: window index in 'wp ls'\n"
        "    -a|--handle 123: window handle/id (hexadecimal)\n"
        "    -c|--class abc: window class\n"
        "    -n|--name abc: window name\n"
        "    -t|--wait: wait until a matching window is found\n"
        "    -v|--wait-visible: wait until a matching window is visible\n"
        "    -f|--fullscreen: expand window to fullscreen after adding it"
    },
    {
        "focus", focus,
        "try to convince a captured window that it's focused, useful for "
        "interactive stuff that stops rendering"
    },
    {
        "daemon", daemon,
        "run as a daemon, currently used to maintain fake focus of captured "
        "windows, but might eventually be used for a tray icon or gui\n"
        "note that usually a single call to wp focus will do the job and this "
        "is not needed. only run it if your particular application isn't "
        "maintaining focus\n"
        "    -r|--rate 123: poll rate in milliseconds"
    },
    {
        "del", del,
        "pop a window out of the wallpaper and restore its border. "
        "the window can be specified in the same way as 'wp add'"
    },
    {
        "kill", kill,
        "kill the process associated with a window that was previously added "
        "to the wallpaper. the window can be specified in the same way as "
        "'wp add'"
    },
    {
        "killall", killall,
        "kill the processes associated with all matching windows that were "
        "previously added to the wallpaper. the window can be specified in "
        "the same way as 'wp add'"
    },
    { "rm", del, "alias for del" },
    {
        "mv", mv,
        "move a window. the window can be specified in the same way as 'wp add'"
        ". unspecified sizes and coordinates will be left unchanged\n"
        "    -x 123\n"
        "    -y 123\n"
        "    -w|--width 123\n"
        "    -h|--height 123\n"
        "    --left 123: x coordinate of the top left corner\n"
        "    --top 123: y coordinate of the top left corner\n"
        "    --right 123: the x coordinate of the bottom right corner\n"
        "    --bottom 123: the y coordinate of the bottom right corner\n"
        "    -f|--fullscreen: expand window to fullscreen"
    },
    {
        "ls", ls,
        "list windows\n"
        "    -a|--all: list all windows (by default it lists windows that were "
        "embedded in the desktop with 'wp add')"
    },
    {
        "mpv", mpv,
        "writes a command to \\\\.\\pipe\\mpvsocket. this is meant to be used "
        "in combination with mpv --input-ipc-server=\\\\.\\pipe\\mpvsocket to "
        "control mpv while it's running as the wallpaper\n"
        "    --file abc: overrides the default pipe (useful to control "
        "multiple instances of mpv)"
    },
    { "version", version, "prints the version number" },
    { "help", help, "prints this help page" },
    { 0, 0 }
};

void print_usage(FILE* f)
{
    command_t* c;

    fprintf(f, "wp command parameters\n\n");
    fprintf(f, "wp command help: shows help for a particular command\n\n");

    for (c = commands; c->name; ++c) {
        fprintf(f, "%s: %s\n\n", c->name, c->help);
    }
}

int main(int argc, char* argv[])
{
    int res;
    int show_help = 0;
    command_t* c;

    if (argc < 2)
    {
        print_usage(stderr);
        return 1;
    }

    /* TODO: fuzzy match command names to save typing */

    for (c = commands; c->name && strcmp(argv[1], c->name); ++c);

    if (!c->name) {
        print_usage(stderr);
        return 1;
    }

    if (argc >= 3 && !strcmp(argv[2], "help"))
    {
        res = 0;
        show_help = 1;
    }
    else
    {
        res = c->run(argc - 2, argv + 2);
        show_help = res != 0;
    }

    if (show_help) {
        wp_err("%s\n\n", c->help);
    }

    return res;
}

/* -------------------------------------------------------------------------- */

/*
 * the only reason I'm doing this is that this way if I forget to include a new
 * function in the header part of weebp it will warn me
 */
#define WP_IMPLEMENTATION
#include "weebp.c"