/* Copyright (C) 1997, 1998, 2001 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <windows.h>

/* Truncate the file FD refers to to LENGTH bytes.  */
int
__ftruncate64 (fd, length)
     int fd;
     __off64_t length;
{
	HANDLE hd;
	__int64 len;
	hd = (HANDLE) _get_osfhandle (fd);
	if (hd == (HANDLE) -1) {
		__set_errno (EBADF);
		return -1;
	}		
	len = _lseeki64 (fd, (__int64) length, SEEK_SET);
	if (len != (__int64) length) {
		__set_errno(EINVAL);
		return -1;
	}
	if (SetEndOfFile (hd))
		return 0;
	else {
		__set_errno (werrno);
		return -1;
	}
}

weak_alias (__ftruncate64, ftruncate64)
