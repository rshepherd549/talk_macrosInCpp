---
marp: true
---

# Why I still use macros in C++

## Richard Shepherd
- Basic on Dragon32 as a teenager
- Fortran and Pascal at university
- Modula-2 and Actor at internships
- Fortran and C at Logica
- C++ and Java at Investment banks
- C++ at ESRI R&D Cardiff

*Understand your tools and take the time to make things simple*

<!--
Dragon32 - Port Talbot
-->

---

# C/C++ preprocesser

Macro definition and application are one of the tasks carried out by the C/C++ preprocessor. Other, unavoidable, preprocessor tasks include: `#include`, `#ifdef` etc, `#pragma` e.g.
```cpp
//talk_macrosInCpp.cpp
#include <iostream>
#include <cassert>
int main()
{
  int x = 0;
  std::cin >> x;
  assert(x > 0);
  std::cout << x << "\n";
}
```

<!--
#include is not a euphemism. It means dump the contents into the growing intermediate source file
-->

---

```cpp
//assert.h
#ifndef __ASSERT_H__
#define __ASSERT_H__
#ifdef NDEBUG
    #define assert(expression) ((void)0)
#else
    #define assert(expression) (void)(                                                       \
            (!!(expression)) ||                                                              \
            (_wassert(_CRT_WIDE(#expression), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0) \
        )
#endif //NDEBUG
#endif //__ASSERT_H__
```

---

## Macros

Macros define a text substitution that occurs before the compiler sees the code.

Macros can be evil, despite their good intentions, and should be treated with caution

```cpp
#define PI 3.14159265359   /*Doesn't need initialization of externed shared value*/
#define MAX_NUM_WINDOWS 10
#define SQR(X) X * X             /*Works for multiple types*/
#define MAX(a,b) ((a>b)?(a):(b)) /*Avoid cost of function call*/
```

- PI could be defined multiple times. So you need to undef first.
- No namespacing or scoping, so need to know that MAX_NUM_WINDOWS refers to houses not GUI.
- Safety advice of including brackets

---

## Macro functions are more error prone..

```cpp

#define SQR(X) X * X             /*Works for multiple types*/
int a = 5;
int aSqr = SQR(a); //= 25; //okay
int a1Sqr = SQR(a+1); //= a+1*a+1 = a+a+1 = 11; //not okay!

#define MAX(a,b) ((a>b)?(a):(b)) /*Avoid cost of function call*/
const int limit = 10;
int maxLimitAndA = 0;
int numIterations = 0;
for (int i=0; i != limit;)
{
  maxLimitAndA = MAX(a,i++);
  ++numIterations;
}
std::cout << maxLimitAndA << "\t" << numIterations;
```
They are easily confused with functions, but don't give us the same behavior.
The error messages and debugging are no fun.

<!--
No type checking until the final operations.
Doesn't play nicely with syntax e.g. commas
-->

---

## Uses are disappearing

```cpp
constexpr double pi = 3.14159265359;

namespace MyGUI
{
  constexpr size_t max_num_windows = 255;
}
class House
{
  static constexpr int max_num_windows = 10;
};

template <typename TVal>
constexpr auto Sqr(TVal x) { return x * x;}

template <typename TVal1, typename TVal2>
constexpr std::common_type_t<TVal1,TVal2> Max(TVal1 x, TVal2 y) { return (x > y) ? x : y;}
```

<!--
My lazy test code, duplication, mentally compiling, difficult debugging.
=> take the time to make the helper functions
-->

---

## What's left?

There are a number of useful constans defined, which the preprocessor changes the value of as it proceeds, leaving the current value in the source code, useful for logging and debugging code e.g.:
```cpp
__DATE__ /*Date in "Mmm dd yyyy" format*/
__TIME__ /*Time (starting this compilation unit) in "hh:mm:ss" format*/
__FILE__ /*Fullpath of current source file*/
__LINE__ /*Line number within the current source file*/
__func__ /*Name of current function (not macro)*/
NDEBUG   /*Defined by compiler if not in 'Debug' mode. Relied on by assert*/
```
and..

---

## Powerful tools:

```cpp
#define TO_STRING(A) #A /*Stringify A*/
...
  std::string name{"Richard"};
  int x = 10;
  std::cout << TO_STRING(name) << ":" << name << "\t" << TO_STRING(x) << ":" << x;
```
to give
```
name:Richard   x:10
```
and
```cpp
#define CONCAT(A,B) A ## B /*concatenate A and B*/
...
  int a = 10;
  int b = 25;
  int CONCAT(a,b) = a * b; //Creates variable symbol name ab
```

---

## Case study: Making legacy code safer and simpler

Inherited code from another era, that relied on global variables

```cpp
//WARNING destructive routine that alters the feature, and messes with global variables, in order to calculate its eroded center
FPoint CalcErodedCenter(...)
{
  const int s_ixpxl1 = g_ixpxl1; //Save current values
  const int s_iypxl2 = g_ixpxl2;
  const float s_fradb = g_fradb;
  ..
  g_ixpxl1 = .. //give new values for use by functions that get called
  g_fradb = ..
  ..
  if (..)
  {
      ..
      g_ixpxl1 = s_ixpxl1; //restore for early return
      g_ixpxl1 = s_ixpxl1;
      g_fradb = s_fradb;
      return FPoint(0,0);
  }
  ..
  g_ixpxl1 = s_ixpxl1; //restore
  g_iypxl1 = s_iypxl1;
  g_fradb = s_fradb;
  return pnt;
}  
```
Assuming we're stuck with the global variables for now, how can we make it safer?

---
## RAII
```cpp
template <typename T>
class Keep
{
  T& ref;
  T val;
public:
  Keep(T& t): ref(t), val(t) {}
  ~Keep() {ref = val;}
};

template <typename T> Keep<T> make_keep(const T& t) { return Keep<T>(t); }

FPoint CalcErodedCenter(...)
{
  const auto* keep_ixpxl1 = make_keep(g_ixpxl1); //Automatically restore on exit
  g_ixpxl1 = ..
  if (..)
  {
      return FPoint(0,0);
  }
  return pnt;
}  
```
Restoration is guaranteed and in a known (reverse order)

<!--
A lot of boilerplate typing, and inventing extra names.
Temptation to use the saved value.
Some compilers or checkers complain that the variable isn't used.
-->

---
## The KEEP macro
```cpp
#define CONCAT_(a,b) a ## b
#define CONCAT(a,b) CONCAT_(a,b)
#define KEEP(a) const auto& CONCAT(keep,__LINE__) = make_keep(a); CONCAT(keep,__LINE__);

FPoint CalcErodedCenter(...)
{
  KEEP(g_ixpxl1); //Automatically restore on exit
  g_ixpxl1 = ..
  if (..)
  {
      return FPoint(0,0);
  }
  return pnt;
}  
```
The macro encapsulates the boilerplate and hides knowledge of the name.
A similar example was a macro `FOR_RANGE(element, collection)` until c++11.
(Both were more complex in the days before `auto`)
---


---

## LOGGING macro


---

## SMART assert


---

## Limited DSL


---

![image](./vs_test.png)

