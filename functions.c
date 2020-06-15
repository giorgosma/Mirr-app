#include "structures.h"
#include "functions.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/inotify.h>

//// REFERENCE TO DELIS EXAMPLES
void do_ls(char dirname[]){              
	DIR 	*dir_ptr;
	struct 	dirent *direntp;

	if ( ( dir_ptr = opendir( dirname ) ) == NULL ){
		//printf("error in dirname %s\n",dirname);
		fprintf(stderr, "cannot open %s \n",dirname);
	}
	else {
		printf("dirname %s\n",dirname);
		while ( ( direntp=readdir(dir_ptr) ) != NULL ){
			printf("inode %d of the entry %s \n", \
							(int)direntp->d_ino, direntp->d_name);
		}
		//printf("dirname %s direntp %s\n", dirname, direntp);
		closedir(dir_ptr);
		//printf("Close dirname %s\n", dirname);	
	}
}
////

void call_new_process(char *command1, char *command2, char *pathname){
    pid_t pid;
    // // sprintf(p_key, "%d", key);
    printf("%s %s %s\n", command1, command2, pathname);
    if ((pid = fork()) == 0){
        execlp(command1, command2, pathname, (char *)NULL);
        printf("ERROR IN EXEC\n");
    }
    else{
        int ex = waitpid(pid,NULL,0);
    }
}

char *concatenate_strings(char *a, char *b){
    strcat(a, "/");                
    strcat(a, b);
    return a;
}



void add_connect_father_children(TreeName *t, TreeNameList *lt){
    if(lt != NULL && lt->child != NULL){
        (lt->child)->pointer_to_father = t;
        add_connect_father_children(t, lt->next);
    }
}

void break_name_into_paths(TreeName *t, char *name){
    const char s[2] = "/";
    char *token;
   
    /* get the first token */
    token = strtok(name, s);
   
    /* walk through other tokens */
    while( token != NULL ) {
        printf( " %s\n", token );

        add_child_Treename_node(t, t->list_of_children, token);
        //add_connect_father_children(t, t->list_of_children);

        token = strtok(NULL, s);
   }
}

void get_last_name(char *name, char *n){
    const char s[2] = "/";
    char *token;
    token = strtok(name, s);
    while( token != NULL ) {
        //printf( "%c\n", token[0] );
        strcpy(n, token);
        //n[0] = token[0];
        token = strtok(NULL, s);
    }
}

void sync_algorithm_step_e(TreeName *source, TreeName *backup){
    (source->pointer_to_inode)->pointer_to_destination = backup->pointer_to_inode;
}

int look_for_filename_in_source(TreeName *dir_backup, TreeName *source){
        char name[100], tempname[100];
        strcpy(tempname, (dir_backup)->name);
        get_last_name(tempname, name);
        TreeName *found = find_child_Treename_node(source, source->list_of_children, name);
        if (found != NULL && (found->pointer_to_inode)->type == '-'){
            printf("FOUND on SOURCE %s \n", found->name);
            return 0;
        }
        else{
            printf("DIDN'T FOUND on SOURCE %s \n", name);
            return 1;
        }
}

TreeNameList *sync_algorithm_step_d(TreeName *source, TreeName *backup){
    TreeNameList *i = backup->list_of_children;
    while(i != NULL){
        if(i->child == NULL)
            break;
        printf("INSIDE WHILE, %s\n", (i->child)->name);
        if(((i->child)->pointer_to_inode)->type == '-'){
            int result = look_for_filename_in_source(i->child, source);
            if (result){
                printf("REMOVE FILE %s\n", i->child->name);
                int res = unlink((i->child)->name);
                if (res == 0)
                    printf("unlinked successfully\n");
                else if (res == -1)
                    printf("unlink failed\n");
                i = remove_TreeNameList_node(backup->list_of_children, i->child);
                printf("...Removed\n");
                if (i == NULL){
                    create_TreeNameList(&i);
                }
                backup->list_of_children = i;
            }
        }
        if (i->next != NULL)
            i = i->next;
        else    
            break;
    }
    printf("FINISH OF FIRST WHILE\n");
    if (i == NULL){
        printf("I == NULL\n");
        create_TreeNameList(&i);
    }
    while(i->previous != NULL){
        i = i->previous;
    }
    printf("FINISH OF STEP B\n");
    return i;
}

void copy_source_to_backup(int backup_fd, char *source_pathname){
    int source_fd = open(source_pathname, O_RDONLY);
    if (source_fd == -1){
        printf("ERROR IN OPEN %s\n", source_pathname);
        return;
    }
	ssize_t nread;
	char buffer[1024];

	while ((nread=read(source_fd, buffer, 1024)) > 0){
		if (write(backup_fd,buffer,nread) < nread){
			close(source_fd); 
            printf("ERROR IN WRITING TO BACKUP\n");
            return;
		}
	}
	close(source_fd);
}

void sync_algorithm_step_c(TreeName *source, TreeName *backup, char *name, InodeList *backup_pointer_to_start){
    printf("INSIDE STEP C FOR %s\n", name);
    if (backup->list_of_children == NULL){
        create_TreeNameList(&(backup->list_of_children));
        printf("CREATE LIST OF CHILDREN FOR %s\n", backup->name);
    }
    printf("ADDING IN %s LIST OF CHILDREN THE %s\n", backup->name, name);
    add_child_Treename_node(backup, backup->list_of_children, name);
    printf("FINDING IN %s LIST OF CHILDREN THE %s\n", backup->name, name);
    TreeName *new_tree = find_child_Treename_node(backup, backup->list_of_children, name);
    if(new_tree == NULL)
        printf("ERROR IT DIDN'T FOUND\n");
    else  
        printf("SUCCESS IT FOUND\n");  

    if ((source->pointer_to_inode)->pointer_to_destination != NULL){    // exists copy on backup 
        printf("EXISTS COPY IN BACKUP\n"); 
        char *dest_file_name = (((source->pointer_to_inode)->pointer_to_destination)->list_of_names)->name;
        int result = link(dest_file_name, new_tree->name);
        if (result == 0)
            printf("JUST LINKED %s WITH %s\n", dest_file_name, new_tree->name);
        else if (result == -1) 
            printf("ERROR IN LINK %s WITH %s\n", dest_file_name, new_tree->name);
        new_tree->pointer_to_inode = (source->pointer_to_inode)->pointer_to_destination;
        printf("WILL INSERT %s IN EXISTING INODE LIST OF NAMES\n", new_tree->name);
        init_InodeList_name(new_tree->pointer_to_inode, new_tree->name);
        printf("INSERTED %s IN EXISTING INODE LIST OF NAMES\n", new_tree->name);
    }
    else{                                                              // not exists copy on backup
        printf("NOT EXISTS COPY IN BACKUP\n");
        printf("COPY %s TO %s\n", source->name ,new_tree->name);
        //call_new_process("cp", source->name ,new_tree->name);
        int fd = open(new_tree->name, O_CREAT | O_RDWR, 0664);
        copy_source_to_backup(fd, source->name);
        printf("COPIED %s TO %s\n", source->name ,new_tree->name);
        struct stat statbuf;
        if (stat(new_tree->name, &statbuf) == -1)
	        perror("Failed to get file status");
	    else { 
            printf("CREATE NEW INDOELIST NODE IN BACKUP'S LIST\n");
            add_InodeList_node(backup_pointer_to_start, &statbuf);         // create new inode on InodeList
            printf("FIND NEW INDOELIST NODE IN BACKUP'S LIST\n");
            new_tree->pointer_to_inode = find_InodeList_node(backup_pointer_to_start, statbuf.st_ino); // tree's inode_pointer to new inode
            new_tree->pointer_to_inode->sst_mtime = source->pointer_to_inode->sst_mtime;        // copy source's last modified address to backup's addrress
            if(new_tree->pointer_to_inode == NULL)
                printf("ERROR IT DIDN'T FOUND\n");
            else  
                printf("SUCCESS IT FOUND\n");  
            printf("WILL INSERT %s IN NEW INODE LIST OF NAMES\n", new_tree->name);
            init_InodeList_name(new_tree->pointer_to_inode, new_tree->name);        // tree's inode_pointer = name
            printf("INSERTED %s IN NEW INODE LIST OF NAMES\n", new_tree->name);
        }
        close(fd);
        (source->pointer_to_inode)->pointer_to_destination = new_tree->pointer_to_inode;

    }
}

void delete_directory_contains(TreeName *t){
    TreeNameList *i = t->list_of_children;
    while(i != NULL){       // take TreeName's node whole TreeNameList
        if(i->child == NULL)
            break;
        else{
            if (((i->child)->pointer_to_inode)->type == 'd'){   // for every DIRECTORY that is contained inside TreeName call function to delete his contains
                if(i->child->list_of_children != NULL)
                    delete_directory_contains(i->child);
                int ret = rmdir((i->child)->name);
                if (ret == 0)
                    printf("DIR %s DELETED\n", i->child->name);
                else if (ret == -1)
                    printf("ERROR DIR %s STILL EXISTS\n", i->child->name);
            }
            else
                unlink((i->child)->name);
        }
        i = i->next;
    }

}

int look_for_dirname_in_source(TreeName *dir_backup, TreeName *source){
        char name[100], tempname[100];
        strcpy(tempname, (dir_backup)->name);
        get_last_name(tempname, name);
        TreeName *found = find_child_Treename_node(source, source->list_of_children, name);
        if (found != NULL && (found->pointer_to_inode)->type == 'd'){
            printf("FOUND on SOURCE %s \n", found->name);
            return 0;
        }
        else{
            printf("DIDN'T FOUND on SOURCE %s \n", name);
            return 1;
        }
}

TreeNameList *sync_algorithm_step_b(TreeName *t_s, TreeName *t_b, InodeList *t_s_p_inode, InodeList *t_b_p_inode){
    TreeNameList *i = t_b->list_of_children;
    printf("START OF STEP B\n");
    while(i != NULL){
        if(i->child == NULL)
            break;
        printf("INSIDE WHILE, %s\n", (i->child)->name);
        if(((i->child)->pointer_to_inode)->type == 'd'){
            int result = look_for_dirname_in_source(i->child, t_s);
            if (result){
                delete_directory_contains(i->child);
                rmdir((i->child)->name);
                i = remove_TreeNameList_node(t_b->list_of_children, i->child);
                printf("...Removed\n");
                if (i == NULL){
                    create_TreeNameList(&i);
                }
                t_b->list_of_children = i;
            }
        }
        if (i->next != NULL)
            i = i->next;
        else    
            break;
    }
    printf("FINISH OF FIRST WHILE\n");
    if (i == NULL){
        printf("I == NULL\n");
        create_TreeNameList(&i);
    }
    while(i->previous != NULL){
        i = i->previous;
    }
    printf("FINISH OF STEP B\n");
    return i;
}

TreeNameList *sync_algorithm_step_a(TreeName *t_s, TreeName *t_b, InodeList *t_s_p_inode, InodeList *t_b_p_inode){
    TreeNameList *i = t_s->list_of_children;

    while(i != NULL){
        if(i->child != NULL){
        //     break;
        // else{
            if (((i->child)->pointer_to_inode)->type == 'd'){
                char name[100], tempname[100];
                strcpy(tempname, (i->child)->name);
                get_last_name(tempname, name);
                TreeName *found = find_child_Treename_node(t_b, t_b->list_of_children, name);
                if (found != NULL && found->pointer_to_inode->type == 'd'){
                    printf("FOUND on BACKUP %s \n", found->name);
                    i->child->pointer_to_inode->pointer_to_destination = found->pointer_to_inode;       // source's inode pointer_dest = dest->inode
                }
                else{
                    printf("DIDN'T FOUND on BACKUP %s \n", name);
                    ////
                    TreeName *file_found = find_child_Treename_node(t_b, t_b->list_of_children, name);
                    if (file_found != NULL){
                        printf("REMOVE FILE %s\n", name);
                        unlink(file_found->name);
                        //t_b->list_of_children = remove_child_Treename_node(t_b, t_b->list_of_children, name);
                        t_b->list_of_children = remove_TreeNameList_node(t_b->list_of_children, file_found);
                    }
                    ////
                    struct stat statbuf;
                    char pathname[100];
                    strcpy(pathname, t_b->name);
                    char *pname = concatenate_strings(pathname, name);
                    //printf("pathname %s\n", pname);
                    call_new_process("mkdir", "-p", pname);
                    if (stat(pname, &statbuf) == -1)
		                perror("Failed to get file status");
	                else {
                        //printf("MAKE NEW TREENAMELIST NODE, ON %s with name %s\n", t_b->name, name);
                        if(t_b->list_of_children == NULL)
                            create_TreeNameList(&(t_b->list_of_children));
                        add_child_Treename_node(t_b, t_b->list_of_children, name);          // create new treeName for t_b TreeNameList
                        //printf("GET NEW TREENAMELIST NODE\n");
                        TreeName *new_tree = find_child_Treename_node(t_b, t_b->list_of_children, name);       // get the new TreeNameList node 
                        //printf("NEW TREENAMELIST NODE %s\n", new_tree->name);
                        //printf("MAKE NEW INODELIST NODE with %ld\n", statbuf.st_ino);
                        add_InodeList_node(t_b_p_inode, &statbuf);                                      // create new inode on InodeList
                        //printf("GET NEW INODELIST NODE\n");
                        new_tree->pointer_to_inode = find_InodeList_node(t_b_p_inode, statbuf.st_ino);  // tree's inode_pointer to new inode
                        //printf("NEW TREE's INODE POINTER %ld\n", (new_tree->pointer_to_inode)->st_ino);
                        //printf("INIT NEW TREE's INODE NAME %s\n", new_tree->name);
                        init_InodeList_name(new_tree->pointer_to_inode, new_tree->name);                // init Inode's name 
                        //printf("NEW TREE's INODE NAME %s\n", ((new_tree->pointer_to_inode)->list_of_names)->name);
                        //printf("I's CHILD POINTER TO DEST = NEW TREE's INODE POINTER\n");
                        ((i->child)->pointer_to_inode)->pointer_to_destination = new_tree->pointer_to_inode;    // source's inode pointer_dest = dest->inode
                        //printf("I's POINTER TO DEST %ld\n", (((i->child)->pointer_to_inode)->pointer_to_destination)->st_ino);
                    }
                } 
            }
        }
        i = i->next;
    }
    return t_b->list_of_children;
}

void go_depth(TreeName *t_s, TreeName *t_b, InodeList *t_s_first_inode, InodeList *t_b_first_inode){
    printf("---------------- CALLING STEP A ----------------\n");
    t_b->list_of_children = sync_algorithm_step_a(t_s, t_b, t_s_first_inode, t_b_first_inode);
    printf("---------------- CALLING STEP B ----------------\n");
    t_b->list_of_children = sync_algorithm_step_b(t_s, t_b, t_s_first_inode, t_b_first_inode);
    //return;

    TreeNameList *i = t_s->list_of_children;
    while(i != NULL){
        if (i->child == NULL)                                       // take all directory children and go for them
            break;
        else if ((i->child)->pointer_to_inode != NULL){
            if (((i->child)->pointer_to_inode)->type == 'd'){
                char name[100], tempname[100];
                strcpy(tempname, (i->child)->name);
                get_last_name(tempname, name);
                //printf("CHILD'S NAME %s\n", i->child->name);
                //printf("look for name %s\n", name);
                TreeName *j = find_child_Treename_node(t_b, t_b->list_of_children, name);
                if (j != NULL){
                    printf("Going Call Depthing for %s and %s\n", i->child->name, j->name);
                    go_depth(i->child, j, t_s_first_inode, t_b_first_inode);
                }
            }
        }
        i = i->next;
    }
    printf("---------------- CALLING STEP C ----------------\n");
    i = t_s->list_of_children;                  // i and j pointers to the first node of list_of children
    TreeNameList *j = t_b->list_of_children;
    while(i != NULL){
        TreeName *found_source_in_backup;
        TreeName *found_backup_in_source;                                   // take all file children and call c and d for them
        if (i->child == NULL)
            break; 
        else if (i->child->pointer_to_inode->type == '-'){
            char name[100], tempname[100];
            strcpy(tempname, (i->child)->name);
            get_last_name(tempname, name);

            found_source_in_backup = find_child_Treename_node(t_b, t_b->list_of_children, name);    // look for source's file name in backup
            if (found_source_in_backup == NULL){        // step c
                sync_algorithm_step_c(i->child, t_b, name, t_b_first_inode);
            }
            else if (found_source_in_backup != NULL){   //step e
                printf(" \t Last time modified: \t %s \t Total size: \t %lld bytes\n ",   
                    ctime(&((found_source_in_backup->pointer_to_inode)->sst_mtime)),
                    (long long) (found_source_in_backup->pointer_to_inode)->st_size);
                printf(" \t Last time modified: \t %s \t Total size: \t %lld bytes\n ",   
                    ctime(&(((i->child)->pointer_to_inode)->sst_mtime)),
                    (long long) ((i->child)->pointer_to_inode)->st_size);  
                if((found_source_in_backup->pointer_to_inode)->sst_mtime != ((i->child)->pointer_to_inode)->sst_mtime ||
                    (found_source_in_backup->pointer_to_inode)->st_size != ((i->child)->pointer_to_inode)->st_size){
                        printf("DIF SIZE OR DATE OF MODIFICATION \nREMOVE FILE %s\n", name);
                        unlink(found_source_in_backup->name);
                        //t_b->list_of_children = remove_child_Treename_node(t_b, t_b->list_of_children, name);
                        t_b->list_of_children = remove_TreeNameList_node(t_b->list_of_children, found_source_in_backup);
                        sync_algorithm_step_c(i->child, t_b, name, t_b_first_inode);
                    }
                    else{
                        printf("SAME SIZE AND DATE OF MODIFICATION \nREMOVE FILE %s\n", name);
                        sync_algorithm_step_e(i->child, found_source_in_backup);
                    }
            }
        }
        i = i->next;
    }
    t_b->list_of_children = sync_algorithm_step_d(t_s, t_b);
    if (t_b->list_of_children == NULL)
        create_TreeNameList(&(t_b->list_of_children));
}

int count_rest_TreeNameList_nodes(TreeNameList *l){
    TreeNameList *temp = l;
    int number = 0;
    while (l->next != NULL){
        l = l->next;
        number++;
    }
    return number;
}

void quicksort_TreeNameList(TreeNameList *start, TreeNameList *finish){
    TreeNameList *pivot = finish;
    TreeNameList *pindex = start;
    TreeName *temp;
    if(start == NULL || finish == NULL){
        //printf("start or finish NULL\n");
        return;}
    if (start->child == NULL || finish->child == NULL){
        //printf("start->child or finish->child NULL\n");
        return;}
    if (check_strings((start->child)->name, (finish->child)->name) == 0){
        return;
    }
    if (start == finish->next){
        return;
    }
    TreeNameList *i = start;
    //printf(" \tSTART %s AND FINISH %s\n", start->child->name, finish->child->name);
    while(i != finish->next){
        if (check_strings((i->child)->name, (pivot->child)->name) <= 0){
            //printf("2HECKING %s == %s\n\n", (i->child)->name, (pivot->child)->name);
            if (i == pivot)
                pivot = pindex;
            temp = i->child;            // temp = i
            i->child = pindex->child;   // i = pindex
            pindex->child = temp;       // pindex = temp
            pindex = pindex->next;
        }
            i = i->next;
    }
        quicksort_TreeNameList(start, pivot->previous);
        quicksort_TreeNameList(pivot->next, finish);
}

void sorting_two_columns(TreeNameList *first, TreeNameList *second){
    TreeName *temp;
    if (check_strings((first->child)->name, (second->child)->name) <= 0){
        temp = first->child;            // temp = first
        first->child = second->child;   // first = second
        second->child = temp;       // second = temp
    }
}

void create_children(TreeName *tree, char *name, InodeList *pointer_to_start){
    DIR 	*dir_ptr;
	struct 	dirent *direntp;
    char root_name[100], new_name[100];

    //strcpy(root_name, name);
	if ( ( dir_ptr = opendir( name ) ) == NULL ){
		//printf("error in dirname %s\n",dirname);
		fprintf(stderr, "cannot open %s \n",name);
	}
	else {
        struct stat statbuf;
		printf("dirname %s\n",name);
        create_TreeNameList(&(tree->list_of_children)); //create list for TreeName children
        
		while ( ( direntp=readdir(dir_ptr) ) != NULL ){

            if((check_strings(direntp->d_name,".") == 0) || (check_strings(direntp->d_name,"..") == 0))
                continue;

            add_child_Treename_node(tree, tree->list_of_children, direntp->d_name);
            TreeName *new_tree = find_child_Treename_node(tree, tree->list_of_children, direntp->d_name);

            // strcpy(new_name, root_name);
            // concatenate_strings(new_name, direntp->d_name);

			//printf("inode %d of the entry %s \n", \
			//				(int)direntp->d_ino, new_tree->name);
	        if (stat(new_tree->name, &statbuf) == -1)
		        perror("Failed to get file status");
	        else {
                InodeList *found = find_InodeList_node(pointer_to_start, direntp->d_ino);
                if (found == NULL){     //create new listnode            
                    add_InodeList_node(pointer_to_start, &statbuf);         // create new inode on InodeList
                    new_tree->pointer_to_inode = find_InodeList_node(pointer_to_start, direntp->d_ino); // tree's inode_pointer to new inode
                    init_InodeList_name(new_tree->pointer_to_inode, new_tree->name);        // tree's inode_pointer = name
                }
                else{                   // add to an existing
                    //printf("FOUND SAME INODE ON EXISTING LISTNODE %ld %ld\n", found->st_ino, direntp->d_ino);
                    // init_InodeList_name(found, (char *)(direntp->d_name));
                    new_tree->pointer_to_inode = found;
                    init_InodeList_name(found, new_tree->name);
                }

            }
            
		}
		closedir(dir_ptr);

        TreeNameList *l = tree->list_of_children;
        if (l->child == NULL){
            //printf("DIRNAME %s is empty\n", tree->name);
        }
        else {
            while(l != NULL){
                if (((l->child)->pointer_to_inode)->type == 'd'){
                    create_children(l->child, (l->child)->name, pointer_to_start);
                }
                    l = l->next;

            }
        }
    }
    TreeNameList *l = tree->list_of_children;
    if (l->child == NULL){
        printf("DIRNAME %s is empty\n", tree->name);
    }
    else {
        while(l->next != NULL){
            l = l->next;
        }
        quicksort_TreeNameList(tree->list_of_children, l);
    }
}

TreeName *create_source(TreeName *tree, char *name){
    struct stat statbuf;

    create_TreeName(&tree);                 // create TreeName for source name
    add_TreeName_node(tree, name);          // init with the name

    create_InodeList(&(tree->pointer_to_inode));    // create Inode node for information

    stat(name, &statbuf);                   // take stat information
    add_InodeList_node(tree->pointer_to_inode, &statbuf);   // init inode with information
    init_InodeList_name(tree->pointer_to_inode, name);      // init inode with source name

    //create_TreeNameList(&(tree->list_of_children));         // create a list for children TreeName

    create_children(tree, name, tree->pointer_to_inode);
     return tree;
}

int stopFlag = 0;

void f( int signum ){ /* no explicit call to function f in main()*/
	//signal (SIGINT , f); 
	/* re - establish disposition of the signal SIGINT */

	printf ("OUCH !\n");
	stopFlag = 1;
	//return 1;
	//return;
}

//The fixed size of the event buffer:
#define EVENT_SIZE  ( sizeof (struct inotify_event) )

//The size of the read buffer: estimate 1024 events with 16 bytes per name over and above the fixed size above
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )



void add_to_inotify(TreeName *source, int fd, wd_array *wd_arr){
    int wd = inotify_add_watch(fd, source->name, IN_ALL_EVENTS);
	if( wd == -1 ) {
		printf("failed to add watch %s\n", source->name);
	} 
    else{
		printf("Watching %s as %i\n", source->name, wd);
        add_wd_array(wd_arr, source->name, wd);
    }
    TreeNameList *l = source->list_of_children;
    while(l != NULL){
        if(l->child == NULL)
            break;
        if(((l->child)->pointer_to_inode)->type == 'd'){
            add_to_inotify(l->child, fd, wd_arr);
        }
        l = l->next;
    }
}

wd_array *remove_from_inotify(TreeName *source, int fd, wd_array *wd_arr, int wd){
    printf("fd = %d\n", fd);
    int res = inotify_rm_watch(fd, wd);
    if (res == 0)
        printf("I KICKED HIS ASS OK\n");
    else if (res == -1)
        printf("I GOT THE LOS POULOS HERMANOS\n");
    wd_arr = remove_wd_array(wd_arr, wd);
    return wd_arr;
}

void delete_all(TreeName *source, int fd, wd_array *wd_arr, int wd){
    wd_array *l = wd_arr;
    while(l != NULL){
        int res = inotify_rm_watch(fd, l->wd);
        if (res == 0)
            printf("I KICKED HIS ASS OK\n");
        else if (res == -1)
            printf("I GOT THE LOS POULOS HERMANOS\n");
        l = l->next;
    }
    delete_wd_array(wd_arr);
}

void fail(const char *message) {
	perror(message);
	exit(1);
}
 char * target_type(struct inotify_event *event) {
	if( event->len == 0 )
		return "";
	else
		return event->mask & IN_ISDIR ? "directory" : "file";
}

 char * target_name(struct inotify_event *event) {
	return event->len > 0 ? event->name : NULL;
}

 char *event_name(struct inotify_event *event) {
	if (event->mask & IN_ATTRIB)
		return "attrib";
	else if (event->mask & IN_CLOSE_WRITE)
		return "close write";
	else if (event->mask & IN_CREATE)	
        return "create";
	else if (event->mask & IN_DELETE)
		return "delete";
	else if (event->mask & IN_DELETE_SELF)
		return "watch target deleted";
	else if (event->mask & IN_MODIFY)
		return "modify";
	else if (event->mask & IN_MOVED_FROM)
		return "moved out";
	else if (event->mask & IN_MOVED_TO)
		return "moved into";
	// else
	// 	return "unknown event";
}

TreeName *get_TreeName_node(TreeName *t, char *name){
    TreeName *temp = NULL;
    if(empty_TreeName(t))
        return NULL;
        ///
    if (t->name == NULL)
        return NULL;
        ////
    printf("%s %s\n", t->name, name);
    if (check_strings(t->name, name) == 0)
        return t;
    else{
        TreeNameList *i = t->list_of_children;
        //temp = find_child_Treename_node(t, t->list_of_children, name);
        while(i != NULL){
            if (i->child == NULL){
                printf("i == NULL\n");
                return NULL;
            }
            printf("%s \t child %s %s\n", t->name, i->child->name, name);
            if(check_strings((i->child)->name, name) == 0)
                return i->child;
            if(((i->child)->pointer_to_inode)->type == 'd'){
                printf("going to call get_TreeName for %s %s\n", i->child->name, name);
                temp = get_TreeName_node(i->child, name);
                if (temp != NULL)
                    break;   
            }
            i = i->next;
        }

    }
    return temp; 
}

TreeName *in_create(TreeName *source, wd_array *wd_arr, TreeName *backup, InodeList *source_start, char *name){
    printf("CALL SYNC\n");
    TreeName *s_f = get_TreeName_node(source, wd_arr->name);
    if (s_f != NULL)
        printf("name %s and %s\n", s_f->name, name);
    if (s_f->list_of_children == NULL)
        create_TreeNameList(&(s_f->list_of_children));
    add_child_Treename_node(s_f, s_f->list_of_children, name);
    printf("added to %s\n", s_f->name);
    TreeName *f = find_child_Treename_node(s_f, s_f->list_of_children, name);
    printf("I added name %s\n", f->name);
    struct stat statbuf;
    stat(f->name, &statbuf); 
    printf("search for %ld\n", statbuf.st_ino);
    InodeList *l = find_InodeList_node(source_start, statbuf.st_ino);
    if (l == NULL){
        printf("DIDN'T FIND THE INODE\n");
        add_InodeList_node(source_start, &statbuf);
        printf("Added %ld ON THE INODE\n", statbuf.st_ino);
        f->pointer_to_inode = find_InodeList_node(source_start, statbuf.st_ino); // tree's inode_pointer to new inode
        printf("Found %ld ON THE INODE\n", f->pointer_to_inode->st_ino);
        init_InodeList_name(f->pointer_to_inode, f->name);
        printf("Init %s ON THE INODE\n", f->name);
    }
    else{
        printf("FOUND THE INODE\n");
        init_InodeList_name(l, f->name);
        f->pointer_to_inode = l;
    }
    printf("%s \n", ((s_f->pointer_to_inode)->pointer_to_destination)->list_of_names->name);
    //print_TreeNameList(backup->list_of_children);
    TreeName *b_f = get_TreeName_node(backup, (((s_f->pointer_to_inode)->pointer_to_destination)->list_of_names)->name);
    //print_TreeNameList(backup->list_of_children);
    if (b_f == NULL)
        printf("get_TreeName_node return NULL\n");
    printf("name %s\n", b_f->name);
    if((f->pointer_to_inode)->type == '-'){
        printf("going to call C\n");
        sync_algorithm_step_c(f, b_f, name, backup->pointer_to_inode);
        return NULL;
    }
    else if ((f->pointer_to_inode)->type == 'd'){
        if(b_f->list_of_children == NULL)
            create_TreeNameList(&(b_f->list_of_children));
        b_f->list_of_children = sync_algorithm_step_a(s_f, b_f, source->pointer_to_inode, backup->pointer_to_inode);
        //print_TreeNameList(b_f->list_of_children);
        if (check_strings(backup->name, b_f->name) == 0)
            backup->list_of_children = b_f->list_of_children;
        return s_f;
    }
                //look for backup and create backup
}

TreeNameList *in_delete(TreeName *source, wd_array *wd_arr, TreeName *backup, InodeList *source_start, char *name){
    TreeNameList *new_b_f_l;
    new_b_f_l = sync_algorithm_step_d(source, backup);
    return new_b_f_l;
}

wd_array *monitoring(TreeName *source, int fd, wd_array *wd_arr, TreeName *backup, InodeList *source_start){
    signal(SIGINT, f);
    int length, read_ptr, read_offset; //management of variable length events
	//int i;				// descriptors returned from inotify subsystem
	char buffer[EVENT_BUF_LEN];	//the buffer to use for reading the events

    read_offset = 0; //remaining number of bytes from previous read
	while (stopFlag == 0) {
		/* read next series of events */
		length = read(fd, buffer + read_offset, sizeof(buffer) - read_offset);
		if (length < 0)
			fail("read");
		length += read_offset; // if there was an offset, add it to the number of bytes to process
		read_ptr = 0;
		
		// process each event
		// make sure at least the fixed part of the event in included in the buffer
		while (read_ptr + EVENT_SIZE <= length ) { 
			//point event to beginning of fixed part of next inotify_event structure
			struct inotify_event *event = (struct inotify_event *) &buffer[ read_ptr ];
			
			// if however the dynamic part exceeds the buffer, 
			// that means that we cannot fully read all event data and we need to 
			// deffer processing until next read completes
			if( read_ptr + EVENT_SIZE + event->len > length ) 
				break;
			//event is fully received, process
			if (event_name(event) != NULL){
			printf("WD:%i %s %s %s COOKIE=%u\n", 
				event->wd, event_name(event), 
				target_type(event), target_name(event), event->cookie);
                char *eve = event_name(event);
                if (check_strings(eve, "create") == 0){
                    printf("CALL SYNC\n");
                    int wd = event->wd;
                    print_wd_array(wd_arr);
                    wd_array *found = find_wd_array(wd_arr, wd);
                    if (found != NULL){
                        char name[100];
                        strcpy(name, target_name(event));
                        TreeName *new_dir = in_create(source, found, backup, source_start, name);
                        if (new_dir != NULL){
                            new_dir = find_child_Treename_node(new_dir, new_dir->list_of_children, name);
                            wd = inotify_add_watch(fd, new_dir->name, IN_ALL_EVENTS);
                            if( wd == -1 ) {
                                printf("failed to add watch %s\n", new_dir->name);
                            } 
                            else{
                                printf("Watching %s as %i\n", new_dir->name, wd);
                                add_wd_array(wd_arr, new_dir->name, wd);
                            }
                            TreeNameList *l = new_dir->list_of_children;
                        }
                    }
                    //printf("wd_array %s %d\n",found->name, found->wd);
                }
                else if (check_strings(event_name(event), "attrib") == 0){

                }
                else if (check_strings(event_name(event), "delete") == 0){
                    int wd = event->wd;
                    //print_wd_array(wd_arr);
                    wd_array *found = find_wd_array(wd_arr, wd);
                    if(check_strings(target_type(event), "directory") == 0){
                        char name[100];
                        printf("I FUCKED UP IN STRCPY\n");
                        strcpy(name, target_name(event));

                        printf("GOING TO RMV WD_AR %d\n",wd);
                           //remove from watching list
                        printf("REMOVED WD_AR %d\n",wd);
                        TreeName *s_f = get_TreeName_node(source, found->name);
                        if (s_f != NULL)
                            printf("name %s and %s\n", s_f->name, name);
                        TreeName *b_f = get_TreeName_node(backup, (((s_f->pointer_to_inode)->pointer_to_destination)->list_of_names)->name);
                        printf("Parent Trees %s %s \n",s_f->name, b_f->name);
                        s_f->list_of_children = remove_child_Treename_node(s_f, s_f->list_of_children, name);   //remove dir from source 
                        if (s_f->list_of_children == NULL)
                            create_TreeNameList(&(s_f->list_of_children));
                        if (check_strings(source->name, s_f->name) == 0)
                            source->list_of_children = s_f->list_of_children;

                        b_f->list_of_children = sync_algorithm_step_b(s_f, b_f, source_start, backup->pointer_to_inode); //remove dir from backup
                        if (check_strings(backup->name, b_f->name) == 0)
                            backup->list_of_children = b_f->list_of_children;
                        
                        wd_arr = remove_wd_array(wd_arr, wd);
                    }
                    else{
                        if (found != NULL){
                            char name[100];
                            strcpy(name, target_name(event));

                            TreeName *s_f = get_TreeName_node(source, found->name);
                            if (s_f != NULL)
                                printf("name %s and %s\n", s_f->name, name);
                            TreeName *b_f = get_TreeName_node(backup, (((s_f->pointer_to_inode)->pointer_to_destination)->list_of_names)->name);
                            printf("Parent Trees %s %s \n",s_f->name, b_f->name);
                            s_f->list_of_children = remove_child_Treename_node(s_f, s_f->list_of_children, name);
                            if (s_f->list_of_children == NULL)
                                create_TreeNameList(&(s_f->list_of_children));
                            if (check_strings(source->name, s_f->name) == 0)
                                source->list_of_children = s_f->list_of_children;
                            b_f->list_of_children = in_delete(s_f, found, b_f, source_start, name);
                            if (check_strings(backup->name, b_f->name) == 0)
                                backup->list_of_children = b_f->list_of_children;
                        }
                    }
                }
                else if (check_strings(event_name(event), "watch target deleted") == 0){
                    int wd = event->wd;
                    wd_array *found = find_wd_array(wd_arr, wd);
                    char *n = target_name(event);
                    if (n == NULL)
                        printf("n == nULL\n");
                    
                    if (found != NULL && n!= NULL){
                        char name[100];
                        printf("I FUCKED UP IN STRCPY\n");
                        strcpy(name, target_name(event));

                        printf("GOING TO RMV WD_AR %d\n",wd);
                           //remove from watching list
                        printf("REMOVED WD_AR %d\n",wd);
                        TreeName *s_f = get_TreeName_node(source, found->name);
                        if (s_f != NULL)
                            printf("name %s and %s\n", s_f->name, name);
                        TreeName *b_f = get_TreeName_node(backup, (((s_f->pointer_to_inode)->pointer_to_destination)->list_of_names)->name);
                        printf("Parent Trees %s %s \n",s_f->name, b_f->name);
                        s_f->list_of_children = remove_child_Treename_node(s_f, s_f->list_of_children, name);   //remove dir from source 
                        if (s_f->list_of_children == NULL)
                            create_TreeNameList(&(s_f->list_of_children));
                        if (check_strings(source->name, s_f->name) == 0)
                            source->list_of_children = s_f->list_of_children;

                        b_f->list_of_children = sync_algorithm_step_b(s_f, b_f, source_start, backup->pointer_to_inode); //remove dir from backup
                        if (check_strings(backup->name, b_f->name) == 0)
                            backup->list_of_children = b_f->list_of_children;
                        
                     wd_arr = remove_wd_array(wd_arr, wd);
                    }
                    
                }
                
            }
			//advance read_ptr to the beginning of the next event
			read_ptr += EVENT_SIZE + event->len;
		}
		//check to see if a partial event remains at the end
		if( read_ptr < length ) {
			//copy the remaining bytes from the end of the buffer to the beginning of it
			memcpy(buffer, buffer + read_ptr, length - read_ptr);
			//and signal the next read to begin immediatelly after them			
			read_offset = length - read_ptr;
		} else
			read_offset = 0;
	}
    return wd_arr;
}

void start_inotify(TreeName *source, TreeName *backup){
    int i, watched;
	int fd, wd;					// descriptors returned from inotify subsystem
	char buffer[EVENT_BUF_LEN];	//the buffer to use for reading the events
    wd_array *wd_arr;
    create_wd_array(&wd_arr);

	
	/*creating the INOTIFY instance*/
	fd = inotify_init();
    add_to_inotify(source, fd, wd_arr);
    printf("fd = %d\n", fd);
    char line[100];
    char function, name[100];
	//signal (SIGINT , f);
	while(1){
        printf("PLEASE TYPE A COMMAND\n \
                a \t add inotify\n \
                r \t remove inotify\n \
                p \t print inotify\n \
                m \t monitor with inotify\n \
                e \t exit\n");
        
        scanf("%99[^\n]", line);
        printf("COMMAND: %s\n", line);
        sscanf(line, "%s", &function);

        if (check_strings(&function, "a") == 0){
            printf("COMMAND: Create %c\n", function);
            
            printf("EXPECTED OUTPUT:\n\t- Created NameList\n");
        }
        else if (check_strings(&function, "r") == 0){
            sscanf(line, "%*s %d", &wd);
            printf("COMMAND: Remove %c %d\n", function, wd);
            wd_arr = remove_from_inotify(source, fd, wd_arr, wd);
            printf("EXPECTED OUTPUT:\n\t- Removed %d\n", wd);
            //printf("NOW FIRST wd_array %d %s\n", wd_arr->wd, wd_arr->name);
        }
        else if (check_strings(&function, "p") == 0){
            sscanf(line, "%c", &function);
            printf("COMMAND: Print %c\n",function);
            print_wd_array(wd_arr);
        }
        else if (check_strings(&function, "m") == 0){
            sscanf(line, "%c", &function);
            printf("COMMAND: Print %c\n",function);
            wd_arr = monitoring(source, fd, wd_arr, backup, source->pointer_to_inode);
            printf("I KILLED JUST DELETE AND GO BACK TO MAIN\n");
            delete_all(source, fd, wd_arr, wd);
            close(fd);
            break;
        }
        else if (check_strings(&function, "e") == 0){
            sscanf(line, "%c", &function);
            printf("COMMAND: Exit %c\n", function);
            delete_all(source, fd, wd_arr, wd);
            close(fd);
            break;

        }
        getchar();
	}
}