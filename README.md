# TopLangCompiler
A concise, readable and efficient programming language

# Philosphy
This programming language is a systems programming language targeted for high performance game engine development. Although it it is aimed at a simmilar field to c++, the language shares the c philsophy of simplicity and control over C++'s just add more features and abstractions. 

For these reasons garbage collection is out of question due to it's inherent indeterminism and poor control over memory. Furthermore this language does not contain RAII because it hides memory allocation, promotes allocating single objects, resulting in poor cache performance and adds tremendous complexity to the language, ie move semantics, l-values and r-values. In addition exceptions will not be supported, because they obfuscate program control flow, make manual memory mangement extremely difficult and bloat executable size. Top's approach is to use typed unions instead, see below. Furthermore inheritance and virtual functions are not present as they result inefficient code and brittle class hiearchies. Use composition and Data Oriented Design instead, again see below. 

Now the language aims to smooth complex systems development which require iterative adaptations to get right. Therefore the language tries to minimize buracracy such as having to explictly write variable types, Trait's for generics and encoding ownership through complex type hiararchies. However what the language will have, is support for typed unions, fibers, rapid compilation and compile time execution. Because multi threading is becoming such an integral part of higher performance computing, everything is immutable by default with explicit mutability. In addition capture blocks will restrict a certain block or function to only read or modify, certain variables.

# State of the compiler

This is still very early in the development of the language and currently on parsing is actually implemented.

# Examples

```python
import fmt

struct Thing:
  name string
  
def foo(thing *mut Thing):
  thing.name = "Foo"
  
var mut thing Thing
foo(&thing)
thing.foo()

fmt.println("Hello %", thing.name) 
```

```python
enum Color = Red | Green | Blue

struct Person:
  name string
  favourite_color Color
  age ?int

person := Person{
  name = "Jerry"
  favourite_color = Red
  age = none
}

person.greet()

def greet(using person *Person):
  if person.age:
    fmt.println("Hello %, who is % old and loves %", name, age, favourite_color)
  else:
    fmt.println("% does not want to disclose his age", name)
```




