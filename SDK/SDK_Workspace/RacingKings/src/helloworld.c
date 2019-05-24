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
#include "sprites_green.c"
#include <stdlib.h>     /* srand, rand */
#include <time.h>
#define UP 0b01000000
#define DOWN 0b00000100
#define LEFT 0b00100000
#define RIGHT 0b00001000
#define CENTER 0b00010000
#define COL_WHITE 0b011010000
#define COL_BLACK 0b111110100
#define COL_CURSOR 0b000000000
#define COL_CURSOR_SELECT 0b111000000
#define COL_LEGAL_MOVE 0b000111000

int i, ii, oi, R, G, B, RGB, x, y;

static char chessTable[8][8];
static char legalMoves[8][8];
static int cursor_x = 0;
static int cursor_y = 0;

//end of game

void printOutEndOfGame(int who_won) {

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
	for (i = bx - 1; i >= 0; i--){
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

	for (i = by - 1; i >= 0; i--){
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

	for (i = cx - 1; i >= 0; i--){
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

	for (i = cy - 1; i >= 0; i--){
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
		}
	}

	for(i = cx - 1; i <= cx + 1; i+=2){
		for(j = cy - 2; j <= cy + 2; j+=4){
			if(i >= 0 && i < 8 && j >= 0 && j < 8){
				if(tempTable[i][j] == 5)
					return 1;
			}
		}
	}

	//Provera u kolonama pored
	for(i = bx - 2; i <= bx + 2; i+=4){
		for(j = by - 1; j <= by + 1; j+=2){
			if(i >= 0 && i < 8 && j >= 0 && j < 8){
				if(tempTable[i][j] == 15)
					return 1;
			}
		}
	}

	for(i = cx - 2; i <= cx + 2; i+=4){
		for(j = cy - 1; j <= cy + 1; j+=2){
			if(i >= 0 && i < 8 && j >= 0 && j < 8){
				if(tempTable[i][j] == 5)
					return 1;
			}
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
				for (j = y - 1; j <= y + 1; j ++){
					if (!(i == x && j == y) && i >= 0 && i <= 7 && j >= 0 && j <= 7){
						if (selected_piece > 10){
							if (chessTable[i][j] < 10){
								if (!isKingAttacked(i, j, x, y, selected_piece))
								legalMoves[i][j] = 1;
							}
						}
						else {
							if (chessTable[i][j] > 10 || chessTable[i][j] == 0){
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
			for(i = x-1; i >= 0; i--){
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
			for(j = y-1; j >= 0; j--){
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
			for(i = x-1, j = y-1; (i >= 0) && (j >= 0); i--, j--){
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

			for(i = x-1, j = y + 1; (i >= 0) && (j < 8); i--, j++){
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

			for(i = x+1, j = y - 1; (i < 8) && (j >= 0); i++, j--){
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
			for(i = x-1; i >= 0; i--){
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
			
			for(j = y-1; j >= 0; j--){
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
			for(i = x-1, j = y - 1; (i >= 0) && (j >= 0); i--, j--){
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

			for(i = x-1, j = y + 1; (i >= 0) && (j < 8); i--, j++){
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

			for(i = x+1, j = y - 1; (i < 8) && (j >= 0); i++, j--){
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
			
		//Skakac
		case 5:

			if(x > 0 && y > 1){
				if(!isKingAttacked(x-1, y-2, x, y, selected_piece)){
					if(chessTable[x-1][y-2] == 0 || chessTable[x-1][y-2]/10 != selected_piece/10)
						legalMoves[x-1][y-2] = 1;
				}
			}

			if(x > 0 && y < 6){
				if(!isKingAttacked(x-1, y+2, x, y, selected_piece)){
					if(chessTable[x-1][y+2] == 0 || chessTable[x-1][y+2]/10 != selected_piece/10)
						legalMoves[x-1][y+2] = 1;
				}
			}


			if(x < 7 && y > 1){
				if(!isKingAttacked(x+1, y-2, x, y, selected_piece)){
					if(chessTable[x+1][y-2] == 0 || chessTable[x+1][y-2]/10 != selected_piece/10)
						legalMoves[x+1][y-2] = 1;
				}
			}

			if(x < 7 && y < 6){
				if(!isKingAttacked(x+1, y+2, x, y, selected_piece)){
					if(chessTable[x+1][y+2] == 0 || chessTable[x+1][y+2]/10 != selected_piece/10)
						legalMoves[x+1][y+2] = 1;
				}
			}

			if(x > 1 && y > 0){
				if(!isKingAttacked(x-2, y-1, x, y, selected_piece)){
					if(chessTable[x-2][y-1] == 0 || chessTable[x-2][y-1]/10 != selected_piece/10)
						legalMoves[x-2][y-1] = 1;
				}
			}

			if(x > 1 && y < 7){
				if(!isKingAttacked(x-2, y+1, x, y, selected_piece)){
					if(chessTable[x-2][y+1] == 0 || chessTable[x-2][y+1]/10 != selected_piece/10)
						legalMoves[x-2][y+1] = 1;
				}
			}


			if(x < 6 && y > 0){
				if(!isKingAttacked(x+2, y-1, x, y, selected_piece)){
					if(chessTable[x+2][y-1] == 0 || chessTable[x+2][y-1]/10 != selected_piece/10)
						legalMoves[x+2][y-1] = 1;
				}
			}

			if(x < 6 && y < 7){
				if(!isKingAttacked(x+2, y+1, x, y, selected_piece)){
					if(chessTable[x+2][y+1] == 0 || chessTable[x+2][y+1]/10 != selected_piece/10)
						legalMoves[x+2][y+1] = 1;
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
				if(cursor_y == i && cursor_x == j)
					drawingCursor(79 + j * 19, 20 + i * 25, 98 + j * 19, 45 + i * 25, 2);
				else
					drawingCursor(79 + j * 19, 20 + i * 25, 98 + j * 19, 45 + i * 25, 1);
			}
		}
	}
}


void initLegalMoves(){
	int i,j;
		for (i = 0; i < 8; i++){
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
			if (mod == 0)
				VGA_PERIPH_MEM_mWriteMemory(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ i * 4, COL_CURSOR);
			else if (mod == 2)
				VGA_PERIPH_MEM_mWriteMemory(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ i * 4, COL_CURSOR_SELECT);
			else 
				VGA_PERIPH_MEM_mWriteMemory(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ i * 4, COL_LEGAL_MOVE);
		}
	}

	for (x = startX; x < endX; x++) {
		for (y = endY - 2; y < endY; y++) {
			i = y * 320 + x;
			if (mod == 0)
				VGA_PERIPH_MEM_mWriteMemory(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ i * 4, COL_CURSOR);
			else if (mod == 2)
				VGA_PERIPH_MEM_mWriteMemory(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ i * 4, COL_CURSOR_SELECT);
			else 
				VGA_PERIPH_MEM_mWriteMemory(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ i * 4, COL_LEGAL_MOVE);

		}
	}

	for (x = startX; x < startX + 2; x++) {
		for (y = startY; y < endY; y++) {
			i = y * 320 + x;
			if (mod == 0)
				VGA_PERIPH_MEM_mWriteMemory(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ i * 4, COL_CURSOR);
			else if (mod == 2)
				VGA_PERIPH_MEM_mWriteMemory(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ i * 4, COL_CURSOR_SELECT);
			else 
				VGA_PERIPH_MEM_mWriteMemory(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ i * 4, COL_LEGAL_MOVE);

		}
	}

	for (x = endX - 2; x < endX; x++) {
		for (y = startY; y < endY; y++) {
			i = y * 320 + x;
			if (mod == 0)
				VGA_PERIPH_MEM_mWriteMemory(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ i * 4, COL_CURSOR);
			else if (mod == 2)
				VGA_PERIPH_MEM_mWriteMemory(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ i * 4, COL_CURSOR_SELECT);
			else 
				VGA_PERIPH_MEM_mWriteMemory(XPAR_VGA_PERIPH_MEM_0_S_AXI_MEM0_BASEADDR + GRAPHICS_MEM_OFF
							+ i * 4, COL_LEGAL_MOVE);

		}
	}

}

//function that controls switches and buttons

void move() {

	int startX = 79, startY = 20, endX = 98, endY = 45;
	int oldStartX, oldStartY, oldEndX, oldEndY, fromX, fromY;
	int ic, ib, i, j;
	int prethodnoStanje;
	char figure = 0;
	typedef enum {
		NOTHING_PRESSED, SOMETHING_PRESSED
	} btn_state_t;
	btn_state_t btn_state = NOTHING_PRESSED;

	typedef enum{
		NONE_PICKED, FIGURE_SELECTED
	} pick_mode;

	typedef enum{
		WHITE, BLACK, EOG
	} player_turn;

	pick_mode mode = NONE_PICKED;
	player_turn player = WHITE;
	cursor_x = 0;
	cursor_y = 0;
	drawingCursor(startX, startY, endX, endY, 0);

	while (1) {
		if (btn_state == NOTHING_PRESSED) {
			btn_state = SOMETHING_PRESSED;
			if(legalMoves[cursor_y][cursor_x] == 0)
				drawingCursor(startX, startY, endX, endY, 0);
			else if(legalMoves[cursor_y][cursor_x] == 1)
				drawingCursor(startX, startY, endX, endY, 2);
			markLegalMoves();
			if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & DOWN) == 0) {
				if (endY < 196) {
					oldStartY = startY;
					oldEndY = endY;
					startY += 25;
					endY += 25;
					cursor_y++;
				}

			}

			else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & RIGHT) == 0) {
				if (endX < 213) {
					oldStartX = startX;
					startX += 19;
					endX += 19;
					cursor_x++;

				}
			} else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & LEFT) == 0) {
				if (startX > 97) {
					oldStartX = startX;
					startX -= 19;
					endX -= 19;
					cursor_x--;
				}

			} else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & UP) == 0) {
				if (startY > 44) {
					oldStartY = startY;
					startY -= 25;
					endY -= 25;
					cursor_y--;
				}

			} else if ((Xil_In32(XPAR_MY_PERIPHERAL_0_BASEADDR) & CENTER)
					== 0) {

					if(mode == NONE_PICKED){
						if(player == WHITE){
							if(chessTable[cursor_y][cursor_x]/10 == 0 && chessTable[cursor_y][cursor_x] != 0){
								mode = FIGURE_SELECTED;
								figure = chessTable[cursor_y][cursor_x];
								fromX = cursor_x;
								fromY = cursor_y;
								initLegalMoves();
								findLegalMoves(cursor_y, cursor_x);
							}
						}
						else if(player == BLACK){
							if(chessTable[cursor_y][cursor_x]/10 == 1 && chessTable[cursor_y][cursor_x] != 0){
								mode = FIGURE_SELECTED;
								figure = chessTable[cursor_y][cursor_x];
								fromX = cursor_x;
								fromY = cursor_y;
								initLegalMoves();
								findLegalMoves(cursor_y, cursor_x);
							}
						}

						else if(player == EOG){
							initTableMatrix();
							player = WHITE;
						}
					}
					else if(mode == FIGURE_SELECTED){
						if(legalMoves[cursor_y][cursor_x] == 1){
							mode = NONE_PICKED;
							chessTable[cursor_y][cursor_x] = figure;
							chessTable[fromY][fromX] = 0;
							initLegalMoves();
							if(player == WHITE)
								player = BLACK;
							else if(player == BLACK)
								player = WHITE;

							int i;
							for(i = 0; i < 8; i++){
								if(chessTable[0][i]%10 == 1){
									player = EOG;
								}
							}
						}

						if(cursor_y == fromY && cursor_x == fromX){
							mode = NONE_PICKED;
							initLegalMoves();
						}
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
					== 0) {}
			 else {
				btn_state = NOTHING_PRESSED;
			}
		}

	}

}

int main() {


	init_platform();



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


	cleanup_platform();

	return 0;
}
