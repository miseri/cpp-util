#pragma once

// STL
#include <cassert>
#include <limits>
#include <fstream>
#include <string>

#ifndef _WIN32
	#include <sys/statvfs.h>
#endif

// boost
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

// RTVC
#include "Buffer.h"
#include "ExceptionBase.h"

namespace bfs = boost::filesystem;

/**
 * File utility functions
 */
class FileUtil
{
public:
	static std::string readFile(const std::string& sFile)
	{
		std::ifstream in1(sFile.c_str(), std::ios_base::in | std::ios_base::binary);
		if (in1.is_open())
		{
			// get length of file:
			in1.seekg (0, std::ios::end);
      std::streamoff length = in1.tellg();
			in1.seekg (0, std::ios::beg);

      // Just in case
      assert(length < UINT_MAX);
      unsigned uiSize = static_cast<unsigned>(length);
			char* szBuffer = new char[uiSize];
			// read data as a block:
			in1.read (szBuffer, length);
			in1.close();

			std::string sFileContent(szBuffer, uiSize);
			delete[] szBuffer;
			return sFileContent;
		}
		else
		{
			BOOST_THROW_EXCEPTION(ExceptionBase("Failed to open file " + sFile));
		}
	}

  static bool writeFile(const std::string& sFileName, const std::string& sContent)
  {
    std::ofstream out1(sFileName.c_str(), std::ios_base::out);
    if (out1.is_open())
    {
      out1 << sContent;
      out1.close();
      return true;
    }
    return false;
  }

  static bool writeFile(const std::string& sFileName, const char* szBuffer, unsigned uiSize)
  {
    std::ofstream out1(sFileName.c_str(), std::ios_base::out);
    if (out1.is_open())
    {
      out1.write(szBuffer, uiSize);
      out1.close();
      return true;
    }
    return false;
  }

  static bool writeFile(const std::string& sFileName, const Buffer buffer)
  {
    std::ofstream out1(sFileName.c_str(), std::ios_base::out);
    if (out1.is_open())
    {
      const char* pBuffer = reinterpret_cast<const char*>(buffer.data());
      out1.write(pBuffer, buffer.getSize());
      out1.close();
      return true;
    }
    return false;
  }

  static bool writeBinaryFile(const std::string& sFileName, const char* szBuffer, unsigned uiSize)
  {
    std::ofstream out1(sFileName.c_str(), std::ios_base::out | std::ios_base::binary);
    if (out1.is_open())
    {
      out1.write(szBuffer, uiSize);
      out1.close();
      return true;
    }
    return false;
  }

  static bool writeBinaryFile(const std::string& sFileName, const Buffer buffer)
  {
    std::ofstream out1(sFileName.c_str(), std::ios_base::out | std::ios_base::binary);
    if (out1.is_open()) 
    {
      const char* pBuffer = reinterpret_cast<const char*>(buffer.data());
      out1.write(pBuffer, buffer.getSize());
      out1.close();
      return true;
    }
    return false;
  }

  static double calculateFreeSpacePercentage(const std::string& sRootDirectory)
  {
  #ifdef _WIN32
    { sRootDirectory; }
    ULARGE_INTEGER uliFreeBytesAvailable;
	  ULARGE_INTEGER uliTotalNumberOfBytes;
	  ULARGE_INTEGER uliTotalNumberOfFreeBytes;

	  uliFreeBytesAvailable.QuadPart = 0L;
	  uliTotalNumberOfBytes.QuadPart = 0L;
	  uliTotalNumberOfFreeBytes.QuadPart = 0L;

	  if( GetDiskFreeSpaceEx( 
		  NULL,                           // directory name: NULL causes the call to use the current root
		  &uliFreeBytesAvailable,         // bytes available to caller
		  &uliTotalNumberOfBytes,         // bytes on disk
		  &uliTotalNumberOfFreeBytes) )   // free bytes on disk
	  {
		  double dFreeGigsAvailable = (double)( (signed __int64)(uliFreeBytesAvailable.QuadPart)/1.0e9 );
		  double dTotalGigs = (double)( (signed __int64)(uliTotalNumberOfBytes.QuadPart)/1.0e9 );
		  return (double)dFreeGigsAvailable/dTotalGigs;
	  }
	  else
	  {
		  // Get last error
		  DWORD error = GetLastError();
		  printf("Last error code: %d", error);
		  return 0;
	  }
  #else
	  // Linux
	  struct statvfs fiData;
	  struct statvfs *fpData;

	  if((statvfs(sRootDirectory.c_str(), &fiData)) < 0 ) 
	  {
		  printf("Failed to stat %s:\n", sRootDirectory.c_str());
		  return 0;
	  } 
	  else 
	  {
		  // Note the values returned in the structure do NOT match
		  // the df OS call in that the free space given from df
		  // usually takes a 5% reserve into account. Google "Ext3 reserved filesystem space"
		  // for more information
		  /*
		  printf("Disk %s: \n", sRootDirectory.c_str());
		  printf("\tblock size: %u\n", fiData.f_bsize);
		  printf("\tblock size: %u\n", fiData.f_frsize);
		  printf("\ttotal no blocks: %i\n", fiData.f_blocks);
		  printf("\tfree blocks: %i\n", fiData.f_bfree);
		  */
		  // total bytes = f_frsize * f_blocks;
		  fsblkcnt_t fTotalBlocks = fiData.f_blocks;
		  fsblkcnt_t fFreeBlocks = fiData.f_bfree;

		  double dFreePercentage = (double)fFreeBlocks/fTotalBlocks;
		  //printf("Percentage free: %f", dFreePercentage);
		  return dFreePercentage;
	  }
  #endif
  }

  static std::vector<std::string> getFileList( const std::string& sPath, const std::string& sFileSignature )
  {
    std::vector<std::string> vFiles;
    bfs::directory_iterator end_iter;
    for ( bfs::directory_iterator dir_itr( sPath ); dir_itr != end_iter; ++dir_itr )
    {
      if (bfs::is_regular(dir_itr->status()))
      {
        std::string sAbsoluteFileName = dir_itr->path().string();
        std::string sFileName = dir_itr->path().leaf().string();
        if (sFileSignature != "")
        {
          const boost::regex e(sFileSignature);
          //Try and match filname pattern
          if (regex_search(sFileName, e))
          {
            vFiles.push_back(sFileName);
          }
        }
        else
        {
          vFiles.push_back(sFileName);
        }
      }
    }
    return vFiles;
  }
};

