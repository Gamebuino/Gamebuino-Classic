/*
 * 
 * Arduino Wrapper Function Library for Petit FatFs
 * 
 * Petit FatFs - http://elm-chan.org/fsw/ff/00index_p.html
 * by ChanN
 * 
 * Wrapper Functions by Frank Zhao
 * 
 * mmc.c origially written by ChanN, modified by Frank Zhao
 * 
 */

#ifndef PFF_h
#define PFF_h

extern "C" {

#include "integer.h"
#include "pff.h"

}

#include <Arduino.h>

#define max_path_len 32

class PFF
{
  private:
    static FATFS fatfs_obj;
    static char * dir_path;
    static int MMC_CS;
    static void MMC_SELECT(void);
    static void MMC_DESELECT(void);
	static void * stream_dest;
  public:
    static DIR dir_obj;
    PFF();
	
    int begin(int, unsigned char (*)(void), void (*)(unsigned char));
	int open_file(char *);
	int read_file(char *, int, int *);
	void setup_stream(void (*)(void), void (*)(void), char (*)(char), void (*)(void), void (*)(void));
	int stream_file(int, int *);
	int lseek_file(long);
	int open_dir(char *);
	int rewind_dir(void);
	int up_dir(void);
	int read_dir(FILINFO *);
	int open(FILINFO *);
	char * cur_dir();
};

extern PFF PFFS;

#endif

