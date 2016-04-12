#include "boardutils.h"
#include "rules.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "castling.h"


bool white_turn = true;

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
	if ((is_capture && !(mvmt&CAPT)) ||
		(!is_capture && !(mvmt&MOVE)))
		return false;

	while ((fc >= 'A' && fc <= 'H' && fr >= 1 && fr <= 8) && (tc >= 'A' && tc <= 'H' && tr >= 1 && tr <= 8))
	{
		fc += stepc;
		fr += stepr;

		if (fc == tc && fr == tr)
		{
			return CBD(tc, tr) == EMP || is_capture;
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

#define MT_ILLEGAL 0
#define MT_MOVE 1
#define MT_CASTLE_LNG 2
#define MT_CASTLE_SRT 3
#define MT_EMP 4
#define MT_PROMO 5

int get_move_type(char fc, int fr, char tc, int tr)
{
	if ((iscol(fc, fr, true) && iscol(tc, tr, true)) || (iscol(fc, fr, false) && iscol(tc, tr, false)))
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




int make_move(char fc, int fr, char tc, int tr)
{
	fc = toupper(fc);
	tc = toupper(tc);
	if (fc<'A' || fc>'H' || tc<'A' || tc>'H' || fr > 8 || fr < 1 || tr>8 || tr < 1)
	{
		return MV_INVALID;
	}
	int mv_type = get_move_type(fc, fr, tc, tr);

	if (mv_type == MT_MOVE)
	{
		CBD(tc, tr) = CBD(fc, fr);
		CBD(fc, fr) = EMP;
		white_turn = !white_turn;
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
		white_turn = !white_turn;
		return MV_OK;
	}
	return MV_ILLEGAL;

}