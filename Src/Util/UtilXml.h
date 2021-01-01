#pragma once

#include <string>
#include "Util\rapidxml-1.13\rapidxml.hpp"

std::string GetAttribute(rapidxml::xml_node<>* node, const std::string& name);


#include "Util.h"
#include "WinUtil.h"
#include <array>
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

template<size_t n, size_t A>
inline std::string AccessVecElement(const std::array<std::string, A>& vec)
{
    return vec[n];
}

template <class T>
inline T GetAttrVal(rapidxml::xml_node<>* node, const std::string& name)
{
    return ConvertType<T>(GetAttribute(node, name));
}

//template<class T1, class T2>
//inline std::tuple<T1, T2> GetAttributes(rapidxml::xml_node<>* node, const std::vector<std::string>& names)
//{
//    return std::tuple<T1, T2>(
//        GetAttrVal<T1>(node, names[0]),
//        GetAttrVal<T2>(node, names[1]));
//}

// N≠0なら、先頭を削ってNをデクリメント
template <size_t N, class T0, class ... T>
struct type_at : public type_at<N - 1, T ...> {};

// N＝0なら、先頭の型が求めるべき型
template <class T0, class ... T>
struct type_at<0, T0, T...>
{
    typedef T0 type;
};


template <size_t n, size_t A, class ... T>
inline auto GetAttrVal_Auto(rapidxml::xml_node<>* node, const std::array<std::string, A>& names)
{
    return GetAttrVal< type_at<n, T...>::type >(node, AccessVecElement<n, A>(names));
}

template <size_t n, size_t A, class ... T>
inline void SetAttrVal(rapidxml::xml_node<>* node, const std::array<std::string, A>& names, std::tuple<T...>& ret)
{
    std::get<n>(ret) = GetAttrVal_Auto<n, A, T...>(node, names);
}

template<size_t A, std::size_t... indices, class ... T>
void SetTuple(std::index_sequence<indices...>, rapidxml::xml_node<>* node, const std::array<std::string, A>& names, std::tuple<T...>& ret)
{
    using swallow = int[];
    (void)swallow
    {
        (SetAttrVal<indices, A, T...>(node, names, ret), 0)...
    };
}


template <size_t A, class... T>
inline std::tuple<T...> GetAttributes(rapidxml::xml_node<>* node, const std::array<std::string, A>& names)
{
    static_assert(std::tuple_size<std::tuple<T...>>::value == A, "GetAttributes size error.");
    std::tuple<T...> ret;
    SetTuple(std::make_index_sequence< std::tuple_size<std::tuple<T...>>::value >(), node, names, ret);

    // SetAttrVal<0, T...>(node, names, ret);

    return ret;
}