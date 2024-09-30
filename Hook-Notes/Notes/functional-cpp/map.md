## Map

```cpp
#include <vector>
#include <algorithm>

template<class T, class Func>
auto Map(const std::vector<T>& input_array, Func op){
    std::vector<decltype(op(input_array.front()))> result_array;
    std::transform(input_array.begin(), input_array.end(), std::back_inserter(result_array), op);
    return result_array;
}

int main() {
    const std::vector<int> input_array = { 1, 2, 3 };
    const auto output_array = Map(input_array, [](int value) { return 0.1 * value; });
    for (const auto& value : output_array) {
      std::cout << value << std::endl;
    }
}

```

https://gist.github.com/yuki-koyama/ab71d3b34607c3f946af0e8b5f828974

```cpp
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <iterator>

template <class T, class F>
decltype(auto) map(const std::vector<T> a, const F fn) {
  std::vector<decltype( fn(a[0]) )> result = {};
  std::transform(a.cbegin(), a.cend(), std::back_inserter(result), fn);
  return result;
}

int main() {
  const std::vector<int> v = { 3, 5, 6};
  const auto result = map(v, [](const int x){return std::to_string(x * 2); });

  std::for_each(result.begin(), result.end(),
                [](const std::string& s) { std::cout << s << std::endl; });

  std::for_each(v.begin(), v.end(),
                [](const int n) { std::cout << n << std::endl; });

}
```

https://gist.github.com/takoeight0821/730719c448ee1b9ba55f8f354cbbba16
