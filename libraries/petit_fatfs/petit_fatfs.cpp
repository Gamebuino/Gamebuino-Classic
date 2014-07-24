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

extern "C" {

#include <stdlib.h>
#include <string.h>
#include "diskio.h"

}

#include "petit_fatfs.h"

FATFS PFF::fatfs_obj; // stores working copy of FS
DIR PFF::dir_obj; // stores working copy of DIR
char * PFF::dir_path; // stores last accessed directory path
int PFF::MMC_CS; // stores pin number for MMC card's CS pin
void * PFF::stream_dest; // function pointer for stream destination function

/*
Constructor, do not actually call, it's been done for you
*/

PFF::PFF()
{
	dir_path = (char *)calloc(max_path_len, sizeof(dir_path)); // allocate memory for path
}

/*
Chip Select / Deselect Functions for the MMC Card's CS pin
*/

void PFF::MMC_SELECT()
{
	digitalWrite(MMC_CS, LOW);
}

void PFF::MMC_DESELECT()
{
	digitalWrite(MMC_CS, HIGH);
}

///////////////////////////////////
// begin list of public methods
///////////////////////////////////

//! Initialization Function
/*!
  \param cs_pin is the Arduino pin connected to the MMC card's CS pin
  \param rx is a pointer to the SPI recieve function, it must return a byte and accept no parameters
  \param tx is a pointer to the SPI transmit function, it must accept a byte as a parameter and return nothing
  \return error code, see comments in pff.h for FRESULT enumeration
	  
Notes:

The reason why the SPI functions are not built in is because some people
may wish to use software SPI, or want to share SPI functions with other other code
so this reduces redundant code.

If error is detected due to a hardware error, you can re-call this function to re-initialize the MMC card.

*/
int PFF::begin(int cs_pin, unsigned char (* rx)(void), void (* tx)(unsigned char))
{
	MMC_CS = cs_pin; // set CS pin number
	pinMode(MMC_CS, OUTPUT); // set CS pin to output
	MMC_DESELECT();
	disk_attach_spi_functs(tx, rx, MMC_SELECT, MMC_DESELECT); // attach SPI functions
	
	// start disk and mount FS, then open root directory
	disk_initialize();
	pf_mount(&fatfs_obj);
	return open_dir((char *)"/");
}

//! Opens a file with a path
/*!
  \param fn is a string containing the file name and path
  \return error code, see comments in pff.h for FRESULT enumeration
	  
Notes:
path is not relative to current directory

*/
int PFF::open_file(char * fn)
{
	return pf_open(fn);
}

//! Read last file opened into a buffer
/*!
  \param dest is a pointer to a buffer, make sure there is enough room in this buffer
  \param to_read is the maximum number of bytes you want to read into the buffer
  \param read_from is a pointer to a integer which will contain the number of bytes actually read
  \return error code, see comments in pff.h for FRESULT enumeration
	  
Notes:

use read_from to determin whether you've reached the end of the file
a file must be opened for this to work

*/
int PFF::read_file(char * dest, int to_read, int * read_from)
{
	fatfs_obj.flag &= ~FA_STREAM; // disable streaming
	return pf_read((void *)dest, to_read, (WORD *)read_from);	
}

//! Attach functions for streaming
/*!
  \param pre_block is a pointer to a function, refer to notes for when it is called
  \param pre_byte is a pointer to a function, refer to notes for when it is called
  \param dest is a pointer to a function, it must accept a byte as a parameter, it should return non-zero for streaming to continue, returning 0 will end the stream prematurely
  \param post_byte is a pointer to a function, refer to notes for when it is called
  \param post_block is a pointer to a function, refer to notes for when it is called
	  
Notes:

refer to the code below to understand calling order

pre_block();
do
{
	pre_byte();
	res = dest(SPI_RX());
	post_byte();
}
while (--cnt && res);
post_block();

These functions are meant for users who would like to stream directly from the MISO line to another device
For example, when streaming mp3 data to a VS1002d decoder, pre_byte can be used to wait until there is room in the decoder's buffer
and pre_block and post_block can be used to select and deselect the decoder's SDI bus

If not needed, these functions can be empty functions, but they must be provided.

This function must be called at least once before calling stream_file

*/
void PFF::setup_stream(void (* pre_block)(void), void (* pre_byte)(void), char (* dest)(char), void (* post_byte)(void), void (* post_block)(void))
{
	disk_attach_stream_functs(pre_byte, post_byte, pre_block, post_block); // attach functions
	stream_dest = (void *)dest;
}

//! Stream last file opened to a function
/*!
  \param to_read is the maximum number of bytes you want to read into the buffer
  \param read_from is a pointer to a integer which will contain the number of bytes actually read
  \return error code, see comments in pff.h for FRESULT enumeration
	  
Notes:

use read_from to determin whether you've reached the end of the file
you must setup the stream first
a file must be opened before

*/
int PFF::stream_file(int to_read, int * read_from)
{
	fatfs_obj.flag |= FA_STREAM; // enable streaming
	int res = pf_read(stream_dest, to_read, (WORD *)read_from); // perform read
	return res; // return error
}

//! Move file read pointer
/*!
  \param p is the desired pointer location
  \return error code, see comments in pff.h for FRESULT enumeration
  
*/
int PFF::lseek_file(long p)
{
	return pf_lseek(p);
}


//! Opens a directory as the current directory
/*!
  \param dn is a folder path string
  \return error code, see comments in pff.h for FRESULT enumeration
  
Notes:

this will rewind directory file index pointer to first file in directory

*/
int PFF::open_dir(char * dn)
{
	int res = pf_opendir(&dir_obj, dn);
	if (res == 0) // if successful
	{
		strcpy(dir_path, dn); // store path
	}
	return res; // return error if any
}


//! Reopens current directory, which rewinds the file index
/*!
  \return error code, see comments in pff.h for FRESULT enumeration
  
Notes:

this will rewind directory file index pointer to first file in directory

*/
int PFF::rewind_dir()
{
	return pf_opendir(&dir_obj, dir_path);
}

//! Opens the parent directory of the current directory, and become the current directory
/*!
  \return error code, see comments in pff.h for FRESULT enumeration
  
Notes:
this will rewind directory file index pointer to first file in directory

*/
int PFF::up_dir()
{
	int res;
	int i;
	for (i = strlen(dir_path) - 1; i != -1 && dir_path[i] != '/'; i--); // finds last slash
	if (i >= 1) // if not already in root
	{
		char * path = (char *)calloc(i + 1, sizeof(path)); // make string
		path = (char *)memcpy((void *)path, dir_path, sizeof(path) * (i + 1)); // copy up to slash
		path[i] = 0; // null terminate
		res = open_dir(path); // attempt to open
		free(path); // free string
	}
	else
	{
		res = open_dir((char *)"/"); // reopen root
	}
	return res;
}

//! Saves the FILINFO of the next file in currently open directory
/*!
  \param fnfo is the pointer to the user's FILINFO struct
  \return error code, see comments in pff.h for FRESULT enumeration

Notes:

FILINFO can be a file, a directory, or nothing
if the file name is empty, it is invalid and indicates that the entire directory has been read and you need to rewind the file index, or that the directory is empty
to check if the file name is empty, check if the first char of the FILINFO's fname is null, for example:

FILINFO fnfo;
int err = PFFS.read_dir(&fnfo);
if (fnfo.fname[0] == 0) print("end of directory");

if the FILINFO is a directory, check if the AM_DIR flag in the FILINFO's fattrib is set, for example:

FILINFO fnfo;
int err = PFFS.read_dir(&fnfo);
if (fnfo.fattrib & AM_DIR) print("is a directory"); else print("is a file");

*/
int PFF::read_dir(FILINFO * fnfo)
{
	return pf_readdir(&dir_obj, fnfo);
}

//! Opens a file using a FILINFO struct
/*!
  \param fnfo is the pointer to the user's FILINFO struct
  \return error code, see comments in pff.h for FRESULT enumeration

Notes:

the actual file path opened is the last path used in open_dir joined with the file name provided by fnfo
this means that the FILINFO must be read from the same directory or else this will fail
if fnfo is a directory, that directory will be opened and become the current directory

*/
int PFF::open(FILINFO * fnfo)
{
	int res; // stores error code
	char * fpath; // stores string for file path
	fpath = (char *)calloc(strlen(fnfo->fname) + strlen(dir_path) + 1, sizeof(fpath)); // create memory space for file path
	strcpy(fpath, dir_path); // copy dir_path into fpath so the strcat doesn't destroy dir_path
	if (fpath[strlen(fpath) - 1] != '/')
	{
		fpath = strcat(fpath, "/");	 // join path with slash character
	}
	fpath = strcat(fpath, fnfo->fname);	 // join path with file name
	if (fnfo->fattrib & AM_DIR) // is a directory
	{
		res = open_dir(fpath); // open directory
	}
	else // is a file
	{
		res = pf_open(fpath); // open the file, store error
	}
	free(fpath); // free memory for path since it's no longer needed
	return res; // return error code
}

//! Returns current directory path as a string
/*!
  \return pointer to a string containing the current directory path

*/
char * PFF::cur_dir()
{
	return dir_path;
}

//////////////////////////////////////
// end list of public methods
//////////////////////////////////////

PFF PFFS = PFF(); // create usuable instance