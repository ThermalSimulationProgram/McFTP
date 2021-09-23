#include "utils/exceptions.h"
#include <iostream>
using namespace std;
XmlParseException::XmlParseException()
{
}

const char *XmlParseException::what() const throw()
{
   return(message.c_str());
}

XmlFileLoadException::XmlFileLoadException(string filename, int errorId)
{
   message = string(errors[errorId] + ". Input file: " + filename);
}

XmlNodeNotFound::XmlNodeNotFound(string nodename, string parentName)
{
   message = string("Node " + nodename + " was not found in " + parentName);
}

XmlAttributeEmpty::XmlAttributeEmpty(string attributeName)
{
   message = string(attributeName + " attribute is empty in given xml file");
}

XmlAttributeInvalid::XmlAttributeInvalid(std::string attributeName)
{
   message = string(attributeName + " attribute is invalid in given xml file");
}
