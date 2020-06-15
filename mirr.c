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


#include "functions.h"
#include "structures.h"

ssize_t readlinked(char *path, char *buffer, size_t size_of_buffer){
    int fd; 
    ssize_t read_bytes;
    if ( ( fd=open(path, O_RDONLY)) < 0 ){
	    perror("open");
	    return -1;
    }
    read_bytes = read(fd, buffer, size_of_buffer);
    close(fd);
    return read_bytes;
}

int main(int argc, char *argv[]){
    TreeName *tree;
    TreeName *backup;

    tree = create_source(tree, argv[1]);
    backup = create_source(backup, argv[2]);
    (tree->pointer_to_inode)->pointer_to_destination = backup->pointer_to_inode;
    //print_TreeName(tree);
    printf("\n\n");
    //print_TreeName(backup);
    printf("\n\n");
    go_depth(tree, backup, tree->pointer_to_inode, backup->pointer_to_inode);
    printf("\n\n");
    print_TreeName(tree);
    printf("\n\n");
    print_TreeName(backup);

    printf("\n\n");
    start_inotify(tree, backup);
    printf("\n\n");
    
    print_TreeName(tree);
    printf("\n\n");
    print_TreeName(backup);
    printf("\n\n");
    
    delete_TreeName(tree);
    delete_TreeName(backup);

        

}