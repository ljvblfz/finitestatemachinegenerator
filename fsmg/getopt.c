/* Copyright (C) 1999 Free Software Foundation, Inc.
   Author : philippe@prados-obj.nom.fr <Ph. Prados>

This file is part of fsmg.

Fsmg is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

Fsmg is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Fsmg; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  
*/

#include <errno.h>
#include <string.h>
#include <stdio.h>

int     optind  = 1;    /* index of which argument is next      */
char*   optarg;         /* pointer to argument of current option */
int     opterr  = 1;    /* allow error message  */

static  char*   letP = NULL;    /* remember next option char's location */
static  char    SW = 0;         /* DOS switch character, either '-' or '/' */

/*
  Parse the command line options, System V style.

  Standard option syntax is:

    option ::= SW [optLetter]* [argLetter space* argument]

  where
    - SW is either '/' or '-', according to the current setting
      of the MSDOS switchar (int 21h function 37h).
    - there is no space before any optLetter or argLetter.
    - opt/arg letters are alphabetic, not punctuation characters.
    - optLetters, if present, must be matched in optionS.
    - argLetters, if present, are found in optionS followed by ':'.
    - argument is any white-space delimited string.  Note that it
      can include the SW character.
    - upper and lower case letters are distinct.

  There may be multiple option clusters on a command line, each
  beginning with a SW, but all must appear before any non-option
  arguments (arguments not introduced by SW).  Opt/arg letters may
  be repeated: it is up to the caller to decide if that is an error.

  The character SW appearing alone as the last argument is an error.
  The lead-in sequence SWSW ("--" or "//") causes itself and all the
  rest of the line to be ignored (allowing non-options which begin
  with the switch char).

  The string *optionS allows valid opt/arg letters to be recognized.
  argLetters are followed with ':'.  Getopt () returns the value of
  the option character found, or EOF if no more options are in the
  command line.  If option is an argLetter then the global optarg is
  set to point to the argument string (having skipped any white-space).

  The global optind is initially 1 and is always left as the index
  of the next argument of argv[] which getopt has not taken.  Note
  that if "--" or "//" are used then optind is stepped to the next
  argument before getopt() returns EOF.

  If an error occurs, that is an SW char precedes an unknown letter,
  then getopt() will return a '?' character and normally prints an
  error message via perror().  If the global variable opterr is set
  to false (zero) before calling getopt() then the error message is
  not printed.

  For example, if the MSDOS switch char is '/' (the MSDOS norm) and

    *optionS == "A:F:PuU:wXZ:#"

  then 'P', 'u', 'w', and 'X' are option letters and 'F', 'U', 'Z'
  are followed by arguments.  A valid command line may be:

    aCommand  /uPFPi /X /A L /3 someFile

  where:
    - 'u' and 'P' will be returned as isolated option letters.
    - 'F' will return with "Pi" as its argument string.
    - 'X' is an isolated option.
    - 'A' will return with "L" as its argument.
    - "someFile" is not an option, and terminates getOpt.  The
      caller may collect remaining arguments using argv pointers.
*/

/**********/
/* getopt */
/**********/

int getopt(int argc, char* argv[], char* optionS)
{ 
	unsigned char ch;
	char*         optP;

	if (SW == 0)
	{
		#ifdef __MSDOS__
		SW='/';
		#endif
		#ifdef __UNIX__
		SW='-';
		#endif
	}

	if (argc > optind)
	{ 
		if (letP == NULL)
		{ 
			if ((letP = argv[optind]) == NULL
				|| ((*letP != SW)
			    && (*letP != '-'))) goto gopEOF;
				++letP;
		}
    
		if (0 == (ch = *letP))
		{ 
			goto gopEOF;
		}
		if ((strchr(optionS,'#')) != NULL
			&& (strspn(letP,"0123456789-+")==strlen(letP)))
		{ 
			optarg=letP;
			letP=NULL;
			optind++;
			return('#');
		}
		letP++;
		if (':' == ch || '#' == ch || (optP = strchr(optionS, ch)) == NULL)
		{ 
			goto gopError; 
		}
		if (':' == *(++optP))
		{ 
			optind++;
			if (0 == *letP)
			{ 
				if (argc <= optind)  goto  gopError;
				letP = argv[optind++];
			}
			optarg = letP;
			letP = NULL;
		}
		else
		{ 
			if (0 == *letP)
			{ 
				++optind;
				letP = NULL;
			}
			optarg = NULL;
		}
		return(ch);
	}
gopEOF:
	optarg = letP = NULL;
	return(EOF);

gopError:
	optarg = NULL;
	errno  = EINVAL;
	return('\0');
}
