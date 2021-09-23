#ifndef _PARSER_H
#define _PARSER_H

#include <string>
#include <vector>
#include <iostream>
#include <stdlib.h>

#include "core/TaskArgument.h"

#include "utils/pugixml.hpp"

#include "utils/utils.h"
#include "utils/exceptions.h"



class Parser {
private:
   std::string xml_path;
public:
   explicit Parser(std::string);

   // This function parse the file pointed by xml_path, and then
   // save all necessary data required by the simulation in Scratch class.
   int parseFile();

   TaskArgument parseTask(pugi::xml_node task, int taskid);

   struct timespec parseTime(pugi::xml_node n, std::string nodeName);
   unsigned long parseTimeUsec(pugi::xml_node n, std::string nodeName);
};

void ParserPAPITest(int argc, char **argv);

template <typename T> T formatTimeUsec(double v, const std::string& unit)
{
   T r;

   if (unit == "sec")
   {
      r = (T)(v * 1000000);
   }
   else if (unit == "ms")
   {
      r = (T)(v * 1000);
   }
   else if (unit == "us")
   {
      r = (T)(v);
   }
   else
   {
      throw std::invalid_argument("parseTimeVectorMicro: Parser error: could not recognize time unit!\n");
   }
   return(r);
}

template <typename T> std::vector <T> formatTimeUsec(const std::vector <double>& v,
                                                     const std::string& unit)
{
   std::vector <T> ret;

   for (int i = 0; i < (int)v.size(); ++i)
   {
      ret.push_back(formatTimeUsec <T>(v[i], unit));
   }

   return(ret);
}

template <typename T> std::vector <T> parseTimeVectorUsec(pugi::xml_node n, std::string nodeName)
{
   std::vector <double> initvalues;
   std::vector <T>      ret;

   try{
      initvalues = stringToVector <double>(n.attribute("value").value());
   }catch (...) {
      throw XmlAttributeInvalid(nodeName + "'s value");
   }

   try{
      std::string unit = n.attribute("unit").value();
      ret = formatTimeUsec <T>(initvalues, unit);
   }catch (...) {
      throw XmlAttributeInvalid(nodeName + "'s unit");
   }

   return(ret);
}

#endif
