#pragma once

#include <math.h>
#include <memory.h>
#include "tVector.h"

#pragma warning(disable : 4201)

namespace nv
{
	template <class _Type>
	inline _Type Clamp(const _Type& x,
		const _Type& lo,
		const _Type& hi)
	{
		if (x < lo)
			return lo;
		else if (x > hi)
			return hi;
	
		return x;
	}
}


#pragma pack(push,4)


class rgba_t
{
public:
	union
	{
		unsigned long u;
		unsigned char p[4];
		struct
		{
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
		};
	};
	
	rgba_t() {} 

	unsigned long bgra()
	{
		return ((unsigned long)a << 24) | ((unsigned long)r << 16) | ((unsigned long)g << 8) | ((unsigned long)b);
	}

	rgba_t(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a)
	{
		a = _a;
		r = _r;
		g = _g;
		b = _b;
	}

	rgba_t& operator += (const rgba_t& v)     // incrementation by a rgba_t
	{
		r = (unsigned char)nv::Clamp((int)((int)r + (int)v.r), 0, 255);
		g = (unsigned char)nv::Clamp((int)g + (int)v.g, 0, 255);
		b = (unsigned char)nv::Clamp((int)b + (int)v.b, 0, 255);
		a = (unsigned char)nv::Clamp((int)a + (int)v.a, 0, 255);

		return *this;
	}

	rgba_t& operator -= (const rgba_t& v);     // decrementation by a rgba_t
	rgba_t& operator *= (const float d);     // multiplication by a constant
	rgba_t& operator /= (const float d);     // division by a constant

	rgba_t& operator = (const rgba_t& v)
	{
		r = v.r;
		g = v.g;
		b = v.b;
		a = v.a;
		return *this;
	}

	friend rgba_t operator + (const rgba_t& v1, const rgba_t& v2)
	{
		int r, g, b, a;
		r = nv::Clamp((int)v1.r + (int)v2.r, 0, 255);
		g = nv::Clamp((int)v1.g + (int)v2.g, 0, 255);
		b = nv::Clamp((int)v1.b + (int)v2.b, 0, 255);
		a = nv::Clamp((int)v1.a + (int)v2.a, 0, 255);

		return rgba_t((unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a);
	}

	friend rgba_t operator / (const rgba_t& v, float s)
	{
		return rgba_t(
			(unsigned char)(v.r / s),
			(unsigned char)(v.g / s),
			(unsigned char)(v.b / s),
			(unsigned char)(v.a / s));
	}

	friend rgba_t operator / (const rgba_t& v, int s)
	{
		return rgba_t(
			(unsigned char)(v.r / s),
			(unsigned char)(v.g / s),
			(unsigned char)(v.b / s),
			(unsigned char)(v.a / s));
	}

	void set(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

	void SetToZero()
	{
		r = g = b = a = 0;
	}
};





template <class _Type>
class nvImage
{
	size_t m_width;
	size_t m_height;
	nvVector<_Type> m_pixels;

	bool m_RGBE;

public:
	void SetRGBE(bool b)
	{
		m_RGBE = b;
	}

	bool isRGBE() const
	{
		return m_RGBE;
	}

	size_t size()
	{
		return m_width * m_height;
	}

	nvImage <_Type>& operator = (const nvImage <_Type>& v)
	{
		// resize and copy over
		resize(v.width(), v.height());

		m_pixels = v.m_pixels;
		m_RGBE = v.m_RGBE;

		return *this;
	}

	_Type& operator [] (size_t i)
	{
		return m_pixels[i];
	};

	const _Type& operator[](size_t i) const
	{
		return m_pixels[i];
	}

	_Type& operator () (const size_t& y, const size_t& x)
	{
		return m_pixels[y * m_width + x];
	}

	const _Type& operator () (const size_t& y, const size_t& x) const
	{
		return m_pixels[y * m_width + x];
	}

	size_t width() const
	{
		return m_width;
	}

	size_t height() const
	{
		return m_height;
	}

	_Type* pixels(size_t n = 0)
	{
		return &m_pixels[n];
	}

	_Type* pixelsXY(size_t x, size_t y)
	{
		return &m_pixels[y * width() + x];
	}

	_Type* pixelsXY_Safe(size_t x, size_t y)
	{
		if (m_pixels.size() == 0)
			return 0;
		else
			return &m_pixels[y * width() + x];
	}

	_Type* pixelsYX(size_t y, size_t x)
	{
		return &m_pixels[y * width() + x];

	}

	// row / column
	_Type* pixelsRC(size_t y, size_t x)
	{
		return &m_pixels[y * width() + x];
	}

	_Type& pixel_ref(size_t n = 0)
	{
		return m_pixels[n];
	}

	_Type& pixelsXY_ref(size_t x, size_t y)
	{
		return m_pixels[y * width() + x];
	}

	_Type& pixelsYX_ref(size_t y, size_t x)
	{
		return m_pixels[y * width() + x];
	}

	// row / column
	_Type& pixelsRC_ref(size_t y, size_t x)
	{
		return m_pixels[y * width() + x];
	}

	_Type* pixelsXY_wrapped(int x, int y)
	{
		y = mod(y, m_height);
		x = mod(x, m_width);

		return &m_pixels[y * m_width + x];
	}

	nvImage(const nvImage < _Type >& other)
	{
		m_width = other.m_width;
		m_height = other.m_height;

		m_pixels = other.m_pixels;
		m_RGBE = other.m_RGBE;
	}

	nvImage()
	{
		m_width = 0;
		m_height = 0;
		m_RGBE = false;

		m_pixels.clear();
	};

	~nvImage() {}

	void clear()
	{
		m_width = 0;
		m_height = 0;
		m_pixels.clear();
	}

	void resize(size_t width, size_t height)
	{
		m_pixels.resize(width * height);
		m_width = width;
		m_height = height;
	}

	void realloc(size_t width, size_t height)
	{
		m_pixels.realloc(width * height);
		m_width = width;
		m_height = height;
	}

	nvImage<_Type>(size_t width, size_t height)
	{
		m_pixels.resize(width * height);
		m_width = width;
		m_height = height;
		m_RGBE = false;
	};

	void SwapRB()
	{
		_Type* p = &m_pixels[0];
		_Type tmp;

		for (size_t i = 0; i < m_width * m_height; i++)
		{
			tmp.r = p->r;
			p->r = p->b;
			p->b = tmp.r;

			p++;
		}
	}

	void Scale(_Type s)
	{
		_Type* p = &m_pixels[0];

		for (size_t i = 0; i < m_width * m_height; i++)
		{
			*p++ *= s;
		}
	}

	void Bias(_Type b)
	{
		_Type* p = &m_pixels[0];

		for (size_t i = 0; i < m_width * m_height; i++)
		{
			*p++ += b;
		}
	}

	void dot(_Type w)
	{
		_Type* p = &m_pixels[0];

		for (size_t i = 0; i < m_width * m_height; i++)
		{
			p->dot(w);
			p++;
		}
	}

	void Clamp(_Type low, _Type hi)
	{
		_Type* p = &m_pixels[0];

		for (size_t i = 0; i < m_width * m_height; i++)
		{
			p->Clamp(low, hi);
			p++;
		}
	}

	void Wrap(_Type low, _Type hi)
	{
		_Type* p = &m_pixels[0];

		for (size_t i = 0; i < m_width * m_height; i++)
		{
			p->Wrap(low, hi);
			p++;
		}
	}

	void FlipTopToBottom()
	{
		_Type* swap = new _Type[m_width];
		size_t row;

		_Type* end_row;
		_Type* start_row;

		size_t len = sizeof(_Type) * m_width;

		for (row = 0; row < m_height / 2; row++)
		{
			end_row = &m_pixels[m_width * (m_height - row - 1)];
			start_row = &m_pixels[m_width * row];

			// copy row toward end of image into temporary swap buffer
			memcpy(swap, end_row, len);

			// copy row at beginning to row at end
			memcpy(end_row, start_row, len);

			// copy old bytes from row at end (in swap) to row at beginning
			memcpy(start_row, swap, len);
		}

		delete[] swap;
	}

	void SetToZero()
	{
		for (size_t i = 0; i < m_width * m_height; i++)
		{
			pixel_ref(i).SetToZero();
		}
	}

	void SetToZeroDirect()
	{
		for (size_t i = 0; i < m_width * m_height; i++)
		{
			m_pixels[i] = 0;
		}
	}
};

typedef nvImage<rgba_t> RGBAImage;


#pragma pack(pop)
