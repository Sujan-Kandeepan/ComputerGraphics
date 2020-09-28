#include "boilerplateClass.h"
#include <stdio.h> //needed for printf command

Boilerplate::Boilerplate(){	//constructor
	publicVar = 0;
	privateVar = 1;
}

void Boilerplate::hello(){	//our function hello
	printf("hello world, private variable is: %f, public variable is: %f\n",privateVar, publicVar);
}