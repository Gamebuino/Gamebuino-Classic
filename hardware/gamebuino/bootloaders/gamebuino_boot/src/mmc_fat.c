/**********************************************************/
/* mmc_fat.c                                              */
/* Copyright (c) 2010 by thomas seiler                    */ 
/* read a file from a FAT16 formatted MMC card            */
/* Code taken from HolgerBootloader (public domain)       */
/* from mikrokontroller.net and adapted for smaller size  */
/*                                                        */
/* -------------------------------------------------------*/
/*                                                        */
/* This program is free software; you can redistribute it */
/* and/or modify it under the terms of the GNU General    */
/* Public License as published by the Free Software       */
/* Foundation; either version 2 of the License, or        */
/* (at your option) any later version.                    */
/*                                                        */
/* This program is distributed in the hope that it will   */
/* be useful, but WITHOUT ANY WARRANTY; without even the  */
/* implied warranty of MERCHANTABILITY or FITNESS FOR A   */
/* PARTICULAR PURPOSE.  See the GNU General Public        */
/* License for more details.                              */
/*                                                        */
/* You should have received a copy of the GNU General     */
/* Public License along with this program; if not, write  */
/* to the Free Software Foundation, Inc.,                 */
/* 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA */
/*                                                        */
/* Licence can be viewed at                               */
/* http://www.fsf.org/licenses/gpl.txt                    */
/**********************************************************/

#include <avr/io.h>
#include <inttypes.h>

#if !defined(__AVR_ATmega168__) || !defined(__AVR_ATmega328P__)
#include <avr/eeprom.h>  /* filename from eeprom */
#endif

//#include "board.h"
#include "mmc_fat.h"
#include "prog_flash.h"


#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#define MMC_CMD0_RETRY	(unsigned char)16

#ifdef MMC_CS

static unsigned char cmd[6];

/* ---[ SPI Interface ]---------------------------------------------- */

static void spi_send_byte(unsigned char data)
{
	SPDR=data;
	loop_until_bit_is_set(SPSR, SPIF); // wait for byte transmitted...
}

static unsigned char send_cmd(void)
{
	unsigned char i;
	unsigned char *buf;
	
	spi_send_byte(0xFF);      //Dummy delay 8 clocks
	MMC_PORT &= ~(1<<MMC_CS); //MMC Chip Select -> Low (activate mmc)

	/* send the 6 cmd bytes */
	i=6;
	buf = cmd;
	while(i) {
		spi_send_byte(*buf++);
		i--;
	}

	unsigned char result;
	
	/* wait for response */
	for(i=0; i<255; i++) {
	
 		spi_send_byte(0xFF);
		result = SPDR;
		
		if ((result & 0x80) == 0)
			break;
	}

	return(result); // TimeOut !?
}

/* ---[ MMC Interface ]---------------------------------------------- */

//all MMC Commandos needed for reading a file from the card
#define MMC_GO_IDLE_STATE 0
#define MMC_SEND_OP_COND 1
#define MMC_READ_SINGLE_BLOCK 17

/* the sector buffer */
uint8_t buff[512];


/*			
*		Call mmc_init one time after a card has been connected to the �C's SPI bus!
*	
*		return values:
*			MMC_OK:				MMC initialized successfully
*			MMC_INIT:			Error while trying to reset MMC
*			MMC_TIMEOUT:	Error/Timeout while trying to initialize MMC
*/
static inline unsigned char mmc_init(void)
{
	// the default after reset is already input
	//SPI_DDR &= ~(1<<SPI_MISO);	//SPI Data Out -> Input
	SPI_PORT |= 1<<SPI_SS;   //PB2 output: High (deselect other SPI chips)

	SPI_DDR  |= 1<<SPI_CLK | 1<<SPI_MOSI | 1<<SPI_SS; // SPI Data -> Output
	MMC_DDR |= 1<<MMC_CS; 	//MMC Chip Select -> Output
	
	// Enable SPI, Master, clock rate f_osc/128
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);
	// clear double speed
	SPSR &= ~(1 << SPI2X);
	
	unsigned char i;
	
	i=10;
	while(i) { //Pulse 80+ clocks to reset MMC
		spi_send_byte(0xFF);	
 		i--;
	}

	unsigned char res;

	cmd[0] = 0x40 + MMC_GO_IDLE_STATE;
	cmd[1] = 0x00; cmd[2] = 0x00; cmd[3] = 0x00; cmd[4] = 0x00; cmd[5] = 0x95;
	
	for (i=0; i<MMC_CMD0_RETRY; i++)
	{
		res=send_cmd(); //store result of reset command, should be 0x01

		MMC_PORT |= 1<<MMC_CS; //MMC Chip Select -> High (deactivate mmc);
      	 	spi_send_byte(0xFF);
		if (res == 0x01)
			break;
	}

	if(i==MMC_CMD0_RETRY) return(MMC_TIMEOUT);

	if (res != 0x01) //Response R1 from MMC (0x01: IDLE, The card is in idle state and running the initializing process.)
		return(MMC_INIT);
	
	cmd[0]=0x40 + MMC_SEND_OP_COND;
		
//May be this becomes an endless loop ?
//Counting i from 0 to 255 and then timeout
//was to SHORT for some of my cards !
	while(send_cmd() != 0) {
		MMC_PORT |= 1<<MMC_CS; //MMC Chip Select -> High (deactivate);
		spi_send_byte(0xFF);
	}
	
	return(MMC_OK);
}

static inline unsigned char wait_start_byte(void)
{
	unsigned char i;
	
	i=255;
	do {
		spi_send_byte(0xFF);
		if(SPDR == 0xFE) return MMC_OK;
	} while(--i);
	
	return MMC_NOSTARTBYTE;
}

/*
 *		mmc_start_read_sector initializes the reading of a sector
 *
 *		Parameters:
 *			adr: specifies address to be read from
 *
 *		Return values:
 *			MMC_OK:						Command successful
 *			MMC_CMDERROR:			Error while sending read command to mmc
 *			MMC_NOSTARTBYTE:	No start byte received
 */
static unsigned char mmc_start_read_block(unsigned long adr)
{
	adr <<= 1;
	
	cmd[0] = 0x40 + MMC_READ_SINGLE_BLOCK;
	cmd[1] = (adr & 0x00FF0000) >> 0x10;
	cmd[2] = (adr & 0x0000FF00) >> 0x08;
	cmd[3] = (adr & 0x000000FF);
	cmd[4] = 0;
	
	// There is something odd about how this behaves, so I'll disable it - Jonne
	// testing proves theory correct
	//SPCR = 1<<SPE | 1<<MSTR | SPI_READ_CLOCK; //SPI Enable, SPI Master Mode
	
	if (send_cmd() != 0x00 || wait_start_byte()) {
		MMC_PORT |= 1<<MMC_CS; //MMC Chip Select -> High (deactivate mmc);
		return(MMC_CMDERROR); //wrong response!
	}
	
	//mmc_read_buffer
	unsigned char *buf;
	unsigned short len;
 
	buf = buff;
	len= 512;
	
	while (len) {
		spi_send_byte(0xFF);
		*buf++ = SPDR;
		len--;
	}
	
	//mmc_stop_read_block
	//read 2 bytes CRC (not used);
	spi_send_byte(0xFF);
	spi_send_byte(0xFF);
	MMC_PORT |= 1<<MMC_CS; //MMC Chip Select -> High (deactivate mmc);
	return(MMC_OK);
}

/* ---[ FAT16 ]------------------------------------------------------ */

static uint16_t  RootDirRegionStartSec;
static uint32_t  DataRegionStartSec;
static uint16_t  RootDirRegionSize;
static uint8_t   SectorsPerCluster;
static uint16_t  FATRegionStartSec;

static inline unsigned char fat16_init(void)
{
	mbr_t *mbr = (mbr_t*) buff;
	vbr_t *vbr = (vbr_t*) buff;
		
	if (mmc_init() != MMC_OK)
			return 1;
	
    mmc_start_read_block(0);

    // Try sector 0 as a bootsector
	if ((vbr->bsFileSysType[0] == 'F') && (vbr->bsFileSysType[4] == '6'))
	{
		FATRegionStartSec = 0;
	}
	else // Try sector 0 as a MBR	
	{     	 
		FATRegionStartSec = mbr->sector.partition[0].sectorOffset;
          
		mmc_start_read_block(mbr->sector.partition[0].sectorOffset);
	  
        if ((vbr->bsFileSysType[0] != 'F') || (vbr->bsFileSysType[4] != '6'))
			return 2; // No FAT16 found
     }
    
	SectorsPerCluster  			= vbr->bsSecPerClus; // 4
	
	// Calculation Algorithms
	FATRegionStartSec			+= vbr->bsRsvdSecCnt;						// 6
	RootDirRegionStartSec	 	= FATRegionStartSec + (vbr->bsNumFATs * vbr->bsNrSeProFAT16);		// 496	
	RootDirRegionSize		 	= (vbr->bsRootEntCnt / 16); 						// 32
	DataRegionStartSec 			= RootDirRegionStartSec + RootDirRegionSize;	// 528
	
	return 0;
}

static struct _file_s {
	uint16_t startcluster;
 	uint16_t sector_counter;
 	uint32_t size;
 	uint8_t* next;
} file;

static inline uint8_t fat16_readRootDirEntry(uint16_t entry_num) {
	uint8_t direntry_in_sector;
 	direntry_t *dir;
		
	/* Check for end of root dir region reached! */
	if ((entry_num / 16) >= RootDirRegionSize)
		return 0;

	/* this finds the sector in which the entry will be saved */	
	uint32_t dirsector = RootDirRegionStartSec + entry_num / 16;

	/* this is the offset inside the sector */
	/* there are 16 entries in a sector, each 32 bytes long */
    direntry_in_sector = (unsigned char) entry_num % 16;

	/* get the sector into the buffer */
	mmc_start_read_block(dirsector);
	
	/* pointer to the direntry inside the buffer */
	dir = (direntry_t *) buff + direntry_in_sector;

	if ((dir->name[0] == 0) || (dir->name[0] == 0xE5) || (dir->fstclust == 0))
		return 0;

	/* fill in the file structure */
	file.startcluster = dir->fstclust;
	file.size = dir->filesize;
	file.sector_counter = 0;
	file.next = buff + 512;

	/* compare name */
	uint8_t i = 0;
	uint8_t match = 1;
	for (i = 0; FILENAME_LOCATION[i]; i++) { 
	  match &= (FILENAME_LOCATION[i] == dir->name[i]);
	}
	if (!(match && i)) return 0;
	
	/* match ending, seach for HEX => return 1, or EEP => return 2*/
	if (dir->name[9] != 'E') return 0;
	if (dir->name[8] == 'H' && dir->name[10] == 'X') return 1;
	if (dir->name[8] == 'E' && dir->name[10] == 'P') return 2;
	return 0;
}

static void inline fat16_readfilesector()
{
	uint16_t clusteroffset;
	uint8_t currentfatsector;
	uint8_t temp, secoffset;
	uint32_t templong;
	uint16_t cluster = file.startcluster;
	
	fatsector_t *fatsector = (fatsector_t*) buff;

	/* SectorsPerCluster is always power of 2 ! */
	secoffset = (uint8_t)file.sector_counter & (SectorsPerCluster-1);
	
	clusteroffset = file.sector_counter;
	temp = SectorsPerCluster >> 1;
	while(temp) {
		clusteroffset >>= 1;
        temp >>= 1;
    }

	currentfatsector = 0xFF;
	while (clusteroffset)
	{
		temp = (unsigned char)((cluster & 0xFF00) >>8);
          
		if (currentfatsector != temp)
		{
			mmc_start_read_block(FATRegionStartSec + temp);

			currentfatsector = temp;
		}
		
		cluster = fatsector->fat_entry[cluster % 256];
		clusteroffset--;
	}

	templong = cluster - 2;
	temp = SectorsPerCluster>>1;
	while(temp) {
		templong <<= 1;	
		temp >>= 1;
	}
		
	/* read the sector of the file into the buffer */
	mmc_start_read_block(templong + DataRegionStartSec + secoffset);
	
	/* advance to next sector */
	file.sector_counter++;
}

/* ----[ file ]--------------------------------------------------- */

static uint8_t file_read_byte() {	// read a byte from the open file from the mmc...
	if (file.next >= buff + 512) {
	    fat16_readfilesector();
		file.next = file.next - 512;
	}
	file.size--;
	return *file.next++;
}

static char gethexnib(char a) {
	if(a >= 'a') {
		return (a - 'a' + 0x0a);
	} else if(a >= 'A') {
		return (a - 'A' + 0x0a);
	} else if(a >= '0') {
		return(a - '0');
	}
	return a;
}

static uint8_t file_read_hex(void) {
	return (gethexnib(file_read_byte()) << 4) + gethexnib(file_read_byte());
}

static inline void read_hex_file(void) {
	// read file and convert it from intel hex and flash it
    uint8_t num_flash_words = 0;
	uint8_t* out = pagebuffer;
    address = 0;
	while (file.size)
	{
		if (num_flash_words)
		{
			// read (de-hexify)
			*out++ = file_read_hex();
			num_flash_words--;
		
			// if pagebuffer is full
			if (out - pagebuffer == SPM_PAGESIZE) {
			    // write page
				write_pagebuffer(address);
			    address += SPM_PAGESIZE;
				out = pagebuffer;
			}
		} 
		else
		{
			// skip bytes until we find another ':'
			if (file_read_byte() == ':') {
				num_flash_words = file_read_hex();
				file.next+=4; /* skip 4 bytes */
				if (file_read_hex()) break;
			}
		}
	}
	if (out != pagebuffer) write_pagebuffer(address);
}

int load_file() {

	uint16_t entrycounter = 0;
	uint8_t i = 0;

	
	/* first, init mmc / fat */
	if (fat16_init() != 0)
		return 2;

	/* for each file in ROOT... */
	for (entrycounter=0; entrycounter<512; entrycounter++)
	{
		/* skip all unimportant files */
		i = fat16_readRootDirEntry(entrycounter);
		if (i == 1)
		{					
			read_hex_file();
			return 0; // no error
		}
	}	
	
	return 1; // file not found
}

#endif
