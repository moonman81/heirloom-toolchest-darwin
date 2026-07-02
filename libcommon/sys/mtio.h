/*
 * <sys/mtio.h> compile-time stub for Darwin (macOS).
 *
 * Darwin does not ship magnetic-tape I/O headers because Apple removed
 * tape-drive support from the kernel many releases ago. Heirloom cpio,
 * tar, tcopy, and tapecntl reference <sys/mtio.h> unconditionally, but
 * runtime behaviour on Darwin is guarded by `tapeblock > 0`, which stays
 * 0 unless a tape device is successfully opened. On Darwin no such
 * device exists, so the code falls back to lseek()/read()/write()
 * on regular streams and files — the semantics that everyone actually
 * uses today.
 *
 * This shim provides just enough symbol coverage for compilation.
 * Any ioctl(mt, MT*, ...) call at runtime returns -1/ENOTTY, and the
 * calling code already handles that failure path.
 *
 * -- Heirloom Darwin port. Kept in libcommon/sys/ so all consumers
 *    that pass -I../libcommon (ICOMMON in mk.config) pick it up
 *    automatically.
 */

#ifndef _HEIRLOOM_DARWIN_SYS_MTIO_H
#define _HEIRLOOM_DARWIN_SYS_MTIO_H

#include <sys/types.h>
#include <sys/ioctl.h>

/*
 * Tape operation codes for struct mtop.mt_op.
 * Values match the Solaris/SVR4 tradition — the on-disk cpio/tar
 * archive format never encodes these, so the exact numbers do not
 * affect legacy artefact interop; they only matter for the ioctl(2)
 * call, which returns ENOTTY on Darwin regardless.
 */
#define MTWEOF		0	/* write an end-of-file record */
#define MTFSF		1	/* forward space over EOF marks */
#define MTBSF		2	/* backward space over EOF marks */
#define MTFSR		3	/* forward space records */
#define MTBSR		4	/* backward space records */
#define MTREW		5	/* rewind */
#define MTOFFL		6	/* rewind and offline */
#define MTNOP		7	/* no-op, sets status */
#define MTRETEN		8	/* retension */
#define MTERASE		9	/* erase */
#define MTEOM		10	/* position to end of media */
#define MTNBSF		11	/* non-flushing backspace */
#define MTCACHE		12	/* enable cache */
#define MTNOCACHE	13	/* disable cache */
#define MTRESET		14	/* device reset */
#define MTSETBLK	15	/* set block size */
#define MTSETBSIZ	16	/* set physical block size */
#define MTSRSZ		17	/* set record size */
#define MTLOAD		18	/* load tape into drive */
#define MTUNLOAD	19	/* unload tape from drive */
#define MTEOD		20	/* position to end-of-data */
#define MTSETDNSTY	21	/* set density */
#define MTSETDENSITY	22	/* alt spelling */
#define MTCOMPRESSION	23	/* enable/disable data compression */
#define MTCOMP		MTCOMPRESSION
#define MTRETENS	MTRETEN	/* alt spelling used by cpio */

/*
 * Ioctl request numbers. On Darwin ioctl(2) will return -1/ENOTTY for
 * unknown requests, so pick values in a range unlikely to collide with
 * any real Darwin ioctl group (group 'm', minor 1..3).
 */
#define MTIOCTOP		_IOW('m', 1, struct mtop)
#define MTIOCGET		_IOR('m', 2, struct mtget)
#define MTIOCGETDRIVETYPE	_IOWR('m', 3, struct mtdrivetype_request)

/*
 * Argument struct for MTIOCTOP.
 */
struct mtop {
	short	mt_op;		/* operation code (MT* above) */
	int	mt_count;	/* how many */
};

/*
 * Status struct returned by MTIOCGET.
 */
struct mtget {
	short	mt_type;	/* drive type */
	unsigned short	mt_dsreg;	/* driver status register */
	unsigned short	mt_erreg;	/* device-dependent error reg */
	short	mt_resid;	/* residual count */
	int	mt_fileno;	/* file # currently at */
	int	mt_blkno;	/* block # currently at */
	unsigned int	mt_flags;	/* driver flags */
	int	mt_bf;		/* optimum blocking factor */
	long	mt_blksiz;	/* current block size */
	long	mt_density;	/* current density */
	unsigned long	mt_gstat;	/* generic status bits */
};

/*
 * Driver-type query for MTIOCGETDRIVETYPE. Solaris shipped a large
 * struct here; Heirloom only ever accesses .mtdt_name so the minimal
 * layout is sufficient.
 */
struct mtdrivetype_request {
	int	size;
	struct mtdrivetype *mtdtp;
};

struct mtdrivetype {
	char	name[64];
	char	vid[25];
	int	type;
	int	bsize;
	int	options;
	int	max_rretries;
	int	max_wretries;
	unsigned char	densities[4];
	unsigned char	default_density;
	unsigned char	speeds[4];
	unsigned short	non_motion_timeout;
	unsigned short	io_timeout;
	unsigned short	rewind_timeout;
	unsigned short	space_timeout;
	unsigned short	load_timeout;
	unsigned short	unload_timeout;
	unsigned short	erase_timeout;
};

/*
 * mt_gstat bits used by cpio/tar for status printing on non-Darwin.
 * On Darwin ioctl returns ENOTTY so these are never read from a real
 * device; keeping the bit definitions avoids "undeclared identifier"
 * warnings in status-printing branches.
 */
#define MT_ST_BLKSIZE_SHIFT	0
#define MT_ST_BLKSIZE_MASK	0xffffff
#define MT_ST_DENSITY_SHIFT	24
#define MT_ST_DENSITY_MASK	0xff000000

#endif /* _HEIRLOOM_DARWIN_SYS_MTIO_H */
