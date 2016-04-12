#include "boardutils.h"
#include "castling.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

//set to false when king moves, rook moves, or castles
bool white_can_castle_short = true,
white_can_castle_long = true,
black_can_castle_short = true,
black_can_castle_long = true;

bool clear_for_castle(bool white, bool long_castle)
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
	if (fr != op_row || tr != op_row || fc != 'E' || abs(fc - tc) != 2)
		return false;
	if (!clear_for_castle(white, long_castle))
		return false;
	//can't castle into check, out of check, or through check.
	return true;
}