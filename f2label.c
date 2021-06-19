#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define VERSION_NO "0.0.1"


static void
print_message_and_die(const char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    vfprintf(stderr, format, argptr);
    va_end(argptr);
    exit(1);
}

int
main(int argc, char* argv[])
{
    int ret;

    if (geteuid() != 0) print_message_and_die("ERROR: must be root\n");
    if (argc != 3) {
        print_message_and_die("usage: %s <f2fs filepath> <new label>\n", argv[0]);
    }

    printf("f2label v.%s\n", VERSION_NO);

    const char* const filepath = argv[1];
    const char* const new_label = argv[2];

    if (strlen(new_label) >= FSLABEL_MAX) {
        print_message_and_die("ERROR: new label is too long; max %d chars\n", FSLABEL_MAX - 1);
    }


    // determine if path is a directory or file
    struct stat filepath_stat;
    ret = stat(filepath, &filepath_stat);
    if (ret == -1) {
        print_message_and_die("ERROR: couldn't stat() filepath\n");
    }
    bool path_is_dir = S_ISDIR(filepath_stat.st_mode);


    int fd;
    if (path_is_dir) {
        DIR* dir = opendir(filepath);
        fd = dirfd(dir);
    }
    else {
        fd = open(filepath, O_RDWR);
    }

    if (fd == -1) {
        print_message_and_die("ERROR: could not open filepath (%s)\n", strerror(errno));
    }

    ret = ioctl(fd, FS_IOC_SETFSLABEL, new_label);

    if (ret == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        print_message_and_die("ERROR: ioctl() failed (%s)\n", strerror(errno));
    }

    // confirm that it actually worked
    char buf[FSLABEL_MAX];
    ret = ioctl(fd, FS_IOC_GETFSLABEL, buf);

    if (strcmp(buf, new_label)) {
        print_message_and_die("ERROR: did not actually set label\n");
    }

    fprintf(stderr, "new label is: %s\n", buf);

    // TODO call BLKRRPART (partprobe) to force kernel to re-load the partition table
    /*
    int devfd = open("/dev/sdb", O_RDONLY);
    if (devfd == -1) {
        print_message_and_die("ERROR: could not open devfd\n");
    }

    ret = ioctl(devfd, BLKRRPART);
    if (ret != 0) {
        print_message_and_die("could not reload partition table\n");
    }
    */

    fprintf(stderr, "NOTE: you may need to run 'sudo partprobe /dev/sdX', where 'sdX' is the underlying device, for the kernel to pick up the new label.\n");

    fprintf(stderr, "done.\n");

    return 0;
}
