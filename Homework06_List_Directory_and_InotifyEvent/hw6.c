#include <errno.h>
#include <sys/types.h>
#include <assert.h>
#include <string.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <sys/inotify.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))
int level = 0;
char wd[1000][10000];
typedef struct {
    bool isDir;
    char path[32];
} File;

void printInotifyEvent(struct inotify_event *);
void print_file(File *, int, char *);
void listDir(char *);
int file_cmp(const void *, const void *);


int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "[usage]: ./hw6.out path_name\n");
        exit(-1);
    }
    listDir(argv[1]);
    int fd, num, ret, i;
    char *p = NULL;
    char inotify_entity[BUF_LEN];
    fd = inotify_init();

    ret = inotify_add_watch(fd, argv[1], IN_ALL_EVENTS);
    strcpy(wd[ret], argv[1]);
 
    while(1) {
        num = read(fd, inotify_entity, BUF_LEN);
        for (p = inotify_entity; p < inotify_entity + num; ) {
            printInotifyEvent((struct inotify_event *) p);
            p += sizeof(struct inotify_event) + ((struct inotify_event *)p)->len;
        }
    }
}

void print_file(File *file, int count, char *path) {
    // PATH_MAX is defined in linux/limits.h
    char *new_path_name = malloc(PATH_MAX);
    for (int i = 0; i < count; ++i) {
        for (int l = i; l < level; l++)
            puts(" ");
        printf("|%s\n", file[i].path);
        if (file[i].isDir) {
            sprintf(new_path_name, "%s/%s", path, file[i].path);
            listDir(new_path_name);
        }
    }
    free(new_path_name);
    new_path_name = NULL;
}

void listDir(char* pathName)
{
    level++;
    File arr[128];
    int idx = 0;
    DIR* curDir = opendir(pathName);
    assert(curDir != NULL);
    struct dirent *dir_ptr = NULL;
    /* char* newPathName = (char*)malloc(PATH_MAX);
    struct dirent entry;
    struct dirent* result;
    int ret;
    ret = readdir_r(curDir, &entry, &result);
    */ 
    
    while ((dir_ptr = readdir(curDir)) != NULL) {
        strcpy(arr[idx].path, dir_ptr->d_name);
        // is a directory
        if (dir_ptr->d_type == DT_DIR) {
            arr[idx].isDir = !(strcmp(dir_ptr->d_name, ".") == 0 || 
                               strcmp(dir_ptr->d_name, "..") == 0);
        } else {
            arr[idx].isDir = false;
        }
        // ret = readdir_r(curDir, &entry, &result);
        // assert(ret == 0);
        ++idx;
    }
    qsort(arr, idx, sizeof(File), file_cmp);
    print_file(arr, idx, pathName);
    closedir(curDir);
    level--;
}

int file_cmp(const void *ptr1, const void *ptr2) {
    return strcmp((*(File *)ptr1).path, (*(File *)ptr2).path) > 0;
}
void printInotifyEvent(struct inotify_event* event) {
    char buf[4096]="";
    sprintf(buf, "[%s] ", wd[event->wd]);
    strncat(buf, "{", 4096);
    if (event->mask & IN_ACCESS)         strncat(buf, "ACCESS, ", 4096);
    if (event->mask & IN_ATTRIB)         strncat(buf,"ATTRIB, ", 4096);
    if (event->mask & IN_CLOSE_WRITE)    strncat(buf,"CLOSE_WRITE, ", 4096);
    if (event->mask & IN_CLOSE_NOWRITE)  strncat(buf,"CLOSE_NOWRITE, ", 4096);
    if (event->mask & IN_CREATE)         strncat(buf,"CREATE, ", 4096);
    if (event->mask & IN_DELETE)         strncat(buf,"DELETE, ", 4096);
    if (event->mask & IN_DELETE_SELF)    strncat(buf,"DELETE_SELF, ", 4096);
    if (event->mask & IN_MODIFY)         strncat(buf,"MODIFY, ", 4096);
    if (event->mask & IN_MOVE_SELF)      strncat(buf,"MOVE_SELF, ", 4096);
    if (event->mask & IN_MOVED_FROM) strncat(buf,"MOVED_FROM, ", 4096);
    if (event->mask & IN_MOVED_TO)       strncat(buf,"MOVED_TO, ", 4096);
    if (event->mask & IN_OPEN)           strncat(buf,"OPEN, ", 4096);
    if (event->mask & IN_IGNORED)        strncat(buf,"IGNORED, ", 4096);
    if (event->mask & IN_ISDIR)          strncat(buf,"ISDIR, ", 4096);
    if (event->mask & IN_Q_OVERFLOW) strncat(buf,"Q_OVERFLOW, ", 4096);
    buf[strlen(buf)-2]='\0';
    strncat(buf, "}", 4096);
    sprintf(buf, "%s cookie=%d", buf, event->cookie);
    if (event->len>0)
        sprintf(buf, "%s name = %s\n", buf, event->name);
    else
        sprintf(buf, "%s name = null\n", buf);
    printf("%s", buf);
}