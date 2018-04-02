/*---------------------------------------------------------------------------/
/  FatFs - FAT file system module configuration file
/---------------------------------------------------------------------------*/

#define _FFCONF 80186	/* Revision ID  版本ID*/	

/*---------------------------------------------------------------------------/
/ Function Configurations
/---------------------------------------------------------------------------*/

#define _FS_READONLY	0
/* This option switches read-only configuration. (0:Read/Write or 1:Read-only)
/  Read-only configuration removes writing API functions, f_write(), f_sync(),
/  f_unlink(), f_mkdir(), f_chmod(), f_rename(), f_truncate(), f_getfree()
/  and optional writing functions as well. */

/*
此选项切换只读配置。（0：读/写或1：只读）
只读配置删除写的API函数，f_write()，f_sync()，
f_unlink()，f_mkdir()，f_chmod()，f_rename()，f_truncate()，f_getfree()
可选的写作功能以及。
*/



#define _FS_MINIMIZE	0
/* This option defines minimization level to remove some basic API functions.
/
/   0: All basic functions are enabled.
/   1: f_stat(), f_getfree(), f_unlink(), f_mkdir(), f_truncate() and f_rename()
/      are removed.
/   2: f_opendir(), f_readdir() and f_closedir() are removed in addition to 1.
/   3: f_lseek() function is removed in addition to 2. */

/*
此选项定义了删除一些基本的函数的最小化级别。
0：所有基本功能都启用。
1：f_stat()，f_getfree()，f_unlink()，f_mkdir()，f_truncate()和f_rename()
删除/删除。
2：f_opendir()，f_readdir()和f_closedir()除了1删除。
3：f_lseek()功能除了2删除。
*/


#define	_USE_STRFUNC	1
/* This option switches string functions, f_gets(), f_putc(), f_puts() and
/  f_printf().
/
/  0: Disable string functions.
/  1: Enable without LF-CRLF conversion.
/  2: Enable with LF-CRLF conversion. */

/*
这个选项开关的字符串函数，f_gets()，f_putc()，f_puts()和f_printf()。
0：禁用字符串函数。
1：使不lf-crlf转换。
2：使lf-crlf转换。
*/


#define _USE_FIND		0
/* This option switches filtered directory read functions, f_findfirst() and
/  f_findnext(). (0:Disable, 1:Enable 2:Enable with matching altname[] too) */

/*
这个选项开关过滤目录读取功能，f_findfirst()和f_findnext()。
（0：1：2：启用禁用，使匹配altname [ ]太）
*/


#define	_USE_MKFS		1
/* This option switches f_mkfs() function. (0:Disable or 1:Enable) */

/*
这个选项开关f_mkfs(正点原子解释为格式化功能)功能。（0：禁用或1：启用）
*/



#define	_USE_FASTSEEK	1
/* This option switches fast seek function. (0:Disable or 1:Enable) */

/*
此选项切换快速搜索功能。（0：禁用或1：启用)
*/


#define	_USE_EXPAND		0
/* This option switches f_expand function. (0:Disable or 1:Enable) */

/*
这个选项开关f_expand(将一个连续的块分配到文件中)功能。（0：禁用或1：启用）
*/


#define _USE_CHMOD		0
/* This option switches attribute manipulation functions, f_chmod() and f_utime().
/  (0:Disable or 1:Enable) Also _FS_READONLY needs to be 0 to enable this option. */

/*
这个选项开关属性操作的功能，f_chmod(改变文件/目录的属性)和f_utime(改变文件/目录时间戳)。
（0：1：禁用或启用）也_fs_readonly需要0来启用此选项。
*/


#define _USE_LABEL		0
/* This option switches volume label functions, f_getlabel() and f_setlabel().
/  (0:Disable or 1:Enable) */

/*
这个选项开关卷标功能(磁盘盘符（磁盘名字）读取与设置)，f_getlabel()和f_setlabel()。
（0：禁用或1：使能）
*/


#define	_USE_FORWARD	0
/* This option switches f_forward() function. (0:Disable or 1:Enable) */

/*
这个选项开关f_forward()功能。（0：禁用或1：启用）
*/


/*---------------------------------------------------------------------------/
/ Locale and Namespace Configurations
/ 区域设置和命名空间配置
/---------------------------------------------------------------------------*/

#define _CODE_PAGE	936
/*设置语言选项,936为简体中文*/
/* This option specifies the OEM code page to be used on the target system.
/  Incorrect setting of the code page can cause a file open failure.
/
/   1   - ASCII (No extended character. Non-LFN cfg. only)
/   437 - U.S.
/   720 - Arabic
/   737 - Greek
/   771 - KBL
/   775 - Baltic
/   850 - Latin 1
/   852 - Latin 2
/   855 - Cyrillic
/   857 - Turkish
/   860 - Portuguese
/   861 - Icelandic
/   862 - Hebrew
/   863 - Canadian French
/   864 - Arabic
/   865 - Nordic
/   866 - Russian
/   869 - Greek 2
/   932 - Japanese (DBCS)
/   936 - Simplified Chinese (DBCS)
/   949 - Korean (DBCS)
/   950 - Traditional Chinese (DBCS)
*/


#define	_USE_LFN	1
#define	_MAX_LFN	255
/* The _USE_LFN switches the support of long file name (LFN).
/
/   0: Disable support of LFN. _MAX_LFN has no effect.
/   1: Enable LFN with static working buffer on the BSS. Always NOT thread-safe.
/   2: Enable LFN with dynamic working buffer on the STACK.
/   3: Enable LFN with dynamic working buffer on the HEAP.
/
/  To enable the LFN, Unicode handling functions (option/unicode.c) must be added
/  to the project. The working buffer occupies (_MAX_LFN + 1) * 2 bytes and
/  additional 608 bytes at exFAT enabled. _MAX_LFN can be in range from 12 to 255.
/  It should be set 255 to support full featured LFN operations.
/  When use stack for the working buffer, take care on stack overflow. When use heap
/  memory for the working buffer, memory management functions, ff_memalloc() and
/  ff_memfree(), must be added to the project. */

/*
该选项用于设置是否支持长文件名（还需要_CODE_PAGE 支持），取值范
围为 0~3。0，表示不支持长文件名，1~3 是支持长文件名，但是存储地方不一样
*/


#define	_LFN_UNICODE	0
/* This option switches character encoding on the API. (0:ANSI/OEM or 1:UTF-16)
/  To use Unicode string for the path name, enable LFN and set _LFN_UNICODE = 1.
/  This option also affects behavior of string I/O functions. */


#define _STRF_ENCODE	3
/* When _LFN_UNICODE == 1, this option selects the character encoding ON THE FILE to
/  be read/written via string I/O functions, f_gets(), f_putc(), f_puts and f_printf().
/
/  0: ANSI/OEM
/  1: UTF-16LE
/  2: UTF-16BE
/  3: UTF-8
/
/  This option has no effect when _LFN_UNICODE == 0. */


#define _FS_RPATH	1
/* This option configures support of relative path.
/
/   0: Disable relative path and remove related functions.
/   1: Enable relative path. f_chdir() and f_chdrive() are available.
/   2: f_getcwd() function is available in addition to 1.
*/

/*
这个选项配置支持相对路径。
0：禁用相对路径和删除相关功能。
1：使相对路径。f_chdir()和f_chdrive()可用。
2：f_getcwd()功能是除1。
*/

/*---------------------------------------------------------------------------/
/ Drive/Volume Configurations
/ 驱动/体积配置
/---------------------------------------------------------------------------*/

#define _VOLUMES	2
/* Number of volumes (logical drives) to be used. */

/*
使用的卷（逻辑驱动器）的数量。
*/

#define _STR_VOLUME_ID	0
#define _VOLUME_STRS	"RAM","NAND","CF","SD","SD2","USB","USB2","USB3"
/* _STR_VOLUME_ID switches string support of volume ID.
/  When _STR_VOLUME_ID is set to 1, also pre-defined strings can be used as drive
/  number in the path name. _VOLUME_STRS defines the drive ID strings for each
/  logical drives. Number of items must be equal to _VOLUMES. Valid characters for
/  the drive ID strings are: A-Z and 0-9. */

/* 
_str_volume_id开关串支持卷ID。
当_str_volume_id设置为1，也可以使用预定义的字符串作为驱动
路径名称中的/编号。_volume_strs定义每个硬盘ID字符串
逻辑驱动器。项目数必须等于_volumes。有效的字符
驱动器ID字符串：A-Z和0-9。
*/

#define	_MULTI_PARTITION	0
/* This option switches support of multi-partition on a physical drive.
/  By default (0), each logical drive number is bound to the same physical drive
/  number and only an FAT volume found on the physical drive will be mounted.
/  When multi-partition is enabled (1), each logical drive number can be bound to
/  arbitrary physical drive and partition listed in the VolToPart[]. Also f_fdisk()
/  funciton will be available. */

/*
此选项切换在物理驱动器上的多分区的支持。
默认情况下（0），每个逻辑驱动器号绑定到相同的物理驱动器上
将安装在物理驱动器上的一个发现的脂肪卷。
当启用多分区时（1），每个逻辑驱动器号都可以绑定到
任意物理驱动器和分区的voltopart [ ]上市。也f_fdisk()
功能将可。
*/

#define	_MIN_SS		512
#define	_MAX_SS		512
/* These options configure the range of sector size to be supported. (512, 1024,
/  2048 or 4096) Always set both 512 for most systems, all type of memory cards and
/  harddisk. But a larger value may be required for on-board flash memory and some
/  type of optical media. When _MAX_SS is larger than _MIN_SS, FatFs is configured
/  to variable sector size and GET_SECTOR_SIZE command must be implemented to the
/  disk_ioctl() function. */

/*
扇区缓冲的最大值，一般设置为 512。

这些选项配置要支持的扇区大小的范围。（512，1024，2048或4096）
对大多数系统都设置了两个512，所有类型的存储卡和硬盘。
当_max_ss大于_min_ss，FatFs配置
扇区可变大小和get_sector_size命令必须执行的
disk_ioctl()功能。
*/


#define	_USE_TRIM	0
/* This option switches support of ATA-TRIM. (0:Disable or 1:Enable)
/  To enable Trim function, also CTRL_TRIM command should be implemented to the
/  disk_ioctl() function. */


#define _FS_NOFSINFO	0
/* If you need to know correct free space on the FAT32 volume, set bit 0 of this
/  option, and f_getfree() function at first time after volume mount will force
/  a full FAT scan. Bit 1 controls the use of last allocated cluster number.
/
/  bit0=0: Use free cluster count in the FSINFO if available.
/  bit0=1: Do not trust free cluster count in the FSINFO.
/  bit1=0: Use last allocated cluster number in the FSINFO if available.
/  bit1=1: Do not trust last allocated cluster number in the FSINFO.
*/



/*---------------------------------------------------------------------------/
/ System Configurations
/ 系统配置
/---------------------------------------------------------------------------*/

#define	_FS_TINY	0
/* This option switches tiny buffer configuration. (0:Normal or 1:Tiny)
/  At the tiny configuration, size of the file object (FIL) is reduced _MAX_SS bytes.
/  Instead of private sector buffer eliminated from the file object, common sector
/  buffer in the file system object (FATFS) is used for the file data transfer. */

/*
此选项切换微小的缓冲区配置。（0：正常或1：小）
在微小的配置文件的对象的大小（FIL）降低_max_ss字节。
从文件对象，公共部门而不是私有的缓冲区缓冲区
/缓冲文件系统中的对象（系统）是用于文件数据传输。
*/


#define _FS_EXFAT	0
/* This option switches support of exFAT file system in addition to the traditional
/  FAT file system. (0:Disable or 1:Enable) To enable exFAT, also LFN must be enabled.
/  Note that enabling exFAT discards C89 compatibility. */


#define _FS_NORTC	0
#define _NORTC_MON	1
#define _NORTC_MDAY	1
#define _NORTC_YEAR	2016
/* The option _FS_NORTC switches timestamp functiton. If the system does not have
/  any RTC function or valid timestamp is not needed, set _FS_NORTC = 1 to disable
/  the timestamp function. All objects modified by FatFs will have a fixed timestamp
/  defined by _NORTC_MON, _NORTC_MDAY and _NORTC_YEAR in local time.
/  To enable timestamp function (_FS_NORTC = 0), get_fattime() function need to be
/  added to the project to get current time form real-time clock. _NORTC_MON,
/  _NORTC_MDAY and _NORTC_YEAR have no effect. 
/  These options have no effect at read-only configuration (_FS_READONLY = 1). */


#define	_FS_LOCK	0
/* The option _FS_LOCK switches file lock function to control duplicated file open
/  and illegal operation to open objects. This option must be 0 when _FS_READONLY
/  is 1.
/
/  0:  Disable file lock function. To avoid volume corruption, application program
/      should avoid illegal open, remove and rename to the open objects.
/  >0: Enable file lock function. The value defines how many files/sub-directories
/      can be opened simultaneously under file lock control. Note that the file
/      lock control is independent of re-entrancy. */


#define _FS_REENTRANT	0
#define _FS_TIMEOUT		1000
#define	_SYNC_t			HANDLE
/* The option _FS_REENTRANT switches the re-entrancy (thread safe) of the FatFs
/  module itself. Note that regardless of this option, file access to different
/  volume is always re-entrant and volume control functions, f_mount(), f_mkfs()
/  and f_fdisk() function, are always not re-entrant. Only file/directory access
/  to the same volume is under control of this function.
/
/   0: Disable re-entrancy. _FS_TIMEOUT and _SYNC_t have no effect.
/   1: Enable re-entrancy. Also user provided synchronization handlers,
/      ff_req_grant(), ff_rel_grant(), ff_del_syncobj() and ff_cre_syncobj()
/      function, must be added to the project. Samples are available in
/      option/syscall.c.
/
/  The _FS_TIMEOUT defines timeout period in unit of time tick.
/  The _SYNC_t defines O/S dependent sync object type. e.g. HANDLE, ID, OS_EVENT*,
/  SemaphoreHandle_t and etc.. A header file for O/S definitions needs to be
/  included somewhere in the scope of ff.c. */


/*--- End of configuration options ---*/
