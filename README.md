# TopLangCompiler
A concise, readable and efficient programming language

# Philosphy
This programming language is a systems programming language targeted for high performance game engine development. Although it it is aimed at a simmilar field to c++, the language shares the c philsophy of simplicity and control over C++'s just add more features and abstractions. 

For these reasons garbage collection is out of question due to it's inherent indeterminism and poor control over memory. Furthermore this language does not contain RAII because it hides memory allocation, promotes allocating single objects, resulting in poor cache performance and adds tremendous complexity to the language, ie move semantics, l-values and r-values. In addition exceptions will not be supported, because they obfuscate program control flow, make manual memory mangement extremely difficult and bloat executable size. Top's approach is to use typed unions instead, see below. Furthermore inheritance and virtual functions are not present as they result in inefficient code and brittle class hiearchies. Use composition and Data Oriented Design instead. 

Now the language aims to smooth complex systems development which require iterative adaptations to get right. Therefore the language tries to minimize buracracy such as having to explictly write variable types, Trait's for generics and encoding ownership through complex type hiararchies. However what the language will have, is support for typed unions, fibers, rapid compilation and compile time execution. Because multi threading is becoming such an integral part of higher performance computing, everything is immutable by default with explicit mutability. In addition capture blocks will restrict a certain block or function to only read or modify, certain variables.

# State of the compiler

This is still very early in the development of the language and currently on parsing is actually implemented.

# Examples

```scala
import fmt

struct Thing:
  name string
  
def foo(thing *Thing): //note * is a mut pointer whilst & is immutable
  thing.name = "Foo"
  
var thing Thing //unset variable is initialized to zero, and mutable
foo(&thing)
thing.foo()

fmt.println("Hello %", thing.name) 
```

```scala
enum Color = Red | Green | Blue

struct Person:
  name string
  favourite_color Color
  age ?int

person := Person{ //mutable
  name = "Jerry"
  favourite_color = Red
  age = none
}

person_two :: Person{ //immutable
  name = "Peter"
  favourite_color = Blue
  age = 20
}

var person_three Person = Person{..} //mutable long hand variable
let person_four Person = Person{..} //immutable long hand variable

greet(person) //Person -> &Person is implicit as it's an immutable reference
person_two.greet()

def greet(using person &Person):
  if person.age:
    fmt.println("Hello %, who is % old and loves %", name, age, favourite_color)
  else:
    fmt.println("% does not want to disclose his age", name)
```

```scala
enum HttpErrror = E404 | E300

union Result[Ok, Error] = Ok | Error

def http_req(url string) Result[string, HttpError]:
  E404

match http_req("cat_gifs"):
  res is Ok: fmt.println("Got back %", res)
  err is Error: fmt.println("Got error %")
    
resp := http_req("cat_gifs")
if resp is Ok:
  fmt.println("Got back %", resp)
else:
  fmt.println("Got error %", resp)
```

```scala
import task
import fmt
from channel import *

mut ch := chan(string)
defer destroy(ch)

def ping():
  while true:
    fmt.println(<<ch)
    ch <- "Ping!"
  
def pong():
  while true:
    ch <- "Pong!"
    fmt.println(<-ch)
    
task.go(ping)
task.go(pong)
```

```scala
var mut arr [3]int = [10, 20, 30]
var arr_dyn [..]int 
defer destroy(arr_dyn)

append(arr_dyn, 10)
append(arr_dyn, 20)
arr_dyn.append(30)

var arr_slice mut []int = &arr
arr_slice[1] = 5

for val in arr:
  fmt.println("(%) %", it, val)

for arr:
  fmt.println("%", it)
  
for i, arr:
  fmt.println("(%) %", i, arr)
```

```scala
type vec[N] [N]float

def vec3(float x, float y, float z) vec[3]:
  [x,y,z]
   
def op_add(a vec[%N], b vec[N]):
  var res vec[N] 
  for 0..N:
    res[it] = a[it] + b[it]
   
def format(buffer *StringBuffer, value vec[3]):
  buffer <- "Vec3("
  for 0..3: buffer <- value[it]
  buffer <- ")"

fmt.println("%", vec3(10.0, 5.0, 2.5) + vec3(0.0, 3.0, 8.0))
```

