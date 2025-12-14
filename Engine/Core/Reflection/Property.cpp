#include "Property.h"
#include "Core/Reflection/TypeManager.h"

FProperty FPropertyImpl::RegisterAttribute(FName InAttributeName, FName InAttributeValue)
{
    Attributes[InAttributeName] = InAttributeValue;
    return this;
}
