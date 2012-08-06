#pragma once

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

class StringTokenizer
{
public:

  static std::vector<std::string> tokenize(const std::string& sText, const std::string& sTokens = " ", bool trim = false)
  {
    std::vector<std::string> vTokens;
    size_t last_pos = 0;
    for (size_t pos = 0; pos < sText.length(); ++pos)
    {
      for (size_t tokenPos = 0; tokenPos != sTokens.length(); ++tokenPos)
      {
        if (sText[pos] == sTokens[tokenPos])
        {
          vTokens.push_back(sText.substr(last_pos, pos - last_pos));
          last_pos = pos + 1;
        }
      }
    }
    // push back last token
    vTokens.push_back(sText.substr(last_pos));

    if (trim)
    {
      // remove white space
      BOOST_FOREACH(std::string& val, vTokens)
     {
       boost::algorithm::trim(val);
     }
    }

    return vTokens;
  }
};
