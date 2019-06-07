#include <Reflectables/Vector.hpp>
#include <Reflectables/Integer.hpp>
#include <iostream>
#include <vector>
using namespace std;
using namespace Reflectable;

class TestReflectable {
public:
    template <  class T, typename std::enable_if<std::is_base_of<Reflectable::Reflectable, T>::value>::type* = nullptr>
    static void test(T value) {
        cout << value.stringify() << " " << T::parse(value.stringify()).stringify() << endl;
    }

    template <  class R,
                typename std::enable_if<std::is_base_of<Reflectable::Reflectable, R>::value>::type* = nullptr,
                template<typename K> class T,
                typename std::enable_if<std::is_base_of<Reflectable::Reflectable, T<R>>::value>::type* = nullptr>
    static void test(T<R> value) {
        cout << value.stringify() << " " << T<R>::parse(value.stringify()).stringify() << endl;
    }
};

int main() {
    Integer value(3);
    vector<Integer> vec;
    vec.push_back(Integer::valueOf(2));
    TestReflectable::test(Vector<Integer>(vec));

    return 0;
}