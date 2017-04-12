#include "core/Task.h"
#include "utils/Enumerations.h"
#include <iostream>
using namespace std;


int main(){

for (int i = 0; i < 10; ++i)
{
	Task b = Task(singlecore, busywait, 1, 11);
}

}