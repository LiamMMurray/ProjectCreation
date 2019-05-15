// Override export symbols for DLL builds (must be included before interface code).
#include "../DLL_Export_Symbols.h"
#include "../../Interface/G_Math/GVector.h"
#include "../../Source/G_System/GUtility.h"

#include <atomic> //Thread safety
#include <cmath>

//The using statements for specifically what we are using.
using namespace GW;
using namespace MATH;
using namespace std;

class GVectorCpp :public::GW::MATH::GVector
{
	//Reference counter.
	atomic<unsigned int> refCount;

public:
	GVectorCpp();
	~GVectorCpp();

	//float vector

	GReturn AddVectorF(GVECTORF _vector1, GVECTORF _vector2, GVECTORF& _outVector) override;

	GReturn SubtractVectorF(GVECTORF _vector1, GVECTORF _vector2, GVECTORF& _outVector) override;

	GReturn ScaleF(GVECTORF _vector, float _scalar, GVECTORF& _outVector) override;

	GReturn DotF(GVECTORF _vector1, GVECTORF _vector2, float& _outValue) override;

	GReturn CrossVector2F(GVECTORF _vector1, GVECTORF _vector2, float& _outValue) override;

	GReturn CrossVector3F(GVECTORF _vector1, GVECTORF _vector2, GVECTORF& _outVector) override;

	GReturn VectorXMatrixF(GVECTORF _vector, GMATRIXF _matrix, GVECTORF& _outVector) override;

	GReturn TransformF(GVECTORF _vector, GMATRIXF _matrix, GVECTORF& _outVector) override;

	GReturn MagnitudeF(GVECTORF _vector, float& _outMagnitude) override;

	GReturn NormalizeF(GVECTORF _vector, GVECTORF& _outVector) override;

	GReturn LerpF(GVECTORF _vector1, GVECTORF _vector2, float _ratio, GVECTORF& _outVector) override;

	GReturn SplineF(GVECTORF _vector1, GVECTORF _vector2, GVECTORF _vector3, GVECTORF _vector4, float _ratio, GVECTORF& _outVector) override;

	//double vector

	GReturn AddVectorD(GVECTORD _vector1, GVECTORD _vector2, GVECTORD& _outVector) override;

	GReturn SubtractVectorD(GVECTORD _vector1, GVECTORD _vector2, GVECTORD& _outVector) override;

	GReturn ScaleD(GVECTORD _vector, double _scalar, GVECTORD& _outVector) override;

	GReturn DotD(GVECTORD _vector1, GVECTORD _vector2, double& _outValue) override;

	GReturn CrossVector2D(GVECTORD _vector1, GVECTORD _vector2, double& _outValue) override;

	GReturn CrossVector3D(GVECTORD _vector1, GVECTORD _vector2, GVECTORD& _outVector) override;

	GReturn VectorXMatrixD(GVECTORD _vector, GMATRIXD _matrix, GVECTORD& _outVector) override;

	GReturn TransformD(GVECTORD _vector, GMATRIXD _matrix, GVECTORD& _outVector) override;

	GReturn MagnitudeD(GVECTORD _vector, double& _outMagnitude) override;

	GReturn NormalizeD(GVECTORD _vector, GVECTORD& _outVector) override;

	GReturn LerpD(GVECTORD _vector1, GVECTORD _vector2, double _ratio, GVECTORD& _outVector) override;

	GReturn SplineD(GVECTORD _vector1, GVECTORD _vector2, GVECTORD _vector3, GVECTORD _vector4, double _ratio, GVECTORD& _outVector) override;

	GReturn GetCount(unsigned int& _outCount) override;

	GReturn IncrementCount() override;

	GReturn DecrementCount() override;

	GReturn RequestInterface(const GW::GUUIID& _interfaceID, void** _outputInterface) override;

};

GVectorCpp::GVectorCpp() : refCount(1)
{
}

GVectorCpp::~GVectorCpp()
{
}

//float vector

GReturn GVectorCpp::AddVectorF(GVECTORF _vector1, GVECTORF _vector2, GVECTORF& _outVector)
{
	_outVector.x = _vector1.x + _vector2.x;
	_outVector.y = _vector1.y + _vector2.y;
	_outVector.z = _vector1.z + _vector2.z;
	_outVector.w = _vector1.w + _vector2.w;

	return SUCCESS;
}

GReturn GVectorCpp::SubtractVectorF(GVECTORF _vector1, GVECTORF _vector2, GVECTORF& _outVector)
{
	_outVector.x = _vector1.x - _vector2.x;
	_outVector.y = _vector1.y - _vector2.y;
	_outVector.z = _vector1.z - _vector2.z;
	_outVector.w = _vector1.w - _vector2.w;

	return SUCCESS;
}

GReturn GVectorCpp::ScaleF(GVECTORF _vector, float _scalar, GVECTORF& _outVector)
{
	_outVector.x = _scalar * _vector.x;
	_outVector.y = _scalar * _vector.y;
	_outVector.z = _scalar * _vector.z;
	_outVector.w = _scalar * _vector.w;

	return SUCCESS;
}

GReturn GVectorCpp::DotF(GVECTORF _vector1, GVECTORF _vector2, float& _outValue)
{
	_outValue = (_vector1.x * _vector2.x) + (_vector1.y * _vector2.y) + (_vector1.z * _vector2.z) + (_vector1.w * _vector2.w);

	return SUCCESS;
}

GReturn GVectorCpp::CrossVector2F(GVECTORF _vector1, GVECTORF _vector2, float& _outValue)
{
	_outValue = (_vector1.x * _vector2.y) - (_vector2.y * _vector1.x);

	return SUCCESS;
}

GReturn GVectorCpp::CrossVector3F(GVECTORF _vector1, GVECTORF _vector2, GVECTORF& _outVector)
{
	GVECTORF _v1 = _vector1;
	GVECTORF _v2 = _vector2;
	_outVector.x = (_v1.y * _v2.z) - (_v1.z * _v2.y);
	_outVector.y = (_v1.z * _v2.x) - (_v1.x * _v2.z);
	_outVector.z = (_v1.x * _v2.y) - (_v1.y * _v2.x);
	_outVector.w = 0.0f;

	return SUCCESS;
}

GReturn GVectorCpp::VectorXMatrixF(GVECTORF _vector, GMATRIXF _matrix, GVECTORF& _outVector)
{
	GVECTORF _v = _vector;

	_outVector.x = (_v.x * _matrix.row1.x) + (_v.y * _matrix.row2.x) + (_v.z * _matrix.row3.x) + (_v.w * _matrix.row4.x);
	_outVector.y = (_v.x * _matrix.row1.y) + (_v.y * _matrix.row2.y) + (_v.z * _matrix.row3.y) + (_v.w * _matrix.row4.y);
	_outVector.z = (_v.x * _matrix.row1.z) + (_v.y * _matrix.row2.z) + (_v.z * _matrix.row3.z) + (_v.w * _matrix.row4.z);
	_outVector.w = (_v.x * _matrix.row1.w) + (_v.y * _matrix.row2.w) + (_v.z * _matrix.row3.w) + (_v.w * _matrix.row4.w);

	return SUCCESS;
}

GReturn GVectorCpp::TransformF(GVECTORF _vector, GMATRIXF _matrix, GVECTORF& _outVector)
{
	GVECTORF _v = _vector;

	_outVector.x = (_v.x * _matrix.row1.x) + (_v.y * _matrix.row2.x) + (_v.z * _matrix.row3.x) + (_v.w * 0.0f);
	_outVector.y = (_v.x * _matrix.row1.y) + (_v.y * _matrix.row2.y) + (_v.z * _matrix.row3.y) + (_v.w * 0.0f);
	_outVector.z = (_v.x * _matrix.row1.z) + (_v.y * _matrix.row2.z) + (_v.z * _matrix.row3.z) + (_v.w * 0.0f);
	_outVector.w = 1.0f;

	return SUCCESS;
}

GReturn GVectorCpp::MagnitudeF(GVECTORF _vector, float& _outMagnitude)
{
	_outMagnitude = sqrt((_vector.x * _vector.x) + (_vector.y * _vector.y) + (_vector.z * _vector.z) + (_vector.w * _vector.w));
	if (G_COMPARISON_STANDARD_F(_outMagnitude, 0))
		return FAILURE;
	return SUCCESS;
}

GReturn GVectorCpp::NormalizeF(GVECTORF _vector, GVECTORF& _outVector)
{
	float magnitude = 0.0f;
	if (MagnitudeF(_vector, magnitude) != SUCCESS)
		return FAILURE;

	_outVector.x = _vector.x / magnitude;
	_outVector.y = _vector.y / magnitude;
	_outVector.z = _vector.z / magnitude;
	_outVector.w = _vector.w / magnitude;

	return SUCCESS;
}

GReturn GVectorCpp::LerpF(GVECTORF _vector1, GVECTORF _vector2, float _ratio, GVECTORF & _outVector)
{
	_outVector.x = G_LERP(_vector1.x, _vector2.x, _ratio);
	_outVector.y = G_LERP(_vector1.y, _vector2.y, _ratio);
	_outVector.z = G_LERP(_vector1.z, _vector2.z, _ratio);
	_outVector.w = G_LERP(_vector1.w, _vector2.w, _ratio);

	return SUCCESS;
}

GReturn GVectorCpp::SplineF(GVECTORF _vector1, GVECTORF _vector2, GVECTORF _vector3, GVECTORF _vector4, float _ratio, GVECTORF & _outVector)
{
	GVECTORF p0 = _vector1;
	GVECTORF p1 = _vector2;
	GVECTORF p2 = _vector3;
	GVECTORF p3 = _vector4;

	float t0 = 0.0f;
	float t1 = pow(sqrt(pow((p1.x - p0.x), 2) + pow((p1.y - p0.y), 2) + pow((p1.z - p0.z), 2)), 0.5f) + t0;
	float t2 = pow(sqrt(pow((p2.x - p1.x), 2) + pow((p2.y - p1.y), 2) + pow((p2.z - p1.z), 2)), 0.5f) + t1;
	float t3 = pow(sqrt(pow((p3.x - p2.x), 2) + pow((p3.y - p2.y), 2) + pow((p3.z - p2.z), 2)), 0.5f) + t2;

	GVECTORF A1;
	GVECTORF A2;
	GVECTORF A3;
	GVECTORF B1;
	GVECTORF B2;

	float t = t1 + (t2 - t1) * _ratio;

	for (int i = 0; i < 3; i++)
	{
		A1.data[i] = (t1 - t) / (t1 - t0)*p0.data[i] + (t - t0) / (t1 - t0)*p1.data[i];
		A2.data[i] = (t2 - t) / (t2 - t1)*p1.data[i] + (t - t1) / (t2 - t1)*p2.data[i];
		A3.data[i] = (t3 - t) / (t3 - t2)*p2.data[i] + (t - t2) / (t3 - t2)*p3.data[i];

		B1.data[i] = (t2 - t) / (t2 - t0)*A1.data[i] + (t - t0) / (t2 - t0)*A2.data[i];
		B2.data[i] = (t3 - t) / (t3 - t1)*A2.data[i] + (t - t1) / (t3 - t1)*A3.data[i];

		_outVector.data[i] = (t2 - t) / (t2 - t1)*B1.data[i] + (t - t1) / (t2 - t1)*B2.data[i];
	}
	_outVector.w = 0;

	return SUCCESS;
}



//double vector

GReturn GVectorCpp::AddVectorD(GVECTORD _vector1, GVECTORD _vector2, GVECTORD& _outVector)
{
	_outVector.x = _vector1.x + _vector2.x;
	_outVector.y = _vector1.y + _vector2.y;
	_outVector.z = _vector1.z + _vector2.z;
	_outVector.w = _vector1.w + _vector2.w;

	return SUCCESS;
}

GReturn GVectorCpp::SubtractVectorD(GVECTORD _vector1, GVECTORD _vector2, GVECTORD& _outVector)
{
	_outVector.x = _vector1.x - _vector2.x;
	_outVector.y = _vector1.y - _vector2.y;
	_outVector.z = _vector1.z - _vector2.z;
	_outVector.w = _vector1.w - _vector2.w;

	return SUCCESS;
}

GReturn GVectorCpp::ScaleD(GVECTORD _vector, double _scalar, GVECTORD& _outVector)
{
	_outVector.x = _scalar * _vector.x;
	_outVector.y = _scalar * _vector.y;
	_outVector.z = _scalar * _vector.z;
	_outVector.w = _scalar * _vector.w;

	return SUCCESS;
}

GReturn GVectorCpp::DotD(GVECTORD _vector1, GVECTORD _vector2, double& _outValue)
{
	_outValue = (_vector1.x * _vector2.x) + (_vector1.y * _vector2.y) + (_vector1.z * _vector2.z) + (_vector1.w * _vector2.w);

	return SUCCESS;
}

GReturn GVectorCpp::CrossVector2D(GVECTORD _vector1, GVECTORD _vector2, double& _outValue)
{
	_outValue = (_vector1.x * _vector2.y) - (_vector2.y * _vector1.x);

	return SUCCESS;
}

GReturn GVectorCpp::CrossVector3D(GVECTORD _vector1, GVECTORD _vector2, GVECTORD& _outVector)
{
	GVECTORD _v1 = _vector1;
	GVECTORD _v2 = _vector2;
	_outVector.x = (_v1.y * _v2.z) - (_v1.z * _v2.y);
	_outVector.y = (_v1.z * _v2.x) - (_v1.x * _v2.z);
	_outVector.z = (_v1.x * _v2.y) - (_v1.y * _v2.x);
	_outVector.w = 0.0f;

	return SUCCESS;
}

GReturn GVectorCpp::VectorXMatrixD(GVECTORD _vector, GMATRIXD _matrix, GVECTORD& _outVector)
{
	GVECTORD _v = _vector;
	_outVector.x = (_v.x * _matrix.row1.x) + (_v.y * _matrix.row2.x) + (_v.z * _matrix.row3.x) + (_v.w * _matrix.row4.x);
	_outVector.y = (_v.x * _matrix.row1.y) + (_v.y * _matrix.row2.y) + (_v.z * _matrix.row3.y) + (_v.w * _matrix.row4.y);
	_outVector.z = (_v.x * _matrix.row1.z) + (_v.y * _matrix.row2.z) + (_v.z * _matrix.row3.z) + (_v.w * _matrix.row4.z);
	_outVector.w = (_v.x * _matrix.row1.w) + (_v.y * _matrix.row2.w) + (_v.z * _matrix.row3.w) + (_v.w * _matrix.row4.w);

	return SUCCESS;
}

GReturn GVectorCpp::TransformD(GVECTORD _vector, GMATRIXD _matrix, GVECTORD& _outVector)
{
	GVECTORD _v = _vector;
	_outVector.x = (_v.x * _matrix.row1.x) + (_v.y * _matrix.row2.x) + (_v.z * _matrix.row3.x) + (_v.w * 0.0f);
	_outVector.y = (_v.x * _matrix.row1.y) + (_v.y * _matrix.row2.y) + (_v.z * _matrix.row3.y) + (_v.w * 0.0f);
	_outVector.z = (_v.x * _matrix.row1.z) + (_v.y * _matrix.row2.z) + (_v.z * _matrix.row3.z) + (_v.w * 0.0f);
	_outVector.w = 1.0;

	return SUCCESS;
}

GReturn GVectorCpp::MagnitudeD(GVECTORD _vector, double& _outMagnitude)
{
	_outMagnitude = sqrt((_vector.x * _vector.x) + (_vector.y * _vector.y) + (_vector.z * _vector.z) + (_vector.w * _vector.w));
	if (G_COMPARISON_STANDARD_D(_outMagnitude, 0))
		return FAILURE;
	return SUCCESS;
}

GReturn GVectorCpp::NormalizeD(GVECTORD _vector, GVECTORD& _outVector)
{
	double magnitude = 0.0;
	if (MagnitudeD(_vector, magnitude) != SUCCESS)
	{
		return FAILURE;
	}

	_outVector.x = _vector.x / magnitude;
	_outVector.y = _vector.y / magnitude;
	_outVector.z = _vector.z / magnitude;
	_outVector.w = _vector.w / magnitude;

	return SUCCESS;
}

GReturn GVectorCpp::LerpD(GVECTORD _vector1, GVECTORD _vector2, double _ratio, GVECTORD & _outVector)
{
	_outVector.x = G_LERP(_vector1.x, _vector2.x, _ratio);
	_outVector.y = G_LERP(_vector1.y, _vector2.y, _ratio);
	_outVector.z = G_LERP(_vector1.z, _vector2.z, _ratio);
	_outVector.w = G_LERP(_vector1.w, _vector2.w, _ratio);

	return SUCCESS;
}

GReturn GVectorCpp::SplineD(GVECTORD _vector1, GVECTORD _vector2, GVECTORD _vector3, GVECTORD _vector4, double _ratio, GVECTORD & _outVector)
{
	GVECTORD p0 = _vector1;
	GVECTORD p1 = _vector2;
	GVECTORD p2 = _vector3;
	GVECTORD p3 = _vector4;

	double t0 = 0.0;
	double t1 = t1 = pow(sqrt(pow((p1.x - p0.x), 2.0) + pow((p1.y - p0.y), 2.0) + pow((p1.z - p0.z), 2.0)), 0.5) + t0;
	double t2 = t2 = pow(sqrt(pow((p2.x - p1.x), 2.0) + pow((p2.y - p1.y), 2.0) + pow((p2.z - p1.z), 2.0)), 0.5) + t1;
	double t3 = t3 = pow(sqrt(pow((p3.x - p2.x), 2.0) + pow((p3.y - p2.y), 2.0) + pow((p3.z - p2.z), 2.0)), 0.5) + t2;

	GVECTORD A1;
	GVECTORD A2;
	GVECTORD A3;
	GVECTORD B1;
	GVECTORD B2;

	double t = t1 + (t2 - t1) * _ratio;

	for (int i = 0; i < 3; i++)
	{
		A1.data[i] = (t1 - t) / (t1 - t0)*p0.data[i] + (t - t0) / (t1 - t0)*p1.data[i];
		A2.data[i] = (t2 - t) / (t2 - t1)*p1.data[i] + (t - t1) / (t2 - t1)*p2.data[i];
		A3.data[i] = (t3 - t) / (t3 - t2)*p2.data[i] + (t - t2) / (t3 - t2)*p3.data[i];

		B1.data[i] = (t2 - t) / (t2 - t0)*A1.data[i] + (t - t0) / (t2 - t0)*A2.data[i];
		B2.data[i] = (t3 - t) / (t3 - t1)*A2.data[i] + (t - t1) / (t3 - t1)*A3.data[i];

		_outVector.data[i] = (t2 - t) / (t2 - t1) * B1.data[i] + (t - t1) / (t2 - t1) * B2.data[i];
	}
	_outVector.w = 0;
	return SUCCESS;
}

GReturn GVectorCpp::GetCount(unsigned int& _outCount)
{
	//Store reference count.
	_outCount = refCount;

	return GW::SUCCESS;
}

GReturn GVectorCpp::IncrementCount()
{
	//Check to make sure overflow will not occur.
	if (refCount == G_UINT_MAX)
		return GW::FAILURE;

	//Increment reference count.
	++refCount;

	return GW::SUCCESS;
}

GReturn GVectorCpp::DecrementCount()
{
	//Check to make sure underflow will not occur.
	if (refCount == 0)
		return GW::FAILURE;

	//Decrement reference count.
	--refCount;
	if (refCount == 0)
		delete this;

	return GW::SUCCESS;
}

GReturn GVectorCpp::RequestInterface(const GW::GUUIID& _interfaceID, void ** _outputInterface)
{

	if (_outputInterface == nullptr)
		return GW::INVALID_ARGUMENT;

	//If passed in interface is equivalent to current interface (this).
	if (_interfaceID == GW::MATH::GVectorUUIID)
	{
		//Temporary GFile* to ensure proper functions are called.
		GVector* convert = reinterpret_cast<GVector*>(this);

		//Increment the count of the GFile.
		convert->IncrementCount();

		//Store the value.
		(*_outputInterface) = convert;
	}
	//If requested interface is multithreaded.
	else if (_interfaceID == GW::CORE::GSingleThreadedUUIID)
	{
		//Temporary GSingleThreaded* to ensure proper functions are called.
		GSingleThreaded* convert = reinterpret_cast<GSingleThreaded*>(this);

		//Increment the count of the GMultithreaded.
		convert->IncrementCount();

		//Store the value.
		(*_outputInterface) = convert;
	}
	//If requested interface is the primary interface.
	else if (_interfaceID == GW::CORE::GInterfaceUUIID)
	{
		//Temporary GInterface* to ensure proper functions are called.
		GInterface* convert = reinterpret_cast<GInterface*>(this);

		//Increment the count of the GInterface.
		convert->IncrementCount();

		//Store the value.
		(*_outputInterface) = convert;
	}
	//Interface is not supported.
	else
		return GW::INTERFACE_UNSUPPORTED;

	return GW::SUCCESS;
}


GATEWARE_EXPORT_EXPLICIT GReturn CreateGVector(GVector** _outVector)
{
	return GW::MATH::CreateGVector(_outVector);
}

GReturn GW::MATH::CreateGVector(GVector** _outVector)
{
	if (_outVector == nullptr)
	{
		return INVALID_ARGUMENT;
	}
	GVectorCpp* vec = new GVectorCpp();

	if (vec == nullptr)
	{
		return FAILURE;
	}

	(*_outVector) = vec;

	return SUCCESS;
}

