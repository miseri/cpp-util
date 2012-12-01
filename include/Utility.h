#pragma once
#include <cassert>
#include <iomanip>
#include <memory>
#include <cstdint>

#ifdef _WIN32

  #include <Windows.h>
	// From Richter's "Windows via c++"
	/* 
	When the compiler sees a line like this:
	   #pragma chMSG(Fix this later)

	it outputs a line like this:

	  c:\CD\CmnHdr.h(82):Fix this later

	You can easily jump directly to this line and examine the surrounding code.
	*/
  #ifndef chMSG
	  #define chSTR2(x) #x
	  #define chSTR(x)  chSTR2(x)
    #define chMSG(desc) message(__FILE__ "(" chSTR(__LINE__) "):" #desc)
  #endif
#else
  #ifndef chMSG
	  //#define chSTR2(x) #x
	  //#define chSTR(x)  chSTR2(x)
    //#define chMSG(desc) (__FILE__ "(" chSTR(__LINE__) "):" #desc)
    #define chMSG(desc) ("WARNING: " desc)
  #endif
#endif

// MACRO for TODOs
#ifndef TODO
 #define TODO(x) assert 0;return x;
#endif

#ifdef DEF_MAKE_UNIQUE
// Taken from http://stackoverflow.com/questions/7038357/make-unique-and-perfect-forwarding
template<typename T, typename ...Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
#endif

/// Helper templates to print out integers in hexadecimal form
template<typename T>
struct HexStruct
{
  T ui;
  HexStruct(T _ui) : ui(_ui) { }
};

template<typename T>
inline std::ostream& operator<<(std::ostream& o, const HexStruct<T> hs)
{
  return (o << "[0x" << std::setw(8) << std::setfill('0') << std::hex << static_cast<int>(hs.ui) << std::dec << "]");
}

template<uint64_t>
inline std::ostream& operator<<(std::ostream& o, const HexStruct<uint64_t> hs)
{
  return (o << "[0x" << std::setw(16) << std::setfill('0') << std::hex << static_cast<int64_t>(hs.ui) << std::dec << "]");
}

template <typename T>
inline HexStruct<T> hex(T _c)
{
  return HexStruct<T>(_c);
}
