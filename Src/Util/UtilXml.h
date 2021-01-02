#pragma once

#include <string>
#include "Util\rapidxml-1.13\rapidxml.hpp"

std::string GetAttribute(rapidxml::xml_node<>* node, const std::string& name);

#include <functional>
void NodeApply(rapidxml::xml_node<>* node, const std::function<void(rapidxml::xml_node<>*)>& func);


#include "Util.h"
#include "WinUtil.h"
template<class T>
inline T ConvertType(const std::string& str)
{
    return String2Numeric<T>(str);
}

template<>
inline std::wstring ConvertType<std::wstring>(const std::string& str)
{
    return StringToWString(str);
}

template<>
inline std::string ConvertType<std::string>(const std::string& str)
{
    return str;
}


#include <algorithm>
template<>
inline bool ConvertType<bool>(const std::string& str)
{
    std::string lower(str);
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower == "true" ? true : false;
}

template<class T>
inline T ConvertType(const std::string& str, const T& def)
{
    if (str.empty())
    {
        return def;
    }
    return ConvertType<T>(str);
}


template<size_t n>
inline std::string AccessVecElement(const std::vector<std::string>& vec)
{
    return vec[n];
}

template <class T>
inline T GetAttrVal(rapidxml::xml_node<>* node, const std::string& name)
{
    return ConvertType<T>(GetAttribute(node, name));
}

template <class T>
inline T GetAttrVal(rapidxml::xml_node<>* node, const std::string& name, const T& def)
{
    return ConvertType<T>(GetAttribute(node, name), def);
}

// N≠0なら、先頭を削ってNをデクリメント
template <size_t N, class T0, class ... T>
struct type_at : public type_at<N - 1, T ...> {};

// N＝0なら、先頭の型が求めるべき型
template <class T0, class ... T>
struct type_at<0, T0, T...>
{
    typedef T0 type;
};


template <size_t n, class ... T>
inline auto GetAttrVal_Auto(rapidxml::xml_node<>* node, const std::vector<std::string>& names)
{
    return GetAttrVal< type_at<n, T...>::type >(node, AccessVecElement<n>(names));
}

template <size_t n, class U, class ... T>
inline auto GetAttrVal_Auto(rapidxml::xml_node<>* node, const std::vector<std::string>& names, const U& def)
{
    return GetAttrVal< type_at<n, T...>::type >(node, AccessVecElement<n>(names), def);
}

template <size_t n, class ... T>
inline void SetAttrVal(rapidxml::xml_node<>* node, const std::vector<std::string>& names, std::tuple<T...>& ret)
{
    std::get<n>(ret) = GetAttrVal_Auto<n, T...>(node, names);
}

template <size_t n, class ... T>
inline void SetAttrVal(rapidxml::xml_node<>* node, const std::vector<std::string>& names, std::tuple<T...>& ret, const std::tuple<T...>& def)
{
    std::get<n>(ret) = GetAttrVal_Auto<n, type_at<n, T...>::type, T...>(node, names, std::get<n>(def));
}


template<std::size_t... indices, class ... T>
void SetTuple(std::index_sequence<indices...>, rapidxml::xml_node<>* node, const std::vector<std::string>& names, std::tuple<T...>& ret)
{
    using swallow = int[];
    (void)swallow
    {
        (SetAttrVal<indices, T...>(node, names, ret), 0)...
    };
}

template<std::size_t... indices, class ... T>
void SetTuple(std::index_sequence<indices...>, rapidxml::xml_node<>* node, const std::vector<std::string>& names, std::tuple<T...>& ret, const std::tuple<T...>& def)
{
    using swallow = int[];
    (void)swallow
    {
        (SetAttrVal<indices, T...>(node, names, ret, def), 0)...
    };
}

template <class... T>
inline std::tuple<T...> GetAttributes(rapidxml::xml_node<>* node, const std::vector<std::string>& names)
{
    assert(std::tuple_size<std::tuple<T...>>::value == names.size());
    std::tuple<T...> ret;
    SetTuple(std::make_index_sequence< std::tuple_size<std::tuple<T...>>::value >(), node, names, ret);

    // SetAttrVal<0, T...>(node, names, ret);

    return ret;
}

template <class... T>
inline std::tuple<T...> GetAttributes(rapidxml::xml_node<>* node, const std::vector<std::string>& names, const std::tuple<T...>& def)
{
    assert(std::tuple_size<std::tuple<T...>>::value == names.size());
    std::tuple<T...> ret;
    SetTuple(std::make_index_sequence< std::tuple_size<std::tuple<T...>>::value >(), node, names, ret, def);

    // SetAttrVal<0, T...>(node, names, ret);

    return ret;
}