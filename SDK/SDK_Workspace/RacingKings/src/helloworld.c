/*
 * Copyright (c) 2009-2012 Xilinx, Inc.  All rights reserved.
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 *
 *
 *
 */

#include <stdio.h>
#include "platform.h"
#include "xparameters.h"
#include "xio.h"
#include "xil_exception.h"
#include "vga_periph_mem.h"
#include "minesweeper_sprites.h"
#include "sprites_green.c"
#include <stdlib.h>     /* srand, rand */
#include <time.h>
#define SIZE 9
#define UP 0b01000000
#define DOWN 0b00000100
#define LEFT 0b00100000
#define RIGHT 0b00001000
#define CENTER 0b00010000
#define SW0 0b00000001
#define SW1 0b00000010
#define BOMB '*'
#define NUM1 '1'
#define NUM2 '2'
#define NUM3 '3'
#define NUM4 '4'
#define BLANK '0'
#define FLAG '#'
#define NUMOFMINES 9
//BEG---unpened field
#define BEG '@'
#define COL_WHITE 0b011010000
#define COL_BLACK 0b111110100
#define COL_CURSOR 0b000000000
#define COL_CURSOR_SELECT 0b111000000
#define COL_LEGAL_MOVE 0b000111000

int endOfGame;
int inc1;
int inc2;
int i, x, y, ii, oi, R, G, B, RGB, kolona, red, RGBgray;
int numOfFlags;
int flagTrue;
int randomCounter = 50;
int numOfMines;
int firstTimeCenter;
//map that is hidden from the user-it contains the solution
char solvedMap[9][9];
//map that has all of player's moves
char blankMap[9][9];
//map used for opening the blank fields that surround blank field selected
char indicationMap[9][9];

static char chessTable[8][8];
static char legalMoves[8][8];

//end of game
//Rekonfigurisati da ispise ko je pobedio

void printOutEndOfGame(char blankTable[SIZE][SIZE], char solvedMap[SIZE][SIZE]) {
	int i, j, ii, jj;
	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			ii = (i * 16) + 80;
			jj = (j * 16) + 80;
			if (blankTable[i][j] == FLAG) {
				if (solvedMap[i][j] != BOMB) {
					drawMap(16, 16, ii, jj, 16, 16);
				}
			} else if (blankTable[i][j] != FLAG && solvedMap[i][j] == BOMB) {
				drawMap(0, 16, ii, jj, 16, 16);
			}
		}
	}
}

//when the blank field is pressed, open all blank fields around it
// OBRISATI
void clean(int x, int y, char resultTable[SIZE][SIZE],
		char indicationMap[SIZE][SIZE]) {
	int i, j;

	indicationMap[x][y] = 'x';

	if (resultTable[x][y] == BLANK) {
		for (i = x - 1; i <= x + 1; i++) {
			for (j = y - 1; j <= y + 1; j++) {
				if (i >= 0 && j >= 0 && i < 9 && j < 9 && !(x == i && y == j)) {
					if (indicationMap[i][j] == BLANK) {
						clean(i, j, resultTable, indicationMap);
					}
				}

			}
		}
	}
}

//(x,y) - nove koordinate figure
//(y,z) - stare koordinate figure
int isKingAttacked(int x, int y, int z, int u, char figure){
	int i, j;
	int bx, by, cx, cy;

	char tempTable[8][8];

	for (i = 0; i < 8; i++){
		for (j = 0; j < 8; j++)
		tempTable[i][j] = chessTable[i][j];
	}

	tempTable[x][y] = chessTable[z][u];
	tempTable[z][u] = 0;

	for (i = 0; i < 8; i++){
		for (j = 0; j < 8; j++){
			if (tempTable[i][j] == 1){
				bx = i;
				by = j;
			}
			else if (tempTable[i][j] == 11){
				cx = i;
				cy = j;
			}
		}

	}


	//Kralj
	for(i = bx - 1; i <= bx + 1; i++){
		for(j = by - 1; j < by + 1; j++){
			if(i >= 0 && i < 8 && j >= 0 && j < 8){
				if(tempTable[i][j] == 11)
					return 1;
			}
			else
				break;
		}
	}

	for(i = cx - 1; i <= cx + 1; i++){
		for(j = cy - 1; j < cy + 1; j++){
			if(i >= 0 && i < 8 && j >= 0 && j < 8){
				if(tempTable[i][j] == 1)
					return 1;
			}
			else
				break;
		}
	}

	//Kraljica
	for (i = bx - 1; i > 0; i--){
		if(i < 0 || i > 7)
			break;
		if (tempTable[i][by] != 12 && tempTable[i][by] != 0)
			break;
		if (tempTable[i][by] == 12)
			return 1;
	}

	for (i = bx + 1; i < 8; i++){
		if(i < 0 || i > 7)
			break;
		if (tempTable[i][by] != 12 && tempTable[i][by] != 0)
			break;
		if (tempTable[i][by] == 12)
			return 1;

	}

	for (i = by - 1; i > 0; i--){
		if(i < 0 || i > 7)
			break;
		if (tempTable[bx][i] != 12 && tempTable[bx][i] != 0)
			break;
		if (tempTable[bx][i] == 12)
			return 1;

	}

	for (i = by + 1; i < 8; i++){
		if(i < 0 || i > 7)
			break;
		if (tempTable[bx][i] != 12 && tempTable[bx][i] != 0)
			break;
		if (tempTable[bx][i] == 12)
			return 1;

	}

	for (i = cx - 1; i > 0; i--){
		if(i < 0 || i > 7)
			break;
		if (tempTable[i][cy] != 2 && tempTable[i][cy] != 0)
			break;
		if (tempTable[i][cy] == 2)
			return 1;

	}

	for (i = cx + 1; i < 8; i++){
		if(i < 0 || i > 7)
			break;
		if (tempTable[i][cy] != 2 && tempTable[i][cy] != 0)
			break;
		if (tempTable[i][cy] == 2)
			return 1;

	}

	for (i = cy - 1; i > 0; i--){
		if(i < 0 || i > 7)
			break;
		if (tempTable[cx][i] != 2 && tempTable[cx][i] != 0)
			break;
		if (tempTable[cx][i] == 2)
			return 1;

	}

	for (i = cy + 1; i < 8; i++){
		if(i < 0 || i > 7)
			break;
		if (tempTable[cx][i] != 2 && tempTable[cx][i] != 0)
			break;
		if (tempTable[cx][i] == 2)
			return 1;

	}

	for(i = bx-1, j=by-1; (i >= 0) && (j >=0); i--, j--){
		if(i<0 || i>7 || j<0 || j>7)
			break;

		if(tempTable[i][j] == 12)
			return 1;

		else if(tempTable[i][j]!=0)
			break;
	}

	for(i = bx-1, j=by+1; (i >= 0) || (j < 8); i--, j++){
		if(i<0 || i>7 || j<0 || j>7)
			break;

		if(tempTable[i][j] == 12)
			return 1;

		else if(tempTable[i][j]!=0)
			break;
	}

	for(i = bx+1, j=by+1; (i < 8) && (j < 8); i++, j++){
		if(i<0 || i>7 || j<0 || j>7)
			break;

		if(tempTable[i][j] == 12)
			return 1;

		else if(tempTable[i][j]!=0)
			break;
	}

	for(i = bx+1, j=by-1; (i < 8) && (j >= 0); i++, j--){
		if(i<0 || i>7 || j<0 || j>7)
			break;

		if(tempTable[i][j] == 12)
			return 1;

		else if(tempTable[i][j]!=0)
			break;
	}

	for(i = cx-1, j=cy-1; (i >= 0) && (j >=0); i--, j--){
		if(i<0 || i>7 || j<0 || j>7)
			break;

		if(tempTable[i][j] == 2)
			return 1;

		else if(tempTable[i][j]!=0)
			break;
	}

	for(i = cx-1, j= cy+1; (i >= 0) && (j < 8); i--, j++){
		if(i<0 || i>7 || j<0 || j>7)
			break;

		if(tempTable[i][j] == 2)
			return 1;

		else if(tempTable[i][j]!=0)
			break;
	}

	for(i = cx+1, j= cy+1; (i < 8) && (j < 8); i++, j++){
		if(i<0 || i>7 || j<0 || j>7)
			break;

		if(tempTable[i][j] == 2)
			return 1;

		else if(tempTable[i][j]!=0)
			break;
	}

	for(i = cx+1, j= cy-1; (i < 8) && (j >= 0); i++, j--){
		if(i<0 || i>7 || j<0 || j>7)
			break;

		if(tempTable[i][j] == 2)
			return 1;

		else if(tempTable[i][j]!=0)
			break;
	}



	//Top
	for (i = bx - 1; i > 0; i--){
		if(i < 0 || i > 7)
			break;
		if (tempTable[i][by] != 13 && tempTable[i][by] != 0)
			break;
		if (tempTable[i][by] == 13)
			return 1;
	}

	for (i = bx + 1; i < 8; i++){
		if(i < 0 || i > 7)
			break;
		if (tempTable[i][by] != 13 && tempTable[i][by] != 0)
			break;
		if (tempTable[i][by] == 13)
			return 1;
	}

	for (i = by - 1; i > 0; i--){
		if(i < 0 || i > 7)
			break;
		if (tempTable[bx][i] != 13 && tempTable[bx][i] != 0)
			break;
		if (tempTable[bx][i] == 13)
			return 1;
	}

	for (i = by + 1; i < 8; i++){
		if(i < 0 || i > 7)
			break;
		if (tempTable[bx][i] != 13 && tempTable[bx][i] != 0)
			break;
		if (tempTable[bx][i] == 13)
			return 1;
	}

	for (i = cx - 1; i > 0; i--){
		if(i < 0 || i > 7)
			break;
		if (tempTable[i][cy] != 3 && tempTable[i][cy] != 0)
			break;
		if (tempTable[i][cy] == 3)
			return 1;

	}

	for (i = cx + 1; i < 8; i++){
		if(i < 0 || i > 7)
			break;
		if (tempTable[i][cy] != 3 && tempTable[i][cy] != 0)
			break;
		if (tempTable[i][cy] == 3)
			return 1;
	}

	for (i = cy - 1; i > 0; i--){
		if(i < 0 || i > 7)
			break;
		if (tempTable[cx][i] != 3 && tempTable[cx][i] != 0)
			break;
		if (tempTable[cx][i] == 3)
			return 1;

	}

	for (i = cy + 1; i < 8; i++){
		if(i < 0 || i > 7)
			break;
		if (tempTable[cx][i] != 3 && tempTable[cx][i] != 0)
			break;
		if (tempTable[cx][i] == 3)
			return 1;

	}

	//Lovac
	for(i = bx-1, j=by-1; (i >= 0) && (j >=0); i--, j--){
		if(i<0 || i>7 || j<0 || j>7)
			break;

		if(tempTable[i][j] == 14)
			return 1;

		else if(tempTable[i][j]!=0)
			break;
	}

	for(i = bx-1, j=by+1; (i >= 0) || (j < 8); i--, j++){
		if(i<0 || i>7 || j<0 || j>7)
			break;

		if(tempTable[i][j] == 14)
			return 1;

		else if(tempTable[i][j]!=0)
			break;
	}

	for(i = bx+1, j=by+1; (i < 8) && (j < 8); i++, j++){
		if(i<0 || i>7 || j<0 || j>7)
			break;

		if(tempTable[i][j] == 14)
			return 1;

		else if(tempTable[i][j]!=0)
			break;
	}

	for(i = bx+1, j=by-1; (i < 8) && (j >= 0); i++, j--){
		if(i<0 || i>7 || j<0 || j>7)
			break;

		if(tempTable[i][j] == 14)
			return 1;

		else if(tempTable[i][j]!=0)
			break;
	}

	for(i = cx-1, j=cy-1; (i >= 0) && (j >=0); i--, j--){
		if(i<0 || i>7 || j<0 || j>7)
			break;

		if(tempTable[i][j] == 4)
			return 1;

		else if(tempTable[i][j]!=0)
			break;
	}

	for(i = cx-1, j= cy+1; (i >= 0) && (j < 8); i--, j++){
		if(i<0 || i>7 || j<0 || j>7)
			break;

		if(tempTable[i][j] == 4)
			return 1;

		else if(tempTable[i][j]!=0)
			break;
	}

	for(i = cx+1, j= cy+1; (i < 8) && (j < 8); i++, j++){
		if(i<0 || i>7 || j<0 || j>7)
			break;

		if(tempTable[i][j] == 4)
			return 1;

		else if(tempTable[i][j]!=0)
			break;
	}

	for(i = cx+1, j= cy-1; (i < 8) && (j >= 0); i++, j--){
		if(i<0 || i>7 || j<0 || j>7)
			break;

		if(tempTable[i][j] == 4)
			return 1;

		else if(tempTable[i][j]!=0)
			break;
	}

	//Skakac
	//Provera u redu ispod i iznad
	for(i = bx - 1; i <= bx + 1; i+=2){
		for(j = by - 2; j <= by + 2; j+=4){
			if(i >= 0 && i < 8 && j >= 0 && j < 8){
				if(tempTable[i][j] == 15)
					return 1;
			}
			else
				break;
		}
	}

	for(i = cx - 1; i <= cx + 1; i+=2){
		for(j = cy - 2; j <= cy + 2; j+=4){
			if(i >= 0 && i < 8 && j >= 0 && j < 8){
				if(tempTable[i][j] == 5)
					return 1;
			}
			else
				break;
		}
	}

	//Provera u kolonama pored
	for(i = bx - 2; i <= bx + 2; i+=4){
		for(j = by - 1; j <= by + 1; j+=2){
			if(i >= 0 && i < 8 && j >= 0 && j < 8){
				if(tempTable[i][j] == 15)
					return 1;
			}
			else
				break;
		}
	}

	for(i = cx - 2; i <= cx + 2; i+=4){
		for(j = cy - 1; j <= cy + 1; j+=2){
			if(i >= 0 && i < 8 && j >= 0 && j < 8){
				if(tempTable[i][j] == 5)
					return 1;
			}
			else
				break;
		}
	}

	return 0;
}

void findLegalMoves(int x, int y){

	int i, j;
	
	char selected_piece = chessTable[x][y];
	switch (selected_piece % 10){
		//Kralj
		case 1:
			for (i = x - 1; i <= x + 1; i++){
				for (j = y - 1; j <=y + 1; j ++){
					if ((i != x && j != y) && i >= 0 && i <= 7 && j >= 0 && j <= 7){
						if (selected_piece > 10){
							if (chessTable[i][j] < 10){
								if (!isKingAttacked(i, j, x, y, selected_piece))
								legalMoves[i][j] = 1;
							}
						}
						else {
							if (chessTable[i][j] > 10){
								if (!isKingAttacked(i, j, x, y, selected_piece))
								legalMoves[i][j] = 1;
							}
						}
					}
				}
			}
			break;
			

		//Kraljica
		case 2:		
			//Provera u redu
			for(i = x-1; i > 0; i--){
				//Ako je polje prazno
				if(chessTable[i][y] == 0){
					if(!isKingAttacked(i, y, x, y, selected_piece))
						legalMoves[i][y] = 1;
				}
				//Ako su razlicite boje, omoguci da pojede
				else if(chessTable[i][y]/10 != selected_piece/10){
					if(!isKingAttacked(i, y, x, y, selected_piece))
						legalMoves[i][y] = 1;
						break;
				}
				//Ako si naleteo na svoju figuru
				else
					break;
			}
			for(i = x+1; i < 8; i++){
				if(chessTable[i][y] == 0){
					if(!isKingAttacked(i, y,x, y, selected_piece))
						legalMoves[i][y] = 1;
				}

				else if(chessTable[i][y]/10 != selected_piece/10){
					if(!isKingAttacked(i, y, x, y, selected_piece))
						legalMoves[i][y] = 1;
						break;
				}

				else
					break;	
			}
			
			//Provera u koloni
			for(j = y-1; j > 0; j--){
				if(chessTable[x][j] == 0){
					if(!isKingAttacked(x, j, x, y, selected_piece))
						legalMoves[x][j] = 1;
				}
				else if(chessTable[x][j]/10 != selected_piece/10){
					if(!isKingAttacked(x, j,x,y, selected_piece))
						legalMoves[x][j] = 1;
						break;
				}
				else
					break;			
			}
			for(j = y+1; j < 8; j++){
				if(chessTable[x][j] == 0){
					if(!isKingAttacked(x, j, x, y, selected_piece))
						legalMoves[x][j] = 1;
				}
				else if(chessTable[x][j]/10 != selected_piece/10){
					if(!isKingAttacked(x, j,x,y, selected_piece))
						legalMoves[x][j] = 1;
						break;
				}
				else
					break;			
			}

	
			//Provera po dijagonali
			for(i = x-1, j = y-1; (i > 0) && (j > 0); i--, j--){
				if(chessTable[i][j] == 0){
					if(!isKingAttacked(i, j, x, y, selected_piece))
						legalMoves[i][j] = 1;
				}
				else if(chessTable[i][j]/10 != selected_piece/10){
					if(!isKingAttacked(i, j, x, y, selected_piece))
						legalMoves[i][j] = 1;
						break;
				}
				else
					break;
			}

			for(i = x-1, j = y + 1; (i > 0) && (j < 8); i--, j++){
				if(chessTable[i][j] == 0){
					if(!isKingAttacked(i, j, x, y, selected_piece))
						legalMoves[i][j] = 1;
				}
				else if(chessTable[i][j]/10 != selected_piece/10){
					if(!isKingAttacked(i, j, x, y, selected_piece))
						legalMoves[i][j] = 1;
						break;
				}
				else
					break;
			}

			for(i = x+1, j = y - 1; (i < 8) && (j > 0); i++, j--){
				if(chessTable[i][j] == 0){
					if(!isKingAttacked(i, j, x, y, selected_piece))
						legalMoves[i][j] = 1;
				}
				else if(chessTable[i][j]/10 != selected_piece/10){
					if(!isKingAttacked(i, j, x, y, selected_piece))
						legalMoves[i][j] = 1;
						break;
				}
				else
					break;
			}
			
			for(i = x+1, j = y + 1; (i < 8) && (j < 8); i++, j++){
				if(chessTable[i][j] == 0){
					if(!isKingAttacked(i, j, x, y, selected_piece))
						legalMoves[i][j] = 1;
				}
				else if(chessTable[i][j]/10 != selected_piece/10){
					if(!isKingAttacked(i, j, x, y, selected_piece))
						legalMoves[i][j] = 1;
						break;
				}
				else
					break;
			}
			break;
			
		//Top
		case 3:
			for(i = x-1; i > 0; i--){
				if(chessTable[i][y] == 0){
					if(!isKingAttacked(i, y,x, y, selected_piece))
						legalMoves[i][y] = 1;
				}
				else if(chessTable[i][y]/10 != selected_piece/10){
					if(!isKingAttacked(i, y,x, y, selected_piece))
						legalMoves[i][y] = 1;
						break;
				}
				else
					break;
			}
			for(i = x+1; i < 8; i++){
				if(chessTable[i][y] == 0){
					if(!isKingAttacked(i, y,x,y, selected_piece))
						legalMoves[i][y] = 1;
				}

				else if(chessTable[i][y]/10 != selected_piece/10){
					if(!isKingAttacked(i, y,x,y, selected_piece))
						legalMoves[i][y] = 1;
						break;
				}

				else
					break;	
			}
			
			for(j = y-1; j > 0; j--){
				if(chessTable[x][j] == 0){
					if(!isKingAttacked(x, j,x,y, selected_piece))
						legalMoves[x][j] = 1;
				}
				else if(chessTable[x][j]/10 != selected_piece/10){
					if(!isKingAttacked(x, j,x,y, selected_piece))
						legalMoves[x][j] = 1;
						break;
				}
				else
					break;			
			}
			for(j = y+1; j < 8; j++){
				if(chessTable[x][j] == 0){
					if(!isKingAttacked(x, j, x,y,selected_piece))
						legalMoves[x][j] = 1;
				}
				else if(chessTable[x][j]/10 != selected_piece/10){
					if(!isKingAttacked(x, j,x,y, selected_piece))
						legalMoves[x][j] = 1;
						break;
				}
				else
					break;			
			}
			break;
			
		//Lovac
		case 4:
			j = y - 1;
			for(i = x-1; i > 0; i--){
				if(chessTable[i][j] == 0){
					if(!isKingAttacked(i, j, x, y, selected_piece))
						legalMoves[i][j] = 1;
				}
				else if(chessTable[i][j]/10 != selected_piece/10){
					if(!isKingAttacked(i, j, x, y, selected_piece))
						legalMoves[i][j] = 1;
						break;
				}
				else
					break;
				if(j == 0)
					break;
				j--;
			}

			j = y + 1;
			for(i = x-1; i > 0; i--){
				if(chessTable[i][j] == 0){
					if(!isKingAttacked(i, j, x, y, selected_piece))
						legalMoves[i][j] = 1;
				}
				else if(chessTable[i][j]/10 != selected_piece/10){
					if(!isKingAttacked(i, j, x, y, selected_piece))
						legalMoves[i][j] = 1;
						break;
				}
				else
					break;
				if(j == 7)
					break;
				j++;
			}

			j = y-1;
			for(i = x+1; i <8; i++){
				if(chessTable[i][j] == 0){
					if(!isKingAttacked(i, j, x, y, selected_piece))
						legalMoves[i][j] = 1;
				}
				else if(chessTable[i][j]/10 != selected_piece/10){
					if(!isKingAttacked(i, j, x, y, selected_piece))
						legalMoves[i][j] = 1;
						break;
				}
				else
					break;

				if(j == 7)
					break;
				j++;
			}
			
			j = y+1;
			for(i = x+1; i <8; i++){
				if(chessTable[i][j] == 0){
					if(!isKingAttacked(i, j, x, y, selected_piece))
						legalMoves[i][j] = 1;
				}
				else if(chessTable[i][j]/10 != selected_piece/10){
					if(!isKingAttacked(i, j, x, y, selected_piece))
						legalMoves[i][j] = 1;
						break;
				}
				else
					break;

				if(j == 0)
					break;
				j--;
			}
			break;
			
		//Skakac
		case 5:
			//Provera u redu ispod/iznad
			for(i = x - 2; i <= x+2; i+=4){
				for(j = y - 1 ; j <= y+1; y+=2){
					if(i > 0 && j > 0 && i < 8 && j < 8){
						if(!isKingAttacked(i, j,x, y, selected_piece)){
							if(chessTable[i][j] == 0 || chessTable[i][j]/10 != selected_piece/10)
								legalMoves[i][i] = 1;
						}
							
					}
				}
			}
			
			//Provera u kolonama do
			for(i = x - 1; i <= x + 1; i+=2){
				for(j = y - 2 ; j <= y+2; y+=4){
					if(i > 0 && j > 0 && i < 8 && j < 8){
						if(!isKingAttacked(i, j,x, y, selected_piece)){
							if(chessTable[i][j] == 0 || chessTable[i][j]/10 != selected_piece/10)
								legalMoves[i][i] = 1;
						}	
					}
				}
			}
			break;
	}
	
	markLegalMoves();

}

void markLegalMoves(){
	int i,j;
	for (i = 0; i < 8; i++){
		for (j = 0; j < 8; j++){
			if (legalMoves[i][j] == 1){
				drawingCursor(79 + j * 19, 20 + i * 25, 98 + j * 19, 45 + i * 25, 1);
			}
		}
	}
}

//function for opening selected field
// Promena -> selekcija figure
void openField(int x, int y, char map[9][9]) {
	int i, j;
	int x1, y1;
	x1 = (x - 80) / 16;
	y1 = (y - 80) / 16;

	switch (map[x1][y1]) {
	case NUM1:
		drawMap(16, 0, x - 1, y - 1, 16, 16);
		if (map != blankMap)
			blankMap[x1][y1] = NUM1;
		break;

	case NUM2:
		drawMap(32, 0, x - 1, y - 1, 16, 16);
		if (map != blankMap)
			blankMap[x1][y1] = NUM2;
		break;

	case NUM3:
		drawMap(48, 0, x - 1, y - 1, 16, 16);
		if (map != blankMap)
			blankMap[x1][y1] = NUM3;
		break;

	case BLANK:
		drawMap(0, 0, x - 1, y - 1, 16, 16);
		if (map != blankMap)
			blankMap[x1][y1] = BLANK;
		clean(x1, y1, solvedMap, indicationMap);
		for (i = 0; i < 9; i++) {
			for (j = 0; j < 9; j++) {
				xil_printf("%c", indicationMap[i][j]);
			}
			xil_printf("\n");
		}
		break;

	case NUM4:
		drawMap(64, 0, x - 1, y - 1, 16, 16);
		if (map != blankMap)
			blankMap[x1][y1] = NUM4;
		break;

	case BOMB:
		if (map != blankMap)
			blankMap[x1][y1] = BOMB;
		endOfGame = 1;
		printOutEndOfGame(blankMap, solvedMap);
		drawMap(32, 16, x - 1, y - 1, 16, 16);
		drawMap(77, 54, 120, 54, 27, 26);
		break;
	case '@':
		drawMap(80, 16, x - 1, y - 1, 16, 16);
		if (map != blankMap)
			blankMap[x1][y1] = BEG;
		break;

	case '#':
		drawMap(64, 16, x - 1, y - 1, 16, 16);
		if (map != blankMap)
			blankMap[x1][y1] = FLAG;
		break;
	}
}


void initLegalMoves(){
	int i,j;
		for (i = 0; i < 6; i++){
			for(j = 0; j<8; j++){
				legalMoves[i][j] = 0;
			}
		}

}

void initTableMatrix(){
	
	//Polja se namestaju sa leva na desno, od gore ka dole
	//Anuliranje praznih polja
	int i,j;
	for (i = 0; i < 6; i++){
		for(j = 0; j<8; j++){
			chessTable[i][j] = 0;
			legalMoves[i][j] = 0;
		}
	}

	//Namestanje figurica na pocetne polozaje
	chessTable[6][0]=1;
	chessTable[6][1]=3;
	chessTable[6][2]=4;
	chessTable[6][3]=5;
	chessTable[6][4]=15;
	chessTable[6][5]=14;
	chessTable[6][6]=13;
	chessTable[6][7]=11;

	chessTable[7][0]=2;
	chessTable[7][1]=3;
	chessTable[7][2]=4;
	chessTable[7][3]=5;
	chessTable[7][4]=15;
	chessTable[7][5]=14;
	chessTable[7][6]=13;
	chessTable[7][7]=12;
	
}

void drawTable(){
	int i, j;
	for(i = 0; i < 8; i++){
		for(j = 0; j < 8; j++){
			if(chessTable[i][j] != 0){
				drawMap(19 * (chessTable[i][j]%10- 1), chessTable[i][j]/10*25, 79 + j*19, 20 + i*25, 19, 25);
			}
		}
	}
}

void drawBackground(){
	//Tekst RACING KINGS - (224, 10)
	//Pocetak table - (79, 20)
	//Polja 25x19
	//Ispis kolona - (137+50, 460)
	//Ispis redova - (524, 57+50)
	//Podeliti sve sa 4

	int x;
	int y;
	int i;
	int j;
	set_cursor(5*320+112);
	unsigned char natpis[] = "RACING KINGS";
	print_string(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR, natpis, 12);

	for(i = 0; i < 8; i++){
		for(j = 0; j < 8; j++){
			for(x = 0; x < 19; x++){
				for(y = 0; y < 25; y++){
					// Koordinata: red*sirina + kolona
					if((i+j)%2){
						VGA_PERIPH_MEM_mWriteMemory(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF + ((j*25+y+20)*320+(79+i*19+x))*4, COL_WHITE);
					}
					else{
						VGA_PERIPH_MEM_mWriteMemory(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF + ((j*25+y+20)*320+(79+i*19+x))*4, COL_BLACK);
					}
				}
			}
		}
	}

	for(i=0; i<8; i++){
		set_cursor(460*640+137+50*i);
		print_char(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR, 'A'+i);
	}

	for(i=0; i<8; i++){
		set_cursor((57+50*i)*640+524);
		print_char(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR, '8'-i);
	}


	drawMap(0, 50, 79, 10, 60, 8);


	//Ispis redova i kolona

	drawMap(0, 58, 235, 203, 3, 10);
	drawMap(3, 58, 234, 178, 4, 10);
	drawMap(7, 58, 233, 153, 5, 10);
	drawMap(12, 58, 233, 128, 5, 10);
	drawMap(17, 58, 233, 103, 5, 10);
	drawMap(22, 58, 233, 78, 5, 10);
	drawMap(27, 58, 233, 53, 5, 10);
	drawMap(32, 58, 233, 28, 5, 10);

	drawMap(51, 58, 86, 220, 4, 10);
	drawMap(56, 58, 105, 220, 4, 10);
	drawMap(61, 58, 124, 220, 4, 10);
	drawMap(66, 58, 143, 220, 4, 10);
	drawMap(71, 58, 162, 220, 4, 10);
	drawMap(76, 58, 181, 220, 4, 10);
	drawMap(81, 58, 200, 220, 4, 10);
	drawMap(86, 58, 219, 220, 4, 10);

	drawTable();

}

//function that generates random game map
void makeTable(char temp[9][9]) {
	int numOfMines = NUMOFMINES, row, column, i, j, m, surroundingMines = 0;
	char table[9][9];

	srand(randomCounter);

	//popunjava matricu nulama
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			table[i][j] = BLANK;
		}
	}

	//postavlja random mine
	while (numOfMines > 0) {
		row = rand() % 9;
		column = rand() % 9;
		if (table[row][column] == BLANK) {
			table[row][column] = BOMB;
			numOfMines--;
		}

	}

	//proverava poziciju mina i ispisuje brojeve na odg mesta
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			surroundingMines = 0;
			if (table[i][j] != BOMB) {
				if (i > 0 && j > 0) {
					if (table[i - 1][j - 1] == BOMB)
						surroundingMines++;
				}
				if (j > 0) {
					if (table[i][j - 1] == BOMB)
						surroundingMines++;
				}
				if (i < 9 - 1 && j > 0) {
					if (table[i + 1][j - 1] == BOMB)
						surroundingMines++;
				}
				if (i > 0) {
					if (table[i - 1][j] == BOMB)
						surroundingMines++;
				}
				if (i < 9 - 1) {
					if (table[i + 1][j] == BOMB)
						surroundingMines++;
				}
				if (i > 0 && j < 9 - 1) {
					if (table[i - 1][j + 1] == BOMB)
						surroundingMines++;
				}
				if (j < 9 - 1) {
					if (table[i][j + 1] == BOMB)
						surroundingMines++;
				}
				if (i < 9 - 1 && j < 9 - 1) {
					if (table[i + 1][j + 1] == BOMB)
						surroundingMines++;
				}
				table[i][j] = surroundingMines + '0';
			}
		}

	}

	//for testing

	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			xil_printf("%c", table[i][j]);
		}
		xil_printf("\n");
	}

	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			temp[i][j] = table[j][i];

		}
	}

	xil_printf("\n");

	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			xil_printf("%c", temp[j][i]);
		}
		xil_printf("\n");
	}

}


//extracting pixel data from a picture for printing out on the display

void drawMap(int in_x, int in_y, int out_x, int out_y, int width, int height) {
	int ox, oy, oi, iy, ix, ii;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			ox = out_x + x;
			oy = out_y + y;
			oi = oy * 320 + ox;
			ix = in_x + x;
			iy = in_y + y;
			ii = iy * chess_sprites.width + ix;
			R = chess_sprites.pixel_data[ii
					* chess_sprites.bytes_per_pixel] >> 5;
			G = chess_sprites.pixel_data[ii
					* chess_sprites.bytes_per_pixel + 1] >> 5;
			B = chess_sprites.pixel_data[ii
					* chess_sprites.bytes_per_pixel + 2] >> 5;
			R <<= 6;
			G <<= 3;
			RGB = R | G | B;
			if(RGB != 0b000111000)
				VGA_PERIPH_MEM_mWriteMemory(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF + oi * 4, RGB);
		}
	}

}

//drawing cursor for indicating position
void drawingCursor(int startX, int startY, int endX, int endY, int mod) {

	for (x = startX; x < endX; x++) {
		for (y = startY; y < startY + 2; y++) {
			i = y * 320 + x;
			VGA_PERIPH_MEM_mWriteMemory(
					XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ i * 4, COL_CURSOR);
		}
	}

	for (x = startX; x < endX; x++) {
		for (y = endY - 2; y < endY; y++) {
			i = y * 320 + x;
			VGA_PERIPH_MEM_mWriteMemory(
					XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ i * 4, COL_CURSOR);
		}
	}

	for (x = startX; x < startX + 2; x++) {
		for (y = startY; y < endY; y++) {
			i = y * 320 + x;
			VGA_PERIPH_MEM_mWriteMemory(
					XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ i * 4, COL_CURSOR);
		}
	}

	for (x = endX - 2; x < endX; x++) {
		for (y = startY; y < endY; y++) {
			i = y * 320 + x;
			VGA_PERIPH_MEM_mWriteMemory(
					XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ i * 4, COL_CURSOR);
		}
	}

}

//function that controls switches and buttons

void move() {

	int startX = 79, startY = 20, endX = 98, endY = 45;
	int oldStartX, oldStartY, oldEndX, oldEndY;
	int x = 0, y = 0, ic, ib, i, j;
	int prethodnoStanje;
	typedef enum {
		NOTHING_PRESSED, SOMETHING_PRESSED
	} btn_state_t;
	btn_state_t btn_state = NOTHING_PRESSED;

	typedef enum{
		NONE_PICKED, FIGURE_SELECTED
	} pick_mode;

	pick_mode mode = NONE_PICKED;

	//makeTable(solvedMap);
	drawingCursor(startX, startY, endX, endY, 0);

	while (endOfGame != 1) {
		if (btn_state == NOTHING_PRESSED) {
			btn_state = SOMETHING_PRESSED;
			drawingCursor(startX, startY, endX, endY, 0);
			markLegalMoves();
			if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & DOWN) == 0) {
				if (endY < 196) {
					oldStartY = startY;
					oldEndY = endY;
					startY += 25;
					endY += 25;
					y++;
					//drawingCursor(startX, startY, endX, endY);
					//openField(startX, oldStartY, blankMap);
				}

			}

			else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & RIGHT) == 0) {
				//randomCounter++;
				if (endX < 213) {
					oldStartX = startX;
					startX += 19;
					endX += 19;
					x++;
					//drawingCursor(startX, startY, endX, endY);
					//openField(oldStartX, startY, blankMap);

				}
			} else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & LEFT) == 0) {
				if (startX > 97) {
					oldStartX = startX;
					startX -= 19;
					endX -= 19;
					x--;
					//drawingCursor(startX, startY, endX, endY);
					//openField(oldStartX, startY, blankMap);
				}

			} else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & UP) == 0) {
				if (startY > 44) {
					oldStartY = startY;
					startY -= 25;
					endY -= 25;
					y--;
					//drawingCursor(startX, startY, endX, endY);
					//openField(startX, oldStartY, blankMap);
				}

			} else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & CENTER)
					== 0) {
				/*int m = (startX - 80) / 16;
				int n = (startY - 80) / 16;
				firstTimeCenter++;
				if (firstTimeCenter == 1) {
					randomCounter++;
					while (solvedMap[m][n] == BOMB)
						makeTable(solvedMap);
				}
				openField(startX, startY, solvedMap);
				int ii = 0, jj = 0;

				for (i = 0; i < SIZE; i++) {
					for (j = 0; j < SIZE; j++) {
						if (indicationMap[i][j] == 'x') {
							ii = (i * 16) + 80;
							jj = (j * 16) + 80;

							if (solvedMap[i][j] == BLANK) {
								drawMap(0, 0, ii, jj, 16, 16);
								blankMap[i][j] = BLANK;
							}
							if (solvedMap[i][j] == NUM2) {
								drawMap(32, 0, ii, jj, 16, 16);
								blankMap[i][j] = NUM2;
							}
							if (solvedMap[i][j] == NUM1) {
								drawMap(16, 0, ii, jj, 16, 16);
								blankMap[i][j] = NUM1;
							}
							if (solvedMap[i][j] == NUM3) {
								drawMap(48, 0, ii, jj, 16, 16);
								blankMap[i][j] = NUM3;
							}
							if (solvedMap[i][j] == NUM4) {
								drawMap(64, 0, ii, jj, 16, 16);
								blankMap[i][j] = NUM4;
							}
						}
					}*/
					if(mode == NONE_PICKED){
						mode = FIGURE_SELECTED;
						findLegalMoves(x, y);
					}
					else if(mode == FIGURE_SELECTED){
						mode = NONE_PICKED;
						initLegalMoves();
					}

				}

			 else {
				btn_state = NOTHING_PRESSED;
			}
		}
		 else { // SOMETHING_PRESSED

			if((oldStartX != startX) || (oldStartY != startY)){
				drawingCursor(startX, startY, endX, endY, 0);
				drawBackground();
			}

			if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & DOWN) == 0) {
			} else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & RIGHT) == 0) {
			} else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & LEFT) == 0) {
			} else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & UP) == 0) {
			} else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & CENTER)
					== 0) {
			} else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & SW0) != 0) {
			} else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & SW1) != 0) {
			} else {
				btn_state = NOTHING_PRESSED;
			}
		}

	}

}

int main() {

	int j, p, r;
	inc1 = 0;
	inc2 = 0;
	numOfFlags = NUMOFMINES;
	flagTrue = 0;
	numOfMines = NUMOFMINES;
	firstTimeCenter = 0;

	init_platform();


	//helping map for cleaning the table when blank button is pressed
	for (p = 0; p < SIZE; p++) {
		for (r = 0; r < SIZE; r++) {
			indicationMap[p][r] = BLANK;
		}
	}

	//map which contains all the moves of the player
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			blankMap[i][j] = BEG;
		}
	}

	VGA_PERIPH_MEM_mWriteMemory(
			XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x00, 0x0); // direct mode   0
	VGA_PERIPH_MEM_mWriteMemory(
			XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x04, 0x3); // display_mode  1
	VGA_PERIPH_MEM_mWriteMemory(
			XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x08, 0x0); // show frame      2
	VGA_PERIPH_MEM_mWriteMemory(
			XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x0C, 0xff); // font size       3
	VGA_PERIPH_MEM_mWriteMemory(
			XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x10, 0xFFFFFF); // foreground 4
	VGA_PERIPH_MEM_mWriteMemory(
			XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x14, 0x0000FF); // background color 5
	VGA_PERIPH_MEM_mWriteMemory(
			XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x18, 0xFF0000); // frame color      6
	VGA_PERIPH_MEM_mWriteMemory(
			XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + 0x20, 1);

	//black background
	for (x = 0; x < 320; x++) {
		for (y = 0; y < 240; y++) {
			i = y * 320 + x;
			VGA_PERIPH_MEM_mWriteMemory(
					XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ i * 4, 0b110110110);
		}
	}

	drawBackground();
	initTableMatrix();
	drawTable();
	move();

	/*while(1){
		//drawBackground();
		drawingCursor(79, 20, 98, 45);

	}*/

	/*//drawing a map
	for (kolona = 0; kolona < 9; kolona++) {
		for (red = 0; red < 9; red++) {
			drawMap(80, 16, 80 + red * 16, 80 + kolona * 16, 16, 16);
		}
	}

	//smiley
	drawMap(0, 55, 120, 54, 27, 26);

	//flag
	drawMap(65, 17, 154, 60, 13, 13);

	//counter
	drawMap(116, 32, 168, 54, 14, 23);

	//moving through the table
	move();*/


	cleanup_platform();

	return 0;
}
