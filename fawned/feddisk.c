/* ******* feddisk.c: Routines for loading and saving ******* */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "feddefs.h"
#include "fedmain.h"
#include "level.h"




/* **Variables** */
char fname[128];			/*Filename */

LEVEL_HEADER hd;


unsigned short swap_short(unsigned short D)
{
	return((D<<8)|(D>>8));
}

unsigned long swap_long(unsigned long D)
{
	return((D<<24)|((D<<8)&0x00FF0000)|((D>>8)&0x0000FF00)|(D>>24));
}


/* *** Load level *** */
int loadlevel(char *fname)
{
	int fhndl;
	int i,dx,dy;
	LEVEL_HEADER hd;

	fhndl=open(fname, O_RDONLY);
	if (fhndl<0)
	{
		fprintf(stderr,"Could not open level file!\n");
		return -1;
	}

	if (read(fhndl, &hd, sizeof(LEVEL_HEADER)) != sizeof(LEVEL_HEADER))
	{
		fprintf(stderr, "Failed to read header!\n");
		return -1;
	}

	if(hd.hmagic!=(long)0x4641574FL)  /*'FAWO'*/
	{
		hd.hmagic=swap_long(hd.hmagic);
		hd.version=swap_short(hd.version);
		hd.obj_count=swap_short(hd.obj_count);
		hd.r_wdth=swap_short(hd.r_wdth);
		hd.r_hght=swap_short(hd.r_hght);
	}

	if(hd.hmagic!=(long)0x4641574FL)
	{
		fprintf(stderr,"No level file!\n");
		close(fhndl);
		return -1;
	}

	r_width=hd.r_wdth;
	r_height=hd.r_hght;

	for(dy=0; dy<(int)r_height; dy++)
		for(dx=0; dx<(int)r_width; dx++)
		{
			if( read(fhndl, &sfield[dx][dy], sizeof(unsigned char)) != sizeof(unsigned char) )
			{
				fprintf(stderr,"Error reading level file!\n");
				close(fhndl);
				return -1;
			}
		}
	for(dy=0; dy<(int)r_height; dy++)
		for(dx=0; dx<(int)r_width; dx++)
		{
			if( read(fhndl, &ffield[dx][dy], sizeof(unsigned char)) != sizeof(unsigned char) )
			{
				fprintf(stderr,"Error reading level file!\n");
				close(fhndl);
				return -1;
			}
		}

	entry_count=hd.obj_count;

	i=0;
	while( i < entry_count )
	{
		if( read(fhndl, &entries[i], sizeof(LEVEL_ENTRY)) != sizeof(LEVEL_ENTRY) )
		{
			fprintf(stderr,"Error reading level file!\n");
			close(fhndl);
			entry_count=0;
			return -1;
		}
		i+=1;
	}

	close(fhndl);

	rwx=rwy=0;

	return 0;
}




/* *** Save level *** */
int savelevel(char *lname)
{
	int fhndl;
	int dx, dy, i;

	hd.hmagic=0x4641574FL;
	hd.version=0x0250;  /* Version 2.50 */
	hd.obj_count=entry_count;
	hd.r_wdth=r_width;
	hd.r_hght=r_height;


	fhndl=open(lname, O_WRONLY|O_CREAT,0664);
	if(fhndl<0)
	{
		fprintf(stderr,"Couldn't create levelfile!\n");
		return(fhndl);
	}

	if (write(fhndl, &hd, sizeof(hd)) != sizeof(hd))
	{
		fprintf(stderr, "Could not write level file!\n");
		return -1;
	}

	for(dy=0; dy<(int)r_height; dy++)
		for(dx=0; dx<(int)r_width; dx++)
		{
			if( write(fhndl, &sfield[dx][dy], (long)sizeof(unsigned char)) != sizeof(unsigned char) )
			{
				fprintf(stderr,"Write error!\n");
				return -1;
			}
		}
	for(dy=0; dy<(int)r_height; dy++)
		for(dx=0; dx<(int)r_width; dx++)
		{
			if( write(fhndl, &ffield[dx][dy], (long)sizeof(unsigned char)) != sizeof(unsigned char) )
			{
				fprintf(stderr,"Write error!\n");
				return -1;
			}
		}

	i = sizeof(LEVEL_ENTRY) * entry_count;
	if (write(fhndl, entries, (long)i) != i)
	{
		fprintf(stderr, "Write error!\n");
		return -1;
	}

	close(fhndl);

	return 0;
}
