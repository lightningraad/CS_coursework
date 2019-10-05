/*

CS 491: Assignment 7 (Mesh of Springs)
Amar Raad
Dec 5th, 2018

*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"
#include "glui.h"

// title of these windows:

const char *WINDOWTITLE = { "CS 491 | HW 7 -- Amar Raad" };
const char *GLUITITLE   = { "User Interface Window" };




const float MASSMIN =  1.f;
const float MASSMAX =  4.f;

const float KMIN =  20.f;
const float KMAX = 600.f;

const float CDMIN =  1.00f;
const float CDMAX =  7.00f;

const float GMIN = -20.;
const float GMAX =  20.;

const float X0MIN = -25.f;
const float X0MAX =  25.f;

const float Y0MIN = -45.f;
const float Y0MAX =   0.f;

const float XL = -30.f;
const float XR =  30.f;
const float YB = -55.f;
const float YT =   5.f;

const int TOL = 10;		// pick tolerance in pixels

const float LENGTH0 = 0.05f;

const float DTMIN = 0.001f;
const float DTMAX = 0.005f;

const float RADIUS = 0.40f;

const float CIRCX = X0MIN + 0.40f * ( X0MAX - X0MIN );
const float CIRCY = Y0MIN + 0.20f * ( Y0MAX - Y0MIN );
const float CIRCR = 3.f;

// what the glui package defines as true and false:

const int GLUITRUE  = { true  };
const int GLUIFALSE = { false };

// the escape key:

#define ESCAPE		0x1b

// initial window size:

const int INIT_WINDOW_SIZE = { 700 };

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };

// minimum allowable scale factor:

const float MINSCALE = { 0.05f };

// active mouse buttons (or them together):

const int LEFT   = { 4 };
const int MIDDLE = { 2 };
const int RIGHT  = { 1 };

// which projection:

enum Projections
{
	ORTHO,
	PERSP
};

// which button:

enum ButtonVals
{
	RESET,
	QUIT
};

// window background color (rgba):

const float BACKCOLOR[] = { 0., 0., 0., 0. };

// color and line width for the axes:

const GLfloat AXES_COLOR[] = { 1., .5, 0. };
const GLfloat AXES_WIDTH   = { 3. };


struct state
{
	float y;
	float vy;
	float x;
	float vx;
};

struct derivatives
{
	float vy;
	float ay;
	float vx;
	float ax;
};

#define NUMNODES	30

struct state		State[NUMNODES];


//
// non-constant global variables:
//

int	ActiveButton;		// current button that is down
GLuint	AxesList;		// list to hold the axes
int	AxesOn;			// ON or OFF
int	BarrierList;
float	Cd;
int	CircleList;
int	Debug;			// ON means print debug info
int	DepthCueOn;		// TRUE means to use intensity depth cueing
int	DoCircle;
float Dt;			// time step
GLUI *	Glui;			// instance of glui window
int	GluiWindow;		// the glut id for the glui window
float	Gravity;
float	K;
int	MainWindow;		// window id for main graphics window
float	Mass;
int	Order;
GLfloat	RotMatrix[4][4];	// set by glui rotation widget
float	Scale, Scale2;		// scaling factors
int	WhichProjection;	// ORTHO or PERSP
float	X0, Y0;			// top of the chain
int	Xmouse, Ymouse;		// mouse values
float	Xrot, Yrot;		// rotation angles in degrees
float	TransXYZ[3];		// set by glui translation widgets



//
// function prototypes:
//

void	Animate( void );
void	Axes( float );
void	Buttons( int );
void	Circle( float, float, float );
void	Display( void );
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( void );
void	GetDerivs( struct state [ ],  struct derivatives [ ] );
void	InitGlui( void );
void	InitGraphics( void );
void	InitLists( void );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( void );
void	Resize( int, int );
void	Visibility( int );


inline float
Sign( float x )
{
	if( x < 0. )
		return -1.;
	return 1.;
}

int
main( int argc, char *argv[] )
{
	glutInit( &argc, argv );
	InitGraphics( );
	InitLists( );
	Reset( );
	InitGlui( );
	glutMainLoop( );
	return 0;
}


void
Animate( void )
{
	struct state State2[NUMNODES];
	struct derivatives Derivatives1[NUMNODES], Derivatives2[NUMNODES];

	GetDerivs( State, Derivatives1 );

	switch( Order )
	{
		case 0:		// first order
			for( int node = 0; node < NUMNODES; node++ )
			{
				State[node].y  = State[node].y  + Derivatives1[node].vy * Dt;
				State[node].vy = State[node].vy + Derivatives1[node].ay * Dt;
				State[node].x  = State[node].x  + Derivatives1[node].vx * Dt;
				State[node].vx = State[node].vx + Derivatives1[node].ax * Dt;
			}
			break;

		case 1:		// second order
			for( int node = 0; node < NUMNODES; node++ )
			{
				State2[node].y  = State[node].y  + Derivatives1[node].vy * Dt;
				State2[node].vy = State[node].vy + Derivatives1[node].ay * Dt;
				State2[node].x  = State[node].x  + Derivatives1[node].vx * Dt;
				State2[node].vx = State[node].vx + Derivatives1[node].ax * Dt;
			}

			GetDerivs( State2, Derivatives2 );

			for( int node = 0; node < NUMNODES; node++ )
			{
				float aavg = ( Derivatives1[node].ay + Derivatives2[node].ay ) / 2.;
				float vavg = ( Derivatives1[node].vy + Derivatives2[node].vy ) / 2.;
				State[node].y  = State[node].y  + vavg * Dt;
				State[node].vy = State[node].vy + aavg * Dt;
				aavg = ( Derivatives1[node].ax + Derivatives2[node].ax ) / 2.;
				vavg = ( Derivatives1[node].vx + Derivatives2[node].vx ) / 2.;
				State[node].x  = State[node].x  + vavg * Dt;
				State[node].vx = State[node].vx + aavg * Dt;
			}
			break;
	}

	if( DoCircle )
	{
		for( int node = 0; node < NUMNODES; node++ )
		{
			float dx = State[node].x - CIRCX;
			float dy = State[node].y - CIRCY;
			float rsqd = dx*dx + dy*dy;
			if( rsqd < CIRCR*CIRCR )
			{
				float r = sqrt( rsqd );
				dx /= r;
				dy /= r;
				State[node].x = CIRCX + CIRCR * dx;
				State[node].y = CIRCY + CIRCR * dy;
				State[node].vx *=  dy;
				State[node].vy *= -dx;
			}
		}
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// glui buttons callback:

void
Buttons( int id )
{
	switch( id )
	{
		case RESET:
			Reset( );
			Glui->sync_live( );
			glutSetWindow( MainWindow );
			glutPostRedisplay( );
			break;

		case QUIT:
			Glui->close( );
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Button ID %d\n", id );
	}

}


void
Circle( float xc, float yc, float radius )
{
	glBegin( GL_TRIANGLE_FAN );
		glVertex2f( xc, yc );
		int numsegs = 20;
		float dang = 2.*M_PI/(float)numsegs;
		float ang = 0.;
		for( int i = 0; i <= numsegs; i++, ang += dang )
		{
			glVertex2f( xc + radius*cos(ang), yc + radius*sin(ang) );
		}
	glEnd( );
}


void
Display( void )
{
	glutSetWindow( MainWindow );
	glDrawBuffer( GL_BACK );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable( GL_DEPTH_TEST );

	glShadeModel( GL_FLAT );

	GLsizei vx = glutGet( GLUT_WINDOW_WIDTH );
	GLsizei vy = glutGet( GLUT_WINDOW_HEIGHT );
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = ( vx - v ) / 2;
	GLint yb = ( vy - v ) / 2;
	glViewport( xl, yb,  v, v );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	gluOrtho2D( XL, XR,   YB, YT );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );

	glTranslatef( (GLfloat)TransXYZ[0], (GLfloat)TransXYZ[1], -(GLfloat)TransXYZ[2] );

	glScalef( (GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale );
	GLfloat scale2 = 1. + Scale2;		// because glui translation starts at 0.
	if( scale2 < MINSCALE )
		scale2 = MINSCALE;
	glScalef( (GLfloat)scale2, (GLfloat)scale2, (GLfloat)scale2 );

	if( AxesOn )
		glCallList( AxesList );

	// draw the object:

	if( DoCircle )
	{
		glColor3f( 1., 1., 0. );
		glCallList( BarrierList );
	}

	glColor3f( 1., 1., 1. );
	glLineWidth( 1. );
	glBegin( GL_LINES );
		glVertex2f( X0MIN, 0. );
		glVertex2f( X0MAX, 0. );
		glVertex2f( 0., Y0MIN );
		glVertex2f( 0., Y0MAX );
	glEnd( );

	glColor3f( 0., 1., 0. );
	glLineWidth( 3. );
	glBegin( GL_LINE_STRIP );
		glVertex2f( X0, Y0 );

#ifdef Y_ONLY
		for( int node = 0; node < NUMNODES; node++ )
		{
			glVertex2f( X0, State[node].y );
		}
#endif

		for( int node = 0; node < NUMNODES; node++ )
		{
			glVertex2f( State[node].x, State[node].y );
		}

	glEnd( );
	glLineWidth( 1. );

	glColor3f( 1.f, 0.5f, 0. );
	glPushMatrix( );
		glTranslatef( X0, Y0, 0. );
		glCallList( CircleList );
	glPopMatrix( );

	for( int node = 0; node < NUMNODES; node++ )
	{
		glPushMatrix( );
			glTranslatef( State[node].x, State[node].y, 0. );
			glCallList( CircleList );
		glPopMatrix( );
	}

	glutSwapBuffers( );
	glFlush( );
}


// use glut to display a string of characters using a raster font:

void
DoRasterString( float x, float y, float z, char *s )
{
	char c;			// one character to print

	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	char c;			// one character to print
	float sf;		// the scale factor

	glPushMatrix( );
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		sf = ht / ( 119.05 + 33.33 );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix( );
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds( void )
{
	int ms = glutGet( GLUT_ELAPSED_TIME );
	return (float)ms / 1000.;
}



void
GetDerivs( struct state state[NUMNODES], struct derivatives derivs[NUMNODES] )
{
	for( int node = 0; node < NUMNODES; node++ )
	{
		// ------------ Mesh of Springs slide:
		//                GetLinkVelAcc( ), I

		float xm, ym;		// vector from bottom of node #node to preious node
		float xp, yp;		// vector from bottom of node #node to next node

		float sumfx = 0.;
		float sumfy = Mass*Gravity;

		if( node == 0 )	 // node #0 is attached to node Y0 by a spring
		{
			xm = X0 - state[0].x;
			ym = Y0 - state[0].y;
		}
		else
		{
			xm = state[node-1].x - state[node].x;
			ym = state[node-1].y - state[node].y;
		}

		float length = sqrt(xm*xm + ym * ym); // length of the spring
		xm /= length; // normalize the vector
		ym /= length;
		float stretch = length - LENGTH0; // amount the spring is stretched
		float force = K * stretch;

		sumfx += force * xm;		// FIXED !!
		sumfy += force * ym;		// FIXED !!
		// --------------------------------------------------------------------- //
		// ------------ Mesh of Springs slide:
		//                GetLinkVelAcc( ), II

		if( node < NUMNODES-1 )
		{
			xp = state[node + 1].x - state[node].x;
			yp = state[node + 1].y - state[node].y;

			length = sqrt(xp*xp + yp * yp);
			xp /= length;
			yp /= length;
			stretch = length - LENGTH0;
			force = K * stretch;

			sumfx += force * xp;		// FIXED !!
			sumfy += force * yp;		// FIXED !!
		}
		// --------------------------------------------------------------------- //
		// ------------ Mesh of Springs slide:
		//                GetLinkVelAcc( ), II

		float v = sqrt( state[node].vx*state[node].vx + state[node].vy*state[node].vy );
		if( v > 0. )
		{
			// damping:
			sumfx -= Cd * state[node].vx;		// FIXED !!
			sumfy -= Cd * state[node].vx;		// FIXED !!
		}

		derivs[node].vx = state[node].vx;
		derivs[node].ax = sumfx / Mass;
		derivs[node].vy = state[node].vy;
		derivs[node].ay = sumfy / Mass;
		// --------------------------------------------------------------------- //
	}
}



//
// initialize the glui window:
//

void
InitGlui( void )
{
	glutInitWindowPosition( INIT_WINDOW_SIZE + 50, 0 );
	Glui = GLUI_Master.create_glui( (char *) GLUITITLE );

	Glui->add_statictext( (char *) GLUITITLE );
	Glui->add_separator( );

	Glui->add_checkbox( "Circle Barrier", &DoCircle );

	GLUI_HSlider *slider;

	Glui->add_statictext( "X0:" );
	slider = Glui->add_slider( false, GLUI_HSLIDER_FLOAT, &X0 );
    	slider->set_float_limits( X0MIN, X0MAX );
    	slider->set_w( 200 );

	Glui->add_statictext( "Y0:" );
	slider = Glui->add_slider( false, GLUI_HSLIDER_FLOAT, &Y0 );
    	slider->set_float_limits( Y0MIN, Y0MAX );
    	slider->set_w( 200 );
	Glui->add_separator( );

	Glui->add_statictext( "Mass:" );
	slider = Glui->add_slider( false, GLUI_HSLIDER_FLOAT, &Mass );
    	slider->set_float_limits( MASSMIN, MASSMAX );
    	slider->set_w( 200 );
		fprintf(stderr, "Mass: %8.3f, %8.3f   %8.3f\n", MASSMIN, MASSMAX, Mass);
	Glui->add_separator( );

	Glui->add_statictext( "Spring Constant:" );
	slider = Glui->add_slider( false, GLUI_HSLIDER_FLOAT, &K );
    	slider->set_float_limits( KMIN, KMAX );
    	slider->set_w( 200 );
	Glui->add_separator( );

	Glui->add_statictext( "Damping:" );
	slider = Glui->add_slider( false, GLUI_HSLIDER_FLOAT, &Cd );
    	slider->set_float_limits( CDMIN, CDMAX );
    	slider->set_w( 200 );
	Glui->add_separator( );

	Glui->add_statictext( "Gravity:" );
	slider = Glui->add_slider( false, GLUI_HSLIDER_FLOAT, &Gravity );
    	slider->set_float_limits( GMIN, GMAX );
    	slider->set_w( 200 );
	Glui->add_separator( );

	Glui->add_statictext( "Dt:" );
	slider = Glui->add_slider( false, GLUI_HSLIDER_FLOAT, &Dt );
    	slider->set_float_limits( DTMIN, DTMAX );
    	slider->set_w( 200 );
	Glui->add_separator( );

	GLUI_Panel *panel = Glui->add_panel( "Integration Order" );
		GLUI_RadioGroup *group = Glui->add_radiogroup_to_panel( panel, &Order );
			Glui->add_radiobutton_to_group( group, "1" );
			Glui->add_radiobutton_to_group( group, "2" );

	panel = Glui->add_panel( "Object Transformation" );
		Glui->add_column_to_panel( panel, GLUIFALSE );
		GLUI_Translation *scale = Glui->add_translation_to_panel( panel, "Scale",  GLUI_TRANSLATION_Y , &Scale2 );
		scale->set_speed( 0.01f );

		Glui->add_column_to_panel( panel, FALSE );
		GLUI_Translation *trans = Glui->add_translation_to_panel( panel, "Trans XY", GLUI_TRANSLATION_XY, &TransXYZ[0] );
		trans->set_speed( 0.1f );

	Glui->add_checkbox( "Debug", &Debug );


	panel = Glui->add_panel( "", FALSE );

	Glui->add_button_to_panel( panel, "Reset", RESET, (GLUI_Update_CB) Buttons );

	Glui->add_column_to_panel( panel, FALSE );

	Glui->add_button_to_panel( panel, "Quit", QUIT, (GLUI_Update_CB) Buttons );


	// tell glui what graphics window it needs to post a redisplay to:

	Glui->set_main_gfx_window( MainWindow );
	Glui->sync_live();


	// set the graphics window's idle function:

	GLUI_Master.set_glutIdleFunc( Animate );
}



//
// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions
//

void
InitGraphics( void )
{
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( INIT_WINDOW_SIZE, INIT_WINDOW_SIZE );
	MainWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );

	// setup the callback routines:

	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( 0, NULL, 0 );

	// DO NOT SET THE GLUT IDLE FUNCTION HERE !!
	// glutIdleFunc( NULL );
	// let glui take care of it in InitGlui( )
}




//
// initialize the display lists that will not change:
//

void
InitLists( void )
{
	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glColor3fv( AXES_COLOR );
		glLineWidth( AXES_WIDTH );
			Axes( 1.5 );
		glLineWidth( 1. );
	glEndList( );

	CircleList = glGenLists( 1 );
	glNewList( CircleList, GL_COMPILE );
		Circle( 0., 0., RADIUS );
	glEndList( );

	BarrierList = glGenLists( 1 );
	glNewList( BarrierList, GL_COMPILE );
		Circle( CIRCX, CIRCY, CIRCR );
	glEndList( );
}



// the keyboard callback:

void
Keyboard( unsigned char c, int x, int y )
{
	switch( c )
	{
		case 'o':
		case 'O':
			WhichProjection = ORTHO;
			break;

		case 'p':
		case 'P':
			WhichProjection = PERSP;
			break;

		case 'q':
		case 'Q':
		case ESCAPE:
			Buttons( QUIT );	// will not return here
			break;			// happy compiler

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}


	// synchronize the GLUI display with the variables:

	Glui->sync_live( );


	// force a call to Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// called when the mouse button transitions down or up:

void
MouseButton( int button, int state, int x, int y )
{
	int b;			// LEFT, MIDDLE, or RIGHT

	if( Debug )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

	
	// get the proper button bit mask:

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


	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}
}


// called when the mouse moves while a button is down:

void
MouseMotion( int x, int y )
{
	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;


	if( ActiveButton & MIDDLE )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	Glui->sync_live( );

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// reset the transformations and the colors:
//
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset( void )
{
	ActiveButton = 0;
	AxesOn = GLUIFALSE;
	Debug = GLUIFALSE;
	DepthCueOn = FALSE;
	DoCircle = FALSE;
	Order = 1;			// 2nd order
	Scale  = 1.0;
	Scale2 = 0.0;		// because add 1. to it in Display( )
	Xrot = Yrot = 0.;
	TransXYZ[0] = TransXYZ[1] = TransXYZ[2] = 0.;

	                  RotMatrix[0][1] = RotMatrix[0][2] = RotMatrix[0][3] = 0.;
	RotMatrix[1][0]                   = RotMatrix[1][2] = RotMatrix[1][3] = 0.;
	RotMatrix[2][0] = RotMatrix[2][1]                   = RotMatrix[2][3] = 0.;
	RotMatrix[3][0] = RotMatrix[3][1] = RotMatrix[3][3]                   = 0.;
	RotMatrix[0][0] = RotMatrix[1][1] = RotMatrix[2][2] = RotMatrix[3][3] = 1.;

	Mass = ( MASSMIN + MASSMAX ) / 2.;
	K = ( KMIN + KMAX ) / 2.;
	Cd = ( CDMIN + CDMAX ) / 2.;
	X0 = ( X0MIN + X0MAX ) / 2.;
	Y0 = 0.;
	Gravity = -9.8f;
	Dt = ( DTMIN + DTMAX ) / 2.f;

	for( int node = 0; node < NUMNODES; node++ )
	{
		State[node].x = X0;
		State[node].vx = 0.;
		if( node == 0 )
			State[0].y = Y0 + ( (float)NUMNODES * Mass * Gravity ) / K;
		else
			State[node].y = State[node-1].y + ( (float)(NUMNODES-node) * Mass * Gravity ) / K;
		State[node].vy = 0.;
	}
}



// called when user resizes the window:

void
Resize( int width, int height )
{
	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// handle a change to the window's visibility:

void
Visibility ( int state )
{
	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}


// the stroke characters 'X' 'Y' 'Z' :

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


// fraction of the length to use as height of the characters:

#define LENFRAC		0.10


// fraction of length to use as start location of the characters:

#define BASEFRAC	1.10

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd( );
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd( );

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 4; i++ )
		{
			int j = xorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 5; i++ )
		{
			int j = yorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 6; i++ )
		{
			int j = zorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd( );

}
