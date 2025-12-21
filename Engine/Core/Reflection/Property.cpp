#include "Property.h"
#include "Core/Reflection/TypeManager.h"

FProperty FPropertyImpl::RegisterAttribute(FName InAttributeName, FStringView InAttributeValue)
{
    Attributes[InAttributeName] = InAttributeValue;
    return this;
}
