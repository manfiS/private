#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdexcept>

class Buffer
{
protected:

    // true if a buffer was allocated, false otherwise.
    bool     _allocd;
    // Pointer and size of the buffer.
    uint8_t *_data;
    size_t   _size;

public:

    static Buffer *fromFile( const char *filename );
    void toFile( const char *filename );


    Buffer();
    Buffer( size_t size );
    Buffer( size_t size, uint8_t *buffer, bool copy );
    Buffer( const Buffer *b, bool copy );

    virtual ~Buffer();

    uint32_t readULEB(uint32_t& offset, uint32_t limit);
    void writeULEB(uint32_t offset, uint32_t x);

    inline uint8_t *data() const {
        return _data;
    }

    inline size_t size() const {
        return _size;
    }

    // get a pointer of the specified type at the specified offset.
    template <typename T> T *at( uint32_t offset ) {
        if( sizeof(T) > _size || offset >= _size || offset > (int)(_size - sizeof(T)) ) {
            throw std::runtime_error( "Requested object is out of the buffer boundaries." );
        }

        return (T *)&_data[offset];
    }

    // get a pointer of the specified type at the specified offset and increment offset.
    template <typename T> T *at_ref( uint32_t& offset ) {
        if( sizeof(T) > _size || offset >= _size || offset > (int)(_size - sizeof(T)) ) {
            throw std::runtime_error( "Requested object is out of the buffer boundaries." );
        }

        T *ret = (T *)&_data[offset];
        offset += sizeof(T);
        return ret;
    }
};

#endif
