#include "buffer.h"
#include "utils.hpp"

#include <string.h>
#include <stdio.h>

Buffer *Buffer::fromFile( const char *filename ) {
  FILE *fp = fopen( filename, "rb" );
  if( !fp ){
    Utils::throwf( "Could not open %s for reading.", filename );
  }

  fseek(fp, 0L, SEEK_END);
  size_t fsize = ftell(fp);
  fseek(fp, 0L, SEEK_SET);

  uint8_t *data = new uint8_t[fsize];

  // FIXME: Check fread return value.
  fread( data, 1, fsize, fp );
  fclose(fp);

  Buffer *b = new Buffer( fsize, data, true );
  delete[] data;

  return b;
}

void Buffer::toFile( const char *filename ) {
  FILE *fp = fopen( filename, "w+b" );
  // FIXME: Check fwrite return value.
  fwrite( _data, 1, _size, fp );
  fclose(fp);
}

uint32_t Buffer::readULEB(uint32_t& offset, uint32_t limit) {
  uint32_t res = 0, i;

  for( i = 0; offset < limit; ++i ) {
    uint32_t b = *at<uint8_t>( offset );

    ++offset;

    res |= (b & 0x7F) << (i * 7);
    if( ~b & 0x80 ) {
      return res;
    }
  }

  offset = limit + 1;

  return 0;
}

void Buffer::writeULEB(uint32_t offset, uint32_t x) {
  while(x) {
    uint8_t b = (uint8_t)(x & 0x7f);
    x = x >> 7;
    if( x != 0 ){
      b |= 0x80;
    }

    _data[offset++] = b;
  }
}

Buffer::Buffer() : _data(NULL), _size(-1), _allocd(false) {

}

Buffer::Buffer( size_t size ) : _size(size), _allocd( !!size ), _data( _allocd ? new uint8_t[size] : NULL ) {

}

Buffer::Buffer( size_t size, uint8_t *buffer, bool copy ) :
    _size( size ),
    _allocd( copy )
{
    if( copy == false ) {
        _data = buffer;
    }
    else {
        _data = new uint8_t[_size];
        memcpy( _data, buffer, _size );
    }
}

Buffer::Buffer( const Buffer *b, bool copy ) : _size( b->size() ), _allocd( copy ){
  if( copy == false ) {
      _data = b->data();
  }
  else {
      _data = new uint8_t[_size];
      memcpy( _data, b->data(), _size );
  }
}

Buffer::~Buffer() {
    // dealloc only if we actually own the buffer.
    if( _allocd && _data ){
        delete[] _data;
        _size = -1;
    }
}
