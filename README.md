# f2label

Change the label on an existing F2FS filesystem (like e2label, but for F2FS).

Uses the ioctls described in [this Phoronix article](https://www.phoronix.com/scan.php?page=news_item&px=Linux-5.4-F2FS-Features) ([manpage](https://man7.org/linux/man-pages/man2/ioctl_fslabel.2.html)).  

Both the source code and a statically-linked Linux x86-64 binary are provided.

## Dependencies
- Linux v.5.4 or newer
- the `f2fs` kernel module
- `make`
- a C compiler

## Building
- `make`

## Usage

`./f2label <filepath> <new label>`

The F2FS filesystem must be mounted before running the utility.

Note that, unlike in e2label, `<filepath>` points to a file/directory, and not the block device.  

`<filepath>` can point to either the mount point, or to a file on the filesystem -- either will work.  

Be sure to run `partprobe` on the underlying block device afterward so that the kernel picks up the new label:  

`sudo partprobe /dev/sdX`
