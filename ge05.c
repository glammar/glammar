
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 
*/
#include "ge1.h"
extern int argc;
extern char **arg_v;

#ifdef GETNEXTOPTION

/*  getnextoption   */
int Dgetnextoption (A_0)
register AFFIX A_0;
{
  if (argc-- > 0)
  {
    A_0->t = *(arg_v++);
    A_0->l = nil;
    A_0->r = nil;
    return true;
  }
  else
    return false;
}
#endif

#ifdef UGETNEXTOPTION
void Ugetnextoption ()
{
  register cont *rq = q;
  register AFFIX A_0 = (rq + 0)->a;
  if (Dgetnextoption (A_0))
  {
    q = rq + -2;
    (*(rq + -1)->q) ();
    rq = q - -1;
  }
  (rq + 0)->a = A_0;
  (rq + 1)->q = Ugetnextoption;
  q = rq + 1;
}
#endif

#ifdef LAYOUT

/*  layout   */
int Dlayout (A_0)
register AFFIX A_0;
{
  register char *rip = ip, *rc = c;
  if (A_0 == AFX_DONT_CARE)
  {
    while (isspace (*rip))
      rip++;
    ip = rip;
    return true;
  }
  A_0->t = rc;
  A_0->r = nil;
  A_0->l = nil;
  while (isspace (*rip))
    *rc++ = *rip++;
  *rc++ = '\0';
  ip = rip;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
  return true;
}
#endif

#ifdef WHITESPACE

/*  whitespace   */
int Dwhitespace (A_0)
register AFFIX A_0;
{
  return Dlayout (A_0);
}
#endif

#ifdef UWHITESPACE
GLAMMAR_Q1 (Dwhitespace, Uwhitespace)
#endif
#ifdef ULAYOUT
void Ulayout ()
{                               /* quote */
  if (Dlayout ())
  {
    CONTINUE;
  }
  (++q)->q = Ulayout;
}
#endif

#ifdef LENGTH

/*  length   */
int Dlength (A, B)
register AFFIX A, B;
{
  register char *rc = c;
  int len;
  if (((A->r) == nil) && ((A->l) == nil))
    rc = (A->t);
  else
  {
    sprinta (A);
    *c++ = '\0';
  }
  B->t = c;
  B->r = nil;
  B->l = nil;
  sprintf (c, "%d", (int) strlen (rc));
  c += strlen(c);
  *c++ = '\0';
  return true;
}
#endif

#ifdef ULENGTH
void Ulength ()
{
  register cont *rq = q;
  register AFFIX A_1 = (rq + 0)->a;
  register AFFIX A_0 = (rq + -1)->a;
  if (Dlength (A_0, A_1))
  {
    q = rq + -3;
    (*(rq + -2)->q) ();
    rq = q - -2;
  }
  (rq + -1)->a = A_0;
  (rq + 0)->a = A_1;
  (rq + 1)->q = Ulength;
  q = rq + 1;
}
#endif
/*  length   */
#ifdef BASE64



/*************************************************
*          Encode byte-string in base 64         *
*************************************************/

/* This function encodes a string of bytes, containing any values whatsoever,
in base 64 as defined in RFC 2045 (MIME) and required by the SMTP AUTH
extension (RFC 2554). The encoding algorithm is written out in a
straightforward way. Turning it into some kind of compact loop is messy and
would probably run more slowly.

Arguments:
  clear       points to the clear text bytes
  len         the number of bytes to encode

Returns:      a pointer to the zero-terminated base 64 string, which
              is in working store
*/

static char *enc64table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void  base64encode(char *clear, int len, char *code)
{
char *p = code;

while (len-- >0)
  {
  register int x, y;

  x = *clear++;
  *p++ = enc64table[(x >> 2) & 63];

  if (len-- <= 0)
    {
    *p++ = enc64table[(x << 4) & 63];
    *p++ = '=';
    *p++ = '=';
    break;
    }

  y = *clear++;
  *p++ = enc64table[((x << 4) | ((y >> 4) & 15)) & 63];

  if (len-- <= 0)
    {
    *p++ = enc64table[(y << 2) & 63];
    *p++ = '=';
    break;
    }

  x = *clear++;
  *p++ = enc64table[((y << 2) | ((x >> 6) & 3)) & 63];

  *p++ = enc64table[x & 63];
  }

*p = 0;

}
char encodestr[1000000];
int enx =0;        
int menx =1000000;        


static char dec64table[] = {
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, /*  0-15 */
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, /* 16-31 */
  255,255,255,255,255,255,255,255,255,255,255, 62,255,255,255, 63, /* 32-47 */
   52, 53, 54, 55, 56, 57, 58, 59, 60, 61,255,255,255,255,255,255, /* 48-63 */
  255,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, /* 64-79 */
   15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,255,255,255,255,255, /* 80-95 */
  255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, /* 96-111 */
   41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,255,255,255,255,255  /* 112-127*/
};

static int base64decode(char *code, char *result)
{
register int x, y;

char *ptr = result;

/* Each cycle of the loop handles a quantum of 4 input bytes. For the last
 * quantum this may decode to 1, 2, or 3 output bytes. */

while ((x = (*code++)) != 0)
  {
  if (x > 127 || (x = dec64table[x]) == 255) return -1;
  if ((y = (*code++)) == 0 || (y = dec64table[y]) == 255)
    return -1;
  *result++ = (x << 2) | (y >> 4);

  if ((x = (*code++)) == '=')
    {
    if (*code++ != '=' || *code != 0) return -1;
    }
  else
    {
    if (x > 127 || (x = dec64table[x]) == 255) return -1;
    *result++ = (y << 4) | (x >> 2);
    if ((y = (*code++)) == '=')
      {
      if (*code != 0) return -1;
      }
    else
      {
      if (y > 127 || (y = dec64table[y]) == 255) return -1;
      *result++ = (x << 6) | y;
      }
    }
  }

*result = 0;
return result - ptr;
}

int Dbase64encode (A)
register AFFIX A;
{
  register char *rc = c;
  A->t = c;
  A->r = nil;
  A->l = nil;
   if (!encodestr)
   {
     *c++='\0';
     return true;
   }
   if (c + enx*2  > cstore_top)
         cstore_overflow ();

  base64encode(encodestr,enx, c);
  c+= enx*2;
  *c++ = '\0';
  enx=0;
  return true;
}
int Dbase64addencode (A)
register AFFIX A;
{
  register char *rc = c;
  int len;
  if (((A->r) == nil) && ((A->l) == nil))
    rc = (A->t);
  else
  {
    sprinta (A);
    *c++ = '\0';
  }
  len=strlen(rc);
   if (enx + len*2  > menx)
   {
     fprintf(stderr, "glammar.g addbase64encode buffer overflow (max=%d bytes)\n",menx);
     exit(1);
   }
  memcpy(encodestr+enx,rc, len);

  enx+=len;
  encodestr[enx] = '\0';

  return true;
}

static int base64intencode (A,B)
register char * A;
register char * B;
{
  register char *rc = A;
  long long v8 = 0;
  int v4 = 0;
  int len = 0;

  len  =  atoi(rc);
  rc = B;
      switch  (len) {
      case 1: 
             v4 = atoi(rc);
             encodestr[enx++] = (v4 & 0x000000ff);
            break;

      case 2:
             v4 = atoi(rc);
             encodestr[enx++] = (v4 & 0x0000ff00) >>8;
             encodestr[enx++] = (v4 & 0x000000ff);
              break;

      case 4: 
             v4 = atoi(rc);
             encodestr[enx++] = (v4 & 0xff000000) >>24;
             encodestr[enx++] = (v4 & 0x00ff0000) >>16;
             encodestr[enx++] = (v4 & 0x0000ff00) >>8;
             encodestr[enx++] = (v4 & 0x000000ff);
              break;
      case 8: 

             v8 = atoi(rc);
             v4 = v8 >>32; 
             encodestr[enx++] = (v4 & 0xff000000) >>24;
             encodestr[enx++] = (v4 & 0x00ff0000) >>16;
             encodestr[enx++] = (v4 & 0x0000ff00) >>8;
             encodestr[enx++] = (v4 & 0x000000ff);
             v4 = v8 & 0x00000000ffffffff;
             encodestr[enx++] = (v4 & 0xff000000) >>24;
             encodestr[enx++] = (v4 & 0x00ff0000) >>16;
             encodestr[enx++] = (v4 & 0x0000ff00) >>8;
             encodestr[enx++] = (v4 & 0x000000ff);
              break;
      default:  return false;
      }

  encodestr[enx] = '\0';

  return true;
}

int Dbase64intencode (A,B)
register AFFIX A;
register AFFIX B;
{
  register char *rc = c;
  char *a, *b;

  if (((A->r) == nil) && ((A->l) == nil))
    rc = (A->t);
  else
  {
    sprinta (A);
    *c++ = '\0';
  }
  a = rc;
  rc = c;
  if (((B->r) == nil) && ((B->l) == nil))
    rc = (B->t);
  else
  {
    sprinta (B);
    *c++ = '\0';
  }
  b = rc;
  base64intencode (a,b);

  return true;
}

int Dbase64decode (A, B)
register AFFIX A, B;
{
  register char *rc = c;
  int len;
  if (((A->r) == nil) && ((A->l) == nil))
    rc = (A->t);
  else
  {
    sprinta (A);
    *c++ = '\0';
  }
  B->t = c;
  B->r = nil;
  B->l = nil;
   if (c + strlen(rc)  > cstore_top)
         cstore_overflow ();

   base64decode(rc, c);
  return true;
}
GLAMMAR_Q1 (Dbase64encode, Ubase64encode)
GLAMMAR_Q1 (Dbase64addencode, Ubase64addencode)
GLAMMAR_Q2 (Dbase64intencode, Ubase64intencode)
GLAMMAR_Q2 (Dbase64decode, Ubase64decode)

#endif
