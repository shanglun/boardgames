#define EMP ' '
#define SZ 8



extern char bd[SZ][SZ];
#define CBD(colchar,rowint) bd[8 - rowint][char_to_int(colchar)]

extern int char_to_int(char col);
extern char int_to_char(int col);
extern bool iscol(char col, int row, bool white);
extern void save_bd();
extern void restore_bd();
extern bool find_piece(char pc, char &col, int& row);