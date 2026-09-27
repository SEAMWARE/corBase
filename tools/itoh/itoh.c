//
// FILE            itoh.c
//
// AUTHOR          Ken Zangelin
//
// Copyright (C) 2019 Ken Zangelin
//
// This file is part of kbase.
//
// kbase is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// kbase is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with kbase.  If not, see <http://www.gnu.org/licenses/>.
//
#include <stdio.h>
#include <stdlib.h>

int main(int argC, char* argV[])
{
  int ix;
  
  for (ix = 1; ix < argC; ix++)
    printf("0x%llX ", strtoull(argV[ix], NULL, 10));
  printf("\n");

  return 0;
}
