/* array.h: head file for array.c.

   Copyright 1996, 2005, 2006, 2007, 2008, 2013, 2014 Free Software
   Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef ARRAY_H
# define ARRAY_H
# include <stdlib.h>


/* return an array with first length of length and every element's size of n */
void *array_init(int array_size, size_t array_element_size);

/* extend an array's length */
void *array_extend(void *array, int array_size, int array_size_inc, size_t array_element_size);


#endif /* !ARRAY_H */
