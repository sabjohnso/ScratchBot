
#include <assert.h>

const int fwd = 1;
const int bwd = 0;

const int toIn1  = 0;
const int toIn2  = 1;
const int toIn3  = 2;
const int toIn4  = 3;
const int dir    = 4;
const int stp    = 5;
#define UNL2003_VARIABLE_COUNT 6


int
UNL2003_getPID( const int* ptr, int index )
{
  return ptr[ index ];
}

void
UNL2003_setPID( int* ptr, int index, int pinID )
{
  ptr[ index ] = pinID;
}

void
UNL2003_setSignal( int* ptr, int signal )
{
    ptr[ stp ] = signal % 8; 
}

int
UNL2003_getSignal( const int* ptr )
{
  return ptr[ stp ];
}

int
UNL2003_getDir( int* ptr )
{
  return ptr[ dir ];
}

void
UNL2003_nextSignal( int* ptr )
{
    if( UNL2003_getDir( ptr ) == fwd )
    {
        
        UNL2003_setSignal( ptr, UNL2003_getSignal( ptr ) == 7 ? 0 : UNL2003_getSignal( ptr )+1);
        
    }
    else if( UNL2003_getDir( ptr ) == bwd )
    {
        UNL2003_setSignal( ptr, UNL2003_getSignal( ptr ) == 0 ? 7 : UNL2003_getSignal( ptr )-1);
    }
}


void
UNL2003_write( int* ptr, int sig1, int sig2, int sig3, int sig4 )
{
    digitalWrite( UNL2003_getPID( ptr, toIn1 ), sig1 );
    digitalWrite( UNL2003_getPID( ptr, toIn2 ), sig2 );
    digitalWrite( UNL2003_getPID( ptr, toIn3 ), sig3 );
    digitalWrite( UNL2003_getPID( ptr, toIn4 ), sig4 );
}

void
UNL2003_fwd( int* ptr )
{
  ptr[ dir ] = fwd;
}

void
UNL2003_bwd( int* ptr )
{
  ptr[ dir ] = bwd;
}



void
UNL2003_setup( int* ptr, int pinToIn1, int pinToIn2, int pinToIn3, int pinToIn4 )
{
  UNL2003_setPID( ptr, toIn1, pinToIn1 );
  UNL2003_setPID( ptr, toIn2, pinToIn2 );
  UNL2003_setPID( ptr, toIn3, pinToIn3 );
  UNL2003_setPID( ptr, toIn4, pinToIn4 );
  UNL2003_fwd( ptr );
  ptr[ stp   ] = 0;
  
  pinMode( pinToIn1, OUTPUT );
  pinMode( pinToIn2, OUTPUT );
  pinMode( pinToIn3, OUTPUT );
  pinMode( pinToIn4, OUTPUT ); 
  
  

}

void
UNL2003_write_default( int* ptr )
{
  int default_state[] = { LOW, LOW, LOW, LOW };
  UNL2003_write( ptr, default_state[ 0 ], default_state[ 1 ], default_state[ 2 ], default_state[ 3 ] );
}




void 
UNL2003_take_steps( int* ptr, int xw )
{
    for( int x = 0; x < xw; ++x )
    {
      switch( ptr[ stp ] )
      {
        case 0:  UNL2003_write( ptr, LOW,  LOW,  LOW,  HIGH ); break;
        case 1:  UNL2003_write( ptr, LOW,  LOW,  HIGH, HIGH ); break;
        case 2:  UNL2003_write( ptr, LOW,  LOW,  HIGH, LOW  ); break;
        case 3:  UNL2003_write( ptr, LOW,  HIGH, HIGH, LOW  ); break;
        case 4:  UNL2003_write( ptr, LOW,  HIGH, LOW,  LOW  ); break;
        case 5:  UNL2003_write( ptr, HIGH, HIGH, LOW,  LOW  ); break;
        case 6:  UNL2003_write( ptr, HIGH,  LOW, LOW,  LOW  ); break;
        case 7:  UNL2003_write( ptr, HIGH,  LOW, LOW,  HIGH ); break;
        default: UNL2003_write( ptr, LOW,   LOW, LOW,  LOW  ); break;
      }
      UNL2003_nextSignal( ptr );
      delay( 5 );

    }
}

#define UNL2003_MAKE_STEPPER( name ) int name[ UNL2003_VARIABLE_COUNT ] = {0};





//
// ... Read a line from Serial, terminating with the null character: '\0'.
//     If the buffer runs out before the newline character or the null character
//     are encountered, the first character in the buffer is replace with the
//     null character and an error signal is returned
//
//     Arguments:
//     buffer_size - the number of characters in the buffer
//     buffer - the address of the buffer
//
int
readline( unsigned int buffer_size, char* buffer )
{
  
  
  
  assert( buffer_size > 0 );
  assert( buffer != 0 );
  
  int ierr = 0;
  unsigned int index = 0;
  char c = 'x';
  do
  {
    if( Serial.available() > 0 )
    {
        c = Serial.read();
        if( c == '\n' )
        {
           buffer[ index ] = '\0';
        }
        else
        {
          buffer[ index ] = c;
        }
        index++;
    }
  } while( index < buffer_size && c != '\n' && c !='\0' );
  
  
  if( index == buffer_size )
  {
    buffer[0] = '\0';
    ierr = 1;
  }
   
   return ierr;
}




#define XY_READLINE_SUCCESS 0
#define XY_READLINE_FAILURE 1


const unsigned int XY_readline_buffer_length =  32;
char XY_readline_buffer[ XY_readline_buffer_length ] = {0};


const int xloc = 0;
const int yloc = 1;


int XY_xy[ 2 ] = {0};

int
XY_read( int* ptr )
{
  assert( ptr != 0 );
  
  int ierr = readline( XY_readline_buffer_length, XY_readline_buffer );
  
  assert( ierr == 0 );
  
  char* sdeltaX= strtok( XY_readline_buffer, ":\n" );
  char* sdeltaY = strtok( 0, ":\n" );
  
//  Serial.write( "DeltaX: " );
//  Serial.write( sdeltaX );
//  Serial.write( "\n" );
//  
//  Serial.write( "DeltaY: " );
//  Serial.write( sdeltaY );
//  Serial.write( "\n" );
  

  ptr[ xloc ] = atoi( sdeltaX );
  ptr[ yloc ] = atoi( sdeltaY );
  
  return ierr;
}



UNL2003_MAKE_STEPPER( stepper0 );




enum { SUCCESS, ERROR_SIGNAL_NOT_SETUP };

int error_signal = ERROR_SIGNAL_NOT_SETUP;

int successPID = 2;
int failPID = 4;


//////////////////////////////////////////////////////////////////////////////
void
write_error()
{
  if( error_signal == 0 )
  {
    digitalWrite( successPID, HIGH );
  }
  else
  {
    digitalWrite( successPID, LOW );
    digitalWrite( failPID, HIGH );
  }
}
/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
void setup() 
{
 
  error_signal = 0;
  UNL2003_setup( stepper0, 8, 9, 10, 11 );
 
  
  
  if( UNL2003_getPID( stepper0, toIn1 ) != 8 ) ++error_signal;
  if( UNL2003_getPID( stepper0, toIn2 ) != 9 ) ++error_signal;
  if( UNL2003_getPID( stepper0, toIn3 ) != 10) ++error_signal;
  if( UNL2003_getPID( stepper0, toIn4 ) != 11) ++error_signal;
  
  pinMode( successPID, OUTPUT );
  pinMode( failPID, OUTPUT ); 
  
  
  Serial.begin(9600);

  
} // end of function setup
/////////////////////////////////////////////////////////////////////////


void loop() 
{
  if( error_signal == 0 )
  {

    XY_read( XY_xy );        
    
    if( XY_xy[ xloc ] > 0 )
    {
      
      UNL2003_bwd( stepper0 );
      UNL2003_take_steps( stepper0, abs( XY_xy[ xloc ]));
    }
    else if( XY_xy[ xloc ] < 0 )
    {
      UNL2003_fwd( stepper0 );
      UNL2003_take_steps( stepper0, abs( XY_xy[ xloc ]));
    }
    XY_xy[ xloc ] = 0;
    XY_xy[ yloc ] = 0;
    
    UNL2003_write_default( stepper0 );

    delay( 100 );

  }
  else
  {
    Serial.write( "Error!!!" );
  }
   
}
