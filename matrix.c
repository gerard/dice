#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "matrix.h"

int is_dice(dice d) {
	if(d.p_left==0) return FALSE;
	else return TRUE;
}

int dice_top(dice d) {
	return d.p_top;
}

int dice_left(dice d) {
	return d.p_left;
}

int dice_right(dice d) {
	return d.p_right;
}

int dice_rtop(dice d) {
	return d.r_top;
}

int dice_rleft(dice d) {
	return d.r_left;
}

int dice_rright(dice d) {
	return d.r_right;
}

/*
 * A part de canviar les cares, necessitem canviar
 * l'orientaci— de les cares per exemple aquest 2:
 *
 *   *    *
 *  *  ->  *
 *
 * Les cares problematiques son el 2 el 3 i el 6, la
 * resta son totalment simŽtriques
 */
void dice_rxp(dice *d) {
	int t;

	t=d->p_left;
	d->p_left=7-(d->p_top);
	d->p_top=t;
	
	d->r_top=d->r_left;
	d->r_right=(d->r_right+1)%2;
}

void dice_rxm(dice *d) {
	int t;

	t=d->p_left;
	d->p_left=d->p_top;
	d->p_top=7-t;
	
	d->r_left=d->r_top;
	d->r_right=(d->r_right+1)%2;
}

void dice_ryp(dice *d) {
	int t;

	t=d->p_right;
	d->p_right=7-(d->p_top);
	d->p_top=t;
	
	d->r_top=d->r_right;
	d->r_left=(d->r_left+1)%2;
}

void dice_rym(dice *d) {
	int t;

	t=d->p_right;
	d->p_right=d->p_top;
	d->p_top=7-t;
	
	d->r_right=d->r_top;
	d->r_left=(d->r_left+1)%2;
}

void putdice(dice **mat, dice d, int x, int y) {
	mat[x][y]=d;
}

void deldice(dice **mat, int x, int y) {
	mat[x][y].p_left=0;
}

dice randdice() {
	dice d;
	
	d.p_top=1;
	d.p_left=2;
	d.p_right=3;
	
	d.r_top=0;
	d.r_left=0;
	d.r_right=0;
	
	return d;
}

dice **dice_randmat(int perc) {
	int i, j;
	dice **mat;
	
	mat=(dice **)malloc(sizeof(dice *)*MATSIZE);
	for(i=0; i<MATSIZE; i++) mat[i]=(dice *)malloc(sizeof(dice)*MATSIZE);
	
	srandom(time(NULL));
	for(i=0; i<MATSIZE; i++) {
		for(j=0; j<MATSIZE; j++) {
			// Si superem certa probabilitat, situem un dau a la matriu
			if(random()%100<perc) mat[i][j]=randdice();
			else deldice(mat, i, j);
		}
	}
	
	// La posici— inicial deu estar buida
	deldice(mat, 0, 0);
	
	return mat;
}
