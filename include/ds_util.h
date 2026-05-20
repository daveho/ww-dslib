// Copyright 2025, David H. Hovemeyer <david.hovemeyer@gmail.com>

// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// “Software”), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
// THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef DS_UTIL_H
#define DS_UTIL_H

// Convenience for declaring copy ctor and assignment
// operator as nonexistent
#define DS_NO_VALUE_SEMANTICS(Type) \
  Type( const Type & ) = delete; \
  Type &operator=( const Type & ) = delete

#ifndef NDEBUG
namespace dslib {

// The project using dslib should define this function
void assert_fail( const char *msg, const char *filename, int line ) __attribute__ ((noreturn));

}

#define DS_ASSERT( expr ) \
if ( !(expr) ) \
  dslib::assert_fail( "Assertion failed: " #expr, __FILE__, __LINE__ )
#else
#define DS_ASSERT( expr )
#endif

#endif // DS_UTIL_H
