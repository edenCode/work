#ifndef	_OSERROR_H_
#define	_OSERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Basic Code */
#define ERR_OK            0  /* OK */
#define ERR_NOERROR       0  /* No error */
#define ERR_SUCCESS       0  /* Sucess */
#define	ERR_PERM	      1	/* Operation not permitted */
#define	ERR_NOENT		  2	/* No such file or directory */
#define	ERR_SRCH	      3	/* No such process */
#define	ERR_INTR	      4	/* Interrupted system call */
#define	ERR_IO            5 /* I/O error */
#define	ERR_NXIO	      6	/* No such device or address */
#define	ERR_2BIG	      7	/* Argument list too long */
#define	ERR_NOEXEC		  8	/* Exec format error */
#define	ERR_BADF	      9	/* Bad file number */
#define	ERR_CHILD		 10	/* No child processes */
#define	ERR_AGAIN		 11	/* Try again */
#define	ERR_NOMEM		 12	/* Out of memory */
#define	ERR_ACCES		 13	/* Permission denied */
#define	ERR_FAULT		 14	/* Bad address */
#define	ERR_NOTBLK		 15	/* Block device required */
#define	ERR_BUSY	     16	/* Device or resource busy */
#define	ERR_EXIST		 17	/* File exists */
#define	ERR_XDEV	     18	/* Cross-device link */
#define	ERR_NODEV		 19	/* No such device */
#define	ERR_NOTDIR		 20	/* Not a directory */
#define	ERR_ISDIR		 21	/* Is a directory */
#define	ERR_INVAL		 22	/* Invalid argument */
#define	ERR_NFILE		 23	/* File table overflow */
#define	ERR_MFILE		 24	/* Too many open files */
#define	ERR_NOTTY		 25	/* Not a typewriter */
#define	ERR_TXTBSY		 26	/* Text file busy */
#define	ERR_FBIG	     27	/* File too large */
#define	ERR_NOSPC		 28	/* No space left on device */
#define	ERR_SPIPE		 29	/* Illegal seek */
#define	ERR_ROFS	     30	/* Read-only file system */
#define	ERR_MLINK		 31	/* Too many links */
#define	ERR_PIPE	     32	/* Broken pipe */
#define	ERR_DOM		     33	/* Math argument out of domain of func */
#define	ERR_RANGE		 34	/* Math result not representable */
#define	ERR_DEADLK		 35	/* Resource deadlock would occur */
#define	ERR_NAMETOOLONG  36	/* File name too long */
#define	ERR_NOLCK		 37	/* No record locks available */
#define	ERR_NOSYS		 38	/* Function not implemented */
#define	ERR_NOTEMPTY     39	/* Directory not empty */
#define	ERR_LOOP	     40	/* Too many symbolic links encountered */
#define	ERR_NOMSG		 42	/* No message of desired type */
#define	ERR_IDRM	     43	/* Identifier removed */
#define	ERR_CHRNG		 44	/* Channel number out of range */
#define	ERR_L2NSYNC	     45	/* Level 2 not synchronized */
#define	ERR_L3HLT		 46	/* Level 3 halted */
#define	ERR_L3RST		 47	/* Level 3 reset */
#define	ERR_LNRNG		 48	/* Link number out of range */
#define	ERR_UNATCH		 49	/* Protocol driver not attached */
#define	ERR_NOCSI		 50	/* No CSI structure available */
#define	ERR_L2HLT		 51	/* Level 2 halted */
#define	ERR_BADE	     52	/* Invalid exchange */
#define	ERR_BADR	     53	/* Invalid request descriptor */
#define	ERR_XFULL		 54	/* Exchange full */
#define	ERR_NOANO		 55	/* No anode */
#define	ERR_BADRQC		 56	/* Invalid request code */
#define	ERR_BADSLT		 57	/* Invalid slot */
#define	ERR_BFONT		 59	/* Bad font file format */
#define	ERR_NOSTR		 60	/* Device not a stream */
#define	ERR_NODATA		 61	/* No data available */
#define	ERR_TIME	     62 /* Timer expired */
#define	ERR_NOSR	     63 /* Out of streams resources */
#define	ERR_NONET		 64	/* Machine is not on the network */
#define	ERR_NOPKG		 65	/* Package not installed */
#define	ERR_REMOTME		 66	/* Object is remote */
#define	ERR_NOLINK		 67	/* Link has been severed */
#define	ERR_ADV		     68 /* Advertise error */
#define	ERR_SRMNT		 69	/* Srmount error */
#define	ERR_COMM	     70 /* Communication error on send */
#define	ERR_PROTO		 71	/* Protocol error */
#define	ERR_MULTIHOP     72 /* Multihop attempted */
#define	ERR_DOTDOT		 73	/* RFS specific error */
#define	ERR_BADMSG		 74	/* Not a data message */
#define	ERR_OVERFLOW     75 /* Value too large for defined data type */
#define	ERR_NOTUNIQ	     76 /* Name not unique on network */
#define	ERR_BADFD		 77	/* File descriptor in bad state */
#define	ERR_REMCHG		 78	/* Remote address changed */
#define	ERR_LIBACC		 79	/* Can not access a needed shared library */
#define	ERR_LIBBAD		 80	/* Accessing a corrupted shared library */
#define	ERR_LIBSCN		 81	/* .lib section in a.out corrupted */
#define	ERR_LIBMAX		 82	/* Attempting to link in too many shared libraries */
#define	ERR_LIBEXEC	     83 /* Cannot exec a shared library directly */
#define	ERR_ILSEQ		 84 /* Illegal byte sequence */
#define	ERR_RESTART	     85 /* Interrupted system call should be restarted */
#define	ERR_STRPIPE	     86 /* Streams pipe error */
#define	ERR_USERS		 87 /* Too many users */
#define	ERR_NOTSOCK      88 /* Socket operation on non-socket */
#define	ERR_DESTADDRREQ  89 /* Destination address required */
#define	ERR_MSGSIZE      90 /* Message too long */
#define	ERR_PROTOTYPE	 91	/* Protocol wrong type for socket */
#define	ERR_NOPROTOOPT	 92	/* Protocol not available */
#define	ERR_PROTONOSUPPORT	93	/* Protocol not supported */
#define	ERR_SOCKTNOSUPPORT	94	/* Socket type not supported */
#define	ERR_OPNOTSUPP	 95	/* Operation not supported on transport endpoint */
#define	ERR_PFNOSUPPORT	 96	/* Protocol family not supported */
#define	ERR_AFNOSUPPORT	 97	/* Address family not supported by protocol */
#define	ERR_ADDRINUSE	 98	/* Address already in use */
#define	ERR_ADDRNOTAVAIL 99	/* Cannot assign requested address */
#define	ERR_NETDOWN	    100	/* Network is down */
#define	ERR_NETUNREACH	101	/* Network is unreachable */
#define	ERR_NETRESET    102	/* Network dropped connection because of reset */
#define	ERR_CONNABORTED	103	/* Software caused connection abort */
#define	ERR_CONNRESET	104	/* Connection reset by peer */
#define	ERR_NOBUFS		105	/* No buffer space available */
#define	ERR_ISCONN		106	/* Transport endpoint is already connected */
#define	ERR_NOTCONN	    107	/* Transport endpoint is not connected */
#define	ERR_SHUTDOWN    108	/* Cannot send after transport endpoint shutdown */
#define	ERR_TOOMANYREFS	109	/* Too many references: cannot splice */
#define	ERR_TIMEDOUT    110	/* Connection timed out */
#define	ERR_CONNREFUSED	111	/* Connection refused */
#define	ERR_HOSTDOWN    112	/* Host is down */
#define	ERR_HOSTUNREACH	113	/* No route to host */
#define	ERR_ALREADY	    114	/* Operation already in progress */
#define	ERR_INPROGRESS	115	/* Operation now in progress */
#define	ERR_STALE		116	/* Stale NFS file handle */
#define	ERR_UCLEAN		117	/* Structure needs cleaning */
#define	ERR_NOTNAM		118	/* Not a XENIX named type file */
#define	ERR_NAVAIL		119	/* No XENIX semaphores available */
#define	ERR_ISNAM		120	/* Is a named type file */
#define	ERR_REMOTEIO    121	/* Remote I/O error */
#define	ERR_DQUOT		122	/* Quota exceeded */
#define	ERR_NOMEDIUM    123	/* No medium found */
#define	ERR_MEDIUMTYPE	124	/* Wrong medium type */
#define	ERR_CANCELED    125	/* Operation Canceled */
#define	ERR_NOKEY		126	/* Required key not available */
#define	ERR_KEYEXPIRED	127	/* Key has expired */
#define	ERR_KEYREVOKED	128	/* Key has been revoked */
#define	ERR_KEYREJECTED	129	/* Key was rejected by service */

#define	ERR_CHKSUM		256	/* Checksum error */

/* Error Micros */
#define ERR_MK(id, no) (((id) << 16) | (no))
#define ERR_ID(no)     (((no) >> 16) & 0xFFFF)
#define ERR_NO(no)     ((no) & 0xFFFF)

/* function declarations */

int  OSError_Get( void );
void OSError_Set( int err );

#ifdef __cplusplus
}
#endif

#endif

/* End of file */
