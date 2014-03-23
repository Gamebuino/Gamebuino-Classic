/**********************************************************/
/* prog_flash.h                                           */
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

#ifndef _prog_flash_h_
#define _prog_flash_h_

#include <avr/io.h>   /* for SPM_PAGESIZE */
#include <inttypes.h>

extern uint8_t pagebuffer[SPM_PAGESIZE];
void write_flash_page();

extern uint16_t address;

#endif