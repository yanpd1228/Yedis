#include <string.h>
#include <stdio.h>
template<typename T> struct identity { typedef T type; };
template<typename Dst> Dst implicit_cast(typename identity<Dst>::type t)
{ return t; }
