///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for planes.
 *	\file		IcePlane.h
 *	\author		Pierre Terdiman
 *	\date		April, 4, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __ICEPLANE_H__
#define __ICEPLANE_H__

	#define PLANE_EPSILON		(1.0e-7f)

	class ICEMATHS_API Plane
	{
		public:
		//! Constructor
		inline			Plane()															{													}
		//! Constructor
		inline			Plane(float nx, float ny, float nz, float d)					{ Set(nx, ny, nz, d);								}
		//! Constructor
		inline			Plane(const Point& p, const Point& n)							{ Set(p, n);										}
		//! Constructor
		inline			Plane(const Point& p0, const Point& p1, const Point& p2)		{ Set(p0, p1, p2);									}
		//! Constructor
		inline			Plane(const Point& n, float d)									{ this->n = n; this->d = d;							}
		//! Copy constructor
		inline			Plane(const Plane& plane) : n(plane.n), d(plane.d)				{													}
		//! Destructor
		inline			~Plane()														{													}

		inline	Plane&	Zero()															{ n.Zero(); d = 0.0f;				return *this;	}
		inline	Plane&	Set(float nx, float ny, float nz, float dd)						{ n.Set(nx, ny, nz); this->d = dd;	return *this;	}
		inline	Plane&	Set(const Point& p, const Point& nn)								{ this->n = nn; d = - p | nn;			return *this;	}
				Plane&	Set(const Point& p0, const Point& p1, const Point& p2);

		inline	float	Distance(const Point& p)			const						{ return (p | n) + d;								}
		inline	bool	Belongs(const Point& p)				const						{ return _abs(Distance(p)) < PLANE_EPSILON;		}

		inline	void	Normalize()
		{
			float Denom = 1.0f / n.Magnitude();
			n.x	*= Denom;
			n.y	*= Denom;
			n.z	*= Denom;
			d	*= Denom;
		}

		public:
		// Members
				Point	n;		//!< The normal to the plane
				float	d;		//!< The distance from the origin

		// Cast operators
		inline			operator Point()					const	{ return n;											}
/*		inline			operator HPoint()					const	{ return HPoint(n, d);								}

		// Arithmetic operators
		inline	Plane	operator*(const Matrix4x4& m)		const
						{
							// Old code from Irion. Kept for reference.
							Plane Ret(*this);
							return Ret *= m;
						}

		inline	Plane&	operator*=(const Matrix4x4& m)
						{
							// Old code from Irion. Kept for reference.
							Point n2 = HPoint(n, 0.0f) * m;
							d = -((Point) (HPoint( -d*n, 1.0f ) * m) | n2);
							n = n2;
							return *this;
						}
*/
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/**
	 *	Transforms a plane by a 4x4 matrix. Same as Plane * Matrix4x4 operator, but faster.
	 *	\param		transformed	[out] transformed plane
	 *	\param		plane		[in] source plane
	 *	\param		transform	[in] transform matrix
	 */
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/**
	 *	Transforms a plane by a 4x4 matrix. Same as Plane * Matrix4x4 operator, but faster.
	 *	\param		plane		[in/out] source plane (transformed on return)
	 *	\param		transform	[in] transform matrix
	 */
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif // __ICEPLANE_H__
