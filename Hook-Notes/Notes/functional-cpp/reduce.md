## Reduce

```cpp
#include <iostream>
#include <sstream>
#include <functional>
#include <vector>

template <typename T, typename U>
U foldLeft(
  const std::vector<T>& data,
  const U& initialValue,
  const std::function<U(U,T)>& foldFn
) {
    typedef typename std::vector<T>::const_iterator Iterator;
    U accumulator = initialValue;
    Iterator end = data.cend();
    for (Iterator it = data.cbegin(); it != end; ++it) {
        accumulator = foldFn(accumulator, *it);
    }
    return accumulator;
}

template <typename T>
T reduce(
  const std::vector<T>& data,
  const std::function<T(T, T)>& reduceFn
) {
    typedef typename std::vector<T>::const_iterator Iterator;
    Iterator it = data.cbegin();
    Iterator end = data.cend();
    if (it == end) {
        throw 0;
    } else {
        T accumulator = *it;
        ++it;
        for (; it != end; ++it) {
            accumulator = reduceFn(accumulator, *it);
        }
        return accumulator;
    }
}

template <typename T, typename U>
std::vector<U> map(
  const std::vector<T>& data,
  const std::function<U(T)> mapper
) {
    std::vector<U> result;
    foldLeft<T, std::vector<U>&>(data, result, [mapper] (std::vector<U>& res, T value)  -> std::vector<U>& {
        res.push_back(mapper(value));
        return res;
    });
    return result;
}


template <typename T>
std::string mkString(
  const std::vector<T>& data, const std::string& separator = ",", const std::string& begin = "",
  const std::string& end = ""
) {
    std::stringstream buffer;
    bool first_item = true;
    buffer << begin;
    foldLeft<T, std::stringstream*>(data, &buffer, [separator, &first_item] (std::stringstream* res, T item) -> std::stringstream* {
        if (first_item) {
            *res << item;
            first_item = false;
        } else {
            *res << separator << item;
        }
        return res;
    });
    buffer << end;
    return buffer.str();
}

template <typename T>
std::vector<T> filter(
  const std::vector<T>& data,
  std::function<bool(T)> filterFn
) {
    std::vector<T> result;
    foldLeft<T, std::vector<T>&>(data, result, [filterFn] (std::vector<T>& res, T value)  -> std::vector<T>& {
        if (filterFn(value)) {
            res.push_back(value);
        }
        return res;
    });
    return result;
}

int main(int argc, char* argv[]) {
    const std::vector<int> test = {1, 2, 3, 4};
    std::cout << "folding 1,2,3,4 using sum and 0 init should = 10." << std::endl;
    int foldResult = reduce<int>(test, std::plus<int>());
    std::cout << "And the actual result is: " << foldResult << std::endl;

    std::cout << "lets map it now over +10!" << std::endl;
    std::vector<int> biggerInts = map<int,int>(test, [] (int num) { return num + 10; });
    std::cout << "result is " << mkString<int>(biggerInts) << std::endl;

    int foldMapped = reduce<int>(biggerInts, std::plus<int>());
    std::cout << "folding mapped version should yield 50!: " <<  foldMapped << std::endl;

    std::cout << "filtering odd numbers: " << mkString<int>(filter<int>(test, [] (int num) { return num % 2 != 0; }), "|", "[", "]") << std::endl;

    return foldResult != 10 + foldMapped != 50;
}
```

https://gist.github.com/dant3/fe28dd98539c4b671990
