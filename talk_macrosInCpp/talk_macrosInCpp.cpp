#include <fstream>
#include <iostream>
#include <cassert>
#include <string>
#include <type_traits>
#include <vector>

using namespace std;


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
constexpr TVal Sqr(TVal x) { return x * x;}
template <typename TVal1, typename TVal2>
constexpr auto Max(TVal1 x, TVal2 y) { return (x > y) ? x : y;}

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
#define KEEP(a) const auto& CONCAT(keep,__LINE__) = make_keep(a); CONCAT(keep,__LINE__);

void calc2()
{
  KEEP(g_a);
}

struct Point { double x; double y; Point(double x, double y): x(x), y(y) {} };

class MakePoints
{
public:
  MakePoints(double x, double y) { operator()(x,y); }
  MakePoints& operator()(double x, double y) { m_data.push_back(Point(x,y)); return *this;}
  operator std::vector<Point>() const { return m_data;}
private:
  std::vector<Point> m_data;
};

template< typename T >
class MakeVectorImpl
{
public:
  template< typename TData >
  MakeVectorImpl& operator()(const TData& t) { m_data.push_back(t); return *this;}
  operator std::vector<T>() const { return m_data;}
private:
  std::vector<T> m_data;
};

template< typename T >
MakeVectorImpl<T> MakeVector(const T& t)
{
  return MakeVectorImpl<T>()(t);
}

void test()
{
  const auto v = MakePoints(5,10)(5,20)(15,20)(15,5);
  const auto v2 = MakeVector(1)(2)(3)(4)(5);
  const auto v3 = MakeVector<string>("Hello")("world")("!");
}

struct Log
{
  static Log& Instance() { static Log log; return log;}

  Log& LOG_A;
  Log& LOG_B;
  std::ofstream m_file;

  Log(): LOG_A(*this), LOG_B(*this), m_file("c:/temp/log.log") {}

  template< typename T >
  Log& Print(char const * name, const T& value)
  { m_file << name << ":" << value << "\t"; return *this;}

  Log& NewLine() { m_file << "\n"; return *this;}
};

//#ifdef NDEBUG
//
//const int ASSERT_A = 0;
//const int ASSERT_B = 0;
//
//#define ASSERT(x) ASSERT_A
//
//#define ASSERT_A(x) ASSERT_B
//#define ASSERT_B(x) ASSERT_A
//
//#else

#define LOG Log::Instance().NewLine().LOG_A

#define LOG_A(x) Print(#x, (x)).LOG_B
#define LOG_B(x) Print(#x, (x)).LOG_A

//LOG(a)(limit)("world")(text);
//Log::Instance().NewLine().LOG_A(a)(limit)("world")(text);
//Log::Instance().NewLine().LOG_A.Print("a", (a)).LOG_B(limit)("world")(text);
//Log::Instance().NewLine().LOG_A.Print("a", (a)).LOG_B.Print("limit", (limit)).LOG_A("world")(text);



//#endif

//#define CONCAT(A,B,C) A ## _ ## B ## C

int main()
{
  test();

  const auto s = Sqr(3.14);
  const auto m = Max(1, '3');

  const int a = 5;
  const int limit = 10;
  string text{"HELP"};
  Point pnt(31.4,1.52);

  //auto CONCAT(my,a,limit) = a * limit;
  //std::cout << my_alimit;

  LOG(a)(limit)("world")(text);

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
