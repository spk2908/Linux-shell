// FUCKKK \0 SHOULDNT GO TO PASTEVENTS

typedef struct node
{
    pid_t pid;
    char cmd[4096];
    struct node *next;
} node;

typedef node *list;

typedef struct cmd{
    char** argv;
    int argc;
    int bg;
} cmd;

extern struct timeval start_time, end_time;
extern char* commandtoprint;

extern volatile sig_atomic_t ctc;
extern volatile sig_atomic_t ctz;

// prompt
int prompt(char* home, int len);
int prefix(const char *pre, const char *str);

// format
char* format(char* input);

// parse
int execute(cmd *input, int fd1, int fd2, char *home, char *prev_dir, list proc_list);
void parse(char *input, char *home, char *prev_dir, list proc_list);
int parse_cmd(char *command, int fd1, int fd2, cmd* new, int bg, char *home, char *prev_dir, list proc_list);

// pipe
void pipeline(char *command, cmd *new, int bg, char *home, char *prev_dir, list proc_list);

// redirect
int redirect(char *command, cmd *new, int bg, char *home, char *prev_dir, list proc_list);

// warp
int changedir(const char* path);
int warp(cmd *input, const char *home, char *prev_dir);

// peek
int peek(cmd *input, const char *home);
void print_permissions(mode_t mode);
void print_deets(struct dirent *file, int dir, int link, int size, int user, int grp);
int get_digits(int n);
int get_total(struct dirent **namelist, int n, int dir, int *max_link, int *max_size, int* max_user, int* max_group, int a);

// bgprocs
list create_proclist();
void add_proc(list L, pid_t pid, char *command);
void delete_endedprocs(list L);

// pastevents
int pastevents(cmd *input, char *home, char *prev_dir, list proc_list);
void pasteventsadd(char *command, char* home);
void readpastevents(char **arr, int *count, char *home);
char* pasteventsreplace(char *input, char *home);

// proclore
int proclore(cmd *input, char* home);

// seek
int fn(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);
int seek(cmd *input, char *home, char *prev_dir);

// activities
int activities(list proc_list);

// ping
int ping(cmd *input);

// fgbg
int fg(cmd *input, list bg_procs);
int bg(cmd *input, list bg_procs);
void ctrlc(int signal_number);
void ctrlz(int signal_number);

// neonate
void die(const char *s);
void disableRawMode();
void enableRawMode();
int neonate(cmd *input);

// iman
void parsehtml(char *html);
int iman(cmd* input);
