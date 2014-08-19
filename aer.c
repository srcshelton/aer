
#include <stdio.h>
//#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <dmedia/audio.h>

/* Compile with -laudio */

int main( int argc, char *argv[] ) {
  //extern int errno;

  long long samplerate;
  ALpv audiopv;
  ALeventQueue audioqueue;
  ALevent audioevent;
  int param = AL_RATE;

  if( argc > 2 ) {
    fprintf( stdout, "USAGE: %s <sample rate>\n", argv[0] );
    fprintf( stdout, "       where <sample rate> is one of 22050, 44100, 48000, etc.\n" );
    exit( 1 );
  }

  if( 1 == argc ) {
    fprintf( stderr, "Warning: Using default rate of 44.1kHz\n" );
    samplerate = atol( "44100" );
  } else {
    samplerate = atol( argv[1] );
  }
  if( samplerate != 22050 && samplerate != 44100 && samplerate != 48000 )
    fprintf( stderr, "Warning: Trying non-standard sample rate of %lldHz\n", samplerate );
  samplerate = alDoubleToFixed( (double) samplerate );

  /* Set what sample rate we actually get... */
  audiopv.param = AL_RATE;
  audiopv.value.ll = samplerate;
  if( alSetParams( AL_DEFAULT_OUTPUT, &audiopv, 1 ) < 0 ) {
    fprintf( stderr, "ERROR: Failure setting sample rate to %f\n", alFixedToDouble( samplerate ) );
    exit( 1 );
  }
  alGetParams( AL_DEFAULT_OUTPUT, &audiopv, 1 );
  fprintf( stderr, "Successfully set sample rate to %.fHz\n", alFixedToDouble( audiopv.value.ll ) );

  /* Setup and start the Audio EventQueue system... */
  audioqueue = alOpenEventQueue( "rate" );
  if( NULL == audioqueue ) {
    fprintf( stderr, "ERROR: Failed to open Audio Event queue\n" );
    exit( 1 );
  }
  if( alSelectEvents( audioqueue, AL_DEFAULT_OUTPUT, &param, 1 ) ) {
    fprintf( stderr, "ERROR: Initial SelectEvents on Queue failed\n" );
    exit( 1 );
  }

  /* We need a blank Event for later */
  audioevent = alNewEvent();
  if( NULL == audioevent ) {
    fprintf( stderr, "ERROR: Failed to create Audio Event\n" );
    exit( 1 );
  }

  while( 1 ) {
    /* select() for an Event */
    if( 0 == alNextEvent( audioqueue, audioevent ) ) {
      /* We should only receive RATE events, but it's worth checking... */
      if( AL_RATE == alGetEventParam( audioevent ) ) {
        /* We don't want an Event saying that we've changed things
         * ourselves!
         */
        if( alDeselectEvents( audioqueue, AL_DEFAULT_OUTPUT, &param, 1 ) ) {
          fprintf( stderr, "ERROR: DeselectEvents on Queue failed\n" );
          exit( 1 );
        }

        /* Check that the rate really is different - probably redundant,
         * but it could potentially avoid any audio disruption due to spurious
         * events
         */
        // fprintf( stderr, "DEBUG: AL_RATE event received\n" );
        alGetParams( AL_DEFAULT_OUTPUT, &audiopv, 1 );
        if( audiopv.value.ll != samplerate ) {
          // fprintf( stderr, "DEBUG: Confirm sample rate change\n" );
          audiopv.value.ll = samplerate;
          alSetParams( AL_DEFAULT_OUTPUT, &audiopv, 1 );
        }

        /* Re-enable our Event notifications */
        if( alSelectEvents( audioqueue, AL_DEFAULT_OUTPUT, &param, 1 ) ) {
          fprintf( stderr, "ERROR: SelectEvents on Queue failed\n" );
          exit( 1 );
        }
      } else {
        /* As above, we should _only_ receive RATE notices, so this shouldn't
         * happen...
         */
        fprintf( stderr, "WARN: Unknown Audio Event received\n" );
      }
    } else {
      /* alNextEvent failed?! */
      fprintf( stderr, "WARN: Audio Event system failure\n" );
    }
  }
}

/*
 * AER (c) 2005, Stuart Shelton
 *
 * AER was originally based on EnforceRate;
 * EnforceRate Copyright (c) 2003, Lisa J Parratt
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */
