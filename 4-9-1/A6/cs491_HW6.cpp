/*

CS 491: Assignment 6 (Keyframe Animation)
Amar Raad
Nov 21st, 2018

*/
#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <ctype.h>

#ifndef WIN32
#include <sys/time.h>
#endif

#ifdef WIN32
#pragma warning(disable:4996)
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"
#include "glui.h"


/* title of these windows:						*/

const char *WINDOWTITLE = { "CS 491 | HW 6 -- Amar Raad" };
const char *GLUITITLE   = { "User Interface Window" };


/* what the glui package defines as true and false:			*/

const int GLUITRUE  = { true  };
const int GLUIFALSE = { false };


/* the escape key:							*/

#define ESCAPE		0x1b


/* initial window size:							*/

const int INIT_WINDOW_SIZE = { 600 };



/* multiplication factors for input interaction:			*/
/*  (these are known from previous experience)				*/

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };


/* minimum allowable scale factor:					*/

const float MINSCALE = { 0.05f };


/* active mouse buttons (or them together):				*/

const int LEFT   = { 4 };
const int MIDDLE = { 2 };
const int RIGHT  = { 1 };


/* which projection:							*/

const int ORTHO = { GLUIFALSE };
const int PERSP = { GLUITRUE  };


/* which button:							*/

#define RESET		0
#define QUIT		1


/* window background color (rgba):					*/

const float BACKCOLOR[] = { 0., 0., 0., 0. };


/* color and line width for the axes:					*/

const float AXES_COLOR[] = { 1., .5, 0. };
const float AXES_WIDTH   = 3.;


/* time in milliseconds for cycle to complete:				*/

const int MSEC = { 10*1000 };


/* handy to have around:						*/

const int OFF = { 0 };
const int ON  = { 1 };

#define FALSE	0
#define TRUE	1




const float BACKGROUND_COLOR[4] = { 0.,0.,0.,0. };
const float KEYFRAME_COLOR[3] = { 0.6f,0.6f,0. };

/* torus size:								*/

const float RADIUS1 = { 50. };
const float RADIUS2 = { 80. };


/**
 ** non-constant global variables:
 **/

int	ActiveButton;		/* current button that is down		*/
int	AnimateOn;		/* TRUE means we are animating		*/
float	Ax, Ay, Az;		/* current angles			*/
int	AxesList;		/* list to hold the axes		*/
int	AxesOn;			/* ON or OFF				*/
int	Debug;			/* ON means print debug info		*/
GLUI *	Glui;			/* instance of glui window		*/
int	GluiWindow;		/* the glut id for the glui window	*/
int	GrWindow;		/* window id for graphics window	*/
float	H;			/* current hue				*/
int	KeyFrameList;		/* list to hold the keyframes		*/
int	KeyFrameMenu;		/* id of the keyframe pop-up menu	*/
int	KeyFrameOn;		/* ON or OFF				*/
int	KeyFrameShadowMenu;	/* id of the keyframe shadow pop-up menu*/
int	KeyFrameShadowOnOff;	/* ON or OFF				*/
int	NowFrame;		/* current frame #			*/
int NowKeyFrame;		// we are between keyframes NowKeyFrame and NowKeyFrame+1
int	ObjectList;		/* list to hold the object		*/
float	RotMatrix[4][4];	/* set by glui rotation widget		*/
float	Scale, Scale2;		/* scaling factors			*/
int	Time0;			/* base elapsed time			*/
int	TimerOn;		/* if TRUE, then finish in 10 seconds	*/
int	WallList;		/* list to hold the wall		*/
int	WhichProjection;	/* ORTHO or PERSP			*/
float	X, Y, Z;		/* current location			*/
float ThetaX, ThetaY, ThetaZ;
int	Xmouse, Ymouse;		/* mouse values				*/
float	Xrot, Yrot;		/* rotation angles in degrees		*/
float	TransXYZ[3];		/* set by glui translation widgets	*/

GLUI_Rotation	*GluiRot;




/* the keyframes:							*/

struct keyframe
{
	int f;				/* frame #			*/
	float x, y, z;			/* x, y, and z locations	*/
	float ax, ay, az;		/* angles in degrees		*/
	float h;			/* object's hue (s=v=1.)	*/
	float dxdf, dydf, dzdf;		/* slopes			*/
	float daxdf, daydf, dazdf;	/* slopes			*/
	float dhdf;			/* slopes			*/
} Frames[] =

{
	//start
	{	0,		170.,	50.,	0.,		0.,		0.,		0.,		0.	},
	
	//in-betweens
	{	50,		170.,	50.,	300.,	0.,		0.,		0.,		0.	},		// grab and
	{	100,	170.,	90.,	300.,	-10.,	-25.,	0.,		0.	},		//   tilt the
	{	200,	170.,	190.,	300.,	-10.,	-10.,	0.,		0.	},		//        teapot
	{	300,	100.,	90.,	0.,		0.,		-20.,	0.,		0.	},		// and then bring to
	{	400,	100.,	500.,	0.,		0.,		0.,		0.,		0.	},		//     pour next.....
	{	500,	100.,	580.,	0.,		0.,		0.,		-45.,	0.	},		// teapot tilts
	{	600,	75.,	650.,	0.,		0.,		0.,		-55.,	0.	},		// tilts more
	{	700,	100.,	500.,	0.,		0.,		0.,		-10.,	0.	},		// finished "pouring"
	//end
	{	800,	170.,	50.,	0.,		0.,		0.,		0.,		0.	},		// puts down & back to restart
	{	-1								}
};


float	White[] = { 1.,1.,1.,1. };	/* handy to have around		*/

/**
 ** function prototypes:
 **/

void	Animate( void );
float *	Array3( float, float, float );
void	Axes( float length );
void	Buttons( int );
void	Display( void );
void	DisplayObj( void );
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
void	DrawGraph( int );
void	HsvRgb( float [3], float [3] );
void	InitGraphics( void );
void	InitLists( void );
void	InitGlui( void );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
float *	MulArray3( float, float [3] );
void	Reset( void );
void	Resize( int, int );
void	SetAnimate( int );
void	Sliders( int );
void	Visibility( int );

int
main( int argc, char *argv[] )
{

	/* turn on the glut package:					*/
	/* (do this before checking argc and argv since it might	*/
	/* pull some command line arguments out)			*/

	glutInit( &argc, argv );

	/* setup all the graphics stuff:				*/

	InitGraphics();

	/* create the display structures that will not change:		*/

	InitLists();

	/* init all the global variables used by Display():		*/
	/* this will also post a redisplay				*/
	/* it is important to call this before InitGlui():		*/
	/* so that the variables that glui will control are correct	*/
	/* when each glui widget is created				*/

	Reset();

	/* setup all the user interface stuff:				*/

	InitGlui();

	/* draw the scene once and wait for some interaction:		*/
	/* (will never return)						*/

	glutMainLoop();


	/* this is here to make the compiler happy:			*/

	return 0;
}



/**
 ** this is where one would put code that is to be called
 ** everytime the glut main loop has nothing to do
 **
 ** this is typically where animation parameters are set
 **
 ** do not call Display() from here -- let glutMainLoop() do it
 **/

void
Animate( void )
{
	struct keyframe *fp, *fpn;		/* current, next frames	*/
	float dx0dt, dy0dt, dz0dt;	/* velocities at pt #0		*/
	float dax0dt, day0dt, daz0dt;	/* velocities at pt #0		*/
	float dh0dt;			/* velocities at pt #0		*/
	float dx1dt, dy1dt, dz1dt;	/* velocities at pt #1		*/
	float dax1dt, day1dt, daz1dt;	/* velocities at pt #1		*/
	float dh1dt;			/* velocities at pt #1		*/
	float ax, bx, cx, dx;   /* x coefficients                       */
	float ay, by, cy, dy;   /* y coefficients                       */
	float az, bz, cz, dz;   /* z coefficients                       */
	float aax, bax, cax, dax;   /* x angle coefficients             */
	float aay, bay, cay, day;   /* y angle coefficients             */
	float aaz, baz, caz, daz;   /* z angle coefficients             */
	float ah, bh, ch, dh;	    /* hue coefficients             	*/
	float t;		/* current parameter			*/
	int foundIt;		/* TRUE means found this frame # in a	*/
				/* keyframe pair			*/
	int msec;		/* elapsed time since start of program	*/
	float maxframes;	/* max # frames in this animation	*/


	/* if the timer is on, figure out what frame number we should be on:	*/

	if( TimerOn )
	{
		for( fp = Frames; fp->f >= 0; fp++ )
		{
			maxframes = fp->f;
		}
		msec = ( glutGet( GLUT_ELAPSED_TIME ) - Time0 )  %  MSEC;
		NowFrame = (float)maxframes * (float)msec / (float)MSEC;
	}

	if( Debug )
		fprintf( stderr, "NowFrame = %3d\n", NowFrame );


	foundIt = FALSE;

	while( ! foundIt )
	{
		dx0dt = 200;
		dy0dt = 200;
		dz0dt = 200;
		dax0dt = 200;
		day0dt = 200;
		daz0dt = 200;
		dh0dt = 200;
		dx1dt = 200;
		dy1dt = 200;
		dz1dt = 200;
		dax1dt = 200;
		day1dt = 200;
		daz1dt = 200;
		dh1dt = 200;

		for( fp = Frames, fpn = fp+1; fpn->f >= 0; fp = fpn, fpn++ )
		{
			if( ( fp == Frames && NowFrame == 0 )    ||    ( fp->f < NowFrame && NowFrame <= fpn->f ) )
			{
				foundIt = TRUE;

				dx0dt = fp->dxdf * ( fpn->f - fp->f );
				dy0dt = fp->dydf * ( fpn->f - fp->f );
				dz0dt = fp->dzdf * ( fpn->f - fp->f );
				dax0dt = fp->daxdf * ( fpn->f - fp->f );
				day0dt = fp->daydf * ( fpn->f - fp->f );
				daz0dt = fp->dazdf * ( fpn->f - fp->f );
				dh0dt = fp->dhdf * ( fpn->f - fp->f );

				dx1dt = fpn->dxdf * ( fpn->f - fp->f );
				dy1dt = fpn->dydf * ( fpn->f - fp->f );
				dz1dt = fpn->dzdf * ( fpn->f - fp->f );
				dax1dt = fpn->daxdf * ( fpn->f - fp->f );
				day1dt = fpn->daydf * ( fpn->f - fp->f );
				daz1dt = fpn->dazdf * ( fpn->f - fp->f );
				dh1dt = fpn->dhdf * ( fpn->f - fp->f );

				ax = 2.*fp->x - 2.*fpn->x + dx0dt + dx1dt;
				ay = 2.*fp->y - 2.*fpn->y + dy0dt + dy1dt;
				az = 2.*fp->z - 2.*fpn->z + dz0dt + dz1dt;
				aax = 2.*fp->ax - 2.*fpn->ax + dax0dt + dax1dt;
				aay = 2.*fp->ay - 2.*fpn->ay + day0dt + day1dt;
				aaz = 2.*fp->az - 2.*fpn->az + daz0dt + daz1dt;
				ah = 2.*fp->h - 2.*fpn->h + dh0dt + dh1dt;

				bx = -3.*fp->x + 3.*fpn->x - 2.*dx0dt - dx1dt;
				by = -3.*fp->y + 3.*fpn->y - 2.*dy0dt - dy1dt;
				bz = -3.*fp->z + 3.*fpn->z - 2.*dz0dt - dz1dt;
				bax = -3.*fp->ax + 3.*fpn->ax - 2.*dax0dt - dax1dt;
				bay = -3.*fp->ay + 3.*fpn->ay - 2.*day0dt - day1dt;
				baz = -3.*fp->az + 3.*fpn->az - 2.*daz0dt - daz1dt;
				bh = -3.*fp->h + 3.*fpn->h - 2.*dh0dt - dh1dt;

				cx = dx0dt;
				cy = dy0dt;
				cz = dz0dt;
				cax = dax0dt;
				cay = day0dt;
				caz = daz0dt;
				ch = dh0dt;

				dx = fp->x;
				dy = fp->y;
				dz = fp->z;
				dax = fp->ax;
				day = fp->ay;
				daz = fp->az;
				dh = fp->h;

				t = (float)( NowFrame - fp->f )  /  (float)( fpn->f - fp->f );
				X = dx + t * ( cx + t * ( bx + t * ax ) );
				Y = dy + t * ( cy + t * ( by + t * ay ) );
				Z = dz + t * ( cz + t * ( bz + t * az ) );
				Ax = dax + t * ( cax + t * ( bax + t * aax ) );
				Ay = day + t * ( cay + t * ( bay + t * aay ) );
				Az = daz + t * ( caz + t * ( baz + t * aaz ) );
				H = dh + t * ( ch + t * ( bh + t * ah ) );
			}
		}

		NowFrame++;
		if( !foundIt )
			NowFrame = 0;
	}


	/* force a call to Display() next time it is convenient:	*/

	glutSetWindow( GrWindow );
	glutPostRedisplay();
}



/**
 ** glui buttons callback:
 **/

void
Buttons( int id )
{
	switch( id )
	{
		case RESET:
			Reset();
			break;

		case QUIT:
			/* gracefully close the glui window:				*/
			/* gracefully close out the graphics:				*/
			/* gracefully close the graphics window:			*/
			/* gracefully exit the program:					*/

			Glui->close();
			glFinish();
			glutDestroyWindow( GrWindow );
			exit( 0 );
	}

	Glui->sync_live();
}



/**
 ** draw the complete scene:
 **/

void
Display( void )
{
	int dx, dy, d;		/* viewport dimensions			*/
	int xl, yb;		/* lower-left corner of viewport	*/
	float scale2;		/* real glui scale factor		*/
	float hsv[3], rgb[4];

	if( Debug )
	{
		fprintf( stderr, "Display\n" );
	}


	/* set which window we want to do the graphics into:		*/

	glutSetWindow( GrWindow );


	/* erase the background:					*/

	glDrawBuffer( GL_BACK );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable( GL_DEPTH_TEST );


	/* specify shading to be flat:					*/

	glShadeModel( GL_FLAT );


	/* set the viewport to a square centered in the window:		*/

	dx = glutGet( GLUT_WINDOW_WIDTH );
	dy = glutGet( GLUT_WINDOW_HEIGHT );
	d = dx < dy ? dx : dy;			/* minimum dimension	*/
	xl = ( dx - d ) / 2;
	yb = ( dy - d ) / 2;
	glViewport( xl, yb,  d, d );


	/* set the viewing volume:					*/
	/* remember that the eye is at the origin looking in -Z		*/
	/* remember that the Z values are actually			*/
	/* given as DISTANCES IN FRONT OF THE EYE			*/

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	if( WhichProjection == ORTHO )
		glOrtho( -800., 800.,     -800., 800.,     0.8, 4000. );
	else
		gluPerspective( 70., 1.,	10., 4000. );


	/* place the object into the viewing volume:			*/

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	gluLookAt( 0., 0., 2000.,	0., 0., 0.,	0., 1., 0. );


	/* light position:						*/

	glLightfv( GL_LIGHT0, GL_POSITION,  Array3( 0., 1000., 0. ) );
	glLightfv( GL_LIGHT1, GL_POSITION,  Array3( 1000., 0., 0. ) );


	/* material color:						*/

	hsv[0] = H;
	hsv[1] = hsv[2] = rgb[3] = 1.;
	HsvRgb( hsv, rgb );
	glMaterialfv( GL_FRONT, GL_AMBIENT, rgb );
	glMaterialfv( GL_FRONT, GL_DIFFUSE, rgb );


	/* rotate the scene:						*/

	glRotatef( Yrot, 0., 1., 0. );
	glRotatef( Xrot, 1., 0., 0. );
	glMultMatrixf( (const GLfloat *) RotMatrix );


	/* scale the scene:						*/

	glScalef( Scale, Scale, Scale );
	scale2 = 1. + Scale2;		/* because glui translation starts at 0. */
	if( scale2 < MINSCALE )
		scale2 = MINSCALE;
	glScalef( scale2, scale2, scale2 );


	/* possibly draw the axes:					*/

	glDisable( GL_LIGHTING );
	glDisable( GL_LIGHT0 );
	glDisable( GL_LIGHT1 );

	if( AxesOn )
		glCallList( AxesList );


	/* translate the object:					*/
	/* note the minus sign on the z value				*/
	/* this is to make the appearance of the glui z translate	*/
	/* widget more intuitively match the translate behavior		*/

	glTranslatef( TransXYZ[0], TransXYZ[1], -TransXYZ[2] );

	glTranslatef( -350., -350., -500. );


	/* the key positions:						*/

	if( KeyFrameOn == ON )
	{
		glColor3fv( KEYFRAME_COLOR );
		glCallList( KeyFrameList );
	}


	/* draw the object:						*/
	// glDepthMask( GL_TRUE );

	glPushMatrix();
		glTranslatef( X, Y, Z );
		glRotatef( Ax,   1., 0., 0. );
		glRotatef( Ay,   0., 1., 0. );
		glRotatef( Az,   0., 0., 1. );
		glEnable( GL_LIGHTING );
		glEnable( GL_LIGHT0 );
		glEnable( GL_LIGHT1 );
		glCallList( ObjectList );
		glDisable( GL_LIGHTING );
		glDisable( GL_LIGHT0 );
		glDisable( GL_LIGHT1 );
	glPopMatrix();



	/* swap the double-buffered framebuffers:			*/

	glutSwapBuffers();


	/* be sure the graphics buffer has been sent:			*/

	glFlush();
}



/**
 ** use glut to display a string of characters using a raster font:
 **/

void
DoRasterString( float x, float y, float z, char *s )
{
	char c;			/* one character to print		*/

	glRasterPos3f( x, y, z );
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}



/**
 ** use glut to display a string of characters using a stroke font:
 **/

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	char c;			/* one character to print		*/
	float sf;		/* the scale factor			*/

	glPushMatrix();
		glTranslatef( x, y, z );
		sf = ht / ( 119.05 + 33.33 );
		glScalef( sf, sf, sf );
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix();
}



/**
 ** initialize the glut and OpenGL libraries:
 **	also setup display lists and callback functions
 **/

void
InitGraphics( void )
{
	struct keyframe *fp;

	if( Debug )
		fprintf( stderr, "InitGraphics\n" );


	/* setup the display mode:					*/
	/* ( *must* be done before call to glutCreateWindow() )		*/
	/* ask for color, double-buffering, and z-buffering:		*/

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );


	/* set the initial window configuration:			*/

	glutInitWindowSize( INIT_WINDOW_SIZE, INIT_WINDOW_SIZE );


	/* open the window and set its title:				*/

	GrWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );


	/* setup the clear values:					*/

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );


	/* setup the callback routines:					*/


	/* DisplayFunc -- redraw the window				*/
	/* ReshapeFunc -- handle the user resizing the window		*/
	/* KeyboardFunc -- handle a keyboard input			*/
	/* MouseFunc -- handle the mouse button going down or up	*/
	/* MotionFunc -- handle the mouse moving with a button down	*/
	/* PassiveMotionFunc -- handle the mouse moving with a button up*/
	/* VisibilityFunc -- handle a change in window visibility	*/
	/* EntryFunc	-- handle the cursor entering or leaving the window */
	/* SpecialFunc -- handle special keys on the keyboard		*/
	/* SpaceballMotionFunc -- handle spaceball translation		*/
	/* SpaceballRotateFunc -- handle spaceball rotation		*/
	/* SpaceballButtonFunc -- handle spaceball button hits		*/
	/* ButtonBoxFunc -- handle button box hits			*/
	/* DialsFunc -- handle dial rotations				*/
	/* TabletMotionFunc -- handle digitizing tablet motion		*/
	/* TabletButtonFunc -- handle digitizing tablet button hits	*/
	/* MenuStateFunc -- declare when a pop-up menu is in use	*/
	/* IdleFunc -- what to do when nothing else is going on		*/
	/* TimerFunc -- trigger something to happen every so often	*/

	glutSetWindow( GrWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );

	// DO NOT SET THE GLUT IDLE FUNCTION HERE !!
	// glutIdleFunc( NULL );
	// let glui take care of it in InitGlui()

	glutTimerFunc( 0, NULL, 0 );


	/* init the lighting:						*/

	glShadeModel( GL_FLAT );
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, MulArray3( .2f, White ) );
	glLightfv( GL_LIGHT0, GL_AMBIENT,   Array3( 0., 0., 0. ) );
	glLightf ( GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1. );
	glLightf ( GL_LIGHT0, GL_LINEAR_ATTENUATION, 0. );
	glLightf ( GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0. );
	glLightfv( GL_LIGHT0, GL_DIFFUSE,   Array3( 1., 1., 1. )  );
	glLightfv( GL_LIGHT0, GL_SPECULAR,  Array3( 1., 1., 1. )  );
	glLightfv( GL_LIGHT1, GL_AMBIENT,   Array3( 0., 0., 0. ) );
	glLightf ( GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1. );
	glLightf ( GL_LIGHT1, GL_LINEAR_ATTENUATION, 0. );
	glLightf ( GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0. );
	glLightfv( GL_LIGHT1, GL_DIFFUSE,   Array3( .8f, .8f, .8f )  );
	glLightfv( GL_LIGHT1, GL_SPECULAR,  Array3( .8f, .8f, .8f )  );
	glEnable( GL_NORMALIZE );
	glLightModeli ( GL_LIGHT_MODEL_TWO_SIDE, TRUE );
	glMaterialf ( GL_FRONT, GL_SHININESS, 8. );
	glMaterialfv( GL_FRONT, GL_SPECULAR, MulArray3( .7f, Array3( 1., 1., 1. )  ) );


	/* init the slopes:						*/

	for( fp = Frames; fp->f >= 0; fp++ )
	{
		if( fp == Frames )
		{
			fp->dxdf = fp->dydf = fp->dzdf = 0.;
			fp->daxdf = fp->daydf = fp->dazdf = 0.;
			fp->dhdf = 0.;
		}
		else
		{
			if( (fp+1)->f < 0 )
			{
				fp->dxdf = fp->dydf = fp->dzdf = 0.;
				fp->daxdf = fp->daydf = fp->dazdf = 0.;
				fp->dhdf = 0.;
			}
			else
			{
				fp->dxdf = ( (fp+1)->x  -  (fp-1)->x ) / ( (fp+1)->f  -  (fp-1)->f );
				fp->dydf = ( (fp+1)->y  -  (fp-1)->y ) / ( (fp+1)->f  -  (fp-1)->f );
				fp->dzdf = ( (fp+1)->z  -  (fp-1)->z ) / ( (fp+1)->f  -  (fp-1)->f );
				fp->daxdf = ( (fp+1)->ax  -  (fp-1)->ax ) / ( (fp+1)->f  -  (fp-1)->f );
				fp->daydf = ( (fp+1)->ay  -  (fp-1)->ay ) / ( (fp+1)->f  -  (fp-1)->f );
				fp->dazdf = ( (fp+1)->az  -  (fp-1)->az ) / ( (fp+1)->f  -  (fp-1)->f );
				fp->dhdf = ( (fp+1)->h  -  (fp-1)->h ) / ( (fp+1)->f  -  (fp-1)->f );
			}
		}
	}
}




/**
 ** initialize the display lists that will not change:
 **/

void
InitLists( void )
{
	struct keyframe *fp;

	if( Debug )
		fprintf( stderr, "InitLists\n" );


	/* create the keyframes:					*/

	KeyFrameList = glGenLists( 1 );
	glNewList( KeyFrameList, GL_COMPILE );
		for( fp = Frames; fp->f >= 0; fp++ )
		{
			glPushMatrix();
				glTranslatef( fp->x, fp->y, fp->z );
				glRotatef( fp->ax,   1., 0., 0. );
				glRotatef( fp->ay,   0., 1., 0. );
				glRotatef( fp->az,   0., 0., 1. );
				glutWireTeapot(200);
			glPopMatrix();
		}
	glEndList();


	/* create the object:						*/

	ObjectList = glGenLists( 1 );
	glNewList( ObjectList, GL_COMPILE );
		glMaterialfv( GL_BACK, GL_AMBIENT, MulArray3( .4f, White ) );
		glMaterialfv( GL_BACK, GL_DIFFUSE, MulArray3( 1., White ) );
		glMaterialfv( GL_BACK, GL_SPECULAR, Array3( 0., 0., 0. ) );
		glMaterialf ( GL_BACK, GL_SHININESS, 7.f );
		glMaterialfv( GL_BACK, GL_EMISSION, Array3( 0., 0., 0. ) );
	
		glMaterialfv( GL_FRONT, GL_EMISSION, Array3( 0., 0., 0. ) );
		glShadeModel(GL_SMOOTH);
		glutSolidTeapot( 200 );
	glEndList();


	/* create the axes:						*/

	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glColor3fv( AXES_COLOR );
		glLineWidth( AXES_WIDTH );
			Axes( 400. );
		glLineWidth( 1. );
	glEndList();
}


/**
 ** initialize the glui window:
 **/

void
InitGlui( void )
{
	GLUI_Panel *panel;
	GLUI_RadioGroup *group;
	GLUI_Translation *trans, *scale;

	if( Debug )
		fprintf( stderr, "InitGlui\n" );


	/* setup the glui window:					*/

	glutInitWindowPosition( INIT_WINDOW_SIZE+50, 0 );
	Glui = GLUI_Master.create_glui( (char *) GLUITITLE );


	Glui->add_statictext( (char *) GLUITITLE );
	Glui->add_separator();

	Glui->add_checkbox( "Axes", &AxesOn );

	Glui->add_checkbox( "Perspective", &WhichProjection );

	Glui->add_checkbox( "Keyframes", &KeyFrameOn );

	Glui->add_checkbox( "Use Timer", &TimerOn );

	panel = Glui->add_panel( "Animation" );
		group = Glui->add_radiogroup_to_panel( panel, &AnimateOn, 0, (GLUI_Update_CB) SetAnimate );
			Glui->add_radiobutton_to_group( group, "Off" );
			Glui->add_radiobutton_to_group( group, "On" );

	panel = Glui->add_panel( "Object Transformation" );

		GluiRot = Glui->add_rotation_to_panel( panel, "Rotation", (float *) RotMatrix );
		GluiRot->set_spin( 1. );

		Glui->add_column_to_panel( panel, FALSE );
		scale = Glui->add_translation_to_panel( panel, "Scale",  GLUI_TRANSLATION_Y , &Scale2 );
		scale->set_speed( 0.01f );

		Glui->add_column_to_panel( panel, FALSE );
		trans = Glui->add_translation_to_panel( panel, "Trans XY", GLUI_TRANSLATION_XY, &TransXYZ[0] );
		trans->set_speed( 0.1f );

		Glui->add_column_to_panel( panel, FALSE );
		trans = Glui->add_translation_to_panel( panel, "Trans Z",  GLUI_TRANSLATION_Z , &TransXYZ[2] );
		trans->set_speed( 0.1f );


	panel = Glui->add_panel( "", FALSE );

	Glui->add_button_to_panel( panel, "Reset", RESET, (GLUI_Update_CB) Buttons );

	Glui->add_column_to_panel( panel, FALSE );

	Glui->add_button_to_panel( panel, "Quit", QUIT, (GLUI_Update_CB) Buttons );

	Glui->add_column_to_panel( panel, FALSE );

	Glui->add_checkbox_to_panel( panel, "Debug", &Debug );


	/* tell glui what graphics window it needs to post a redisplay to:	*/

	Glui->set_main_gfx_window( GrWindow );


	/* set the graphics window's idle function:				*/

	GLUI_Master.set_glutIdleFunc( NULL );
}



/**
 ** the keyboard callback:
 **/

void
Keyboard( unsigned char c, int x, int y )
{
	if( Debug )
		fprintf( stderr, "DoKeyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{
		case 'q':
		case 'Q':
		case ESCAPE:
			Buttons( QUIT );	/* will not return here		*/
			break;			/* happy compiler		*/

		case 'f':
		case 'F':
			KeyFrameOn = ! KeyFrameOn;
			break;

		case 'o':
		case 'O':
			WhichProjection = ORTHO;
			break;

		case 'p':
		case 'P':
			WhichProjection = PERSP;
			break;


		default:
			fprintf( stderr, "Don't know what to do with keyboard hit:: '%c' (0x%0x)\n", c, c );
	}


	/* synchronize the GLUI display with the variables:		*/

	Glui->sync_live();


	/* force a call to Display():					*/

	glutSetWindow( GrWindow );
	glutPostRedisplay();
}



/**
 ** called when the mouse button transitions down or up:
 **/

void
MouseButton
(
	int button,		/* GLUT_*_BUTTON			*/
	int state,		/* GLUT_UP or GLUT_DOWN			*/
	int x, int y		/* where mouse was when button hit	*/
)
{
	int b;			/* LEFT, MIDDLE, or RIGHT		*/

	if( Debug )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

	
	/* get the proper button bit mask:				*/

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}


	/* button down sets the bit, up clears the bit:			*/

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		/* set the proper bit	*/
	}
	else
		ActiveButton &= ~b;		/* clear the proper bit	*/
}



/**
 ** called when the mouse moves while a button is down:
 **/

void
MouseMotion
(
	int x, int y		/* mouse coords				*/
)
{
	int dx, dy;		/* change in mouse coordinates		*/

	if( Debug )
		fprintf( stderr, "MouseMotion: %d, %d\n", x, y );


	dx = x - Xmouse;		/* change in mouse coords	*/
	dy = y - Ymouse;

	if( ActiveButton & LEFT )
	{
			Xrot += ( ANGFACT*dy );
			Yrot += ( ANGFACT*dx );
	}

	if( ActiveButton & MIDDLE )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		/* keep object from turning inside-out or disappearing:	*/

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	Xmouse = x;			/* new current position		*/
	Ymouse = y;

	glutSetWindow( GrWindow );
	glutPostRedisplay();
}



/**
 ** reset the transformations and the colors:
 **
 ** this only sets the global variables --
 ** the glut main loop is responsible for redrawing the scene
 **/

void
Reset( void )
{
	NowFrame = 0;
	X = Frames[0].x;
	Y = Frames[0].y;
	Z = Frames[0].z;
	Ax = Frames[0].ax;
	Ay = Frames[0].ay;
	Az = Frames[0].az;
	KeyFrameOn = TRUE;

	AnimateOn = FALSE;
	ActiveButton = 0;
	AxesOn = GLUITRUE;
	Debug = GLUIFALSE;
	Scale  = 1.0;
	Scale2 = 0.0;		/* because add 1. to it in Display()	*/
	TimerOn = TRUE;
	WhichProjection = PERSP;
	Xrot = Yrot = 0.;
	TransXYZ[0] = TransXYZ[1] = TransXYZ[2] = 0.;

	                  RotMatrix[0][1] = RotMatrix[0][2] = RotMatrix[0][3] = 0.;
	RotMatrix[1][0]                   = RotMatrix[1][2] = RotMatrix[1][3] = 0.;
	RotMatrix[2][0] = RotMatrix[2][1]                   = RotMatrix[2][3] = 0.;
	RotMatrix[3][0] = RotMatrix[3][1] = RotMatrix[3][3]                   = 0.;
	RotMatrix[0][0] = RotMatrix[1][1] = RotMatrix[2][2] = RotMatrix[3][3] = 1.;

	glutSetWindow( GrWindow );
	glutPostRedisplay();
}



/**
 ** called when user resizes the window:
 **/

void
Resize( int width, int height )
{
	if( Debug )
		fprintf( stderr, "ReSize: %d, %d\n", width, height );

	/* don't really need to do anything since window size is	*/
	/* checked each time in Display():				*/

	glutSetWindow( GrWindow );
	glutPostRedisplay();
}




void
SetAnimate( int id )
{
	if( AnimateOn )
	{
		GLUI_Master.set_glutIdleFunc( Animate );
	}
	else
	{
		GLUI_Master.set_glutIdleFunc( NULL );
	}

	NowFrame = 0;


	/* create the base time:					*/

	// Time0 = glutGet( GLUT_ELAPSED_TIME );
	Time0 = 0;



}



/**
 ** handle a change to the window's visibility:
 **/

void
Visibility
(
	int state		/* GLUT_VISIBLE or GLUT_NOT_VISIBLE	*/
)
{
	if( Debug )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( GrWindow );
		glutPostRedisplay();
	}
	else
	{
		/* could optimize by keeping track of the fact		*/
		/* that the window is not visible and avoid		*/
		/* animating or redrawing it ...			*/
	}
}







/* the stroke characters 'X' 'Y' 'Z' :					*/

static float xx[] = {
		0., 1., 0., 1.
	      };

static float xy[] = {
		-.5, .5, .5, -.5
	      };

static int xorder[] = {
		1, 2, -3, 4
		};


static float yx[] = {
		0., 0., -.5, .5
	      };

static float yy[] = {
		0., .6f, 1., 1.
	      };

static int yorder[] = {
		1, 2, 3, -2, 4
		};


static float zx[] = {
		1., 0., 1., 0., .25, .75
	      };

static float zy[] = {
		.5, .5, -.5, -.5, 0., 0.
	      };

static int zorder[] = {
		1, 2, 3, 4, -5, 6
		};


/* fraction of the length to use as height of the characters:		*/

#define LENFRAC		0.10


/* fraction of length to use as start location of the characters:	*/

#define BASEFRAC	1.10


/**
 **	Draw a set of 3D axes:
 **	(length is the axis length in world coordinates)
 **/

void
Axes( float length )
{
	int i, j;			/* counters			*/
	float fact;			/* character scale factor	*/
	float base;			/* character start location	*/


	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd();
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd();

	fact = LENFRAC * length;
	base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( i = 0; i < 4; i++ )
		{
			j = xorder[i];
			if( j < 0 )
			{
				
				glEnd();
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd();

	glBegin( GL_LINE_STRIP );
		for( i = 0; i < 5; i++ )
		{
			j = yorder[i];
			if( j < 0 )
			{
				
				glEnd();
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd();

	glBegin( GL_LINE_STRIP );
		for( i = 0; i < 6; i++ )
		{
			j = zorder[i];
			if( j < 0 )
			{
				
				glEnd();
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd();

}



float *
MulArray3( float factor, float array0[3] )
{
	static float array[4];

	array[0] = factor * array0[0];
	array[1] = factor * array0[1];
	array[2] = factor * array0[2];
	array[3] = 1.;
	return( array );
}


float *
Array3( float a, float b, float c )
{
	static float array[4];

	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 1.;
	return( array );
}


void
HsvRgb( float hsv[3], float rgb[3] )
{
	float h, s, v;			/* hue, sat, value		*/
	float r, g, b;			/* red, green, blue		*/
	float i, f, p, q, t;		/* interim values		*/


	/* guarantee valid input:					*/

	h = hsv[0] / 60.;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;


	/* if sat==0, then is a gray:					*/

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}


	/* get an rgb from the hue itself:				*/
	
	i = floor( h );
	f = h - i;
	p = v * ( 1. - s );
	q = v * ( 1. - s*f );
	t = v * ( 1. - ( s * (1.-f) ) );

	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;
	
		case 1:
			r = q;	g = v;	b = p;
			break;
	
		case 2:
			r = p;	g = v;	b = t;
			break;
	
		case 3:
			r = p;	g = q;	b = v;
			break;
	
		case 4:
			r = t;	g = p;	b = v;
			break;
	
		case 5:
			r = v;	g = p;	b = q;
			break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;

}
