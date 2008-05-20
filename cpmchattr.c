/* #includes */ /*{{{C}}}*//*{{{*/
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"

#include "getopt.h"
#include "cpmfs.h"
/*}}}*/

const char cmd[]="cpmchattr";

int main(int argc, char *argv[]) /*{{{*/
{
  /* variables */ /*{{{*/
  const char *err;
  const char *image;
  const char *format=FORMAT;
  const char *devopts=NULL;
  int c,i,usage=0,exitcode=0;
  struct cpmSuperBlock drive;
  struct cpmInode root;
  int gargc;
  char **gargv;
  const char *attrs; 
  /*}}}*/

  /* parse options */ /*{{{*/
  while ((c=getopt(argc,argv,"T:f:h?"))!=EOF) switch(c)
  {
    case 'T': devopts=optarg; break;
    case 'f': format=optarg; break;
    case 'h':
    case '?': usage=1; break;
  }

  if (optind>=(argc-2)) usage=1;
  else 
  {
    image=argv[optind];
    attrs = argv[optind+1];
  }    

  if (usage)
  {
    fprintf(stderr,"Usage: %s [-f format] [-T dsktype] image [NMrsa1234] pattern ...\n",cmd);
    exit(1);
  }
  /*}}}*/
  /* open image */ /*{{{*/
  if ((err=Device_open(&drive.dev, image, O_RDWR, devopts)))
  {
    fprintf(stderr,"%s: can not open %s (%s)\n",cmd,image,err);
    exit(1);
  }
  cpmReadSuper(&drive,&root,format);
  /*}}}*/
  cpmglob(optind,argc,argv,&root,&gargc,&gargv);
  for (i=0; i<gargc; ++i)
  {
    struct cpmInode ino;
    int err;
    cpm_attr_t attrib;

    err = cpmNamei(&root,gargv[i], &ino)==-1;
    if (!err) err = cpmAttrGet(&ino, &attrib);
    if (!err)
    {
        int n, m;
        m = 0;
	for (n = 0; n < strlen(attrs); n++)
	{
          int mask = 0;
	  switch (attrs[n])
          {
            case 'n':
            case 'N': mask = 0; attrib = 0; break;
            case 'm':
            case 'M': mask = 0; m = !m;   break;
            case '1': mask = CPM_ATTR_F1; break;
            case '2': mask = CPM_ATTR_F2; break;
            case '3': mask = CPM_ATTR_F3; break;
            case '4': mask = CPM_ATTR_F4; break;
            case 'r': 
            case 'R': mask = CPM_ATTR_RO; break;
            case 's':
            case 'S': mask = CPM_ATTR_SYS; break;
            case 'a':
            case 'A': mask = CPM_ATTR_ARCV; break;
	    default: fprintf(stderr, "%s: Unknown attribute %c\n", cmd, attrs[n]);
                     exit(1);
          } 
          if (m) attrib &= ~mask; else attrib |= mask;
	}
        err = cpmAttrSet(&ino, attrib);
    }
    if (err)
    {
      fprintf(stderr,"%s: can not set attributes for %s: %s\n",cmd,gargv[i],boo);
      exitcode=1;
    }
  }
  exit(exitcode);
}
/*}}}*/
