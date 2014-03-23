/**********************************************************/
/* prog_flash.c                                           */
/* Copyright (c) 2010 by thomas seiler                    */ 
/* write to the internal flash of the uC                  */
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

#include "prog_flash.h"
#include <avr/boot.h>

/* page buffer ---------------------------------------------------- */
uint8_t pagebuffer[SPM_PAGESIZE];


/* address buffer */
uint16_t address;

/* access to flash memory------------------------------------------ */

void write_flash_page()
{
	uint16_t i = 0;

	eeprom_busy_wait ();

	boot_page_erase (address);
	boot_spm_busy_wait ();      // Wait until the memory is erased.

	for (i=0; i<SPM_PAGESIZE; i+=2)
	{
		// Set up little-endian word.
		uint16_t w = *((uint16_t*)(pagebuffer + i));
		boot_page_fill (address + i, w);
	}

	boot_page_write(address);     // Store buffer in flash page.
	boot_spm_busy_wait();            // Wait until the memory is written.

	boot_rww_enable ();
}
