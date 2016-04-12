#include "boardutils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

char bd[SZ][SZ], sbd[SZ][SZ]; //save board to test hypothetical boards

int char_to_int(char col)
{
	return toupper(col) - 'A';
}

char int_to_char(int col)
{
	return col + 'A';
}

bool iscol(char col, int row, bool white)
{

	if (col < 'A' || col > 'H' || row < 1 || row > 8)
	{
		return false;
	}
	char pc = CBD(col, row);
	return (isupper(pc) == white) && pc != EMP;
}

void save_bd()
{
	for (int i = 0; i < SZ; i++)
		for (int j = 0; j < SZ; j++)
		{
			sbd[i][j] = bd[i][j];
		}
}

void restore_bd()
{
	for (int i = 0; i < SZ; i++)
		for (int j = 0; j < SZ; j++)
		{
			bd[i][j] = sbd[i][j];
		}
}

bool find_piece(char pc, char &col, int& row)
{
	for (int i = 0; i < SZ; i++)
	{
		for (int j = 0; j < SZ; j++)
		{
			if (bd[i][j] == pc)
			{
				col = int_to_char(j);
				row = i + 1;
				return true;
			}
		}
	}
	return false;
}