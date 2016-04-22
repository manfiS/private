#include "utils.hpp"
#include "buffer.h"
#include <vector>
#include <ctype.h>
#include <map>

using namespace std;

#define DEX_VERSION_FOR_API_13 "035"
#define DEX_VERSION_CURRENT    "036"
#define STRING_ID_ELEMENT_SIZE 4

#define DEX_STRUCT(NAME,FIELDS) \
    typedef struct { \
        FIELDS \
    } __attribute__((packed)) NAME

DEX_STRUCT( dex_magic_t,
    char dex[3];
    char newline;
    char ver[3];
    char zero;
);

DEX_STRUCT( dex_header_t,
    dex_magic_t magic;
    uint32_t checksum;
    uint8_t signature[20];
    uint32_t file_size;
    uint32_t header_size;
    uint32_t endian_tag;
    uint32_t link_size;
    uint32_t link_off;
    uint32_t map_off;
    uint32_t string_ids_size;
    uint32_t string_ids_off;
    uint32_t type_ids_size;
    uint32_t type_ids_off;
    uint32_t proto_ids_size;
    uint32_t proto_ids_off;
    uint32_t field_ids_size;
    uint32_t field_ids_off;
    uint32_t method_ids_size;
    uint32_t method_ids_off;
    uint32_t class_defs_size;
    uint32_t class_defs_off;
    uint32_t data_size;
    uint32_t data_off;
);

typedef struct dex_string {
  uint32_t  size_offset;
  uint32_t  offset;
  char     *data;
  uint32_t  size;

  dex_string( uint32_t so, uint32_t o, char *p, uint32_t s ) : size_offset(so), offset(o), data(p), size(s) {

  }
}
dex_string_t;

#define SS3TOKEN "Lcom/streamlinedmobile/sunshine3/"
#define PADBYTE 'A'

static bool isObfuscated( dex_string_t *s ) {
  // FIXME: Ci serve un criterio migliore per distinguere le stringhe
  // offuscate da quelle semplicemente in jap, arabo e così via.
  if( strstr( s->data, SS3TOKEN ) == s->data && strchr( s->data, ';' ) ){
    for( char *p = s->data + strlen(SS3TOKEN), *e = s->data + s->size; p < e; ++p ) {
      if( !isprint( *p ) ){
        return true;
      }
    }
  }

  return false;
}

static bool patch( Buffer *buffer, dex_string_t *s, uint32_t index ) {
  // "Lcom/streamlinedmobile/sunshine3/GARGBAGEGARGBAGEGARGBAGEGARGBAGE;" -> "Lcom/streamlinedmobile/sunshine3/SNNNN;"
  char newname[0xFF] = {0};
  sprintf( newname, "S%u;", index );

  uint32_t pindex = strlen(SS3TOKEN),
           left = s->size - pindex;

  if( left < strlen(newname) ){
    return false;
  }

  memcpy( &s->data[pindex], newname, strlen(newname) );

  uint32_t newlen = strlen(SS3TOKEN) + strlen(newname);

  // fix size
  buffer->writeULEB( s->size_offset, newlen );

  return true;
}

int main( int argc, char **argv )
{
  if( argc < 2 ){
    printf( "Usage: %s input.dex\n", argv[0] );
    return 1;
  }

  Buffer *buffer( NULL );
  vector<dex_string_t *> strings;

  try
  {
    buffer = Buffer::fromFile( argv[1] );

    printf( "@ Read %lu bytes from %s.\n", buffer->size(), argv[1] );

    dex_header_t *_header( buffer->at<dex_header_t>(0) );
    uint32_t _data_limit;

    if( buffer->size() < sizeof(dex_header_t) ) {
      Utils::throwf( "File classes.dex is smaller than DEX header size." );
    }
    else if( memcmp( &_header->magic.dex, "dex", 3 ) != 0 || _header->magic.newline != '\n' || _header->magic.zero != '\x00' ) {
      Utils::throwf( "Unexpected DEX magic value." );
    }
    else if( memcmp( &_header->magic.ver, DEX_VERSION_FOR_API_13, 3 ) != 0 && memcmp( &_header->magic.ver, DEX_VERSION_CURRENT, 3 ) != 0 )  {
      Utils::throwf( "Unsupported DEX version %s.", _header->magic.ver );
    }
    else if( _header->header_size != sizeof(dex_header_t) ) {
      Utils::throwf( "Invalid DEX header size field, expected %u got %u.", sizeof(dex_header_t), _header->header_size );
    }
    else if( _header->file_size != buffer->size() ){
      Utils::throwf( "Invalid file size, expected 0x%X, found 0x%X.", buffer->size(), _header->file_size );
    }
    else if( _header->endian_tag != 0x12345678 ){
      Utils::throwf( "Unsupported endian tag, expected 0x12345678 got 0x%X.", _header->endian_tag );
    }
    else if( _header->string_ids_off >= buffer->size() ){
      Utils::throwf( "Invalid string ids offset 0x%X.", _header->string_ids_off );
    }
    else if( _header->string_ids_size != 0 && _header->string_ids_off + STRING_ID_ELEMENT_SIZE * _header->string_ids_size > buffer->size() ){
      Utils::throwf( "DEX string table is not within file bounds." );
    }

    _data_limit = _header->data_off + _header->data_size;

    printf( "@ Parsing string section @ 0x%08X ...\n", _header->string_ids_off );

    uint32_t offset = _header->string_ids_off;

    for( uint32_t i = 0; i < _header->string_ids_size; ++i, offset += sizeof(uint32_t) ){
      uint32_t data_offset = *buffer->at<uint32_t>(offset);
      uint32_t lenoff = data_offset;
      uint32_t length = buffer->readULEB( data_offset, _data_limit );

      if( data_offset + length < _data_limit && length ) {
        strings.push_back( new dex_string_t( lenoff, data_offset, buffer->at<char>(data_offset), length ) );
      }
    }

    printf( "@ Extracted %lu strings, processing ...\n", strings.size() );

    for( uint32_t i = 0, e = strings.size(); i < e; ++i ){
      dex_string_t *s = strings[i];
      if( isObfuscated(s) ) {
        printf( "  Patching %u bytes @ 0x%08X: %s\n", s->size, s->offset, patch( buffer, s, i ) ? "SUCCESS" : "FAILED" );
      }
    }

    char outname[0xFF] = {0};
    sprintf( outname, "%s-deobf", argv[1] );

    buffer->toFile( outname );
  }
  catch( const exception& e )
  {
    printf( "\nERROR: %s\n", e.what() );
  }


  if( buffer ){
    delete buffer;
  }

  return 0;
}
