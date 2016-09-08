/*
** yp_misc.h
**
** Copyright (c) 1993 Signum Support AB, Sweden
**
** This file is part of the NYS Library.
**
** The NYS Library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public License as
** published by the Free Software Foundation; either version 2 of the
** License, or (at your option) any later version.
**
** The NYS Library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
** 
** You should have received a copy of the GNU Library General Public
** License along with the NYS Library; see the file COPYING.LIB.  If
** not, write to the Free Software Foundation, Inc., 675 Mass Ave,
** Cambridge, MA 02139, USA.
**
** Authors: Peter Eriksson <pen@signum.se>
**          Michael A. Griffith <grif@cs.ucr.edu>
*/

#ifndef __YPLIB_MISC_H__
#define __YPLIB_MISC_H__

extern char *_yp_xcopy(char **cp, char *str);
extern char *_yp_xstrtok(char *cp, int delim);
extern char *_yp_strip_names(char *buff);
extern char *_yp_printaddr(unsigned long addr);
extern unsigned long _yp_readaddr(char *buff);
extern char *_ypopts_getmd(char *defmap, char *suffix, char **domain);
			   
#endif



