#include <stdarg.h>
#include <stddef.h>

void* stdout = NULL;
char printf_buffer[1024];

struct timeval {
    long tv_sec;
    long tv_usec;
};

int gettimeofday(struct timeval *tv, struct timezone *tz) {
    if (tv == NULL) {
        return -1;
    }

    tv->tv_sec = (long int) current_time();
    tv->tv_usec = 0L;

    return 0;
}

struct tm * localtime_r (const long int *t, struct tm *tp) {
  printf("localtime_r()\n");
  return tp;
}

size_t strlen(const char *str) {
  const char *char_ptr;
  const unsigned long int *longword_ptr;
  unsigned long int longword, himagic, lomagic;
  /* Handle the first few characters by reading one character at a time.
     Do this until CHAR_PTR is aligned on a longword boundary.  */
  for (char_ptr = str; ((unsigned long int) char_ptr
                        & (sizeof (longword) - 1)) != 0;
       ++char_ptr)
    if (*char_ptr == '\0')
      return char_ptr - str;
  /* All these elucidatory comments refer to 4-byte longwords,
     but the theory applies equally well to 8-byte longwords.  */
  longword_ptr = (unsigned long int *) char_ptr;
  /* Bits 31, 24, 16, and 8 of this number are zero.  Call these bits
     the "holes."  Note that there is a hole just to the left of
     each byte, with an extra at the end:
     bits:  01111110 11111110 11111110 11111111
     bytes: AAAAAAAA BBBBBBBB CCCCCCCC DDDDDDDD
     The 1-bits make sure that carries propagate to the next 0-bit.
     The 0-bits provide holes for carries to fall into.  */
  himagic = 0x80808080L;
  lomagic = 0x01010101L;
  if (sizeof (longword) > 4)
    {
      /* 64-bit version of the magic.  */
      /* Do the shift in two steps to avoid a warning if long has 32 bits.  */
      himagic = ((himagic << 16) << 16) | himagic;
      lomagic = ((lomagic << 16) << 16) | lomagic;
    }
  if (sizeof (longword) > 8)
    abort ();
  /* Instead of the traditional loop which tests each character,
     we will test a longword at a time.  The tricky part is testing
     if *any of the four* bytes in the longword in question are zero.  */
  for (;;)
    {
      longword = *longword_ptr++;
      if (((longword - lomagic) & ~longword & himagic) != 0)
        {
          /* Which of the bytes was the zero?  If none of them were, it was
             a misfire; continue the search.  */
          const char *cp = (const char *) (longword_ptr - 1);
          if (cp[0] == 0)
            return cp - str;
          if (cp[1] == 0)
            return cp - str + 1;
          if (cp[2] == 0)
            return cp - str + 2;
          if (cp[3] == 0)
            return cp - str + 3;
          if (sizeof (longword) > 4)
            {
              if (cp[4] == 0)
                return cp - str + 4;
              if (cp[5] == 0)
                return cp - str + 5;
              if (cp[6] == 0)
                return cp - str + 6;
              if (cp[7] == 0)
                return cp - str + 7;
            }
        }
    }
}

int printf(const char *fmt, ...) {
    va_list ap;
    int total;

    va_start(ap, fmt);
    char *buffer = &printf_buffer;
    buffer[0] = 0;
    total = vsnprintf(buffer, 1024, fmt, ap);
    puts(buffer);
    va_end(ap);

    return total;
}

int strcmp(const char *p1, const char *p2) {
  const unsigned char *s1 = (const unsigned char *) p1;
  const unsigned char *s2 = (const unsigned char *) p2;
  unsigned char c1, c2;

  do {
      c1 = (unsigned char) *s1++;
      c2 = (unsigned char) *s2++;
      if (c1 == '\0') {
          return c1 - c2;
      }
  } while (c1 == c2);
  return c1 - c2;
}


char* strcpy(char *dest, const char *src) {
    return memcpy(dest, src, strlen (src) + 1);
}

char* strcat(char *dest, const char *src) {
  strcpy (dest + strlen (dest), src);
  return dest;
}

char* strchr(const char *s, int c_in) {
  const unsigned char *char_ptr;
  const unsigned long int *longword_ptr;
  unsigned long int longword, magic_bits, charmask;
  unsigned char c;

  c = (unsigned char) c_in;

  /* Handle the first few characters by reading one character at a time.
     Do this until CHAR_PTR is aligned on a longword boundary.  */
  for (char_ptr = (const unsigned char *) s;
       ((unsigned long int) char_ptr & (sizeof (longword) - 1)) != 0;
       ++char_ptr)
    if (*char_ptr == c)
      return (void *) char_ptr;
    else if (*char_ptr == '\0')
      return NULL;

  /* All these elucidatory comments refer to 4-byte longwords,
     but the theory applies equally well to 8-byte longwords.  */

  longword_ptr = (unsigned long int *) char_ptr;

  /* Bits 31, 24, 16, and 8 of this number are zero.  Call these bits
     the "holes."  Note that there is a hole just to the left of
     each byte, with an extra at the end:
     bits:  01111110 11111110 11111110 11111111
     bytes: AAAAAAAA BBBBBBBB CCCCCCCC DDDDDDDD
     The 1-bits make sure that carries propagate to the next 0-bit.
     The 0-bits provide holes for carries to fall into.  */
  magic_bits = -1;
  magic_bits = magic_bits / 0xff * 0xfe << 1 >> 1 | 1;

  /* Set up a longword, each of whose bytes is C.  */
  charmask = c | (c << 8);
  charmask |= charmask << 16;
  if (sizeof (longword) > 4)
    /* Do the shift in two steps to avoid a warning if long has 32 bits.  */
    charmask |= (charmask << 16) << 16;
  if (sizeof (longword) > 8)
    abort ();

  /* Instead of the traditional loop which tests each character,
     we will test a longword at a time.  The tricky part is testing
     if *any of the four* bytes in the longword in question are zero.  */
  for (;;)
    {
      /* We tentatively exit the loop if adding MAGIC_BITS to
     LONGWORD fails to change any of the hole bits of LONGWORD.
     1) Is this safe?  Will it catch all the zero bytes?
     Suppose there is a byte with all zeros.  Any carry bits
     propagating from its left will fall into the hole at its
     least significant bit and stop.  Since there will be no
     carry from its most significant bit, the LSB of the
     byte to the left will be unchanged, and the zero will be
     detected.
     2) Is this worthwhile?  Will it ignore everything except
     zero bytes?  Suppose every byte of LONGWORD has a bit set
     somewhere.  There will be a carry into bit 8.  If bit 8
     is set, this will carry into bit 16.  If bit 8 is clear,
     one of bits 9-15 must be set, so there will be a carry
     into bit 16.  Similarly, there will be a carry into bit
     24.  If one of bits 24-30 is set, there will be a carry
     into bit 31, so all of the hole bits will be changed.
     The one misfire occurs when bits 24-30 are clear and bit
     31 is set; in this case, the hole at bit 31 is not
     changed.  If we had access to the processor carry flag,
     we could close this loophole by putting the fourth hole
     at bit 32!
     So it ignores everything except 128's, when they're aligned
     properly.
     3) But wait!  Aren't we looking for C as well as zero?
     Good point.  So what we do is XOR LONGWORD with a longword,
     each of whose bytes is C.  This turns each byte that is C
     into a zero.  */

      longword = *longword_ptr++;

      /* Add MAGIC_BITS to LONGWORD.  */
      if ((((longword + magic_bits)

        /* Set those bits that were unchanged by the addition.  */
        ^ ~longword)

       /* Look at only the hole bits.  If any of the hole bits
          are unchanged, most likely one of the bytes was a
          zero.  */
       & ~magic_bits) != 0 ||

      /* That caught zeroes.  Now test for C.  */
      ((((longword ^ charmask) + magic_bits) ^ ~(longword ^ charmask))
       & ~magic_bits) != 0)
    {
      /* Which of the bytes was C or zero?
         If none of them were, it was a misfire; continue the search.  */

      const unsigned char *cp = (const unsigned char *) (longword_ptr - 1);

      if (*cp == c)
        return (char *) cp;
      else if (*cp == '\0')
        return NULL;
      if (*++cp == c)
        return (char *) cp;
      else if (*cp == '\0')
        return NULL;
      if (*++cp == c)
        return (char *) cp;
      else if (*cp == '\0')
        return NULL;
      if (*++cp == c)
        return (char *) cp;
      else if (*cp == '\0')
        return NULL;
      if (sizeof (longword) > 4)
        {
          if (*++cp == c)
        return (char *) cp;
          else if (*cp == '\0')
        return NULL;
          if (*++cp == c)
        return (char *) cp;
          else if (*cp == '\0')
        return NULL;
          if (*++cp == c)
        return (char *) cp;
          else if (*cp == '\0')
        return NULL;
          if (*++cp == c)
        return (char *) cp;
          else if (*cp == '\0')
        return NULL;
        }
    }
    }

  return NULL;
}

char* strrchr(const char *s, int c) {
    const char *found, *p;
    c = (unsigned char) c;

    /* Since strchr is fast, we use it rather than the obvious loop.  */

    if (c == '\0') {
        return strchr(s, '\0');
    }

    found = NULL;
    while ((p = strchr (s, c)) != NULL) {
        found = p;
        s = p + 1;
    }

    return (char *) found;
}

void* memchr(void const *s, int c_in, size_t n) {
  /* On 32-bit hardware, choosing longword to be a 32-bit unsigned
     long instead of a 64-bit uintmax_t tends to give better
     performance.  On 64-bit hardware, unsigned long is generally 64
     bits already.  Change this typedef to experiment with
     performance.  */
  typedef unsigned long int longword;

  const unsigned char *char_ptr;
  const longword *longword_ptr;
  longword repeated_one;
  longword repeated_c;
  unsigned char c;

  c = (unsigned char) c_in;

  /* Handle the first few bytes by reading one byte at a time.
     Do this until CHAR_PTR is aligned on a longword boundary.  */
  for (char_ptr = (const unsigned char *) s;
       n > 0 && (size_t) char_ptr % sizeof (longword) != 0;
       --n, ++char_ptr)
    if (*char_ptr == c)
      return (void *) char_ptr;

  longword_ptr = (const longword *) char_ptr;

  /* All these elucidatory comments refer to 4-byte longwords,
     but the theory applies equally well to any size longwords.  */

  /* Compute auxiliary longword values:
     repeated_one is a value which has a 1 in every byte.
     repeated_c has c in every byte.  */
  repeated_one = 0x01010101;
  repeated_c = c | (c << 8);
  repeated_c |= repeated_c << 16;
  if (0xffffffffU < (longword) -1)
    {
      repeated_one |= repeated_one << 31 << 1;
      repeated_c |= repeated_c << 31 << 1;
      if (8 < sizeof (longword))
    {
      size_t i;

      for (i = 64; i < sizeof (longword) * 8; i *= 2)
        {
          repeated_one |= repeated_one << i;
          repeated_c |= repeated_c << i;
        }
    }
    }

  /* Instead of the traditional loop which tests each byte, we will test a
     longword at a time.  The tricky part is testing if *any of the four*
     bytes in the longword in question are equal to c.  We first use an xor
     with repeated_c.  This reduces the task to testing whether *any of the
     four* bytes in longword1 is zero.
     We compute tmp =
       ((longword1 - repeated_one) & ~longword1) & (repeated_one << 7).
     That is, we perform the following operations:
       1. Subtract repeated_one.
       2. & ~longword1.
       3. & a mask consisting of 0x80 in every byte.
     Consider what happens in each byte:
       - If a byte of longword1 is zero, step 1 and 2 transform it into 0xff,
     and step 3 transforms it into 0x80.  A carry can also be propagated
     to more significant bytes.
       - If a byte of longword1 is nonzero, let its lowest 1 bit be at
     position k (0 <= k <= 7); so the lowest k bits are 0.  After step 1,
     the byte ends in a single bit of value 0 and k bits of value 1.
     After step 2, the result is just k bits of value 1: 2^k - 1.  After
     step 3, the result is 0.  And no carry is produced.
     So, if longword1 has only non-zero bytes, tmp is zero.
     Whereas if longword1 has a zero byte, call j the position of the least
     significant zero byte.  Then the result has a zero at positions 0, ...,
     j-1 and a 0x80 at position j.  We cannot predict the result at the more
     significant bytes (positions j+1..3), but it does not matter since we
     already have a non-zero bit at position 8*j+7.
     So, the test whether any byte in longword1 is zero is equivalent to
     testing whether tmp is nonzero.  */

  while (n >= sizeof (longword))
    {
      longword longword1 = *longword_ptr ^ repeated_c;

      if ((((longword1 - repeated_one) & ~longword1)
       & (repeated_one << 7)) != 0)
    break;
      longword_ptr++;
      n -= sizeof (longword);
    }

  char_ptr = (const unsigned char *) longword_ptr;

  /* At this point, we know that either n < sizeof (longword), or one of the
     sizeof (longword) bytes starting at char_ptr is == c.  On little-endian
     machines, we could determine the first such byte without any further
     memory accesses, just by looking at the tmp result from the last loop
     iteration.  But this does not work on big-endian machines.  Choose code
     that works in both cases.  */

  for (; n > 0; --n, ++char_ptr)
    {
      if (*char_ptr == c)
    return (void *) char_ptr;
    }

  return NULL;
}

int atoi(const char* s) {
    long int v=0;
    int sign=1;
    while ( *s == ' '  ||  (unsigned int)(*s - 9) < 5u) s++;
    switch (*s) {
        case '-': sign=-1; /* fall through */
        case '+': ++s;
    }
    while ((unsigned int) (*s - '0') < 10u) {
        v=v*10+*s-'0'; ++s;
    }
    return sign==-1?-v:v;
}

#define ldbltype double
double strtod(const char* s, char** endptr) {
    register const char*  p     = s;
    register ldbltype     value = 0.;
    int                   sign  = +1;
    ldbltype              factor;
    unsigned int          expo;

    while ((unsigned int)(*p - 9) < 5u  ||  *p == ' ')
        p++;

    switch (*p) {
    case '-': sign = -1;    /* fall through */
    case '+': p++;
    default : break;
    }

    while ( (unsigned int)(*p - '0') < 10u )
        value = value*10 + (*p++ - '0');

    if ( *p == '.' ) {
        factor = 1.;

        p++;
        while ( (unsigned int)(*p - '0') < 10u ) {
            factor *= 0.1;
            value  += (*p++ - '0') * factor;
        }
    }

    if ( (*p | 32) == 'e' ) {
        expo   = 0;
        factor = 10.;

        switch (*++p) {                 // ja hier weiß ich nicht, was mindestens nach einem 'E' folgenden MUSS.
        case '-': factor = 0.1;    /* fall through */
        case '+': p++;
                  break;
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                  break;
        default : value = 0.;
                  p     = s;
                  goto done;
        }

        while ( (unsigned int)(*p - '0') < 10u )
            expo = 10 * expo + (*p++ - '0');

        while ( 1 ) {
            if ( expo & 1 )
                value *= factor;
            if ( (expo >>= 1) == 0 )
                break;
            factor *= factor;
        }
    }

done:
    if ( endptr != NULL )
        *endptr = (char*)p;

    return value * sign;
}
