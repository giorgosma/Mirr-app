#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include  <fcntl.h>

typedef struct NameList{
    char *name;
    struct NameList *next;
    struct NameList *previous;
}NameList;

typedef struct InodeList{
    //struct stat statbuf;
    ino_t st_ino;           /* Inode number */
    mode_t st_mode;         /* File type and mode */

    time_t sst_mtime;        /* Time of last modification */
    off_t st_size;          /* Total size, in bytes */

    nlink_t st_nlink;       /* Number of hard links */

    char type;

    NameList *list_of_names;
    int number_of_names;
    struct InodeList *pointer_to_destination;

    struct InodeList *next;
    struct InodeList *previous;
}InodeList;

typedef struct TreeNameList{
    struct TreeName *child;
    struct TreeNameList *next;
    struct TreeNameList *previous;
}TreeNameList;

typedef struct TreeName{
    char *name;
    InodeList *pointer_to_inode;
    struct TreeName *pointer_to_father;
    TreeNameList *list_of_children;
}TreeName;

typedef struct wd_array{
    char *name;
    int wd;
    struct wd_array *previous;
    struct wd_array *next;
}wd_array;

// About TreeNameList

void create_TreeNameList(TreeNameList **lt);

void add_TreeNameList_node(TreeNameList *lt, TreeName *t);

void connect_TreeName_node_to_InodeList(TreeName *t, InodeList *il);

int empty_TreeNameList(TreeNameList *lt);

TreeNameList *find_TreeNameList_node(TreeNameList *lt, TreeName *t);

TreeNameList *remove_TreeNameList_node(TreeNameList *lt, TreeName *t);

void delete_TreeNameList(TreeNameList *lt);

void print_TreeNameList(TreeNameList *lt);

// About TreeName

void create_TreeName(TreeName **t);

void add_TreeName_node(TreeName *t, char *name);

void add_child_Treename_node(TreeName *t, TreeNameList *lt, char *name);

TreeNameList *remove_child_Treename_node(TreeName *t, TreeNameList *lt, char *name);

TreeName * find_child_Treename_node(TreeName *t, TreeNameList *lt, char *name);

int empty_TreeName(TreeName *t);

void find_TreeName_node();

void remove_TreeName_node(TreeName *t, char *name);

void delete_TreeName(TreeName *t);

void print_TreeName(TreeName *t);

// About InodeList

void create_InodeList(InodeList **il);

void init_InodeList_node(InodeList *il, struct stat *statbuf);

void init_InodeList_name(InodeList *il, char *name);

void add_InodeList_node(InodeList *il, struct stat *statbuf);

int empty_InodeList(InodeList *il);

InodeList *find_InodeList_node(InodeList *il, ino_t st_ino);

InodeList *remove_InodeList_node(InodeList *il, ino_t st_ino);

void delete_InodeList(InodeList *il);

void print_InodeList(InodeList *il);

void print_Inode_node(InodeList *il);

// About NameList

void create_NameList(NameList **l);

void init_NameList_name(NameList *l, char *name);

void add_NameList_node(NameList *l, char *name);

int empty_NameList(NameList *l);

void find_NameList_node();

NameList *remove_NameList_node(NameList *l, char *name);

void delete_NameList(NameList *l);

void print_NameList(NameList *l);

// About wd_array

void create_wd_array(wd_array **wd_ar);

void delete_wd_array(wd_array *wd_ar);

void add_wd_array(wd_array *wd_ar, char *name, int wd);

wd_array *remove_wd_array(wd_array *wd_ar, int wd);

void print_wd_array(wd_array *wd_ar);

wd_array *find_wd_array(wd_array *wd_ar, int wd);

int check_strings(char *column1, char *column2);

int check_memory_bytes(void *column1, void *column2, int size);

char *get_parent_name(TreeName *t);