typedef struct NameList NameList;

typedef struct InodeList InodeList;

typedef struct TreeNameList TreeNameList;

typedef struct TreeName TreeName;

typedef struct wd_array wd_array;

void do_ls(char dirname[]);

void call_new_process(char *command1, char *command2, char *pathname);

char *concatenate_strings(char *a, char *b);

void add_connect_father_children(TreeName *t, TreeNameList *lt);

void break_name_into_paths(TreeName *t, char *name);

void get_last_name(char *name, char *n);

// ABOUT SYNCHRONIZATION

void sync_algorithm_step_e(TreeName *source, TreeName *backup);

int look_for_filename_in_source(TreeName *dir_backup, TreeName *source);

TreeNameList *sync_algorithm_step_d(TreeName *source, TreeName *backup);

void copy_source_to_backup(int backup_fd, char *source_pathname);

void sync_algorithm_step_c(TreeName *source, TreeName *backup, char *name, InodeList *backup_pointer_to_start);

void delete_directory_contains(TreeName *t);

int look_for_dirname_in_source(TreeName *dir_backup, TreeName *source);

TreeNameList *sync_algorithm_step_b(TreeName *t_s, TreeName *t_b, InodeList *t_s_p_inode, InodeList *t_b_p_inode);

TreeNameList *sync_algorithm_step_a(TreeName *t_s, TreeName *t_b, InodeList *t_s_p_inode, InodeList *t_b_p_inode);

void go_depth(TreeName *t_s, TreeName *t_b, InodeList *t_s_first_inode, InodeList *t_b_first_inode);

int count_rest_TreeNameList_nodes(TreeNameList *l);

// ABOUT CREATION AND SORTING

void sorting_two_columns(TreeNameList *first, TreeNameList *second);

void quicksort_TreeNameList(TreeNameList *start, TreeNameList *finish);

void create_children(TreeName *tree, char *name, InodeList *pointer_to_start);

TreeName *create_source(TreeName *tree, char *name);

// ABOUT INOTIFY AND MONITORING

void add_to_inotify(TreeName *source, int fd, wd_array *wd_arr);

wd_array *remove_from_inotify(TreeName *source, int fd, wd_array *wd_arr, int wd);

void delete_all(TreeName *source, int fd, wd_array *wd_arr, int wd);

void fail(const char *message);

TreeName *get_TreeName_node(TreeName *t, char *name);

TreeName *in_create(TreeName *source, wd_array *wd_arr, TreeName *backup, InodeList *source_start, char *name);

TreeNameList *in_delete(TreeName *source, wd_array *wd_arr, TreeName *backup, InodeList *source_start, char *name);

wd_array *monitoring(TreeName *source, int fd, wd_array *wd_arr, TreeName *backup, InodeList *source_start);

void start_inotify(TreeName *source, TreeName *backup);



wd_array *monitoring(TreeName *source, int fd, wd_array *wd_arr, TreeName *backup, InodeList *source_start);

void f(int);

void start_inotify(TreeName *source, TreeName *backup);