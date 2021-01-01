#pragma once

#include <string>
#include "Util\rapidxml-1.13\rapidxml.hpp"

std::string GetAttribute(rapidxml::xml_node<>* node, const std::string& name);

