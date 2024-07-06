#ifndef HEADERS_H_
#define HEADERS_H_

#define _DEFAULT_SOURCE

#define _XOPEN_SOURCE 500

#define _GNU_SOURCE
// #define __USE_GNU 1

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define BLUE "\033[1;34m"
#define RESET "\033[0m"

#include <pwd.h>
#include <grp.h>
#include <ftw.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/utsname.h>

#include "functions.h"
#endif
