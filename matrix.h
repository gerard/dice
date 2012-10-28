#define MATSIZE 8
#define TRUE 1
#define FALSE 0

typedef struct _dice {
	int p_top;
	int p_left;
	int p_right;
	
	int r_top;
	int r_left;
	int r_right;
} dice;

int is_dice(dice d);

void putdice(dice **mat, dice d, int x, int y);
void deldice(dice **mat, int x, int y);

void dice_rym(dice *d);
void dice_rym(dice *d);
void dice_rym(dice *d);
void dice_rym(dice *d);

int dice_top(dice d);
int dice_left(dice d);
int dice_right(dice d);
int dice_rtop(dice d);
int dice_rleft(dice d);
int dice_rright(dice d);

dice randdice();
dice **dice_randmat(int perc);
