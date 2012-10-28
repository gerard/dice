#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <unistd.h>

#include <GL/glut.h>


#include "matrix.h"

#define WIDE 8
#define LONG 8
#define DIST 8

#define DELAY 8000
#define CHAOS (float)(random()%100)/1000

#define X_PLUS 1
#define X_MINUS 2
#define Y_PLUS 3
#define Y_MINUS 4

static int anglex, angley;
static int x, y;
static int dx, dy;
static dice **dicemat;
static pthread_mutex_t mov_mutex;
static dice playdice;

void draw_point() {
	glBegin(GL_POLYGON);
		glNormal3f(0, 0, -1);
		glVertex3f(-0.04, 0.08, 0);
		glNormal3f(0, 0, -1);
		glVertex3f(-0.08, 0.04, 0);
		glNormal3f(0, 0, -1);
		glVertex3f(-0.08, -0.04, 0);
		glNormal3f(0, 0, -1);
		glVertex3f(-0.04, -0.08, 0);
		glNormal3f(0, 0, -1);
		glVertex3f(0.04, -0.08, 0);
		glNormal3f(0, 0, -1);
		glVertex3f(0.08, -0.04, 0);
		glNormal3f(0, 0, -1);
		glVertex3f(0.08, 0.04, 0);
		glNormal3f(0, 0, -1);
		glVertex3f(0.04, 0.08, 0);
	glEnd();
}

void draw_face(int num, int r) {
	float mat_diffuse[4];
	mat_diffuse[3]=1;
	
	glPushMatrix();
		if(r) {
			glTranslatef(0.5, 0.5, 0);
			glRotatef(90, 0, 0, 1);
			glTranslatef(-0.5, -0.5, 0);
		}
	
		switch(num) {
		case 1:
			mat_diffuse[0]=1;
			mat_diffuse[1]=1;
			mat_diffuse[2]=0.5;
			//glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);  
			glTranslatef(0.5, 0.5, 0);
			draw_point();
			break;
		case 2:
			mat_diffuse[1]=0.5;
			mat_diffuse[2]=1;
			//glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);  
			glTranslatef(0.2, 0.2, 0);
			draw_point();
			glTranslatef(0.6, 0.6, 0);
			draw_point();
			break;
		case 3:
			mat_diffuse[0]=0.5;
			mat_diffuse[1]=1;
			//glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);  
			glTranslatef(0.2, 0.2, 0);
			draw_point();
			glTranslatef(0.3, 0.3, 0);
			draw_point();
			glTranslatef(0.3, 0.3, 0);
			draw_point();
			break;
		case 4:
			mat_diffuse[0]=1;
			mat_diffuse[1]=0.5;
			mat_diffuse[2]=0.5;
			//glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);  
			glPushMatrix();
				glTranslatef(0.8, 0.8, 0);
				draw_point();
			glPopMatrix();
			glPushMatrix();
				glTranslatef(0.2, 0.8, 0);
				draw_point();
			glPopMatrix();
			glPushMatrix();
				glTranslatef(0.8, 0.2, 0);
				draw_point();
			glPopMatrix();
			glPushMatrix();
				glTranslatef(0.2, 0.2, 0);
				draw_point();
			glPopMatrix();
			break;
		case 5:
			mat_diffuse[0]=0.5;
			mat_diffuse[1]=1;
			//glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);  
			glPushMatrix();
				glTranslatef(0.5, 0.5, 0);
				draw_point();
			glPopMatrix();
			draw_face(4, 0);
			break;
		case 6:
			mat_diffuse[1]=0.5;
			mat_diffuse[2]=1;
			//glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);  
			glPushMatrix();
				glTranslatef(0.5, 0.2, 0);
				draw_point();
			glPopMatrix();
			glPushMatrix();
				glTranslatef(0.5, 0.8, 0);
				draw_point();
			glPopMatrix();
			draw_face(4, 0);
			break;
		default:
			break;
		}
	glPopMatrix();
}

void draw_playdice() {
	// Dau actiu
	glPushMatrix();
		// Rotacions sobre X
		glTranslatef(x+1-dx, 0, 0);
		glRotatef(anglex, 0, 1, 0);
		glTranslatef(-x-1+dx, 0, 0);
		
		// Rotacions sobre Y
		glTranslatef(0, y+1-dy, 0);
		glRotatef(angley, 1, 0, 0);
		glTranslatef(0, -y-1+dy, 0);
		
		// Situaci— de la fitxa al tauler
		glTranslatef(x, y, 0);
		
		// Cub en la posici— (0,0)
		glPushMatrix();
			glTranslatef(0.5, 0.5, 0.5);
			glutSolidCube(1);
		glPopMatrix();
		
		// Cares del dau
		glPushMatrix();
			glTranslatef(0, 0, 1);
			draw_face(dice_top(playdice), dice_rtop(playdice));
		glPopMatrix();
		glPushMatrix();
			glRotatef(-90, 0, 1, 0);
			draw_face(dice_left(playdice), dice_rleft(playdice));
		glPopMatrix();
		glPushMatrix();
			glRotatef(90, 1, 0, 0);
			draw_face(dice_right(playdice), dice_rright(playdice));
		glPopMatrix();
	glPopMatrix();
}

void display(void) {
	int i, j;
	
	srandom(time(NULL));
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//Inicializaci—n de la proyecci—n
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-6, 6, -4, 8, 0, 10000);

	//Inicializaci—n de la c‡mara
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	
	gluLookAt(-WIDE/2, -LONG/2, DIST, WIDE/2, LONG/2, 0, 0, 0, 1);
	//gluLookAt(4, 4, 10, 4, 4, 0, 0, 1, 0);
	
	// Rejilla
	glColor3f (0, 1, 0);
	for(i=0; i<WIDE; i++) {
		for(j=0; j<LONG; j++) {
			glBegin(GL_QUADS);
				glNormal3f(CHAOS, CHAOS, -1);
				glVertex3f(i, j+1, 0);
				glNormal3f(CHAOS, CHAOS, -1);
				glVertex3f(i+1, j+1, 0);
				glNormal3f(CHAOS, CHAOS, -1);
				glVertex3f(i+1, j, 0);
				glNormal3f(CHAOS, CHAOS, -1);
				glVertex3f(i, j, 0);
			glEnd();
		}
	}
	
	// Daus 
	glColor3f(1, 0, 0);
	for(i=WIDE-1; i>=0; i--)
		for(j=LONG-1; j>=0; j--) {
			if(is_dice(dicemat[i][j])) {
				glPushMatrix();
					glTranslatef(i, j, 0);
					glTranslatef(0.5, 0.5, 0.5);
					glutSolidCube(1);
				glPopMatrix();
			}
			if(x==i && y==j) draw_playdice();
		}
			
	// Cares dels daus
	glColor3f(0, 0, 1);
	for(i=0; i<WIDE; i++)
		for(j=0; j<LONG; j++)
			if(is_dice(dicemat[i][j])) {
				glPushMatrix();
					glTranslatef(i, j, 1); 
					draw_face(dice_top(dicemat[i][j]), dice_rtop(dicemat[i][j]));
				glPopMatrix();
				glPushMatrix();
					glTranslatef(i, j, 0);
					glRotatef(-90, 0, 1, 0);
					draw_face(dice_left(dicemat[i][j]), dice_rleft(dicemat[i][j]));
				glPopMatrix();
				glPushMatrix();
					glTranslatef(i, j, 0);
					glRotatef(90, 1, 0, 0);
					draw_face(dice_right(dicemat[i][j]), dice_rright(dicemat[i][j]));
				glPopMatrix();
			}

	glutSwapBuffers();
}

void reshape (int w, int h)
{
	glutReshapeWindow(w, h);
}

void *mov_x_plus(void *data) {
	// Comprovem que no s'estiga executant ningun moviment,
	// i una vegada estigam dins del mutex, comprovem que
	// no ens sortim del tauler.
	// El comportament es idntic per a la resta de funcions mov_
	if(pthread_mutex_trylock(&mov_mutex)) return NULL;
	if(x>=WIDE-1) {
		pthread_mutex_unlock(&mov_mutex);
		return NULL;
	}
	
	// Si es tracta de un dau, ens mourem all’
	if(is_dice(dicemat[x+1][y])) {
		dicemat[x][y]=playdice;
		playdice=dicemat[x+1][y];
		deldice(dicemat, x+1, y);
	} else { // Moviment normal
		dx=0;
		while(anglex<90) {
			anglex=anglex+2;
			glutPostRedisplay();
			usleep(DELAY);
		}
		dice_rxp(&playdice);
	}

	anglex=0;
	x++;
	
	glutPostRedisplay();
	
	pthread_mutex_unlock(&mov_mutex);
	return NULL;
}

void *mov_x_minus(void *data) {
	if(pthread_mutex_trylock(&mov_mutex)) return NULL;
	if(x<=0) {
		pthread_mutex_unlock(&mov_mutex);
		return NULL;
	}
	
	if(is_dice(dicemat[x-1][y])) {
		dicemat[x][y]=playdice;
		playdice=dicemat[x-1][y];
		deldice(dicemat, x-1, y);
	} else {
		dx=1;
		while(anglex>-90) {
			anglex=anglex-2;
			glutPostRedisplay();
			usleep(DELAY);
		}
		dice_rxm(&playdice);
	}
	
	anglex=0;
	x--;
	
	glutPostRedisplay();
	
	pthread_mutex_unlock(&mov_mutex);
	return NULL;
}

void *mov_y_plus(void *data) {
	if(pthread_mutex_trylock(&mov_mutex)) return NULL;
	if(y>=LONG-1) {
		pthread_mutex_unlock(&mov_mutex);
		return NULL;
	}
	
	if(is_dice(dicemat[x][y+1])) {
		dicemat[x][y]=playdice;
		playdice=dicemat[x][y+1];
		deldice(dicemat, x, y+1);
	} else {
		dy=0;
		while(angley>-90) {
			angley=angley-2;
			glutPostRedisplay();
			usleep(DELAY);
		}
		dice_ryp(&playdice);
	}
	
	// El dau ja ha mogut completament
	angley=0;
	y++;
	
	glutPostRedisplay();
	
	pthread_mutex_unlock(&mov_mutex);
	return NULL;
}

void *mov_y_minus(void *data) {
	if(pthread_mutex_trylock(&mov_mutex)) return NULL;
	if(y<=0) {
		pthread_mutex_unlock(&mov_mutex);
		return NULL;
	}
	
	if(is_dice(dicemat[x][y-1])) {
		dicemat[x][y]=playdice;
		playdice=dicemat[x][y-1];
		deldice(dicemat, x, y-1);
	} else {
		dy=1;
		while(angley<90) {
			angley=angley+2;
			glutPostRedisplay();
			usleep(DELAY);
		}
		dice_rym(&playdice);
	}
	
	// El dau ja ha mogut completament
	angley=0;
	y--;
	
	glutPostRedisplay();
	
	pthread_mutex_unlock(&mov_mutex);
	return NULL;
}

void keyboard (unsigned char key, int xx, int yy)
{
	// Usem threads per aconseguir exclusi— mutua i evitar dos moviments simultanis
	pthread_t thread_a, thread_d, thread_w, thread_s;
	pthread_attr_t thread_attr;
	
	// Els fils son desatesos
	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	
	switch (key) {
		case 'a':
			pthread_create(&thread_a, &thread_attr, mov_y_plus, NULL);
			break;
		case 'd':
			pthread_create(&thread_d, &thread_attr, mov_y_minus, NULL);
			break;
		case 'w':
			pthread_create(&thread_w, &thread_attr, mov_x_plus, NULL);
			break;
		case 's':
			pthread_create(&thread_s, &thread_attr, mov_x_minus, NULL);
			break;
		case 27:
			exit(0);
			break;
		default:
			break;
	}
}

void scene_light() {
	float light_position1[] = {1, 1, 1, 1};
	float mat_diffuse[] = {1, 1, 1, 1};
	
	glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);  
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void idle() {
	glutPostRedisplay();
	usleep(DELAY);
}

int main(int argc, char** argv)
{
	dicemat=dice_randmat(25);
	playdice=randdice();
	pthread_mutex_init(&mov_mutex, NULL);

	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (500, 500); 
	glutInitWindowPosition (100, 100);
	glutCreateWindow ("Helldice");
	scene_light();
	
	glutDisplayFunc(display); 
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();
	
	return 0;
}
