#include "structures.h"

char *get_parent_name(TreeName *t){
    return (t->pointer_to_father)->name;
}

int check_strings(char *column1, char *column2){
    int compare = strcmp(column1, column2);
    return compare;
}

int check_memory_bytes(void *column1, void *column2, int size){
    int compare = memcmp(column1, column2, size);
    return compare;
}

// About TreeNameList

void create_TreeNameList(TreeNameList **lt){
    *lt = malloc(sizeof(TreeNameList));
    (*lt)->child = NULL;
    (*lt)->previous = NULL;
    (*lt)->next = NULL;
}

void add_TreeNameList_node(TreeNameList *lt, TreeName *t){
    if (lt->child != NULL){            // list != empty
    //printf("list not empty\n");
        if(lt->next == NULL){            // list->next == empty
            TreeNameList *new_listnode;
            create_TreeNameList(&new_listnode);
            lt->next = new_listnode;
            new_listnode->previous = lt;
            //create_TreeName(&(new_listnode->child));
            //add_TreeName_node(new_listnode->child, t->name);
            new_listnode->child = t;
            
        }
        else{                           // list->next != empty
            add_TreeNameList_node(lt->next, t);
        }
    }
    else{                               // list == empty
        //printf("list empty\n");
        // create_TreeName(&(lt->child)); 
        // add_TreeName_node(lt->child, t->name);
        lt->child = t;

    }
}

int empty_TreeNameList(TreeNameList *lt){
    return (lt == NULL);
}

TreeNameList *find_TreeNameList_node(TreeNameList *lt, TreeName *t){
    if(empty_TreeNameList(lt))
        return NULL;
        ///
    if (lt->child == NULL)
        return NULL;
        ////
    if (check_strings((lt->child)->name, t->name) == 0)
        return lt;
    else{
        TreeNameList *found = find_TreeNameList_node(lt->next, t);
        return found;
    } 
}

TreeNameList *remove_TreeNameList_node(TreeNameList *lt, TreeName *t){
    TreeNameList *new_l;
    printf("lt->child->name %s \t t->name %s\n", (lt->child)->name, t->name);
    if(check_strings((lt->child)->name, t->name) == 0){           // list->name found
        if(lt->previous == NULL){                     // first listnode
            printf("first node\n");
            if (lt->next != NULL){                    // more than one listnodes
                printf("first node exist more\n");
                (lt->next)->previous = NULL;
                new_l = lt->next;
            }
            else{                                        // one last node
                new_l = NULL;
                printf("first node last one\n");
            }
        }
        else if (lt->next == NULL){                   // last listnode
            (lt->previous)->next = NULL;
            printf("last node\n");
        }
        else{                                           // middle listnode
            (lt->previous)->next = lt->next;
            (lt->next)->previous = lt->previous;
            printf("middle node\n");
        }
        remove_TreeName_node(lt->child, (lt->child)->name);
        free(lt);
        //new_l = NULL;
    }
    else{                                               // list->name not here
        if(!empty_TreeNameList(lt->next)){                // list->next not empty
            printf("going for next noden\n");
            new_l = lt;
            remove_TreeNameList_node(lt->next, t);  // go to next
        }
        else                                            // list->next empty
            printf("DIDN'T FOUNDON THE NameList\n");
    }
    return new_l;
}

void delete_TreeNameList(TreeNameList *lt){
    if (!empty_TreeNameList(lt)){
        delete_TreeNameList(lt->next);
        if(!empty_TreeName(lt->child))
            delete_TreeName(lt->child);
        free(lt);
    }
}

void print_TreeNameList(TreeNameList *lt){
    if(lt->child != NULL){
        print_TreeName(lt->child);
        if (!empty_TreeNameList(lt->next)){
            printf("Going for next child\n");
            print_TreeNameList(lt->next);
        }
    }
}

// TreeName

void create_TreeName(TreeName **t){
    *t = malloc(sizeof(TreeName));
    (*t)->name = NULL;
    (*t)->pointer_to_inode = NULL;
    (*t)->pointer_to_father = NULL;
    (*t)->list_of_children = NULL;
}

void add_TreeName_node(TreeName *t, char *name){
    
    if(t->name != NULL){
        // if (t->list_of_children == NULL){
        //     create_TreeNameList(&(t->list_of_children));
        // }
        // add_TreeNameList_node(t->list_of_children, t);
        //printf("must go inside the tree's children\n");
    }
    else{
        // search_for_same_name(t, )
        t->name = malloc(100*sizeof(char));
        if(t->pointer_to_father != NULL){                   // if has parent directory
            strcpy(t->name, (t->pointer_to_father)->name);  // take parent's path
            strcat(t->name, "/");                           // add / for the new file
            strcat(t->name, name);  
        }
        else
            strcpy(t->name, name);                              // add the new name
    }
}

void connect_TreeName_node_to_InodeList(TreeName *t, InodeList *il){
    t->pointer_to_inode = il;
}

void add_child_Treename_node(TreeName *t, TreeNameList *lt, char *name){
    TreeName *new_child_TreeName_node;
    create_TreeName(&new_child_TreeName_node);             // new TreeName node for name
    new_child_TreeName_node->pointer_to_father = t;        // add connect with his father
    add_TreeName_node(new_child_TreeName_node, name);      // add name to new TreeName node
    add_TreeNameList_node(lt, new_child_TreeName_node);    // add new TreeName node to list of childs
}

TreeNameList *remove_child_Treename_node(TreeName *t, TreeNameList *lt, char *name){
    TreeName *temp;
    create_TreeName(&temp);                     // new TreeName node for name
    temp->pointer_to_father = t;                // temp connect with father to take his name
    add_TreeName_node(temp, name);              // add name to new TreeName node
    lt = remove_TreeNameList_node(lt, temp);    // remove TreeName node from list of childs
    remove_TreeName_node(temp, temp->name);           // delete the temp node
    return lt;
}

TreeName *find_child_Treename_node(TreeName *t, TreeNameList *lt, char *name){
    TreeName *temp;
    create_TreeName(&temp);             // new TreeName node for name
    temp->pointer_to_father = t;        // add connect with his father
    add_TreeName_node(temp, name);      // add name to new TreeName node
    TreeNameList *found = find_TreeNameList_node(lt, temp);    // look for new TreeName node to list of childs
    remove_TreeName_node(temp, temp->name);

    if (found == NULL)
        return NULL;
    else 
        return found->child;
}


int empty_TreeName(TreeName *t){
    return (t == NULL);
}

void find_TreeName_node(){
    

}

void remove_TreeName_node(TreeName *t, char *name){
    if (check_strings(t->name, name) == 0){
        if (t->pointer_to_inode != NULL){
            if((t->pointer_to_inode)->number_of_names > 1){
                (t->pointer_to_inode)->list_of_names = remove_NameList_node((t->pointer_to_inode)->list_of_names, t->name);
                printf("TreeName %s has %d hardliks\n", t->name, (t->pointer_to_inode)->number_of_names);
                (t->pointer_to_inode)->number_of_names -= 1;
            }
            else{
                remove_InodeList_node(t->pointer_to_inode, (t->pointer_to_inode)->st_ino);
            }
        }
        if (t->list_of_children != NULL)
            delete_TreeNameList(t->list_of_children);
        free(t->name);
        free(t);
    }
    else{
        printf("must look inside the tree's children\n");
    }

}

void delete_TreeName(TreeName *t){
    if (!empty_TreeName(t)){
        delete_TreeNameList(t->list_of_children);
        if (t->pointer_to_inode != NULL){
            if((t->pointer_to_inode)->number_of_names > 1){
                (t->pointer_to_inode)->list_of_names = remove_NameList_node((t->pointer_to_inode)->list_of_names, t->name);
                printf("TreeName %s has %d hardliks\n", t->name, (t->pointer_to_inode)->number_of_names);
                (t->pointer_to_inode)->number_of_names -= 1;
            }
            else{
                remove_InodeList_node(t->pointer_to_inode, (t->pointer_to_inode)->st_ino);
            }
        }
        free(t->name);
        free(t);
    }
}

void print_TreeName(TreeName *t){
    printf("TreeName's name \t %s\n",t->name);
    if (t->pointer_to_father != NULL)
        printf("father's name %s\n", get_parent_name(t));
    if (t->pointer_to_inode != NULL)
        print_Inode_node(t->pointer_to_inode);
    if (t->list_of_children != NULL){
        printf("Going to print his children\n");
        print_TreeNameList(t->list_of_children);
    }
}

// InodeList

void create_InodeList(InodeList **il){
    *il = malloc(sizeof(InodeList));
    (*il)->st_ino = 0;
    (*il)->st_mode = 0;

    (*il)->sst_mtime = 0;
    (*il)->st_size = 0;

    (*il)->st_nlink = 0;

    (*il)->list_of_names = NULL;
    (*il)->number_of_names = 0;
    (*il)->pointer_to_destination = NULL;

    (*il)->next = NULL;
    (*il)->previous = NULL;  
}

void init_InodeList_node(InodeList *il, struct stat *statbuf){
    il->st_ino = statbuf->st_ino;
    il->st_mode = statbuf->st_mode;

    il->sst_mtime = statbuf->st_mtime;
    il->st_size = statbuf->st_size;

    il->st_nlink = statbuf->st_nlink;

	switch (il->st_mode & S_IFMT){
  	case S_IFREG: il->type = '-'; break;
  	case S_IFDIR: il->type = 'd'; break;
  	default:      il->type = '?'; break;
  	}
}

void init_InodeList_name(InodeList *il, char *name){
    if(il->list_of_names == NULL)
        create_NameList(&(il->list_of_names));
    //printf("init Namelist name\n");
    add_NameList_node(il->list_of_names, name);
    //printf("inited\n");
    il->number_of_names += 1;
}

void add_InodeList_node(InodeList *il, struct stat *statbuf){
    if (il->st_ino != 0){
        //printf("il->st_ino != 0\n");
        if(il->next == NULL){            // list->next == empty
            //printf("il->next == NULL\n");
            InodeList *new_listnode;
            create_InodeList(&new_listnode);
            il->next = new_listnode;
            new_listnode->previous = il;
            //printf("init_InodeList_node\n");
            init_InodeList_node(new_listnode, statbuf);
            //printf("inited_InodeList_node\n");
        }
        else{                           // list->next != empty
            add_InodeList_node(il->next, statbuf);
        }
    }else{
        //printf("il->st_ino = 0\n");
        init_InodeList_node(il, statbuf);
    }
}

int empty_InodeList(InodeList *il){
    return (il == NULL);
}

InodeList *find_InodeList_node(InodeList *il, ino_t st_ino){
    //printf("looking for %ld and %ld\n", (long)il->st_ino, (long)st_ino);
    if ( check_memory_bytes(&(il->st_ino), &st_ino, sizeof(ino_t)) == 0)
        return il;
    else{
        if(!empty_InodeList(il->next)){
            InodeList *found = find_InodeList_node(il->next, st_ino);
            return found;
        }
        else
            return NULL;
    } 
}

// function to check inode with the inodelist and if found dont create new node just link it

InodeList *remove_InodeList_node(InodeList *il, ino_t st_ino){
    InodeList *new_l;

    if(check_memory_bytes(&(il->st_ino), &st_ino, sizeof(ino_t)) == 0){           // list->name found
        if(il->previous == NULL){                     // first listnode
            printf("first node\n");
            if (il->next != NULL){                    // more than one listnodes
                printf("first node exist more\n");
                (il->next)->previous = NULL;
                new_l = il->next;
            }
            else{                                        // one last node
                new_l = NULL;
                printf("first node last one\n");
            }
        }
        else if (il->next == NULL){                   // last listnode
            (il->previous)->next = NULL;
            printf("last node\n");
        }
        else{                                           // middle listnode
            (il->previous)->next = il->next;
            (il->next)->previous = il->previous;
            printf("middle node\n");
        }
        delete_NameList(il->list_of_names);
        free(il);
    }
    else{                                               // list->name not here
        if(!empty_InodeList(il->next)){                // list->next not empty
            printf("going for next noden\n");
            new_l = il;
            remove_InodeList_node(il->next, st_ino);  // go to next
        }
        else                                            // list->next empty
            printf("DIDN'T FOUND %ld ON THE NameList\n", (long)st_ino);
    }
    return new_l;
    
}

void delete_InodeList(InodeList *il){
    if (!empty_InodeList(il)){
        delete_InodeList(il->next);
        delete_NameList(il->list_of_names);
        free(il);
    }   
}

void print_InodeList(InodeList *il){

    print_Inode_node(il);
    if (!empty_InodeList(il->next)){
        printf("\n");
        print_InodeList(il->next);
    }
}

void print_Inode_node(InodeList *il){
    printf(" \t type: \t %c\n",il->type) ;
    printf(" \t Inode number: \t %ld\n \t Mode: \t %lo (octal)\n \t Last time modified: \t %s \t Total size: \t %lld bytes\n \t Number of hard links: \t %ld\n", 
            (long) il->st_ino, 
            (unsigned long) il->st_mode,
            ctime(&(il->sst_mtime)),
            (long long) il->st_size,
            (long) il->st_nlink);
    printf(" \t Number_of_names \t %d\n",il->number_of_names);
    if (il->list_of_names != NULL)
        print_NameList(il->list_of_names);
    if(il->pointer_to_destination != NULL){
        printf(" \t Destination Inode \t \n");
        print_Inode_node(il->pointer_to_destination);
    }
}

// NameList

void create_NameList(NameList **l){
    *l = malloc(sizeof(NameList));
    (*l)->name = NULL;
    (*l)->previous = NULL;
    (*l)->next = NULL;
}

void init_NameList_name(NameList *l, char *name){
    l->name = malloc(100*sizeof(char));
    strcpy(l->name, name);
}

void add_NameList_node(NameList *l, char *name){
    if (l->name != NULL){            // list != empty
    //printf("list not empty\n");
        if(l->next == NULL){            // list->next == empty
            NameList *new_listnode;
            create_NameList(&new_listnode);
            l->next = new_listnode;
            new_listnode->previous = l;
            init_NameList_name(new_listnode, name);
        }
        else{                           // list->next != empty
            add_NameList_node(l->next, name);
        }
    }
    else{                               // list == empty
        //printf("list empty\n");
        init_NameList_name(l, name);
    }
}

int empty_NameList(NameList *l){
    return (l == NULL);
}

void find_NameList_node(){
    
}

NameList *remove_NameList_node(NameList *l, char *name){
    NameList *new_l;

    if(check_strings(l->name, name) == 0){           // list->name found
        if(l->previous == NULL){                     // first listnode
            printf("first node\n");
            if (l->next != NULL){                    // more than one listnodes
                printf("first node exist more\n");
                (l->next)->previous = NULL;
                new_l = l->next;
            }
            else{                                        // one last node
                new_l = NULL;
                printf("first node last one\n");
            }
        }
        else if (l->next == NULL){                   // last listnode
            (l->previous)->next = NULL;
            printf("last node\n");
        }
        else{                                           // middle listnode
            (l->previous)->next = l->next;
            (l->next)->previous = l->previous;
            printf("middle node\n");
        }
        free(l->name);
        free(l);
    }
    else{                                               // list->name not here
        if(!empty_NameList(l->next)){                // list->next not empty
            printf("going for next noden\n");
            new_l = l;
            remove_NameList_node(l->next, name);  // go to next
        }
        else                                            // list->next empty
            printf("DIDN'T FOUND %s ON THE NameList\n", name);
    }
    return new_l;
}

void delete_NameList(NameList *l){
    if (!empty_NameList(l)){
        delete_NameList(l->next);
        free(l->name);
        free(l);
    }
}

void print_NameList(NameList *l){
    printf(" \t Name: %s\n", l->name);
    if (!empty_NameList(l->next)){
        print_NameList(l->next);
    }
}

void create_wd_array(wd_array **wd_ar){
    *wd_ar = malloc(sizeof(wd_array));
    (*wd_ar)->name = NULL;
    (*wd_ar)->wd = 0;
    (*wd_ar)->previous = NULL;
    (*wd_ar)->next = NULL;
}

void delete_wd_array(wd_array *wd_ar){
    if (wd_ar != NULL){
        delete_wd_array(wd_ar->next);
        free(wd_ar);
    }
}

void add_wd_array(wd_array *wd_ar, char *name, int wd){
    if (wd_ar->name != NULL){            // list != empty
    //printf("list not empty\n");
        if(wd_ar->next == NULL){            // list->next == empty
            wd_array *new_listnode;
            create_wd_array(&new_listnode);
            wd_ar->next = new_listnode;
            new_listnode->previous = wd_ar;
            new_listnode->name = name;
            new_listnode->wd = wd;
            
        }
        else{                           // list->next != empty
            add_wd_array(wd_ar->next, name, wd);
        }
    }
    else{                               // list == empty
        //printf("list empty\n");
        wd_ar->name = name;
        wd_ar->wd = wd;
    }
}

wd_array *remove_wd_array(wd_array *wd_ar, int wd){
    wd_array *new_wd_ar;

    if(wd_ar->wd == wd){           // list->name found
        if(wd_ar->previous == NULL){                     // first listnode
            printf("first node\n");
            if (wd_ar->next != NULL){                    // more than one listnodes
                printf("first node exist more\n");
                (wd_ar->next)->previous = NULL;
                new_wd_ar = wd_ar->next;
            }
            else{                                        // one last node
                new_wd_ar = NULL;
                printf("first node last one\n");
            }
        }
        else if (wd_ar->next == NULL){                   // last listnode
            (wd_ar->previous)->next = NULL;
            printf("last node\n");
        }
        else{                                           // middle listnode
            (wd_ar->previous)->next = wd_ar->next;
            (wd_ar->next)->previous = wd_ar->previous;
            printf("middle node\n");
        }
        free(wd_ar);
    }
    else{                                               // list->name not here
        if(wd_ar->next != NULL){                // list->next not empty
            printf("going for next noden\n");
            new_wd_ar = wd_ar;
            remove_wd_array(wd_ar->next, wd);  // go to next
        }
        else                                            // list->next empty
            printf("DIDN'T FOUND %d ON THE NameList\n", wd);
    }
    return new_wd_ar;
}

wd_array *find_wd_array(wd_array *wd_ar, int wd){
    //printf("looking for %ld and %ld\n", (long)il->st_ino, (long)st_ino);
    if ( wd_ar->wd == wd)
        return wd_ar;
    else{
        if(wd_ar->next != NULL){
            wd_array *found = find_wd_array(wd_ar->next, wd);
            return found;
        }
        else
            return NULL;
    } 
}

void print_wd_array(wd_array *wd_ar){
    if(wd_ar != NULL){
        if(wd_ar->name != NULL)
            printf("wd %d and name %s\n", wd_ar->wd, wd_ar->name);
        print_wd_array(wd_ar->next);
    }
    
}