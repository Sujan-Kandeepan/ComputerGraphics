#ifndef __BOILERPLATE_H__	//guard against cyclic dependancy
#define __BOILERPLATE_H__

class Boilerplate{
public:
	Boilerplate();	//constructor
	void hello();	//hello function

	float publicVar;
private:
	float privateVar;
};

#endif