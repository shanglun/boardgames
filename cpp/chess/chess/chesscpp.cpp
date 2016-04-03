/*
lower case represents black pieces, upper case represents white pieces
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define EMP ' '
#define SZ 8
char bd[SZ][SZ];
bool white_turn = true;

int char_to_int(char col)
{
	return toupper(col) - 'A';
}

#define CBD(colchar,rowint) bd[8 - rowint][char_to_int(colchar)]

bool iscol(char col, int row, bool white)
{
	
	if (col < 'A' || col > 'H' || row < 1 || row > 8)
	{
		return false;
	}
	char pc = CBD(col,row);
	return (isupper(pc) == white) && pc != EMP;
}

void set_row(int row, const char* line)
{
	for (int i = 0; i < SZ && i < strlen(line); i++)
	{
		bd[row][i] = line[i];
	}
}

void init_bd()
{
	set_row(0, "rnbqkbnr");
	set_row(1, "pppppppp");
	for (int i = 2; i < SZ - 2; i++)
	{
		set_row(i, "        ");
	}
	set_row(6, "PPPPPPPP");
	set_row(7, "RNBQKBNR");
}

void print_bd()
{
	printf("\n    A B C D E F G H\n");
	printf("   -----------------\n");
	for (int i = 0; i < SZ; i++)
	{
		printf("%d  |", 8 - i);
		for (int j = 0; j < SZ; j++)
		{
			printf("%c|", bd[i][j]);
		}
		printf("\n   -----------------\n");
	}
}

bool within_mvt(char fc, int fr, char tc, int tr, int stepc, int stepr, bool multistep)
{
	/*
	fc - from-column A-H
	fr - from-row 1-8
	stepc - mvt per step in column
	stepr - mvt per step in row
	multistep - just move once, or keep moving until finding obstacles
	*/

	while ((fc >= 'A' && fc <= 'H' && fr >= 1 && fr <= 8) && (tc >= 'A' && tc <= 'H' && tr >= 1 && tr <= 8))
	{
		fc += stepc;
		fr += stepr;
		
		if (fc == tc && fr == tr)
			return true;
		if (CBD(fc, fr) != EMP)
		{
			break;
		}

		if (!multistep)
		{
			break;
		}
	} 
	return false;
}

#define MVCHK(stepc,stepr,mstep) within_mvt(fc,fr,tc,tr,stepc,stepr,mstep)
#define MULTCHK(stepc,stepr,mstep) (MVCHK(stepc,stepr,mstep)||MVCHK(- stepc,stepr,mstep)||MVCHK(stepc,-stepr,mstep) ||MVCHK(- stepc, - stepr, mstep))

bool is_legal(char fc, int fr, char tc, int tr)
{
	if ((iscol(fc, fr,true) && iscol(tc, tr,true)) || (iscol(fc, fr,false) && iscol(tc, tr,false)))
	{
		return false; 
	}
	if (!iscol(fc, fr, white_turn)) return false; 
	switch (toupper(CBD(fc, fr)))
	{
	case 'P':
		//emp check 
		//cap chk
		break;
	case 'N':
		return MULTCHK(2, 1, false) || MULTCHK(1, 2, false);
		break;
	case 'B':
		return MULTCHK(1, 1, true);
		break;
	case 'R':
		return MULTCHK(1, 0, true) || MULTCHK(0, 1, true);
		break;
	case 'Q':
		return MULTCHK(1, 0, true) || MULTCHK(0, 1, true) || MULTCHK(1, 1, true);
		break;
	case 'K':
		return MULTCHK(1, 0, false) || MULTCHK(0, 1, false) || MULTCHK(1, 1, false);
		//castle check
		break; 
	case ' ':
		return false; 
	default:
		return false; 
	}
	return true;
}

#define MV_OK 0
#define MV_INVALID 1
#define MV_ILLEGAL 2

int make_move(char fc, int fr, char tc, int tr)
{
	fc = toupper(fc);
	tc = toupper(tc);
	if (fc<'A' || fc>'H' || tc<'A' || tc>'H' || fr > 8 || fr < 1 || tr>8 || tr < 1)
	{
		return MV_INVALID;
	}
	if (is_legal(fc, fr, tc, tr))
	{
		CBD(tc, tr) = CBD(fc, fr);
		CBD(fc, fr) = EMP;
		return MV_OK;
	}
	return MV_ILLEGAL;
	
}

#define KEEP_GOING 0
#define WHITE_WON 1
#define BLACK_WON 2
#define DRAW 3

int main()
{
	init_bd();
	print_bd();
	int over = KEEP_GOING;
	while (over == KEEP_GOING)
	{
		char fc, tc;
		int fr, tr;
		scanf("%c%d%c%d", &fc, &fr, &tc, &tr);
		getc(stdin); //consume newline 
		int res = make_move(fc,fr,tc,tr);
		if (res == MV_ILLEGAL)
		{
			printf("ILLEGAL MOVE. Please choose another move. \n");
		}
		else if (res == MV_INVALID)
		{
			printf("INVALID MOVE. Please choose another move. \n");
		}
		else if (res == MV_OK)
		{
			print_bd();
			white_turn = !white_turn;
		}
		

	}
	return 0;
}