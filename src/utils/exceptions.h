#ifndef MCFTP_UTILS_EXCEPTIONS_H
#define MCFTP_UTILS_EXCEPTIONS_H

#include <exception>
#include <string>

class XmlParseException : public std::exception
{
public:
   std::string message;
   XmlParseException();
   const char *what() const throw();
};


class XmlFileLoadException : public XmlParseException
{
protected:
   const std::string errors[15] = {
      "No error",
      "File was not found during load_file()",
      "Error reading from file/stream",
      "Could not allocate memory",
      "Internal error occurred",
      "Parser could not determine tag type",
      "Parsing error occurred while parsing document declaration/processing instruction",
      "Parsing error occurred while parsing comment",
      "Parsing error occurred while parsing CDATA section",
      "Parsing error occurred while parsing document type declaration",
      "Parsing error occurred while parsing PCDATA section",
      "Parsing error occurred while parsing start element tag",
      "Parsing error occurred while parsing element attribute",
      "Parsing error occurred while parsing end element tag",
      "There was a mismatch of start-end tags (closing tag had incorrect name, some tag was not closed or there was an excessive closing tag)",
   };
public:
   XmlFileLoadException(std::string filename, int error_id);
};

class XmlNodeNotFound : public XmlParseException
{
public:
   XmlNodeNotFound(std::string nodename, std::string parentName);
};

class XmlAttributeEmpty : public XmlParseException
{
public:
   XmlAttributeEmpty(std::string attributeName);
};

class XmlAttributeInvalid : public XmlParseException
{
public:
   XmlAttributeInvalid(std::string attributeName);
};


#endif
