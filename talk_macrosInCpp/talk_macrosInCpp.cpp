#include <iostream>
#include <cassert>
#include <type_traits>


#define PI 3.14159265359
#define MAX_NUM_WINDOWS 10
#define SQR(X) X * X
#define MAX(a,b) ((a>b)?(a):(b))

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

template <typename T>
class Keep
{
  T& ref;
  T val;
public:
  Keep(T& t): ref(t), val(t) {}
  ~Keep() {ref = val;}
};

template <typename T> Keep<T> make_keep(T& t) { return Keep<T>(t); }

int g_a = 0;

void calc()
{
  const auto& keep = make_keep(g_a);
}

#define CONCAT_(a,b) a ## b
#define CONCAT(a,b) CONCAT_(a,b)
#define KEEP(a) const auto& CONCAT(keep,__LINE__) = make_keep(a);

void calc2()
{
  KEEP(g_a);
}

int main()
{
  const int a = 5;
  const int limit = 10;
  const auto CONCAT(a,__LINE__) = a + limit;
  int maxLimitAndA = 0;
  int numIterations = 0;
  for (int i=0; i < limit;)
  {
    maxLimitAndA = MAX(a,i++);
    ++numIterations;
  }
  std::cout << maxLimitAndA << "\t" << numIterations;
}
