/*
lower case represents black pieces, upper case represents white pieces
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "rules.h"

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

#include <vector>
#include <string>

using namespace std;

#define KEEP_GOING 0
#define WHITE_WON 1
#define BLACK_WON 2
#define DRAW 3

int main(int argc, char ** argv)
{
	init_bd();
	vector<string> queued_moves;
	if (argc >= 2)
	{
		char * file = argv[1];
		FILE* fp_moves = fopen(file,"r");
		if (fp_moves)
		{
			char buf[255]; 
			while (fgets(buf, sizeof buf, fp_moves))
			{
				queued_moves.push_back(buf);
			}
			fclose(fp_moves);
		}
	}

	print_bd();
	int over = KEEP_GOING;
	while (over == KEEP_GOING)
	{
		char fc, tc;
		int fr, tr;
		if (queued_moves.size() == 0)
		{
			scanf("%c%d%c%d", &fc, &fr, &tc, &tr);
			getc(stdin); //consume newline 
		}
		else
		{
			string move = queued_moves.at(0);
			fc = move.at(0);
			fr = move.at(1) - '0';
			tc = move.at(2);
			tr = move.at(3) - '0';
			queued_moves.erase(queued_moves.begin());
		}
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
		}
		

	}
	return 0;
}