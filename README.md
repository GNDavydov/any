# Any

## Implementation of the class any

The class any describes a type-safe container for single values of any copy constructible type.

1) An object of class any stores an instance of any type that satisfies the constructor requirements or is empty, and this is referred to as the state of the class any object. The stored instance is called the contained object. Two states are equivalent if they are either both empty or if both are not empty and if the contained objects are equivalent.
2) The non-member any_cast functions provide type-safe access to the contained object.
   Implementations are encouraged to avoid dynamic allocations for small objects, but such an optimization may only be applied to types for which std::is_nothrow_move_constructible returns true.

## LINKS

https://en.cppreference.com/w/cpp/utility/any

https://www.cppstories.com/2018/06/any/
