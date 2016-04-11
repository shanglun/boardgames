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

char int_to_char(int col)
{
	return col + 'A'; 
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

#define CAPT 0x00000001
#define MOVE 0x00000010
#define CPMV 0x00000011
#define EMPS 0x00000100 //en passant check

bool within_mvt(char fc, int fr, char tc, int tr, int stepc, int stepr, bool multistep, int mvmt)
{
	/*
	fc - from-column A-H
	fr - from-row 1-8
	stepc - mvt per step in column
	stepr - mvt per step in row
	multistep - just move once, or keep moving until finding obstacles
	*/
	bool is_capture = iscol(fc, fr, true) != iscol(tc, tr, true);
	if ( (is_capture && !mvmt&CAPT) ||
		(!is_capture && !mvmt&MOVE))
		return false;

	while ((fc >= 'A' && fc <= 'H' && fr >= 1 && fr <= 8) && (tc >= 'A' && tc <= 'H' && tr >= 1 && tr <= 8))
	{
		fc += stepc;
		fr += stepr;
		
		if (fc == tc && fr == tr)
		{
			return CBD(tc,tr) == EMP || is_capture; 
		}
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

//set to false when king moves, rook moves, or castles
bool white_can_castle_short = true,
	white_can_castle_long = true, 
	black_can_castle_short = true, 
	black_can_castle_long = true;

bool clear_for_castle(bool white,  bool long_castle)
{
	int op_row = white ? 1 : 8;
	for (int i = long_castle ? 'E' - 1 : 'E' + 1; i > 'A' && i < 'H'; i += long_castle ? -1 : 1)
	{
		if (CBD(i, op_row) != EMP)
			return false;
	}
	return (CBD(long_castle ? 'A' : 'H', op_row) == 'R'); //make sure a rook is around.
}

bool castle_check(char fc, int fr, char tc, int tr, bool white)
{
	int op_row = white ? 1 : 8;
	bool long_castle = fc - tc > 0; 
	if ((white && long_castle && !white_can_castle_long) ||
		(white && !long_castle && !white_can_castle_short) ||
		(!white && long_castle && !black_can_castle_long) ||
		(!white && !long_castle && !black_can_castle_short))
		return false;
	if (fr != op_row || tr != op_row || fc != 'E' || abs(fc-tc)!=2)
		return false;
	if (!clear_for_castle(white, long_castle))
		return false;
	//can't castle into check, out of check, or through check.
	return true;
}


#define MVCHK(stepc,stepr,mstep) within_mvt(fc,fr,tc,tr,stepc,stepr,mstep, CPMV)
#define MULTCHK(stepc,stepr,mstep) (MVCHK(stepc,stepr,mstep)||MVCHK(- stepc,stepr,mstep)||MVCHK(stepc,-stepr,mstep) ||MVCHK(- stepc, - stepr, mstep))
#define KNCHK MULTCHK(2, 1, false) || MULTCHK(1, 2, false)
#define BSCHK MULTCHK(1, 1, true)
#define RKCHK MULTCHK(1, 0, true) || MULTCHK(0, 1, true)
#define QNCHK BSCHK || RKCHK
#define KGCHK MULTCHK(1, 0, false) || MULTCHK(0, 1, false) || MULTCHK(1, 1, false)

bool pawn_check(char fc, int fr, char tc, int tr, bool white)
{

	return true;
}

#define MT_ILLEGAL 0
#define MT_MOVE 1
#define MT_CASTLE_LNG 2
#define MT_CASTLE_SRT 3
#define MT_EMP 4
#define MT_PROMO 5


int get_move_type(char fc, int fr, char tc, int tr)
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
		return KNCHK ? MT_MOVE : MT_ILLEGAL;
		break;
	case 'B':
		return BSCHK ? MT_MOVE : MT_ILLEGAL;
		break;
	case 'R':
		return RKCHK ? MT_MOVE : MT_ILLEGAL;
		break;
	case 'Q':
		return QNCHK ? MT_MOVE : MT_ILLEGAL;
		break;
	case 'K':
		if (KGCHK)
			return MT_MOVE;
		if (castle_check(fc, fr, tc, tr, white_turn))
		{
			return tc == 'G' ? MT_CASTLE_SRT : MT_CASTLE_LNG;
		}
		break; 
	case ' ':
		return MT_ILLEGAL; 
	default:
		return MT_ILLEGAL; 
	}
	return MT_MOVE;
}

bool in_check(bool is_white)
{
	char tc, fc;
	int tr, fr;
	if (!find_piece(is_white ? 'K' : 'k', tc, tr)) return false;
	for (int i = 0; i < SZ; i++)
	{
		for (int j = 0; j < SZ; j++)
		{
			fc = int_to_char(j);
			fr = i + 1;
			if (iscol(fc, fr, !is_white))
			{
				switch (toupper(bd[i][j]))
				{
				case 'P':
					break;
				case 'N':
					if (KNCHK)
						return true;
					break;
				case 'B':
					if (BSCHK)
						return true;
					break;
				case 'R':
					if (RKCHK)
						return true;
					break;
				case 'Q':
					if (QNCHK)
						return true;
					break;
				case 'K':
					if (KGCHK)
						return true;
				default:
					break;
				}
			}
		}
	}
	return false;
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
	int mv_type = get_move_type(fc, fr, tc, tr);

	if (mv_type==MT_MOVE)
	{
		CBD(tc, tr) = CBD(fc, fr);
		CBD(fc, fr) = EMP;
		return MV_OK;
	}
	else if (mv_type == MT_CASTLE_SRT || mv_type == MT_CASTLE_LNG)
	{
		char rook_from = mv_type == MT_CASTLE_LNG ? 'A' : 'H',
			rook_to = mv_type == MT_CASTLE_LNG ? 'D' : 'F';
		int op_row = white_turn ? 1 : 8;
		CBD(tc, tr) = CBD(fc, fr);
		CBD(fc, fr) = EMP;
		//move the rook!
		CBD(rook_to, op_row) = CBD(rook_from, op_row);
		CBD(rook_from, op_row) = EMP;
		if (white_turn)
		{
			white_can_castle_long = white_can_castle_short = false;
		}
		else
		{
			black_can_castle_long = black_can_castle_short = false;
		}
		return MV_OK;
	}
	return MV_ILLEGAL;
	
}

#define KEEP_GOING 0
#define WHITE_WON 1
#define BLACK_WON 2
#define DRAW 3

#include <vector>
#include <string>

using namespace std;

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
			white_turn = !white_turn;
		}
		

	}
	return 0;
}