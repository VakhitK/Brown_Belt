#include "test_runner.h"

#include <functional>
#include <memory>
#include <string>
using namespace std;

template<typename T>
class LazyValue
{
public:
  using Builder = std::function<T()>;

  explicit LazyValue(Builder init)
    : builder_{ init }
  {}

  bool HasValue() const
  {
    return !!obj_.get();
  }
  const T& Get() const
  {
    if (!obj_) {
      obj_.reset(new T{builder_()});
    }
    return *obj_;
  }

private:
  Builder builder_;
  mutable std::unique_ptr<T> obj_;
};

void
UseExample()
{
  const string big_string = "Giant amounts of memory";

  LazyValue<string> lazy_string([&big_string] { return big_string; });

  ASSERT(!lazy_string.HasValue());
  ASSERT_EQUAL(lazy_string.Get(), big_string);
  ASSERT_EQUAL(lazy_string.Get(), big_string);
}

void
TestInitializerIsntCalled()
{
  bool called = false;

  {
    LazyValue<int> lazy_int([&called] {
      called = true;
      return 0;
    });
  }
  ASSERT(!called);
}

/*int main()
{
  TestRunner tr;
  RUN_TEST(tr, UseExample);
  RUN_TEST(tr, TestInitializerIsntCalled);

  int i;
  cin >> i;

  return 0;
}*/
