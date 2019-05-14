// Override export symbols for DLL builds (must be included before interface code).
#include "../DLL_Export_Symbols.h"
#include "../../Interface/G_Math/GQuaternion.h"
#include "../../Source/G_System/GUtility.h"

#include <atomic> //Thread safety
#include <cmath>

//The using statements for specifically what we are using.
using namespace GW;
using namespace MATH;
using namespace std;

class GQuaternionCpp : public GW::MATH::GQuaternion
{
	atomic<unsigned int> refCount;

public:
	GQuaternionCpp();
	~GQuaternionCpp();

	//float quaternion

	GReturn AddQuaternionF(GQUATERNIONF _quaternion1, GQUATERNIONF _quaternion2, GQUATERNIONF& _outQuaternion) override;

	GReturn SubtractQuaternionF(GQUATERNIONF _quaternion1, GQUATERNIONF _quaternion2, GQUATERNIONF& _outQuaternion) override;

	GReturn MultiplyQuaternionF(GQUATERNIONF _quaternion1, GQUATERNIONF _quaternion2, GQUATERNIONF& _outQuaternion) override;

	GReturn ScaleF(GQUATERNIONF _quaternion, float _scalar, GQUATERNIONF& _outQuaternion) override;

	GReturn SetByVectorAngleF(GVECTORF _vector, float _radian, GQUATERNIONF& _outQuaternion) override;

	GReturn SetByMatrixF(GMATRIXF _matrix, GQUATERNIONF& _outQuaternion) override;

	GReturn DotF(GQUATERNIONF _quaternion1, GQUATERNIONF _quaternion2, float& _outValue) override;

	GReturn CrossF(GQUATERNIONF _quaternion1, GQUATERNIONF _quaternion2, GVECTORF& _outVector) override;

	GReturn ConjugateF(GQUATERNIONF _quaternion, GQUATERNIONF& _outQuaternion) override;

	GReturn InverseF(GQUATERNIONF _quaternion, GQUATERNIONF& _outQuaternion) override;

	GReturn MagnitudeF(GQUATERNIONF _quaternion, float& _outMagnitude) override;

	GReturn NormalizeF(GQUATERNIONF _quaternion, GQUATERNIONF& _outQuaternion) override;

	GReturn IdentityF(GQUATERNIONF& _outQuaternion) override;

	GReturn LerpF(GQUATERNIONF _quaternion1, GQUATERNIONF _quaternion2, float _ratio, GQUATERNIONF& _outQuaternion) override;

	GReturn SlerpF(GQUATERNIONF _quaternion1, GQUATERNIONF _quaternion2, float _ratio, GQUATERNIONF& _outQuaternion) override;

	//double quaternion

	GReturn AddQuaternionD(GQUATERNIOND _quaternion1, GQUATERNIOND _quaternion2, GQUATERNIOND& _outQuaternion) override;

	GReturn SubtractQuaternionD(GQUATERNIOND _quaternion1, GQUATERNIOND _quaternion2, GQUATERNIOND& _outQuaternion) override;

	GReturn MultiplyQuaternionD(GQUATERNIOND _quaternion1, GQUATERNIOND _quaternion2, GQUATERNIOND& _outQuaternion) override;

	GReturn ScaleD(GQUATERNIOND _quaternion, double _scalar, GQUATERNIOND& _outQuaternion) override;

	GReturn SetByVectorAngleD(GVECTORD _vector, double _radian, GQUATERNIOND& _outQuaternion) override;

	GReturn SetByMatrixD(GMATRIXD _matrix, GQUATERNIOND& _outQuaternion) override;

	GReturn DotD(GQUATERNIOND _quaternion1, GQUATERNIOND _quaternion2, double& _outQuaternion) override;

	GReturn CrossD(GQUATERNIOND _quaternion1, GQUATERNIOND _quaternion2, GVECTORD& _outVector) override;

	GReturn ConjugateD(GQUATERNIOND _quaternion, GQUATERNIOND& _outQuaternion) override;

	GReturn InverseD(GQUATERNIOND _quaternion, GQUATERNIOND& _outQuaternion) override;

	GReturn MagnitudeD(GQUATERNIOND _quaternion, double& _outMagnitude) override;

	GReturn NormalizeD(GQUATERNIOND _quaternion, GQUATERNIOND& _outQuaternion) override;

	GReturn IdentityD(GQUATERNIOND& _outQuaternion) override;

	GReturn LerpD(GQUATERNIOND _quaternion1, GQUATERNIOND _quaternion2, float _ratio, GQUATERNIOND& _outQuaternion) override;

	GReturn SlerpD(GQUATERNIOND _quaternion1, GQUATERNIOND _quaternion2, double _ratio, GQUATERNIOND& _outQuaternion) override;





	GReturn GetCount(unsigned int& _outCount) override;

	GReturn IncrementCount() override;

	GReturn DecrementCount() override;

	GReturn RequestInterface(const GW::GUUIID& _interfaceID, void** _outputInterface) override;
};

GQuaternionCpp::GQuaternionCpp() : refCount(1)
{
}

GQuaternionCpp::~GQuaternionCpp()
{
}

//float quaternion

GW::GReturn GQuaternionCpp::AddQuaternionF(GQUATERNIONF _quaternion1, GQUATERNIONF _quaternion2, GQUATERNIONF& _outQuaternion)
{
	_outQuaternion.x = _quaternion1.x + _quaternion2.x;
	_outQuaternion.y = _quaternion1.y + _quaternion2.y;
	_outQuaternion.z = _quaternion1.z + _quaternion2.z;
	_outQuaternion.w = _quaternion1.w + _quaternion2.w;
	return SUCCESS;
}

GW::GReturn GQuaternionCpp::SubtractQuaternionF(GQUATERNIONF _quaternion1, GQUATERNIONF _quaternion2, GQUATERNIONF& _outQuaternion)
{
	_outQuaternion.x = _quaternion1.x - _quaternion2.x;
	_outQuaternion.y = _quaternion1.y - _quaternion2.y;
	_outQuaternion.z = _quaternion1.z - _quaternion2.z;
	_outQuaternion.w = _quaternion1.w - _quaternion2.w;
	return SUCCESS;
}


GW::GReturn GQuaternionCpp::MultiplyQuaternionF(GQUATERNIONF _quaternion1, GQUATERNIONF _quaternion2, GQUATERNIONF& _outQuaternion)
{
	GQUATERNIONF _q1 = _quaternion1;
	GQUATERNIONF _q2 = _quaternion2;

	_outQuaternion.x = _q1.w * _q2.x + _q1.x * _q2.w + _q1.y * _q2.z - _q1.z * _q2.y;
	_outQuaternion.y = _q1.w * _q2.y - _q1.x * _q2.z + _q1.y * _q2.w + _q1.z * _q2.x;
	_outQuaternion.z = _q1.w * _q2.z + _q1.x * _q2.y - _q1.y * _q2.x + _q1.z * _q2.w;
	_outQuaternion.w = _q1.w * _q2.w - _q1.x * _q2.x - _q1.y * _q2.y - _q1.z * _q2.z;

	return SUCCESS;
}

GW::GReturn GQuaternionCpp::ScaleF(GQUATERNIONF _quaternion, float _scalar, GQUATERNIONF& _outQuaternion)
{
	_outQuaternion.x = _quaternion.x * _scalar;
	_outQuaternion.y = _quaternion.y * _scalar;
	_outQuaternion.z = _quaternion.z * _scalar;
	_outQuaternion.w = _quaternion.w * _scalar;
	return SUCCESS;
}

GW::GReturn GQuaternionCpp::SetByVectorAngleF(GVECTORF _vector, float _radian, GQUATERNIONF& _outQuaternion)
{
	float s = sin(_radian / 2.0f);
	_outQuaternion.x = s * _vector.x;
	_outQuaternion.y = s * _vector.y;
	_outQuaternion.z = s * _vector.z;
	_outQuaternion.w = cos(_radian / 2.0f);
	return SUCCESS;
}

GW::GReturn GQuaternionCpp::SetByMatrixF(GMATRIXF _matrix, GQUATERNIONF& _outQuaternion)
{
	float det;
	float sx = sqrt(_matrix.data[0] * _matrix.data[0] + _matrix.data[4] * _matrix.data[4] + _matrix.data[8] * _matrix.data[8]);
	float sy = sqrt(_matrix.data[1] * _matrix.data[1] + _matrix.data[5] * _matrix.data[5] + _matrix.data[9] * _matrix.data[9]);
	float sz = sqrt(_matrix.data[2] * _matrix.data[2] + _matrix.data[6] * _matrix.data[6] + _matrix.data[10] * _matrix.data[10]);
	if (G_COMPARISON_STANDARD_F(sx, 0.0f) || G_COMPARISON_STANDARD_F(sy, 0.0f) || G_COMPARISON_STANDARD_F(sz, 0.0f))
	{
		//scale too close to zero, can not decompose rotation
		return FAILURE;
	}

	float a0 = _matrix.data[0] * _matrix.data[5] - _matrix.data[1] * _matrix.data[4];
	float a1 = _matrix.data[0] * _matrix.data[6] - _matrix.data[2] * _matrix.data[4];
	float a2 = _matrix.data[0] * _matrix.data[7] - _matrix.data[3] * _matrix.data[4];
	float a3 = _matrix.data[1] * _matrix.data[6] - _matrix.data[2] * _matrix.data[5];
	float a4 = _matrix.data[1] * _matrix.data[7] - _matrix.data[3] * _matrix.data[5];
	float a5 = _matrix.data[2] * _matrix.data[7] - _matrix.data[3] * _matrix.data[6];
	float b0 = _matrix.data[8] * _matrix.data[13] - _matrix.data[9] * _matrix.data[12];
	float b1 = _matrix.data[8] * _matrix.data[14] - _matrix.data[10] * _matrix.data[12];
	float b2 = _matrix.data[8] * _matrix.data[15] - _matrix.data[11] * _matrix.data[12];
	float b3 = _matrix.data[9] * _matrix.data[14] - _matrix.data[10] * _matrix.data[13];
	float b4 = _matrix.data[9] * _matrix.data[15] - _matrix.data[11] * _matrix.data[13];
	float b5 = _matrix.data[10] * _matrix.data[15] - _matrix.data[11] * _matrix.data[14];

	det = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;

	if (det < 0)
	{
		sz = -sz;
	}
	GMATRIXF Rotation = _matrix;
	Rotation.data[0] /= sx;
	Rotation.data[1] /= sy;
	Rotation.data[2] /= sz;
	Rotation.data[4] /= sx;
	Rotation.data[5] /= sy;
	Rotation.data[6] /= sz;
	Rotation.data[8] /= sx;
	Rotation.data[9] /= sy;
	Rotation.data[10] /= sz;

	float trace = _matrix.data[0] + _matrix.data[5] + _matrix.data[10] + 1;

	if (trace > G_EPSILON_F)
	{
		float s = 0.5f / sqrt(trace);
		_outQuaternion.x = (Rotation.row3.y - Rotation.row2.z) * s;
		_outQuaternion.y = (Rotation.row1.z - Rotation.row3.x) * s;
		_outQuaternion.z = (Rotation.row2.x - Rotation.row1.y) * s;
		_outQuaternion.w = 0.25f / s;
	}
	else
	{
		if (Rotation.row1.x > Rotation.row2.y && Rotation.row1.x > Rotation.row3.z)
		{
			float s = 0.5f / sqrt(1.0f + Rotation.row1.x - Rotation.row2.y - Rotation.row3.z);
			_outQuaternion.x = 0.25f / s;
			_outQuaternion.y = (Rotation.row1.y + Rotation.row2.x) * s;
			_outQuaternion.z = (Rotation.row1.z + Rotation.row3.x) * s;
			_outQuaternion.w = (Rotation.row3.y - Rotation.row2.z) * s;
		}
		else if (Rotation.row2.y > Rotation.row3.z)
		{
			float s = 0.5f / sqrt(1.0f + Rotation.row2.y - Rotation.row1.x - Rotation.row3.z);
			_outQuaternion.x = (Rotation.row1.y + Rotation.row2.x) * s;
			_outQuaternion.y = 0.25f / s;
			_outQuaternion.z = (Rotation.row2.z + Rotation.row3.y) * s;
			_outQuaternion.w = (Rotation.row1.z - Rotation.row3.x) * s;
		}
		else
		{
			float s = 0.5f / sqrt(1.0f + Rotation.row3.z - Rotation.row1.x - Rotation.row2.y);
			_outQuaternion.x = (Rotation.row1.z + Rotation.row3.x) * s;
			_outQuaternion.y = (Rotation.row2.z + Rotation.row3.y) * s;
			_outQuaternion.z = 0.25f / s;
			_outQuaternion.w = (Rotation.row2.x - Rotation.row1.y) * s;
		}
	}
	return SUCCESS;
}

GW::GReturn GQuaternionCpp::DotF(GQUATERNIONF _quaternion1, GQUATERNIONF _quaternion2, float& _outValue)
{
	_outValue = _quaternion1.w * _quaternion2.w + _quaternion1.x * _quaternion2.x + _quaternion1.y * _quaternion2.y + _quaternion1.z * _quaternion2.z;
	return SUCCESS;
}

GReturn GQuaternionCpp::CrossF(GQUATERNIONF _quaternion1, GQUATERNIONF _quaternion2, GVECTORF& _outVector)
{
	_outVector.x = (_quaternion1.y * _quaternion2.z) - (_quaternion1.z * _quaternion2.y);
	_outVector.y = (_quaternion1.z * _quaternion2.x) - (_quaternion1.x * _quaternion2.z);
	_outVector.z = (_quaternion1.x * _quaternion2.y) - (_quaternion1.y * _quaternion2.x);
	_outVector.w = 0.0f;

	return SUCCESS;
}

GReturn GQuaternionCpp::ConjugateF(GQUATERNIONF _quaternion, GQUATERNIONF& _outQuaternion)
{
	_outQuaternion.x = - _quaternion.x;
	_outQuaternion.y = - _quaternion.y;
	_outQuaternion.z = - _quaternion.z;
	_outQuaternion.w =  _quaternion.w;
	return SUCCESS;
}

GReturn GQuaternionCpp::InverseF(GQUATERNIONF _quaternion, GQUATERNIONF& _outQuaternion)
{
	GQUATERNIONF temp;
	float magnitude;
	ConjugateF(_quaternion, temp);
	if (MagnitudeF(_quaternion, magnitude) != SUCCESS)
	{
		return FAILURE;
	}
	_outQuaternion.x = temp.x / (magnitude * magnitude);
	_outQuaternion.y = temp.y / (magnitude * magnitude);
	_outQuaternion.z = temp.z / (magnitude * magnitude);
	_outQuaternion.w = temp.w / (magnitude * magnitude);
	return SUCCESS;
}

GReturn GQuaternionCpp::MagnitudeF(GQUATERNIONF _quaternion, float& _outMagnitude)
{
	_outMagnitude = sqrt(_quaternion.x * _quaternion.x + _quaternion.y * _quaternion.y + _quaternion.z * _quaternion.z + _quaternion.w * _quaternion.w);
	if (G_COMPARISON_STANDARD_F(_outMagnitude, G_EPSILON_F))
		return FAILURE;

	return SUCCESS;
}

GReturn GQuaternionCpp::NormalizeF(GQUATERNIONF _quaternion, GQUATERNIONF& _outQuaternion)
{
	float magnitude;
	MagnitudeF(_quaternion, magnitude);
	if (G_COMPARISON_STANDARD_F(magnitude , 1.0f ))
	{
		_outQuaternion = _quaternion;
		return SUCCESS;
	}
	if (G_COMPARISON_STANDARD_F(magnitude, G_EPSILON_F))
	{
		return FAILURE;
	}
	magnitude = 1 / magnitude;
	_outQuaternion.x = _quaternion.x * magnitude;
	_outQuaternion.y = _quaternion.y * magnitude;
	_outQuaternion.z = _quaternion.z * magnitude;
	_outQuaternion.w = _quaternion.w * magnitude;
	return SUCCESS;
}

GReturn GQuaternionCpp::IdentityF(GQUATERNIONF& _outQuaternion)
{
	_outQuaternion = GIdentityQuaternionF;
	return SUCCESS;
}

GReturn GQuaternionCpp::LerpF(GQUATERNIONF _quaternion1, GQUATERNIONF _quaternion2, float _ratio, GQUATERNIONF& _outQuaternion)
{
	_outQuaternion.x = G_LERP(_quaternion1.x, _quaternion2.x, _ratio);
	_outQuaternion.y = G_LERP(_quaternion1.y, _quaternion2.y, _ratio);
	_outQuaternion.z = G_LERP(_quaternion1.z, _quaternion2.z, _ratio);
	_outQuaternion.w = G_LERP(_quaternion1.w, _quaternion2.w, _ratio);
	return SUCCESS;
}

GReturn GQuaternionCpp::SlerpF(GQUATERNIONF _quaternion1, GQUATERNIONF _quaternion2, float _ratio, GQUATERNIONF& _outQuaternion)
{
	GQUATERNIONF q1;
	GQUATERNIONF q2;
	if (NormalizeF(_quaternion1, q1) != SUCCESS || NormalizeF(_quaternion2, q2) != SUCCESS)
	{
		return FAILURE;
	}
	float dot;
	DotF(q1, q2, dot);
	float theta = acos(dot);

	ScaleF(q1, sin(theta * (1 - _ratio)) / sin(theta), q1);
	ScaleF(q2, sin(theta * _ratio) / sin(theta), q2);

	AddQuaternionF(q1, q2, _outQuaternion);

	return SUCCESS;
}


//double quaternion

GReturn GQuaternionCpp::AddQuaternionD(GQUATERNIOND _quaternion1, GQUATERNIOND _quaternion2, GQUATERNIOND& _outQuaternion)
{
	_outQuaternion.x = _quaternion1.x + _quaternion2.x;
	_outQuaternion.y = _quaternion1.y + _quaternion2.y;
	_outQuaternion.z = _quaternion1.z + _quaternion2.z;
	_outQuaternion.w = _quaternion1.w + _quaternion2.w;
	return SUCCESS;
}

GReturn GQuaternionCpp::SubtractQuaternionD(GQUATERNIOND _quaternion1, GQUATERNIOND _quaternion2, GQUATERNIOND& _outQuaternion)
{
	_outQuaternion.x = _quaternion1.x - _quaternion2.x;
	_outQuaternion.y = _quaternion1.y - _quaternion2.y;
	_outQuaternion.z = _quaternion1.z - _quaternion2.z;
	_outQuaternion.w = _quaternion1.w - _quaternion2.w;
	return SUCCESS;
}

GReturn GQuaternionCpp::MultiplyQuaternionD(GQUATERNIOND _quaternion1, GQUATERNIOND _quaternion2, GQUATERNIOND& _outQuaternion)
{
	GQUATERNIOND _q1 = _quaternion1;
	GQUATERNIOND _q2 = _quaternion2;
	_outQuaternion.x = _q1.w * _q2.x + _q1.x * _q2.w + _q1.y * _q2.z - _q1.z * _q2.y;
	_outQuaternion.y = _q1.w * _q2.y - _q1.x * _q2.z + _q1.y * _q2.w + _q1.z * _q2.x;
	_outQuaternion.z = _q1.w * _q2.z + _q1.x * _q2.y - _q1.y * _q2.x + _q1.z * _q2.w;
	_outQuaternion.w = _q1.w * _q2.w - _q1.x * _q2.x - _q1.y * _q2.y - _q1.z * _q2.z;


	return SUCCESS;
}

GReturn GQuaternionCpp::ScaleD(GQUATERNIOND _quaternion, double _scalar, GQUATERNIOND& _outQuaternion)
{
	_outQuaternion.x = _quaternion.x * _scalar;
	_outQuaternion.y = _quaternion.y * _scalar;
	_outQuaternion.z = _quaternion.z * _scalar;
	_outQuaternion.w = _quaternion.w * _scalar;
	return SUCCESS;
}

GReturn GQuaternionCpp::SetByVectorAngleD(GVECTORD _vector, double _radian, GQUATERNIOND& _outQuaternion)
{
	double s = sin(_radian / 2.0);
	_outQuaternion.x = s * _vector.x;
	_outQuaternion.y = s * _vector.y;
	_outQuaternion.z = s * _vector.z;
	_outQuaternion.w = cos(_radian / 2.0);
	return SUCCESS;
}

GReturn GQuaternionCpp::SetByMatrixD(GMATRIXD _matrix, GQUATERNIOND& _outQuaternion)
{
	double det;
	double sx = sqrt(_matrix.data[0] * _matrix.data[0] + _matrix.data[4] * _matrix.data[4] + _matrix.data[8] * _matrix.data[8]);
	double sy = sqrt(_matrix.data[1] * _matrix.data[1] + _matrix.data[5] * _matrix.data[5] + _matrix.data[9] * _matrix.data[9]);
	double sz = sqrt(_matrix.data[2] * _matrix.data[2] + _matrix.data[6] * _matrix.data[6] + _matrix.data[10] * _matrix.data[10]);
	if (G_COMPARISON_STANDARD_D(sx, 0) || G_COMPARISON_STANDARD_D(sy, 0) || G_COMPARISON_STANDARD_D(sz, 0))
	{
		//scale too close to zero, can not decompose rotation
		return FAILURE;
	}
	double a0 = _matrix.data[0] * _matrix.data[5] - _matrix.data[1] * _matrix.data[4];
	double a1 = _matrix.data[0] * _matrix.data[6] - _matrix.data[2] * _matrix.data[4];
	double a2 = _matrix.data[0] * _matrix.data[7] - _matrix.data[3] * _matrix.data[4];
	double a3 = _matrix.data[1] * _matrix.data[6] - _matrix.data[2] * _matrix.data[5];
	double a4 = _matrix.data[1] * _matrix.data[7] - _matrix.data[3] * _matrix.data[5];
	double a5 = _matrix.data[2] * _matrix.data[7] - _matrix.data[3] * _matrix.data[6];
	double b0 = _matrix.data[8] * _matrix.data[13] - _matrix.data[9] * _matrix.data[12];
	double b1 = _matrix.data[8] * _matrix.data[14] - _matrix.data[10] * _matrix.data[12];
	double b2 = _matrix.data[8] * _matrix.data[15] - _matrix.data[11] * _matrix.data[12];
	double b3 = _matrix.data[9] * _matrix.data[14] - _matrix.data[10] * _matrix.data[13];
	double b4 = _matrix.data[9] * _matrix.data[15] - _matrix.data[11] * _matrix.data[13];
	double b5 = _matrix.data[10] * _matrix.data[15] - _matrix.data[11] * _matrix.data[14];

	det = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;

	if (det < 0)
	{
		sz = -sz;
	}
	GMATRIXD Rotation = _matrix;
	Rotation.data[0] /= sx;
	Rotation.data[1] /= sy;
	Rotation.data[2] /= sz;
	Rotation.data[4] /= sx;
	Rotation.data[5] /= sy;
	Rotation.data[6] /= sz;
	Rotation.data[8] /= sx;
	Rotation.data[9] /= sy;
	Rotation.data[10] /= sz;

	double trace = _matrix.data[0] + _matrix.data[5] + _matrix.data[10] + 1;

	if (trace > G_EPSILON_D)
	{
		double s = 0.5 / sqrt(trace);
		_outQuaternion.x = (Rotation.row3.y - Rotation.row2.z) * s;
		_outQuaternion.y = (Rotation.row1.z - Rotation.row3.x) * s;
		_outQuaternion.z = (Rotation.row2.x - Rotation.row1.y) * s;
		_outQuaternion.w = 0.25 / s;
	}
	else
	{
		if (Rotation.row1.x > Rotation.row2.y && Rotation.row1.x > Rotation.row3.z)
		{
			double s = 0.5 / sqrt(1.0 + Rotation.row1.x - Rotation.row2.y - Rotation.row3.z);
			_outQuaternion.x = 0.25 / s;
			_outQuaternion.y = (Rotation.row1.y + Rotation.row2.x) * s;
			_outQuaternion.z = (Rotation.row1.z + Rotation.row3.x) * s;
			_outQuaternion.w = (Rotation.row3.y - Rotation.row2.z) * s;
		}
		else if (Rotation.row2.y > Rotation.row3.z)
		{
			double s = 0.5 / sqrt(1.0 + Rotation.row2.y - Rotation.row1.x - Rotation.row3.z);
			_outQuaternion.x = (Rotation.row1.y + Rotation.row2.x) * s;
			_outQuaternion.y = 0.25 / s;
			_outQuaternion.z = (Rotation.row2.z + Rotation.row3.y) * s;
			_outQuaternion.w = (Rotation.row1.z - Rotation.row3.x) * s;
		}
		else
		{
			double s = 0.5 / sqrt(1.0 + Rotation.row3.z - Rotation.row1.x - Rotation.row2.y);
			_outQuaternion.x = (Rotation.row1.z + Rotation.row3.x) * s;
			_outQuaternion.y = (Rotation.row2.z + Rotation.row3.y) * s;
			_outQuaternion.z = 0.25 / s;
			_outQuaternion.w = (Rotation.row2.x - Rotation.row1.y) * s;
		}
	}
	return SUCCESS;
}

GReturn GQuaternionCpp::DotD(GQUATERNIOND _quaternion1, GQUATERNIOND _quaternion2, double& _outValue)
{
	_outValue = _quaternion1.w * _quaternion2.w + _quaternion1.x * _quaternion2.x + _quaternion1.y * _quaternion2.y + _quaternion1.z * _quaternion2.z;
	return SUCCESS;
}

GReturn GQuaternionCpp::CrossD(GQUATERNIOND _quaternion1, GQUATERNIOND _quaternion2, GVECTORD& _outVector)
{
	_outVector.x = (_quaternion1.y * _quaternion2.z) - (_quaternion1.z * _quaternion2.y);
	_outVector.y = (_quaternion1.z * _quaternion2.x) - (_quaternion1.x * _quaternion2.z);
	_outVector.z = (_quaternion1.x * _quaternion2.y) - (_quaternion1.y * _quaternion2.x);
	_outVector.w = 0.0;

	return SUCCESS;
}

GReturn GQuaternionCpp::ConjugateD(GQUATERNIOND _quaternion, GQUATERNIOND& _outQuaternion)
{
	_outQuaternion.x = -_quaternion.x;
	_outQuaternion.y = -_quaternion.y;
	_outQuaternion.z = -_quaternion.z;
	_outQuaternion.w = _quaternion.w;

	return SUCCESS;
}

GReturn GQuaternionCpp::InverseD(GQUATERNIOND _quaternion, GQUATERNIOND& _outQuaternion)
{
	GQUATERNIOND temp;
	double magnitude;
	ConjugateD(_quaternion, temp);
	if (MagnitudeD(_quaternion, magnitude) != SUCCESS)
	{
		return FAILURE;
	};
	
	_outQuaternion.x = temp.x / (magnitude * magnitude);
	_outQuaternion.y = temp.y / (magnitude * magnitude);
	_outQuaternion.z = temp.z / (magnitude * magnitude);
	_outQuaternion.w = temp.w / (magnitude * magnitude);
	return SUCCESS;
}

GReturn GQuaternionCpp::MagnitudeD(GQUATERNIOND _quaternion, double& _outMagnitude)
{
	_outMagnitude = sqrt(_quaternion.x * _quaternion.x + _quaternion.y * _quaternion.y + _quaternion.z * _quaternion.z + _quaternion.w * _quaternion.w);
	if (G_COMPARISON_STANDARD_D(_outMagnitude, 0))
		return FAILURE;
	return SUCCESS;
}

GReturn GQuaternionCpp::NormalizeD(GQUATERNIOND _quaternion, GQUATERNIOND& _outQuaternion)
{
	double magnitude;
	MagnitudeD(_quaternion, magnitude);
	if (G_COMPARISON_STANDARD_D(magnitude, 1))
	{
		_outQuaternion = _quaternion;
		return SUCCESS;
	}
	if (G_COMPARISON_STANDARD_D(magnitude, G_EPSILON_D))
	{
		return FAILURE;
	}
	magnitude = 1 / magnitude;
	_outQuaternion.x = _quaternion.x * magnitude;
	_outQuaternion.y = _quaternion.y * magnitude;
	_outQuaternion.z = _quaternion.z * magnitude;
	_outQuaternion.w = _quaternion.w * magnitude;
	return SUCCESS;
}

GReturn GQuaternionCpp::IdentityD(GQUATERNIOND& _outQuaternion)
{
	_outQuaternion = GIdentityQuaternionD;
	return SUCCESS;
}

GReturn GQuaternionCpp::LerpD(GQUATERNIOND _quaternion1, GQUATERNIOND _quaternion2, float _ratio, GQUATERNIOND& _outQuaternion)
{
	if (_ratio > 1)
		_ratio = 1;
	if (_ratio < 0)
		_ratio = 0;

	if (G_COMPARISON_STANDARD_D(_ratio,0))
	{
		_outQuaternion = _quaternion1;
		return SUCCESS;
	}
	else if (G_COMPARISON_STANDARD_D(_ratio, 1))
	{
		_outQuaternion = _quaternion2;
		return SUCCESS;
	}

	_outQuaternion.x = G_LERP(_quaternion1.x, _quaternion2.x, _ratio);
	_outQuaternion.y = G_LERP(_quaternion1.y, _quaternion2.y, _ratio);
	_outQuaternion.z = G_LERP(_quaternion1.z, _quaternion2.z, _ratio);
	_outQuaternion.w = G_LERP(_quaternion1.w, _quaternion2.w, _ratio);
	return SUCCESS;
}

GReturn GQuaternionCpp::SlerpD(GQUATERNIOND _quaternion1, GQUATERNIOND _quaternion2, double _ratio, GQUATERNIOND& _outQuaternion)
{
	if (_ratio > 1)
		_ratio = 1;
	if (_ratio < 0)
		_ratio = 0;

	GQUATERNIOND q1;
	GQUATERNIOND q2;
	if (NormalizeD(_quaternion1, q1) != SUCCESS || NormalizeD(_quaternion2, q2) != SUCCESS)
	{
		return FAILURE;
	}

	if (G_COMPARISON_STANDARD_D(_ratio, 0))
	{
		_outQuaternion = _quaternion1;
		return SUCCESS;
	}
	else if (G_COMPARISON_STANDARD_D(_ratio, 1))
	{
		_outQuaternion = _quaternion2;
		return SUCCESS;
	}


	double dot;
	DotD(q1, q2, dot);
	double theta = acos(dot);
	
	ScaleD(q1, sin(theta * (1 - _ratio)) / sin(theta), q1);
	ScaleD(q2, sin(theta * _ratio) / sin(theta), q2);

	AddQuaternionD(q1, q2, _outQuaternion);

	return SUCCESS;
}


GReturn GQuaternionCpp::GetCount(unsigned int& _outCount)
{
	//Store reference count.
	_outCount = refCount;

	return GW::SUCCESS;
}

GReturn GQuaternionCpp::IncrementCount()
{
	//Check for possible overflow.
	if (refCount == G_UINT_MAX)
		return FAILURE;

	++refCount;

	return SUCCESS;
}

GReturn GQuaternionCpp::DecrementCount()
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

GReturn GQuaternionCpp::RequestInterface(const GW::GUUIID& _interfaceID, void ** _outputInterface)
{
	if (_outputInterface == nullptr)
		return GW::INVALID_ARGUMENT;

	//If passed in interface is equivalent to current interface (this).
	if (_interfaceID == GW::MATH::GQuaternionUUIID)
	{
		//Temporary GQuaternion* to ensure proper functions are called.
		GQuaternion* convert = reinterpret_cast<GQuaternion*>(this);

		//Increment the count of the GFile.
		convert->IncrementCount();

		//Store the value.
		(*_outputInterface) = convert;
	}
	//If requested interface is multithreaded.
	else if (_interfaceID == GW::CORE::GSingleThreadedUUIID)
	{
		//Temporary GMultiThreaded* to ensure proper functions are called.
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


GATEWARE_EXPORT_EXPLICIT GReturn CreateGQuaternion(GQuaternion** _outQuaternion)
{
	return GW::MATH::CreateGQuaternion(_outQuaternion);
}

GReturn GW::MATH::CreateGQuaternion(GQuaternion** _outQuaternion)
{
	if (_outQuaternion == nullptr)
	{
		return INVALID_ARGUMENT;
	}
	GQuaternionCpp* mat = new GQuaternionCpp();

	if (mat == nullptr)
	{
		return FAILURE;
	}

	(*_outQuaternion) = mat;

	return SUCCESS;
}


