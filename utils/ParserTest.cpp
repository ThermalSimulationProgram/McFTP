#include "UnitTesting.h"


#include "Parser.h"
#include "Scratch.h"

using namespace std;

void testParser(string xmlfile){
	Parser* p = new Parser(xmlfile);
	p->parseFile();
	Scratch::print();
}