#pragma once

#include <numeric>
#include <limits>
#include <initializer_list>
#include <functional>
#include <cmath>
#include <type_traits>

namespace LA
{

template<size_t S, typename T>
class Vec
{
public:
static_assert(std::is_arithmetic_v<T>);
    template<typename... Types>
	Vec(Types... init) :_data{{init...}} {};

template<typename N>constexpr Vec(const Vec<S, N> & vec)
{
    std::transform(vec.begin(), vec.end(), begin(), [](const N& l)
    {
        return static_cast<T>(l);
    }
    );
};

template<typename N> constexpr T & operator =(const Vec<S, N> & vec)
    {
        if (&vec != this)
        {
            if constexpr(std::is_same<T,N>())
                _data = vec._data;
            else
            std::transform(vec.begin(), vec.end(), begin(), [](const N& l)
            {
                return static_cast<T>(l);
            }
            );
        }
        return *this;
    };

	virtual ~Vec() = default;

	Vec & fill (const T& d)
	{
        _data.fill(d);
	};

    inline T operator[](size_t i) const
    {
        return _data[i];
    };

    inline T & operator[](size_t i)
    {
        return _data[i];
    };

    auto begin()
    {
        return _data.begin();
    };

    auto begin() const
    {
        return _data.begin();
    };

    auto end()
    {
        return _data.end();
    };

    auto end() const
    {
        return _data.end();
    };

	Vec operator - () const
	{
        Vec tmp;
        std::transform(begin(), end(), tmp.begin(), std::negate<T>());
		return tmp;
	};

	Vec & operator += ( const Vec & vec)
	{
        std::transform(begin(), end(), vec.begin(), begin(), std::plus<T>());
		return *this;
	};

	Vec & operator -= ( const Vec & vec)
	{
		std::transform(begin(), end(), vec.begin(), begin(), std::minus<T>());
		return *this;
	};

template<typename N> Vec & operator *= ( const N& c)
	{
        static_assert(std::is_arithmetic_v<N>);
		for(auto& elem: _data)
            elem *= c;
		return *this;
	};

template<typename N> Vec & operator /= ( const N& c)
	{
        static_assert(std::is_arithmetic_v<N>);
		for(auto& elem: _data)
            elem /= c;
		return *this;
	};

	bool operator ! () const
	{
		for(size_t i = 0; i < S; ++i)
        {
            if(!this->operator()(i))
                return false;
        };
        return true;
	};

	Vec & set_norm()
	{
		const T t = norm(*this);
		if (t)
            *this /= t;
		return *this;
	};


template<typename N> Vec & set_norm(N& p)
	{
		static_assert(std::is_arithmetic_v<N>);

		T t = norm(*this);
		if (t)
		{
			t = p / t;
            *this /= t;
		}
		return *this;
	};


	constexpr inline bool operator ==( const Vec & b) const
	{
		if constexpr (std::is_floating_point_v<T>)
		{
            return std::equal(begin(), end(), b.begin(), [] (const T& l, const T& r)
            {
                return std::abs(l - r) < std::numeric_limits<T>::epsilon();
            }
            );
        }
		else
		{
			return std::equal(begin(), end(), b.begin());
        }
	}

	constexpr inline bool operator !=( const Vec & b) const
	{
		return *this == b;
	};

	Vec operator + ( const Vec & b) const
	{
		Vec tmp;
        std::transform(begin(), end(), b.begin(), tmp.begin(), std::plus<T>());
		return tmp;
	};

	Vec operator - ( const Vec & b) const
	{
		Vec tmp;
        std::transform(begin(), end(), b.begin(), tmp.begin(), std::minus<T>());
		return tmp;
	};

template<typename N> Vec operator / ( const N& c)
	{
        static_assert(std::is_arithmetic_v<N>);
        Vec tmp;
        std::transform(begin(), end(), tmp.begin(), [&c](const T& l){return l/c;});
		return tmp;
	};

inline T operator * ( const Vec& b) const
	{
        return std::inner_product(_data.begin(), _data.end(), b._data.begin(), static_cast<T>(0));
	};

private:
    std::array<T, S> _data;
};

template <size_t S, typename T, typename N >
Vec<S,T> operator * ( const Vec<S,T> & vec, const N& c)
{
	static_assert(std::is_arithmetic_v<N>);
    Vec<S,T> tmp;
    std::transform(vec.begin(), vec.end(), tmp.begin(), [&c](const T& l){return l * c;});
    return tmp;
};


template <size_t S, typename T, typename N >
Vec<S,T> operator * ( const N& c, const Vec<S,T> & vec)
{
	static_assert(std::is_arithmetic_v<N>);
    Vec<S,T> tmp;
    std::transform(vec.begin(), vec.end(), tmp.begin(), [&c](const T& l){return c * l;});
    return tmp;
};

template<typename T>
Vec<3,T>  operator % (const Vec<3,T>& a,  const Vec<3,T> & b)
	{
            return {a[1] * b[2] - a[2] * b[1],
                    a[2] * b[0] - a[0] * b[2],
                    a[0] * b[1] - a[1] * b[0]};
	};

template<size_t S,typename T>
constexpr T const norm(const Vec<S, T> & vec)
{
	if constexpr (std::is_floating_point_v<T>)
    {
        if constexpr(S == 3)
            return std::hypot(vec[0], vec[1], vec[2]);
        else if constexpr(S == 2)
            return std::hypot(vec[0], vec[1]);
        else
        {
            return std::sqrt(vec * vec);
        }
    }
	else
    {
        return std::accumulate(vec._data.begin() , vec._data.end(), static_cast<T>(0));
    };
};

using Vec3f = Vec<3, float>;
using Vec3i = Vec<3, int>;

}
