#pragma once
#include <cstdint>
#include <map>
#include <string>
#include <boost/optional.hpp>

class GenericParameters
{
public:

    boost::optional<int32_t> getIntParameter(const std::string& sParamName) const
    {
        auto it = m_mIntParameters.find(sParamName);
        if (it != m_mIntParameters.end())
            return boost::optional<int32_t>(it->second);
        return boost::optional<int32_t>();
    }

    boost::optional<uint32_t> getUintParameter(const std::string& sParamName) const
    {
        auto it = m_mUintParameters.find(sParamName);
        if (it != m_mUintParameters.end())
            return boost::optional<uint32_t>(it->second);
        return boost::optional<uint32_t>();
    }

    boost::optional<bool> getBoolParameter(const std::string& sParamName) const
    {
        auto it = m_mBoolParameters.find(sParamName);
        if (it != m_mBoolParameters.end())
            return boost::optional<bool>(it->second);
        return boost::optional<bool>();
    }

    boost::optional<double> getDoubleParameter(const std::string& sParamName) const
    {
        auto it = m_mDoubleParameters.find(sParamName);
        if (it != m_mDoubleParameters.end())
            return boost::optional<double>(it->second);
        return boost::optional<double>();
    }

    boost::optional<std::string> getStringParameter(const std::string& sParamName) const
    {
        auto it = m_mStringParameters.find(sParamName);
        if (it != m_mStringParameters.end())
            return boost::optional<std::string>(it->second);
        return boost::optional<std::string>();
    }

    void setIntParameter( const std::string& sParamName, int32_t iValue)
    {
      m_mIntParameters[sParamName] = iValue;
    }

    void setUintParameter( const std::string& sParamName, uint32_t uiValue)
    {
      m_mUintParameters[sParamName] = uiValue;
    }

    void setBoolParameter( const std::string& sParamName, bool bValue)
    {
      m_mBoolParameters[sParamName] = bValue;
    }

    void setDoubleParameter( const std::string& sParamName, const double dParamValue)
    {
      m_mDoubleParameters[sParamName] = dParamValue;
    }

    void setStringParameter( const std::string& sParamName, const std::string& sParamValue)
    {
      m_mStringParameters[sParamName] = sParamValue;
    }

    std::map<std::string, int32_t> m_mIntParameters;
    std::map<std::string, uint32_t> m_mUintParameters;
    std::map<std::string, bool> m_mBoolParameters;
    std::map<std::string, double> m_mDoubleParameters;
    std::map<std::string, std::string> m_mStringParameters;
};
