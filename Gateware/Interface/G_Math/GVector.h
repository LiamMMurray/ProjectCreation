#ifndef GVECTOR_H
#define GVECTOR_H
/*!
File: GVector.h
Purpose: A Gateware interface that handles all vector functions.
Author: Shuo-Yi Chang
Contributors: N/A
Last Modified: 9/20/2017
Interface Status: Beta
Copyright: 7thGate Software LLC.
License: MIT
*/

//! GMathDefines inherits directly from GSingleThreaded.
#include "../G_Core/GSingleThreaded.h"
#include "GMathDefines.h"

//! The core namespace to which all Gateware interfaces/structures/defines must belong.
namespace GW
{
	//! The namespace to which all math library interface must belong.
	namespace MATH
	{
		//! Unique Identifier for this interface. {22D6C3B1-1150-40AE-83D8-920C8C778937}
		static const GUUIID GVectorUUIID =
		{
			0x22d6c3b1, 0x1150, 0x40ae,{ 0x83, 0xd8, 0x92, 0xc, 0x8c, 0x77, 0x89, 0x37 }
		};

		//! Vector functions
		/*!
		*	Include float vector and double vector's functions
		*/
		class GVector : public CORE::GSingleThreaded
		{
		public:
			//float vector

			//! Add two vectors
			/*!
			*	Adds the two specified vectors and stores the result in the output vector.
			*
			*	\param [in]  _vector1		The first vector
			*	\param [in]  _vector2		The second vecotr
			*	\param [out] _outVector		The result of addition
			*
			*	\retval SUCCESS				The calculation succeed
			*/
			virtual GReturn AddVectorF(GVECTORF _vector1, GVECTORF _vector2, GVECTORF& _outVector) = 0;

			//! Subtract two vector
			/*!
			*	Subtracts the two specified vectors and stores the result in the output vector.
			*
			*	\param [in]  _vector1		The first vector
			*	\param [in]  _vector2		The second vecotr
			*	\param [out] _outVector		The vector for store the result
			*
			*	\retval SUCCESS				The calculation succeed
			*/
			virtual GReturn SubtractVectorF(GVECTORF _vector1, GVECTORF _vector2, GVECTORF& _outVector) = 0;

			//! Scale the vector
			/*!
			*	Scales all elements of the input vector by the specified value
			*	and stores the result in the output vector.
			*
			*	\param [in]  _vector1		The first vector
			*	\param [in]  _scalar		The specified value to scale
			*	\param [out] _outVector		The vector for store the result
			*
			*	\retval SUCCESS				The calculation succeed
			*/
			virtual GReturn ScaleF(GVECTORF _vector, float _scalar, GVECTORF& _outVector) = 0;

			//! Calculates the dot product of the two vectors
			/*!
			*	Calculates the dot product of two specified vectors and stores the result in the output Value.
			*
			*	\param [in]  _vector1		The first vector
			*	\param [in]  _vector2		The second vecotr
			*	\param [out] _outValue		The value of the dot product
			*
			*	\retval SUCCESS				The calculation succeed
			*/
			virtual GReturn DotF(GVECTORF _vector1, GVECTORF _vector2, float& _outValue) = 0;

			//! Calculates the Cross product of the two vectors which are treated as 2 elements vector 
			/*!
			*	Calculates the cross product of two specified vectors which are treated as 2 elements vectors
			*	and stores the result in the output value. The input vectors' z and w value will be ignored.
			*
			*	\param [in]  _vector1		The first vector
			*	\param [in]  _vector2		The second vecotr
			*	\param [out] _outValue		The value of 2D vecotrs' cross product
			*
			*	\retval SUCCESS				The calculation succeed
			*/
			virtual GReturn CrossVector2F(GVECTORF _vector1, GVECTORF _vector2, float& _outValue) = 0;

			//! Calculates the cross product of the two vectors which are treated as 3 elements vectors. 
			/*!
			*	Calculates the cross product of two specified vectors which are treated as 2 elements vector
			*	and stores the result in the output vector. The input vectors' w value will be returned with 0.
			*
			*	\param [in]  _vector1		The first vector
			*	\param [in]  _vector2		The second vecotr
			*	\param [out] _outVector		The vector of 3D vectors' cross product
			*
			*	\retval SUCCESS				The calculation succeed
			*/
			virtual GReturn CrossVector3F(GVECTORF _vector1, GVECTORF _vector2, GVECTORF& _outVector) = 0;

			//! multiply the specified vector by the specified matrix.
			/*!
			*	Multiplies the specified vector by the specified matrix
			*	and stores the result in the output vector. The input vectors' w value will be returned with 0.
			*
			*	\param [in]  _vector		The input vector
			*	\param [in]  _matrix		The input matrix
			*	\param [out] _outVector		The result of multiplicataion
			*
			*	\retval SUCCESS				The calculation succeed
			*/
			virtual GReturn VectorXMatrixF(GVECTORF _vector, GMATRIXF _matrix, GVECTORF& _outVector) = 0;

			//! Transform specified specified vector by specified matrix.
			/*!
			*	Transforms the specified vector by the specified matrix by treating the fourth row as (0, 0, 0, 1),
			*	and the w component of result vector will return 1.The result will store in the output vector.
			*
			*	\param [in]  _vector		The specified vector
			*	\param [in]  _matrix		The transform matrix
			*	\param [out] _outVector		The result of the transformation
			*
			*	\retval SUCCESS				The calculation succeed
			*/
			virtual GReturn TransformF(GVECTORF _vector, GMATRIXF _matrix, GVECTORF& _outVector) = 0;

			//! Computes the magnitude of the specified vector.
			/*!
			*	Computes the magnitude of the specified vector,	and stores the result in the output value.
			*
			*	\param [in]  _vector		The specified vector
			*	\param [out] _outValue		The magnitude of the vector
			*
			*	\retval SUCCESS				The calculation succeed
			*	\retval FAILURE				The calculation failed
			*/
			virtual GReturn MagnitudeF(GVECTORF _vector, float& _outMagnitude) = 0;

			//! Normalizes the specified vector.
			/*!
			*	Normalizes the specified vector, and stores the result in the output vector.
			*
			*	\param [in]  _vector		The specified vector
			*	\param [out] _outVector		The result of the normalization.
			*
			*	\retval SUCCESS				The calculation succeed
			*	\retval FAILURE				The calculation failed
			*/
			virtual GReturn NormalizeF(GVECTORF _vector, GVECTORF& _outVector) = 0;

			//! Linear interpolates between two specified vectors
			/*!
			*	Linear interpolates between two specified vectors
			*	and stores the result in the output quaternion.
			*
			*	\param [in]  _vector1			The start vector
			*	\param [in]  _vector2			The end vector
			*	\param [in]  _ratio				The interpolation coefficient
			*	\param [out] _outVector			The result of the lerp
			*
			*	\retval SUCCESS					The calculation succeed
			*/
			virtual GReturn LerpF(GVECTORF _vector1, GVECTORF _vector2, float _ratio, GVECTORF& _outVector) = 0;

			//! Calculate the interpolation on a cuvre which connects two specified 3D vectors
			/*!
			*	Calculate the interpolation on a cuvre which connects two specified 3D vectors
			*	and stores the result in the output quaternion. The component of w
			*	will return 0. The interpolation will happen between the second point
			*	and thrid point.
			*
			*	\param [in]  _vector1			The first control point
			*	\param [in]  _vector1			The second control point
			*	\param [in]  _vector1			The thrid control point
			*	\param [in]  _vector2			The fourth control point
			*	\param [in]  _ratio				The interpolation coefficient
			*	\param [out] _outVector			The result of the spline
			*
			*	\retval SUCCESS					The calculation succeed
			*/
			virtual GReturn SplineF(GVECTORF _vector1, GVECTORF _vector2, GVECTORF _vector3, GVECTORF _vector4, float _ratio, GVECTORF& _outVector) = 0;

			//double vector

			//! Add two vectors
			/*!
			*	Adds the two specified vectors and stores the result in the output vector.
			*
			*	\param [in]  _vector1		The first vector
			*	\param [in]  _vector2		The second vecotr
			*	\param [out] _outVector		The result of addition
			*
			*	\retval SUCCESS				The calculation succeed
			*/
			virtual GReturn AddVectorD(GVECTORD _vector1, GVECTORD _vector2, GVECTORD& _outVector) = 0;

			//! Subtract two vector
			/*!
			*	Subtracts the two specified vectors and stores the result in the output vector.
			*
			*	\param [in]  _vector1		The first vector
			*	\param [in]  _vector2		The second vecotr
			*	\param [out] _outVector		The vector for store the result
			*
			*	\retval SUCCESS				The calculation succeed
			*/
			virtual GReturn SubtractVectorD(GVECTORD _vector1, GVECTORD _vector2, GVECTORD& _outVector) = 0;

			//! Scale the vector
			/*!
			*	Scales all elements of the input vector by the specified value
			*	and stores the result in the output vector.
			*
			*	\param [in]  _vector1		The first vector
			*	\param [in]  _scalar		The specified value to scale
			*	\param [out] _outVector		The vector for store the result
			*
			*	\retval SUCCESS				The calculation succeed
			*/
			virtual GReturn ScaleD(GVECTORD _vector, double _scalar, GVECTORD& _outVector) = 0;

			//! Calculates the dot product of the two vectors
			/*!
			*	Calculates the dot product of two specified vectors and stores the result in the output Value.
			*
			*	\param [in]  _vector1		The first vector
			*	\param [in]  _vector2		The second vecotr
			*	\param [out] _outValue		The value of the dot product
			*
			*	\retval SUCCESS				The calculation succeed
			*/
			virtual GReturn DotD(GVECTORD _vector1, GVECTORD _vector2, double& _outValue) = 0;

			//! Calculates the Cross product of the two vectors which are treated as 2 elements vector 
			/*!
			*	Calculates the cross product of two specified vectors which are treated as 2 elements vectors
			*	and stores the result in the output value. The input vectors' z and w value will be ignored.
			*
			*	\param [in]  _vector1		The first vector
			*	\param [in]  _vector2		The second vecotr
			*	\param [out] _outValue		The value of 2D vecotrs' cross product
			*
			*	\retval SUCCESS				The calculation succeed
			*/
			virtual GReturn CrossVector2D(GVECTORD _vector1, GVECTORD _vector2, double& _outValue) = 0;

			//! multiply the specified vector by the specified matrix.
			/*!
			*	Multiplies the specified vector by the specified matrix
			*	and stores the result in the output vector. The input vectors' w value will be returned with 0.
			*
			*	\param [in]  _vector		The input vector
			*	\param [in]  _matrix		The input matrix
			*	\param [out] _outVector		The result of multiplicataion
			*
			*	\retval SUCCESS				The calculation succeed
			*/
			virtual GReturn CrossVector3D(GVECTORD _vector1, GVECTORD _vector2, GVECTORD& _outVector) = 0;

			//! multiply the specified vector by the specified matrix.
			/*!
			*	Multiplies the specified vector by the specified matrix
			*	and stores the result in the output vector. The input vectors' w value will be returned with 0.
			*
			*	\param [in]  _vector		The input vector
			*	\param [in]  _matrix		The input matrix
			*	\param [out] _outVector		The result of multiplicataion
			*
			*	\retval SUCCESS				The calculation succeed
			*/
			virtual GReturn VectorXMatrixD(GVECTORD _vector, GMATRIXD _matrix, GVECTORD& _outVector) = 0;

			//! Transform specified specified vector by specified matrix.
			/*!
			*	Transforms the specified vector by the specified matrix by treating the fourth row as (0, 0, 0, 1),
			*	and the w component of result vector will return 1.The result will store in the output vector.
			*
			*	\param [in]  _vector		The specified vector
			*	\param [in]  _matrix		The transform matrix
			*	\param [out] _outVector		The result of the transformation
			*
			*	\retval SUCCESS				The calculation succeed
			*/
			virtual GReturn TransformD(GVECTORD _vector, GMATRIXD _matrix, GVECTORD& _outVector) = 0;

			//! Computes the magnitude of the specified vector.
			/*!
			*	Computes the magnitude of the specified vector,	and stores the result in the output value.
			*
			*	\param [in]  _vector		The specified vector
			*	\param [out] _outValue		The magnitude of the vector
			*
			*	\retval SUCCESS				The calculation succeed
			*	\retval FAILURE				The calculation failed
			*/
			virtual GReturn MagnitudeD(GVECTORD _vector, double& _outMagnitude) = 0;

			//! Normalizes the specified vector.
			/*!
			*	Normalizes the specified vector, and stores the result in the output vector.
			*
			*	\param [in]  _vector		The specified vector
			*	\param [out] _outVector		The result of the normalization.
			*
			*	\retval SUCCESS				The calculation succeed
			*	\retval FAILURE				The calculation failed
			*/
			virtual GReturn NormalizeD(GVECTORD _vector, GVECTORD& _outVector) = 0;

			//! Spherical linear interpolates between two specified vectors
			/*!
			*	Spherical linear interpolates between two specified vectors
			*	and stores the result in the output quaternion.
			*
			*	\param [in]  _vector1			The start vector
			*	\param [in]  _vector2			The end vector
			*	\param [in]  _ratio				The interpolation coefficient
			*	\param [out] _outVector			The result of the lerp
			*
			*	\retval SUCCESS					The calculation succeed
			*/
			virtual GReturn LerpD(GVECTORD _vector1, GVECTORD _vector2, double _ratio, GVECTORD& _outVector) = 0;

			//! Calculate the interpolation between two specified 3D vectors
			/*!
			*	Calculate the interpolation between two specified 3D vectors
			*	and stores the result in the output quaternion. The component of w
			*	will return 0. The interpolation will happen between the second point
			*	and thrid point.
			*
			*	\param [in]  _vector1			The first control point
			*	\param [in]  _vector1			The second control point
			*	\param [in]  _vector1			The thrid control point
			*	\param [in]  _vector2			The fourth control point
			*	\param [in]  _ratio				The interpolation coefficient
			*	\param [out] _outVector			The result of the spline
			*
			*	\retval SUCCESS					The calculation succeed
			*/
			virtual GReturn SplineD(GVECTORD _vector1, GVECTORD _vector2, GVECTORD _vector3, GVECTORD _vector4, double _ratio, GVECTORD& _outVector) = 0;


		}; //! end GVector class

		//! Creates and outputs a new GVector object.
		/*!
		*	Creates a new GVector object and store the object in _outVector.
		*
		*	\param [out] _outVector				The GVector object
		*
		*	\retval SUCCESS						A GVector object was successfully created.
		*	\retval FAILURE						A GVector object was not created. _outVector will be null.
		*	\retval INVALID_ARGUMENT			_outVector is nullptr.
		*/
		GATEWARE_EXPORT_IMPLICIT GReturn CreateGVector(GVector** _outVector);

	};//! end SYSTEM namespace
} //! end GW namespace


#endif