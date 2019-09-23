# TopLangCompiler
A concise, readable and efficient programming language

# Philosphy
This programming language is a systems programming language targeted for high performance game engine development. Although it it is aimed at a simmilar field to c++, the language shares the c philsophy of simplicity and control over C++'s just add more features and abstractions. 

For these reasons garbage collection is out of question due to it's inherent indeterminism and poor control over memory. Furthermore this language does not contain RAII because it hides memory allocation, promotes allocating single objects, resulting in poor cache performance and adds tremendous complexity to the language, ie move semantics, l-values and r-values. In addition exceptions will not be supported, because they obfuscate program control flow, make manual memory mangement extremely difficult and bloat executable size. Top's approach is to use typed unions instead, see below. Furthermore inheritance and virtual functions are not present as they result inefficient code and brittle class hiearchies. Use composition and Data Oriented Design instead, again see below. 

Now the language aims to smooth complex systems development which require iterative adaptations to get right. Therefore the language tries to minimize buracracy such as having to explictly write variable types, Trait's for generics and encoding ownership through complex type hiararchies. However what the language will have, is support for typed unions, fibers, rapid compilation and compile time execution. Because multi threading is becoming such an integral part of higher performance computing, everything is immutable by default with explicit mutability. In addition capture blocks will restrict a certain block or function to only read or modify, certain variables.

# State of the compiler

This is still very early in the development of the language and currently on parsing is actually implemented.

# Examples

```scala
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

```scala
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

```scala
from math import vec3, quat

struct Translate:
  position Vec3
  
struct Rotate:
  rotation Vec3
  
struct Scale:
  scale Vec3

union GizmoState = Translate | 

struct Gizmo:
  using gizmo_state GizmoState
  
gizmo := Gizmo{
  gizmo_state = Translate{ vec3() }
}

if gizmo is Translate:
  printf("Moving object %", gizmo.position) 
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
    ch << "Ping!"
  
def pong():
  while true:
    ch << "Pong!"
    fmt.println(<<ch)
    
task.go(ping)
task.go(pong)
```

```scala
var mut arr [3]int = [10, 20, 30]
var arr_dyn [..]int
append(arr_dyn, 10)
append(arr_dyn, 20)
arr_dyna.append(30)

var arr_slice mut []int = &arr

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
  var mut res vec[N]
  for 0..N:
    res[it] = a[it] + b[it]
   
def format(buffer *mut StringBuffer, value *vec[3]):
  buffer.append("Vec3(")
  for 0..3: buffer.append(value[it])
  buffer.append(")")

fmt.println("%", vec3(10.0, 5.0, 2.5) + vec3(0.0, 3.0, 8.0))
```

