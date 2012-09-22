/*  **************************************************************************
 * 
 * --- File: abc2prt.c
 * 
 * --- Purpose: this program extract parts from an ABC file.
 * 
 * --- Copyright (C) Guido Gonzato <ggonza@tin.it>
 *     Multivoice extensions by Matthew K. Junker <junker@alum.mit.edu>
 * 
 * --- Last updated: 3 April 2003
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * 
 * ************************************************************************ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define PROGNAME       "abc2prt"
#define DATE           "3 April 2003"
#define VERSION        "1.0.2"
#define LINE_LENGHT    1024  /* max. length of input line */
#define STR_LENGHT     200
#define TRUE           1
#define FALSE          0
#define MAXV           5

void usage (void);

/* ----- */

int main (int argc, char *argv[])
{
  FILE *in, *out;
  char line [LINE_LENGHT];
  char *tmp, *tmp2, *tmp3;
  int i = 1, optind = 0;
  int key_found = FALSE;
  int output_line = TRUE;
  char voice_name [STR_LENGHT]; /* name of part to extract */
  char voicenames[MAXV][STR_LENGHT];
  int voices = 0;
  int found;

  if ( (NULL == (tmp = malloc (STR_LENGHT))) ||
       (NULL == (tmp2 = malloc (STR_LENGHT))) ||
       (NULL == (tmp3 = malloc (STR_LENGHT))) ) {
    fprintf (stderr, "%s: couldn't allocate memory\n", PROGNAME);
    exit (1);
  }
  
  /* parse arguments */

  if (argc > 1)
    while ('-' == argv [i][0]) {
      switch (argv[i][1]) {
      case 'h':
        usage ();
        exit (0);
	break;
      case 'v':
	if (voices < MAXV) {
	  strcpy (voicenames[voices++], &argv[i][2]);
	} else {
	  usage ();
	  exit (1);
	  break;
	}
	break;
      }
      optind++;
      i++;
      if (i == argc)
        break;
    }

  in = stdin;
  out = stdout;
  
  switch (argc - optind - 1) {
    
   case 0: /* use stdin and stdout */
    ;
    break;
    
   case 1: /* use file + stdout */
    strcpy (tmp2, argv [++optind]);
    if (NULL == (in = fopen (tmp2, "r"))) {
      sprintf (tmp, "%s: can't open %s\n", PROGNAME, tmp2);
      exit (1);
    }
    break;
    
   case 2: /* use files */
    
    strcpy (tmp2, argv [++optind]);
    if (NULL == (in = fopen (tmp2, "r"))) {
      sprintf (tmp, "%s: can't open %s\n", PROGNAME, tmp2);
      exit (1);
    }
    strcpy (tmp3, argv [++optind]);
    /* check if input and output are the same file */
    if (!strcmp (tmp2, tmp3)) {
      sprintf (tmp, "%s and %s cannot be the same file.", tmp2, tmp3);
      exit (1);
    }
    if (NULL == (out = fopen (tmp3, "w"))) {
      sprintf (tmp, "%s: can't open %s\n", PROGNAME, tmp3);
      exit (1);
    }
    break;
    
   default:
    usage ();
    exit (1);
    
  } /* switch */
  
  do {
  
    key_found = FALSE;
    
    /* skip the header and empty lines */
    
    do {
      
      if (NULL == fgets (line, LINE_LENGHT, in))
        break;
      output_line = TRUE;
      
      /* toss out %%staves definitions */
      if (NULL != strstr (line, "%%staves"))
        output_line = FALSE;
      
      /* toss out other voice definitions */
      tmp2 = strstr (line, "V:");
      if (NULL != tmp2) { /* found, search the voice name */
        tmp2 += 2; /* skip V: */
        while ( isspace (*tmp2) )
          tmp2++; /* skip spaces */
	for (found=0, i=0; i < voices && !found; i++) {
	  found = !strncmp (tmp2, voicenames[i], strlen(voicenames[i]));
	}
        if ( !found )
          output_line = FALSE;
        else
          output_line = TRUE;
      }
      
      if (TRUE == output_line)
        fputs (line, out);
      
      if (NULL != strstr (line, "K:"))
        key_found = TRUE;
      
    } while (FALSE == key_found);
    
    /* output only the requested voice until the end of the file
     * or blank lines are found */
    
    do {
      if (NULL == fgets (line, LINE_LENGHT, in))
        break;
      
      if (1 == strlen (line)) /* empty line */
        output_line = TRUE;
        
      /* output all lines unless a V: field is found that
       * isn't what the user specified */
      
      tmp2 = strstr (line, "V:");
      if (NULL != tmp2) { /* found, search the voice name */
        tmp2 += 2; /* skip V: */
        while ( isspace (*tmp2) )
          tmp2++; /* skip spaces */
	for (found=0, i=0; i < voices && !found; i++) {
	  found = !strncmp (tmp2, voicenames[i], strlen(voicenames[i]));
	}
        if ( !found )
          output_line = FALSE;
        else
          output_line = TRUE;
      }
      if (output_line)
        fputs (line, out);
    } while (strlen (line) > 1);
    
  } while (!feof (in));
  
  
  fclose (in);
  fclose (out);
  exit (0);
}

/* ----- */

void usage ()
{
  fprintf (stderr, "%s %s, %s\n", PROGNAME, VERSION, DATE);
  fprintf (stderr, "Copyright 2001, 2003 Guido Gonzato <ggonza@tin.it>\n");
  fprintf (stderr, "Multivoice extensions by Matthew K. Junker <junker@alum.mit.edu>\n\n");
  fprintf (stderr, "This is free software with ABSOLUTELY NO WARRANTY.\n\n");
  fprintf (stderr, 
           "Usage: %s [-h] [-v<voicename>] [inputfile] [outputfile]\n\n",
           PROGNAME);
  fprintf (stderr, "-h:\t\tshow usage\n");
  fprintf (stderr, "-v<voicename>:\toutput voice <voicename>\n");
  fprintf (stderr, "\t\tAt most %d -v fields are allowed.\n", MAXV );
  exit (0);
} /* void usage () */

/* ----- */

/* --- End of file abc2prt --- */
