#include "ShapeFactory.h"

ShapeFactory& ShapeFactory::Instance()
{
	static ShapeFactory instance;
	return instance;
}

