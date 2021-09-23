#include "UnitTest/UnitTest.h"


#include "utils/Parser.h"
#include "configuration/Scratch.h"

using namespace std;

void testParser(string xmlfile)
{
   Parser *p = new Parser(xmlfile);

   p->parseFile();
   Scratch::print();
}
