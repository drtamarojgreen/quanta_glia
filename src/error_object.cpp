#include "error_object.h"

ErrorObject::ErrorObject()
    : warnings(0),
      typeErrors(0),
      valueErrors(0),
      syntaxErrors(0),
      importErrors(0),
      otherErrors(0),
      totalErrors(0)
{
}

ErrorObject::~ErrorObject() {}
