#include "UtilXml.h"

std::string GetAttribute(rapidxml::xml_node<>* node, const std::string & name)
{
    for (auto attr = node->first_attribute(); attr != nullptr; attr = attr->next_attribute())
    {
        std::string attrName(attr->name());
        if (attrName == name)
        {
            return std::string(attr->value());
        }
    }

    return std::string();
}

void NodeApply(rapidxml::xml_node<>* node, const std::function<void(rapidxml::xml_node<>*)>& func)
{
    for (rapidxml::xml_node<>* child = node;
        child != nullptr;
        child = child->next_sibling())
    {
        func(child);
    }
}
