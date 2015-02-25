/******************************************************************************
 * $LastChangedDate: 2013-09-01 20:32:42 -0400 (Sun, 01 Sep 2013) $
 * @file
 * @author  Jim E. Brooks
 * @brief   Class for an output stream class composed with a class for the stream's storage.
 * @remarks Based on code written by Nicolai Josuttis.
 *//*
 * LEGAL:   COPYRIGHT (C) 2011 JIM E. BROOKS  http://www.jimbrooks.org
 ******************************************************************************/

#define EXAMPLE_CC 1
#include "base.hh"
#include "stream_input.hh"
#include "stream_output.hh"
#include "stream_input_storage.hh"
#include "stream_output_storage.hh"
#include "stream_input_storage_lzma.hh"
#include "stream_output_storage_lzma.hh"
using namespace base;

/*******************************************************************************
 * 
 *******************************************************************************/
bool Test( const uint n )
{
    const string pathname = "test.dat.xz";

    // Write compressed file.
    {
        shptr<StreamOutputStorageLZMA> streamOutputStorageLZMA = new StreamOutputStorageLZMA( pathname );
        StreamOutput out( streamOutputStorageLZMA.PTR() );
        for ( uint i = 0; i < n; ++i )
        {
          //out << 'a' << "test" << i << (i + i) << std::endl;  // wrong, need to write a space separate explicitly
            const char sep = ' ';
            out << 'a' << sep << "test" << sep << i << sep << (i + i) << std::endl;

            if ( not out.good() )
            {
              //RemoveFile( pathname );
                return false;
            }
        }
        out << "end" << std::endl;
    }

  //if ( not IfFileExists( pathname ) )
  //    return false;

    // Read compressed file.
    {
        shptr<StreamInputStorageLZMA> streamInputStorageLZMA = new StreamInputStorageLZMA( pathname );
        StreamInput in( streamInputStorageLZMA.PTR() );
        for ( int i = 0; i < int(n); ++i )
        {
            char c = 0; in >> c; if ( c != 'a' ) return false;
            string s;   in >> s; if ( s != "test" ) return false;
            int i2 = 0; in >> i2; if ( i2 != i ) return false;
            int i_i = 0; in >> i_i; if ( i_i != i+i ) return false;

            if ( not in.good() )
            {
              //RemoveFile( pathname );
                return false;
            }
        }
        string end; in >> end; if ( end != "end" ) return false;
    }

  //RemoveFile( pathname );

    return true;
}

/*******************************************************************************
 * 
 *******************************************************************************/
int main( int argc, char** argv )
{
    if ( Test( 1000 ) )
        std::cout << "Test passed." << std::endl;
    else
        std::cerr << "Test FAILED!" << std::endl;

    return 0;
}
