#ifndef INCLUDED_USRP_BYTESEX_H
#define INCLUDED_USRP_BYTESEX_H

# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif

#ifdef WORDS_BIGENDIAN

static inline unsigned int
host_to_usrp_u32 (unsigned int x)
{
  return bswap_32(x);
}

static inline unsigned int
usrp_to_host_u32 (unsigned int x)
{
  return bswap_32(x);
}

static inline short int
host_to_usrp_short (short int x)
{
  return bswap_16 (x);
}

static inline short int
usrp_to_host_short (short int x)
{
  return bswap_16 (x);
}

#else

static inline unsigned int
host_to_usrp_u32 (unsigned int x)
{
  return x;
}

static inline unsigned int
usrp_to_host_u32 (unsigned int x)
{
  return x;
}

static inline short int
host_to_usrp_short (short int x)
{
  return x;
}

static inline short int
usrp_to_host_short (unsigned short int x)
{
  return x;
}

#endif

#endif /* INCLUDED_USRP_BYTESEX_H */
