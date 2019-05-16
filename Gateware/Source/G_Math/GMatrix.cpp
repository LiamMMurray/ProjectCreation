// Override export symbols for DLL builds (must be included before interface code).
#include "../DLL_Export_Symbols.h"
#include "../../Interface/G_Math/GMatrix.h"
#include "../../Source/G_System/GUtility.h"

#include <atomic> //Thread safety
#include <cmath>

//The using statements for specifically what we are using.
using namespace GW;
using namespace MATH;

class GMatirxCpp :public GMatrix
{
	//Reference counter.
	std::atomic<unsigned int> refCount;

public:
	GMatirxCpp();
	~GMatirxCpp();


	//float Matrix

	GReturn AddMatrixF(GMATRIXF _matrix1, GMATRIXF _matrix2, GMATRIXF& _outMatrix) override;

	GReturn SubtractMatrixF(GMATRIXF _matrix1, GMATRIXF _matrix2, GMATRIXF& _outMatrix) override;

	GReturn MultiplyMatrixF(GMATRIXF _matrix1, GMATRIXF _matrix2, GMATRIXF& _outMatrix) override;

	GReturn VectorXMatrixF(GMATRIXF _matrix, GVECTORF _vector, GVECTORF& _outVector) override;

	GReturn ConvertQuaternionF(GQUATERNIONF _quaternion, GMATRIXF& _outMatrix) override;

	GReturn MultiplyNumF(GMATRIXF _matrix, float _scalar, GMATRIXF& _outMatrix) override;

	GReturn DeterminantF(GMATRIXF _matrix, float& _outValue) override;

	GReturn TransposeF(GMATRIXF _matrix, GMATRIXF& _outMatrix) override;

	GReturn InverseF(GMATRIXF _matrix, GMATRIXF& _outMatrix) override;

	GReturn IdentityF(GMATRIXF& _outMatrix) override;

	GReturn GetRotationF(GMATRIXF _matrix, GQUATERNIONF& _outQuaternion) override;

	GReturn GetTranslationF(GMATRIXF _matrix, GVECTORF& _outMatrix) override;

	GReturn GetScaleF(GMATRIXF _matrix, GVECTORF& _outMatrix) override;

	GReturn RotationXF(GMATRIXF _matrix, float _radian, GMATRIXF& _outMatrix) override;

	GReturn RotationYF(GMATRIXF _matrix, float _radian, GMATRIXF& _outMatrix) override;

	GReturn RotationZF(GMATRIXF _matrix, float _radian, GMATRIXF& _outMatrix) override;

	GReturn RotationYawPitchRollF(float _yaw, float _pitch, float _roll, GMATRIXF& _outMatrix) override;

	GReturn RotationByVectorF(GVECTORF _vector, float _radian, GMATRIXF& _outMatrix) override;

	GReturn TranslatelocalF(GMATRIXF _matrix, GVECTORF _vector, GMATRIXF& _outMatrix) override;

	GReturn ScalingF(GMATRIXF _matrix, GVECTORF _vector, GMATRIXF& _outMatirx) override;

	GReturn LerpF(GMATRIXF _matrix1, GMATRIXF _matrix2, float _ratio, GMATRIXF& _outMatrix) override;

	GReturn ProjectionLHF(float _fovY, float _aspect, float _zn, float _zf, GMATRIXF& _outMatrix) override;

	GReturn LookAtLHF(GVECTORF _eye, GVECTORF _at, GVECTORF _up, GMATRIXF& _outMatrix) override;

	//double Matrix

	GReturn AddMatrixD(GMATRIXD _matrix1, GMATRIXD _matrix2, GMATRIXD& _outMatrix) override;

	GReturn SubtractMatrixD(GMATRIXD _matrix1, GMATRIXD _matrix2, GMATRIXD& _outMatrix) override;

	GReturn MultiplyMatrixD(GMATRIXD _matrix1, GMATRIXD _matrix2, GMATRIXD& _outMatrix) override;

	GReturn VectorXMatrixD(GMATRIXD _matrix, GVECTORD _vector, GVECTORD& _outVector) override;

	GReturn ConvertQuaternionD(GQUATERNIOND _quaternion, GMATRIXD& _outMatrix) override;

	GReturn MultiplyNumD(GMATRIXD _matrix, double _scalar, GMATRIXD& _outMatrix) override;

	GReturn DeterminantD(GMATRIXD _matrix, double& _outValue) override;

	GReturn TransposeD(GMATRIXD _matrix, GMATRIXD& _outMatrix) override;

	GReturn InverseD(GMATRIXD _matrix, GMATRIXD& _outMatrix) override;

	GReturn IdentityD(GMATRIXD& _outMatrix) override;

	GReturn GetRotationD(GMATRIXD _matrix, GQUATERNIOND& _outQuaternion) override;

	GReturn GetTranslationD(GMATRIXD _matrix, GVECTORD& _outMatrix) override;

	GReturn GetScaleD(GMATRIXD _matrix, GVECTORD& _outMatrix) override;

	GReturn RotationXD(GMATRIXD _matrix, double _radian, GMATRIXD& _outMatrix) override;

	GReturn RotationYD(GMATRIXD _matrix, double _radian, GMATRIXD& _outMatrix) override;

	GReturn RotationZD(GMATRIXD _matrix, double _radian, GMATRIXD& _outMatrix) override;

	GReturn RotationYawPitchRollD(double _yaw, double _pitch, double _roll, GMATRIXD& _outMatrix) override;

	GReturn RotationByVectorD(GVECTORD _vector, double _radian, GMATRIXD& _outMatrix) override;

	GReturn TranslatelocalD(GMATRIXD _matrix, GVECTORD _vector, GMATRIXD& _outMatrix) override;

	GReturn ScalingD(GMATRIXD _matrix, GVECTORD _vector, GMATRIXD& _outMatirx) override;

	GReturn LerpD(GMATRIXD _matrix1, GMATRIXD _matrix2, double _ratio, GMATRIXD& _outMatrix) override;

	GReturn ProjectionLHD(double _fovY, double _aspect, double _zn, double _zf, GMATRIXD& _outMatrix) override;

	GReturn LookAtLHD(GVECTORD _eye, GVECTORD _at, GVECTORD _up, GMATRIXD& _outMatrix) override;

	GReturn GetCount(unsigned int& _outCount) override;

	GReturn IncrementCount() override;

	GReturn DecrementCount() override;

	GReturn RequestInterface(const GW::GUUIID& _interfaceID, void** _outputInterface) override;


};

GMatirxCpp::GMatirxCpp() : refCount(1)
{
}

GMatirxCpp::~GMatirxCpp()
{
}

//float Matrix

GReturn GMatirxCpp::AddMatrixF(GMATRIXF _matrix1, GMATRIXF _matrix2, GMATRIXF & _outMatrix)
{
	GMATRIXF _m1 = _matrix1;
	GMATRIXF _m2 = _matrix2;
	for (int i = 0; i < 16; i++)
	{
		_outMatrix.data[i] = _m1.data[i] + _m2.data[i];
	}
	return SUCCESS;
}

GReturn GMatirxCpp::SubtractMatrixF(GMATRIXF _matrix1, GMATRIXF _matrix2, GMATRIXF & _outMatrix)
{
	GMATRIXF _m1 = _matrix1;
	GMATRIXF _m2 = _matrix2;
	for (int i = 0; i < 16; i++)
	{
		_outMatrix.data[i] = _m1.data[i] - _m2.data[i];
	}
	return SUCCESS;
}

GReturn GMatirxCpp::MultiplyMatrixF(GMATRIXF _matrix1, GMATRIXF _matrix2, GMATRIXF & _outMatrix)
{
	GMATRIXF _m1 = _matrix1;
	GMATRIXF _m2 = _matrix2;

	_outMatrix.data[0] = _m1.data[0] * _m2.data[0] + _m1.data[1] * _m2.data[4] + _m1.data[2] * _m2.data[8] + _m1.data[3] * _m2.data[12];
	_outMatrix.data[1] = _m1.data[0] * _m2.data[1] + _m1.data[1] * _m2.data[5] + _m1.data[2] * _m2.data[9] + _m1.data[3] * _m2.data[13];
	_outMatrix.data[2] = _m1.data[0] * _m2.data[2] + _m1.data[1] * _m2.data[6] + _m1.data[2] * _m2.data[10] + _m1.data[3] * _m2.data[14];
	_outMatrix.data[3] = _m1.data[0] * _m2.data[3] + _m1.data[1] * _m2.data[7] + _m1.data[2] * _m2.data[11] + _m1.data[3] * _m2.data[15];

	_outMatrix.data[4] = _m1.data[4] * _m2.data[0] + _m1.data[5] * _m2.data[4] + _m1.data[6] * _m2.data[8] + _m1.data[7] * _m2.data[12];
	_outMatrix.data[5] = _m1.data[4] * _m2.data[1] + _m1.data[5] * _m2.data[5] + _m1.data[6] * _m2.data[9] + _m1.data[7] * _m2.data[13];
	_outMatrix.data[6] = _m1.data[4] * _m2.data[2] + _m1.data[5] * _m2.data[6] + _m1.data[6] * _m2.data[10] + _m1.data[7] * _m2.data[14];
	_outMatrix.data[7] = _m1.data[4] * _m2.data[3] + _m1.data[5] * _m2.data[7] + _m1.data[6] * _m2.data[11] + _m1.data[7] * _m2.data[15];

	_outMatrix.data[8] = _m1.data[8] * _m2.data[0] + _m1.data[9] * _m2.data[4] + _m1.data[10] * _m2.data[8] + _m1.data[11] * _m2.data[12];
	_outMatrix.data[9] = _m1.data[8] * _m2.data[1] + _m1.data[9] * _m2.data[5] + _m1.data[10] * _m2.data[9] + _m1.data[11] * _m2.data[13];
	_outMatrix.data[10] = _m1.data[8] * _m2.data[2] + _m1.data[9] * _m2.data[6] + _m1.data[10] * _m2.data[10] + _m1.data[11] * _m2.data[14];
	_outMatrix.data[11] = _m1.data[8] * _m2.data[3] + _m1.data[9] * _m2.data[7] + _m1.data[10] * _m2.data[11] + _m1.data[11] * _m2.data[15];

	_outMatrix.data[12] = _m1.data[12] * _m2.data[0] + _m1.data[13] * _m2.data[4] + _m1.data[14] * _m2.data[8] + _m1.data[15] * _m2.data[12];
	_outMatrix.data[13] = _m1.data[12] * _m2.data[1] + _m1.data[13] * _m2.data[5] + _m1.data[14] * _m2.data[9] + _m1.data[15] * _m2.data[13];
	_outMatrix.data[14] = _m1.data[12] * _m2.data[2] + _m1.data[13] * _m2.data[6] + _m1.data[14] * _m2.data[10] + _m1.data[15] * _m2.data[14];
	_outMatrix.data[15] = _m1.data[12] * _m2.data[3] + _m1.data[13] * _m2.data[7] + _m1.data[14] * _m2.data[11] + _m1.data[15] * _m2.data[15];

	return SUCCESS;
}

GReturn GMatirxCpp::VectorXMatrixF(GMATRIXF _matrix, GVECTORF _vector, GVECTORF & _outVector)
{
	GVECTORF _v = _vector;

	_outVector.x = _v.data[0] * _matrix.data[0] + _v.data[1] * _matrix.data[4] + _v.data[2] * _matrix.data[8] + _v.data[3] * _matrix.data[12];
	_outVector.y = _v.data[0] * _matrix.data[1] + _v.data[1] * _matrix.data[5] + _v.data[2] * _matrix.data[9] + _v.data[3] * _matrix.data[13];
	_outVector.z = _v.data[0] * _matrix.data[2] + _v.data[1] * _matrix.data[6] + _v.data[2] * _matrix.data[10] + _v.data[3] * _matrix.data[14];
	_outVector.w = _v.data[0] * _matrix.data[3] + _v.data[1] * _matrix.data[7] + _v.data[2] * _matrix.data[11] + _v.data[3] * _matrix.data[15];

	return SUCCESS;
}

GReturn GMatirxCpp::ConvertQuaternionF(GQUATERNIONF _quaternion, GMATRIXF & _outMatrix)
{

	float xx2 = 2 * _quaternion.x * _quaternion.x;
	float yy2 = 2 * _quaternion.y * _quaternion.y;
	float zz2 = 2 * _quaternion.z * _quaternion.z;

	float xy2 = 2 * _quaternion.x * _quaternion.y;
	float xz2 = 2 * _quaternion.x * _quaternion.z;
	float yz2 = 2 * _quaternion.y * _quaternion.z;

	float wx2 = 2 * _quaternion.w * _quaternion.x;
	float wy2 = 2 * _quaternion.w * _quaternion.y;
	float wz2 = 2 * _quaternion.w * _quaternion.z;

	_outMatrix.data[0] = 1.0f - yy2 - zz2;
	_outMatrix.data[1] = xy2 - wz2;
	_outMatrix.data[2] = xz2 + wy2;
	_outMatrix.data[3] = 0.0f;
	_outMatrix.data[4] = xy2 + wz2;
	_outMatrix.data[5] = 1.0f - xx2 - zz2;
	_outMatrix.data[6] = yz2 - wx2;
	_outMatrix.data[7] = 0.0f;
	_outMatrix.data[8] = xz2 - wy2;
	_outMatrix.data[9] = yz2 + wx2;
	_outMatrix.data[10] = 1.0f - xx2 - yy2;
	_outMatrix.data[11] = 0.0f;
	_outMatrix.data[12] = 0.0f;
	_outMatrix.data[13] = 0.0f;
	_outMatrix.data[14] = 0.0f;
	_outMatrix.data[15] = 1.0f;

	return SUCCESS;
}

GReturn GMatirxCpp::MultiplyNumF(GMATRIXF _matrix, float _scalar, GMATRIXF & _outMatrix)
{
	GMATRIXF _m = _matrix;
	for (int i = 0; i < 16; i++)
	{
		_outMatrix.data[i] = _m.data[i] * _scalar;
	}
	return SUCCESS;
}

GReturn GMatirxCpp::DeterminantF(GMATRIXF _matrix, float & _outValue)
{
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

	_outValue = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;

	return SUCCESS;
}

GReturn GMatirxCpp::TransposeF(GMATRIXF _matrix, GMATRIXF & _outMatrix)
{
	GMATRIXF _m = _matrix;

	_outMatrix.data[0] = _m.data[0];
	_outMatrix.data[1] = _m.data[4];
	_outMatrix.data[2] = _m.data[8];
	_outMatrix.data[3] = _m.data[12];
	_outMatrix.data[4] = _m.data[1];
	_outMatrix.data[5] = _m.data[5];
	_outMatrix.data[6] = _m.data[9];
	_outMatrix.data[7] = _m.data[13];
	_outMatrix.data[8] = _m.data[2];
	_outMatrix.data[9] = _m.data[6];
	_outMatrix.data[10] = _m.data[10];
	_outMatrix.data[11] = _m.data[14];
	_outMatrix.data[12] = _m.data[3];
	_outMatrix.data[13] = _m.data[7];
	_outMatrix.data[14] = _m.data[11];
	_outMatrix.data[15] = _m.data[15];
	return SUCCESS;
}

GReturn GMatirxCpp::InverseF(GMATRIXF _matrix, GMATRIXF & _outMatrix)
{
	GMATRIXF _m = _matrix;

	float det;
	float a0 = _m.data[0] * _m.data[5] - _m.data[1] * _m.data[4];
	float a1 = _m.data[0] * _m.data[6] - _m.data[2] * _m.data[4];
	float a2 = _m.data[0] * _m.data[7] - _m.data[3] * _m.data[4];
	float a3 = _m.data[1] * _m.data[6] - _m.data[2] * _m.data[5];
	float a4 = _m.data[1] * _m.data[7] - _m.data[3] * _m.data[5];
	float a5 = _m.data[2] * _m.data[7] - _m.data[3] * _m.data[6];
	float b0 = _m.data[8] * _m.data[13] - _m.data[9] * _m.data[12];
	float b1 = _m.data[8] * _m.data[14] - _m.data[10] * _m.data[12];
	float b2 = _m.data[8] * _m.data[15] - _m.data[11] * _m.data[12];
	float b3 = _m.data[9] * _m.data[14] - _m.data[10] * _m.data[13];
	float b4 = _m.data[9] * _m.data[15] - _m.data[11] * _m.data[13];
	float b5 = _m.data[10] * _m.data[15] - _m.data[11] * _m.data[14];

	det = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;
	if (G_COMPARISON_STANDARD_F(det,0.0f))
	{
		return FAILURE;
	}
	_outMatrix.data[0] = _m.data[5] * b5 - _m.data[6] * b4 + _m.data[7] * b3;
	_outMatrix.data[1] = -_m.data[1] * b5 + _m.data[2] * b4 - _m.data[3] * b3;
	_outMatrix.data[2] = _m.data[13] * a5 - _m.data[14] * a4 + _m.data[15] * a3;
	_outMatrix.data[3] = -_m.data[9] * a5 + _m.data[10] * a4 - _m.data[11] * a3;

	_outMatrix.data[4] = -_m.data[4] * b5 + _m.data[6] * b2 - _m.data[7] * b1;
	_outMatrix.data[5] = _m.data[0] * b5 - _m.data[2] * b2 + _m.data[3] * b1;
	_outMatrix.data[6] = -_m.data[12] * a5 + _m.data[14] * a2 - _m.data[15] * a1;
	_outMatrix.data[7] = _m.data[8] * a5 - _m.data[10] * a2 + _m.data[11] * a1;

	_outMatrix.data[8] = _m.data[4] * b4 - _m.data[5] * b2 + _m.data[7] * b0;
	_outMatrix.data[9] = -_m.data[0] * b4 + _m.data[1] * b2 - _m.data[3] * b0;
	_outMatrix.data[10] = _m.data[12] * a4 - _m.data[13] * a2 + _m.data[15] * a0;
	_outMatrix.data[11] = -_m.data[8] * a4 + _m.data[9] * a2 - _m.data[11] * a0;

	_outMatrix.data[12] = -_m.data[4] * b3 + _m.data[5] * b1 - _m.data[6] * b0;
	_outMatrix.data[13] = _m.data[0] * b3 - _m.data[1] * b1 + _m.data[2] * b0;
	_outMatrix.data[14] = -_m.data[12] * a3 + _m.data[13] * a1 - _m.data[14] * a0;
	_outMatrix.data[15] = _m.data[8] * a3 - _m.data[9] * a1 + _m.data[10] * a0;

	MultiplyNumF(_outMatrix, 1.0f / det, _outMatrix);

	return SUCCESS;
}

GReturn GMatirxCpp::IdentityF(GMATRIXF & _outMatrix)
{
	_outMatrix = GIdentityMatrixF;
	return SUCCESS;
}

GReturn GMatirxCpp::GetRotationF(GMATRIXF _matrix, GQUATERNIONF & _outQuaternion)
{
	GMATRIXF _m = _matrix;

	float det;
	float sx = sqrt(_m.data[0] * _m.data[0] + _m.data[4] * _m.data[4] + _m.data[8] * _m.data[8]);
	float sy = sqrt(_m.data[1] * _m.data[1] + _m.data[5] * _m.data[5] + _m.data[9] * _m.data[9]);
	float sz = sqrt(_m.data[2] * _m.data[2] + _m.data[6] * _m.data[6] + _m.data[10] * _m.data[10]);
	DeterminantF(_m, det);

	if (G_COMPARISON_STANDARD_F(det, 0.0f))
		return FAILURE;

	if (det < 0)
	{
		sx = -sx;
	}

	GMATRIXF Rotation = _m;
	Rotation.data[0] /= sx;
	Rotation.data[4] /= sx;
	Rotation.data[8] /= sx;
	Rotation.data[1] /= sy;
	Rotation.data[5] /= sy;
	Rotation.data[9] /= sy;
	Rotation.data[2] /= sz;
	Rotation.data[6] /= sz;
	Rotation.data[10] /= sz;


	float trace = Rotation.data[0] + Rotation.data[5] + Rotation.data[10] + 1;

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

GReturn GMatirxCpp::GetTranslationF(GMATRIXF _matrix, GVECTORF& _outVector)
{
	_outVector.x = _matrix.data[3];
	_outVector.y = _matrix.data[7];
	_outVector.z = _matrix.data[11];
	_outVector.w = 0;
	return SUCCESS;
}

GReturn GMatirxCpp::GetScaleF(GMATRIXF _matrix, GVECTORF& _outVector)
{
	float det;

	_outVector.x = sqrt(_matrix.data[0] * _matrix.data[0] + _matrix.data[4] * _matrix.data[4] + _matrix.data[8] * _matrix.data[8]);
	_outVector.y = sqrt(_matrix.data[1] * _matrix.data[1] + _matrix.data[5] * _matrix.data[5] + _matrix.data[9] * _matrix.data[9]);
	_outVector.z = sqrt(_matrix.data[2] * _matrix.data[2] + _matrix.data[6] * _matrix.data[6] + _matrix.data[10] * _matrix.data[10]);
	_outVector.w = 0;

	DeterminantF(_matrix, det);
	if (det < 0)
	{
		_outVector.x = -_outVector.x;
	}

	return SUCCESS;
}

GReturn GMatirxCpp::RotationXF(GMATRIXF _matrix, float _radian, GMATRIXF & _outMatrix)
{
	float c = cos(_radian);
	float s = sin(_radian);
	GMATRIXF Rotation = GIdentityMatrixF;
	Rotation.data[5] = c;
	Rotation.data[6] = s;
	Rotation.data[9] = -s;
	Rotation.data[10] = c;

	MultiplyMatrixF(_matrix, Rotation, _outMatrix);

	return SUCCESS;
}

GReturn GMatirxCpp::RotationYF(GMATRIXF _matrix, float _radian, GMATRIXF & _outMatrix)
{
	float c = cos(_radian);
	float s = sin(_radian);
	GMATRIXF Rotation = GIdentityMatrixF;
	Rotation.data[0] = c;
	Rotation.data[2] = -s;
	Rotation.data[8] = s;
	Rotation.data[10] = c;

	MultiplyMatrixF(_matrix, Rotation, _outMatrix);
	return SUCCESS;
}

GReturn GMatirxCpp::RotationZF(GMATRIXF _matrix, float _radian, GMATRIXF & _outMatrix)
{
	float c = cos(_radian);
	float s = sin(_radian);
	GMATRIXF Rotation = GIdentityMatrixF;
	Rotation.data[0] = c;
	Rotation.data[1] = s;
	Rotation.data[4] = -s;
	Rotation.data[5] = c;

	MultiplyMatrixF(_matrix, Rotation, _outMatrix);
	return SUCCESS;
}

GReturn GMatirxCpp::RotationYawPitchRollF(float _yaw, float _pitch, float _roll, GMATRIXF & _outMatrix)
{
	_outMatrix = GIdentityMatrixF;
	GMATRIXF RotationX;
	GMATRIXF RotationY;
	GMATRIXF RotationZ;
	GMATRIXF reuslt;
	RotationZF(GIdentityMatrixF, _roll, RotationZ);
	RotationXF(GIdentityMatrixF, _pitch, RotationX);
	RotationYF(GIdentityMatrixF, _yaw, RotationY);

	MultiplyMatrixF(RotationX, RotationY, reuslt);
	MultiplyMatrixF(RotationZ, reuslt, _outMatrix);

	return SUCCESS;
}

GReturn GMatirxCpp::RotationByVectorF(GVECTORF _vector, float _radian, GMATRIXF & _outMatrix)
{
	float x = _vector.x;
	float y = _vector.y;
	float z = _vector.z;

	float magnitude = x * x + y * y + z * z;
	if (!G_COMPARISON_STANDARD_F(magnitude, 1))
	{
		magnitude = sqrt(magnitude);
		if (!G_COMPARISON_STANDARD_F(magnitude, 0))
		{
			magnitude = 1.0f / magnitude;
			x = x * magnitude;
			y = y * magnitude;
			z = z * magnitude;
		}
		else return FAILURE;
	}
	float c = cos(_radian);
	float s = sin(_radian);

	float t = 1.0f - c;
	float tx = t * x;
	float ty = t * y;
	float tz = t * z;
	float txy = tx * y;
	float txz = tx * z;
	float tyz = ty * z;
	float sx = s * x;
	float sy = s * y;
	float sz = s * z;

	_outMatrix.data[0] = c + tx*x;
	_outMatrix.data[1] = txy + sz;
	_outMatrix.data[2] = txz - sy;
	_outMatrix.data[3] = 0.0f;

	_outMatrix.data[4] = txy - sz;
	_outMatrix.data[5] = c + ty*y;
	_outMatrix.data[6] = tyz + sx;
	_outMatrix.data[7] = 0.0f;

	_outMatrix.data[8] = txz + sy;
	_outMatrix.data[9] = tyz - sx;
	_outMatrix.data[10] = c + tz*z;
	_outMatrix.data[11] = 0.0f;

	_outMatrix.data[12] = 0.0f;
	_outMatrix.data[13] = 0.0f;
	_outMatrix.data[14] = 0.0f;
	_outMatrix.data[15] = 1.0f;

	return SUCCESS;
}

GReturn GMatirxCpp::TranslatelocalF(GMATRIXF _matrix, GVECTORF _vector, GMATRIXF & _outMatrix)
{
	// *BUG FIX* for some reason this was column major & global?
	GMATRIXF Translation = GIdentityMatrixF;
	Translation.data[12] = _vector.x;
	Translation.data[13] = _vector.y;
	Translation.data[14] = _vector.z;
	// order switched
	MultiplyMatrixF(Translation, _matrix, _outMatrix);

	return SUCCESS;
}

GReturn GMatirxCpp::ScalingF(GMATRIXF _matrix, GVECTORF _vector, GMATRIXF & _outMatrix)
{
	GMATRIXF Scale = GIdentityMatrixF;
	Scale.data[0] = _vector.x;
	Scale.data[5] = _vector.y;
	Scale.data[10] = _vector.z;

	MultiplyMatrixF(_matrix, Scale, _outMatrix);

	return SUCCESS;
}

GReturn GMatirxCpp::LerpF(GMATRIXF _matrix1, GMATRIXF _matrix2, float _ratio, GMATRIXF & _outMatrix)
{

	GMATRIXF _m1 = _matrix1;
	GMATRIXF _m2 = _matrix2;

	for (int i = 0; i < 16; i++)
	{
		_outMatrix.data[i] = G_LERP(_m1.data[i], _m2.data[i], _ratio);
	}
	return SUCCESS;
}

GReturn GMatirxCpp::ProjectionLHF(float _fovY, float _aspect, float _zn, float _zf, GMATRIXF & _outMatrix)
{
	if (G_COMPARISON_STANDARD_F(_aspect, 0.0f)) return FAILURE;

	_outMatrix = GZeroMatrixF;
	_outMatrix.row2.data[1] = 1 / tanf(_fovY / 2.0f);
	_outMatrix.row1.data[0] = _outMatrix.row2.data[1] / _aspect;
	_outMatrix.row3.data[2] = _zf / (_zf - _zn);
	_outMatrix.row3.data[3] = 1;
	_outMatrix.row4.data[2] = -_zn * _zf / (_zf - _zn);

	return SUCCESS;
}

GReturn GMatirxCpp::LookAtLHF(GVECTORF _eye, GVECTORF _at, GVECTORF _up, GMATRIXF & _outMatrix)
{
	_outMatrix = GIdentityMatrixF;
	GVECTORF temp;
	GVECTORF camDir;	  //zaxis
	GVECTORF camRight;	  //xaxis
	GVECTORF camUp;		  //yaxis
	float magnitudeX;
	float magnitudeZ;

	camDir.x = _at.x - _eye.x;
	camDir.y = _at.y - _eye.y;
	camDir.z = _at.z - _eye.z;

	magnitudeZ = sqrt((camDir.x * camDir.x) + (camDir.y * camDir.y) + (camDir.z * camDir.z));
	if (G_COMPARISON_STANDARD_F(magnitudeZ, 0.0f)) return FAILURE;


	camDir.x /= magnitudeZ;
	camDir.y /= magnitudeZ;
	camDir.z /= magnitudeZ;


	camRight.x = (_up.y * camDir.z) - (_up.z * camDir.y);
	camRight.y = (_up.z * camDir.x) - (_up.x * camDir.z);
	camRight.z = (_up.x * camDir.y) - (_up.y * camDir.x);

	magnitudeX = sqrt((camRight.x * camRight.x) + (camRight.y * camRight.y) + (camRight.z * camRight.z));
	if (G_COMPARISON_STANDARD_F(magnitudeX, 0.0f)) return FAILURE;


	camRight.x /= magnitudeX;
	camRight.y /= magnitudeX;
	camRight.z /= magnitudeX;


	camUp.x = (camDir.y * camRight.z) - (camDir.z * camRight.y);
	camUp.y = (camDir.z * camRight.x) - (camDir.x * camRight.z);
	camUp.z = (camDir.x * camRight.y) - (camDir.y * camRight.x);

	temp.x = camRight.x * _eye.x + camRight.y * _eye.y + camRight.z * _eye.z;
	temp.y = camUp.x * _eye.x + camUp.y * _eye.y + camUp.z * _eye.z;
	temp.z = camDir.x * _eye.x + camDir.y * _eye.y + camDir.z * _eye.z;

	_outMatrix.data[0] = camRight.x;
	_outMatrix.data[4] = camRight.y;
	_outMatrix.data[8] = camRight.z;
	_outMatrix.data[12] = -temp.x;

	_outMatrix.data[1] = camUp.x;
	_outMatrix.data[5] = camUp.y;
	_outMatrix.data[9] = camUp.z;
	_outMatrix.data[13] = -temp.y;

	_outMatrix.data[2] = camDir.x;
	_outMatrix.data[6] = camDir.y;
	_outMatrix.data[10] = camDir.z;
	_outMatrix.data[14] = -temp.z;
	return SUCCESS;
}


//double Matrix

GReturn GMatirxCpp::AddMatrixD(GMATRIXD _matrix1, GMATRIXD _matrix2, GMATRIXD & _outMatrix)
{
	GMATRIXD _m1 = _matrix1;
	GMATRIXD _m2 = _matrix2;
	for (int i = 0; i < 16; i++)
	{
		_outMatrix.data[i] = _m1.data[i] + _m2.data[i];
	}
	return SUCCESS;
}

GReturn GMatirxCpp::SubtractMatrixD(GMATRIXD _matrix1, GMATRIXD _matrix2, GMATRIXD & _outMatrix)
{
	GMATRIXD _m1 = _matrix1;
	GMATRIXD _m2 = _matrix2;
	for (int i = 0; i < 16; i++)
	{
		_outMatrix.data[i] = _m1.data[i] - _matrix2.data[i];
	}
	return SUCCESS;
}

GReturn GMatirxCpp::MultiplyMatrixD(GMATRIXD _matrix1, GMATRIXD _matrix2, GMATRIXD & _outMatrix)
{
	GMATRIXD _m1 = _matrix1;
	GMATRIXD _m2 = _matrix2;
	_outMatrix.data[0] = _m1.data[0] * _m2.data[0] + _m1.data[1] * _m2.data[4] + _m1.data[2] * _m2.data[8] + _m1.data[3] * _m2.data[12];
	_outMatrix.data[1] = _m1.data[0] * _m2.data[1] + _m1.data[1] * _m2.data[5] + _m1.data[2] * _m2.data[9] + _m1.data[3] * _m2.data[13];
	_outMatrix.data[2] = _m1.data[0] * _m2.data[2] + _m1.data[1] * _m2.data[6] + _m1.data[2] * _m2.data[10] + _m1.data[3] * _m2.data[14];
	_outMatrix.data[3] = _m1.data[0] * _m2.data[3] + _m1.data[1] * _m2.data[7] + _m1.data[2] * _m2.data[11] + _m1.data[3] * _m2.data[15];

	_outMatrix.data[4] = _m1.data[4] * _m2.data[0] + _m1.data[5] * _m2.data[4] + _m1.data[6] * _m2.data[8] + _m1.data[7] * _m2.data[12];
	_outMatrix.data[5] = _m1.data[4] * _m2.data[1] + _m1.data[5] * _m2.data[5] + _m1.data[6] * _m2.data[9] + _m1.data[7] * _m2.data[13];
	_outMatrix.data[6] = _m1.data[4] * _m2.data[2] + _m1.data[5] * _m2.data[6] + _m1.data[6] * _m2.data[10] + _m1.data[7] * _m2.data[14];
	_outMatrix.data[7] = _m1.data[4] * _m2.data[3] + _m1.data[5] * _m2.data[7] + _m1.data[6] * _m2.data[11] + _m1.data[7] * _m2.data[15];

	_outMatrix.data[8] = _m1.data[8] * _m2.data[0] + _m1.data[9] * _m2.data[4] + _m1.data[10] * _m2.data[8] + _m1.data[11] * _m2.data[12];
	_outMatrix.data[9] = _m1.data[8] * _m2.data[1] + _m1.data[9] * _m2.data[5] + _m1.data[10] * _m2.data[9] + _m1.data[11] * _m2.data[13];
	_outMatrix.data[10] = _m1.data[8] * _m2.data[2] + _m1.data[9] * _m2.data[6] + _m1.data[10] * _m2.data[10] + _m1.data[11] * _m2.data[14];
	_outMatrix.data[11] = _m1.data[8] * _m2.data[3] + _m1.data[9] * _m2.data[7] + _m1.data[10] * _m2.data[11] + _m1.data[11] * _m2.data[15];

	_outMatrix.data[12] = _m1.data[12] * _m2.data[0] + _m1.data[13] * _m2.data[4] + _m1.data[14] * _m2.data[8] + _m1.data[15] * _m2.data[12];
	_outMatrix.data[13] = _m1.data[12] * _m2.data[1] + _m1.data[13] * _m2.data[5] + _m1.data[14] * _m2.data[9] + _m1.data[15] * _m2.data[13];
	_outMatrix.data[14] = _m1.data[12] * _m2.data[2] + _m1.data[13] * _m2.data[6] + _m1.data[14] * _m2.data[10] + _m1.data[15] * _m2.data[14];
	_outMatrix.data[15] = _m1.data[12] * _m2.data[3] + _m1.data[13] * _m2.data[7] + _m1.data[14] * _m2.data[11] + _m1.data[15] * _m2.data[15];

	return SUCCESS;
}

GReturn GMatirxCpp::VectorXMatrixD(GMATRIXD _matrix, GVECTORD _vector, GVECTORD & _outVector)
{
	GMATRIXD _m = _matrix;
	_outVector.x = _vector.data[0] * _m.data[0] + _vector.data[1] * _m.data[4] + _vector.data[2] * _m.data[8] + _vector.data[3] * _m.data[12];
	_outVector.y = _vector.data[0] * _m.data[1] + _vector.data[1] * _m.data[5] + _vector.data[2] * _m.data[9] + _vector.data[3] * _m.data[13];
	_outVector.z = _vector.data[0] * _m.data[2] + _vector.data[1] * _m.data[6] + _vector.data[2] * _m.data[10] + _vector.data[3] * _m.data[14];
	_outVector.w = _vector.data[0] * _m.data[3] + _vector.data[1] * _m.data[7] + _vector.data[2] * _m.data[11] + _vector.data[3] * _m.data[15];

	return SUCCESS;
}

GReturn GMatirxCpp::ConvertQuaternionD(GQUATERNIOND _quaternion, GMATRIXD & _outMatrix)
{
	double xx2 = 2 * _quaternion.x * _quaternion.x;
	double yy2 = 2 * _quaternion.y * _quaternion.y;
	double zz2 = 2 * _quaternion.z * _quaternion.z;

	double xy2 = 2 * _quaternion.x * _quaternion.y;
	double xz2 = 2 * _quaternion.x * _quaternion.z;
	double yz2 = 2 * _quaternion.y * _quaternion.z;

	double wx2 = 2 * _quaternion.w * _quaternion.x;
	double wy2 = 2 * _quaternion.w * _quaternion.y;
	double wz2 = 2 * _quaternion.w * _quaternion.z;

	_outMatrix.data[0] = 1.0f - yy2 - zz2;
	_outMatrix.data[1] = xy2 - wz2;
	_outMatrix.data[2] = xz2 + wy2;
	_outMatrix.data[3] = 0.0f;
	_outMatrix.data[4] = xy2 + wz2;
	_outMatrix.data[5] = 1.0f - xx2 - zz2;
	_outMatrix.data[6] = yz2 - wx2;
	_outMatrix.data[7] = 0.0f;
	_outMatrix.data[8] = xz2 - wy2;
	_outMatrix.data[9] = yz2 + wx2;
	_outMatrix.data[10] = 1.0f - xx2 - yy2;
	_outMatrix.data[11] = 0.0f;
	_outMatrix.data[12] = 0.0f;
	_outMatrix.data[13] = 0.0f;
	_outMatrix.data[14] = 0.0f;
	_outMatrix.data[15] = 1.0f;

	return SUCCESS;
}

GReturn GMatirxCpp::MultiplyNumD(GMATRIXD _matrix, double _scalar, GMATRIXD & _outMatrix)
{
	GMATRIXD _m = _matrix;
	for (int i = 0; i < 16; i++)
	{
		_outMatrix.data[i] = _m.data[i] * _scalar;
	}
	return SUCCESS;
}

GReturn GMatirxCpp::DeterminantD(GMATRIXD _matrix, double & _outValue)
{
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

	_outValue = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;

	return SUCCESS;
}

GReturn GMatirxCpp::TransposeD(GMATRIXD _matrix, GMATRIXD & _outMatrix)
{
	GMATRIXD _m = _matrix;
	_outMatrix.data[0] = _m.data[0];
	_outMatrix.data[1] = _m.data[4];
	_outMatrix.data[2] = _m.data[8];
	_outMatrix.data[3] = _m.data[12];
	_outMatrix.data[4] = _m.data[1];
	_outMatrix.data[5] = _m.data[5];
	_outMatrix.data[6] = _m.data[9];
	_outMatrix.data[7] = _m.data[13];
	_outMatrix.data[8] = _m.data[2];
	_outMatrix.data[9] = _m.data[6];
	_outMatrix.data[10] = _m.data[10];
	_outMatrix.data[11] = _m.data[14];
	_outMatrix.data[12] = _m.data[3];
	_outMatrix.data[13] = _m.data[7];
	_outMatrix.data[14] = _m.data[11];
	_outMatrix.data[15] = _m.data[15];
	return SUCCESS;
}

GReturn GMatirxCpp::InverseD(GMATRIXD _matrix, GMATRIXD & _outMatrix)
{
	GMATRIXD _m = _matrix;
	double det;
	double a0 = _m.data[0] * _m.data[5] - _m.data[1] * _m.data[4];
	double a1 = _m.data[0] * _m.data[6] - _m.data[2] * _m.data[4];
	double a2 = _m.data[0] * _m.data[7] - _m.data[3] * _m.data[4];
	double a3 = _m.data[1] * _m.data[6] - _m.data[2] * _m.data[5];
	double a4 = _m.data[1] * _m.data[7] - _m.data[3] * _m.data[5];
	double a5 = _m.data[2] * _m.data[7] - _m.data[3] * _m.data[6];
	double b0 = _m.data[8] * _m.data[13] - _m.data[9] * _m.data[12];
	double b1 = _m.data[8] * _m.data[14] - _m.data[10] * _m.data[12];
	double b2 = _m.data[8] * _m.data[15] - _m.data[11] * _m.data[12];
	double b3 = _m.data[9] * _m.data[14] - _m.data[10] * _m.data[13];
	double b4 = _m.data[9] * _m.data[15] - _m.data[11] * _m.data[13];
	double b5 = _m.data[10] * _m.data[15] - _m.data[11] * _m.data[14];

	det = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;
	if (G_COMPARISON_STANDARD_D(det, 0))
	{
		return FAILURE;
	}
	_outMatrix.data[0] = _m.data[5] * b5 - _m.data[6] * b4 + _m.data[7] * b3;
	_outMatrix.data[1] = -_m.data[1] * b5 + _m.data[2] * b4 - _m.data[3] * b3;
	_outMatrix.data[2] = _m.data[13] * a5 - _m.data[14] * a4 + _m.data[15] * a3;
	_outMatrix.data[3] = -_m.data[9] * a5 + _m.data[10] * a4 - _m.data[11] * a3;

	_outMatrix.data[4] = -_m.data[4] * b5 + _m.data[6] * b2 - _m.data[7] * b1;
	_outMatrix.data[5] = _m.data[0] * b5 - _m.data[2] * b2 + _m.data[3] * b1;
	_outMatrix.data[6] = -_m.data[12] * a5 + _m.data[14] * a2 - _m.data[15] * a1;
	_outMatrix.data[7] = _m.data[8] * a5 - _m.data[10] * a2 + _m.data[11] * a1;

	_outMatrix.data[8] = _m.data[4] * b4 - _m.data[5] * b2 + _m.data[7] * b0;
	_outMatrix.data[9] = -_m.data[0] * b4 + _m.data[1] * b2 - _m.data[3] * b0;
	_outMatrix.data[10] = _m.data[12] * a4 - _m.data[13] * a2 + _m.data[15] * a0;
	_outMatrix.data[11] = -_m.data[8] * a4 + _m.data[9] * a2 - _m.data[11] * a0;

	_outMatrix.data[12] = -_m.data[4] * b3 + _m.data[5] * b1 - _m.data[6] * b0;
	_outMatrix.data[13] = _m.data[0] * b3 - _m.data[1] * b1 + _m.data[2] * b0;
	_outMatrix.data[14] = -_m.data[12] * a3 + _m.data[13] * a1 - _m.data[14] * a0;
	_outMatrix.data[15] = _m.data[8] * a3 - _m.data[9] * a1 + _m.data[10] * a0;

	MultiplyNumD(_outMatrix, 1.0f / det, _outMatrix);

	return SUCCESS;
}

GReturn GMatirxCpp::IdentityD(GMATRIXD & _outMatrix)
{
	_outMatrix = GIdentityMatrixD;
	return SUCCESS;
}

GReturn GMatirxCpp::GetRotationD(GMATRIXD _matrix, GQUATERNIOND& _outQuaternion)
{
	double det;
	double sx = sqrt(_matrix.data[0] * _matrix.data[0] + _matrix.data[4] * _matrix.data[4] + _matrix.data[8] * _matrix.data[8]);
	double sy = sqrt(_matrix.data[1] * _matrix.data[1] + _matrix.data[5] * _matrix.data[5] + _matrix.data[9] * _matrix.data[9]);
	double sz = sqrt(_matrix.data[2] * _matrix.data[2] + _matrix.data[6] * _matrix.data[6] + _matrix.data[10] * _matrix.data[10]);
	DeterminantD(_matrix, det);
	if (G_COMPARISON_STANDARD_D(det, 0))
	{
		return FAILURE;
	}
	if (det < 0)
	{
		sx = -sx;
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

	double trace = Rotation.data[0] + Rotation.data[5] + Rotation.data[10] + 1;

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
			double s = 0.5f / sqrt(1.0 + Rotation.row1.x - Rotation.row2.y - Rotation.row3.z);
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

GReturn GMatirxCpp::GetTranslationD(GMATRIXD _matrix, GVECTORD& _outVector)
{
	_outVector.x = _matrix.data[3];
	_outVector.y = _matrix.data[7];
	_outVector.z = _matrix.data[11];
	_outVector.w = 0;
	return SUCCESS;
}

GReturn GMatirxCpp::GetScaleD(GMATRIXD _matrix, GVECTORD& _outVector)
{
	double det;

	_outVector.x = sqrt(_matrix.data[0] * _matrix.data[0] + _matrix.data[4] * _matrix.data[4] + _matrix.data[8] * _matrix.data[8]);
	_outVector.y = sqrt(_matrix.data[1] * _matrix.data[1] + _matrix.data[5] * _matrix.data[5] + _matrix.data[9] * _matrix.data[9]);
	_outVector.z = sqrt(_matrix.data[2] * _matrix.data[2] + _matrix.data[6] * _matrix.data[6] + _matrix.data[10] * _matrix.data[10]);
	_outVector.w = 0;

	DeterminantD(_matrix, det);
	if (det < 0)
	{
		_outVector.x = -_outVector.x;
	}

	return SUCCESS;
}

GReturn GMatirxCpp::RotationXD(GMATRIXD _matrix, double _radian, GMATRIXD & _outMatrix)
{
	double c = cos(_radian);
	double s = sin(_radian);
	GMATRIXD Rotation = GIdentityMatrixD;
	Rotation.data[5] = c;
	Rotation.data[6] = s;
	Rotation.data[9] = -s;
	Rotation.data[10] = c;

	MultiplyMatrixD(_matrix, Rotation, _outMatrix);

	return SUCCESS;
}

GReturn GMatirxCpp::RotationYD(GMATRIXD _matrix, double _radian, GMATRIXD & _outMatrix)
{
	double c = cos(_radian);
	double s = sin(_radian);
	GMATRIXD Rotation = GIdentityMatrixD;
	Rotation.data[0] = c;
	Rotation.data[2] = -s;
	Rotation.data[8] = s;
	Rotation.data[10] = c;

	MultiplyMatrixD(_matrix, Rotation, _outMatrix);
	return SUCCESS;
}

GReturn GMatirxCpp::RotationZD(GMATRIXD _matrix, double _radian, GMATRIXD & _outMatrix)
{
	double c = cos(_radian);
	double s = sin(_radian);
	GMATRIXD Rotation = GIdentityMatrixD;
	Rotation.data[0] = c;
	Rotation.data[1] = s;
	Rotation.data[4] = -s;
	Rotation.data[5] = c;

	MultiplyMatrixD(_matrix, Rotation, _outMatrix);
	return SUCCESS;
}

GReturn GMatirxCpp::RotationYawPitchRollD(double _yaw, double _pitch, double _roll, GMATRIXD & _outMatrix)
{
	_outMatrix = GIdentityMatrixD;
	GMATRIXD RotationX;
	GMATRIXD RotationY;
	GMATRIXD RotationZ;
	GMATRIXD reuslt;
	RotationZD(GIdentityMatrixD, _roll, RotationZ);
	RotationXD(GIdentityMatrixD, _pitch, RotationX);
	RotationYD(GIdentityMatrixD, _yaw, RotationY);

	MultiplyMatrixD(RotationX, RotationY, reuslt);
	MultiplyMatrixD(RotationZ, reuslt, _outMatrix);

	return SUCCESS;
}

GReturn GMatirxCpp::RotationByVectorD(GVECTORD _vector, double _radian, GMATRIXD & _outMatrix)
{
	double x = _vector.x;
	double y = _vector.y;
	double z = _vector.z;

	double magnitude = x * x + y * y + z * z;
	if (!G_COMPARISON_STANDARD_D(magnitude, 1))
	{
		magnitude = sqrt(magnitude);
		if (!G_COMPARISON_STANDARD_D(magnitude, 0))
		{
			magnitude = 1.0 / magnitude;
			x = x * magnitude;
			y = y * magnitude;
			z = z * magnitude;
		}
		else return FAILURE;
	}
	double c = cos(_radian);
	double s = sin(_radian);

	double t = 1.0f - c;
	double tx = t * x;
	double ty = t * y;
	double tz = t * z;
	double txy = tx * y;
	double txz = tx * z;
	double tyz = ty * z;
	double sx = s * x;
	double sy = s * y;
	double sz = s * z;

	_outMatrix.data[0] = c + tx*x;
	_outMatrix.data[1] = txy + sz;
	_outMatrix.data[2] = txz - sy;
	_outMatrix.data[3] = 0.0f;

	_outMatrix.data[4] = txy - sz;
	_outMatrix.data[5] = c + ty*y;
	_outMatrix.data[6] = tyz + sx;
	_outMatrix.data[7] = 0.0f;

	_outMatrix.data[8] = txz + sy;
	_outMatrix.data[9] = tyz - sx;
	_outMatrix.data[10] = c + tz*z;
	_outMatrix.data[11] = 0.0f;

	_outMatrix.data[12] = 0.0f;
	_outMatrix.data[13] = 0.0f;
	_outMatrix.data[14] = 0.0f;
	_outMatrix.data[15] = 1.0f;

	return SUCCESS;
}

GReturn GMatirxCpp::TranslatelocalD(GMATRIXD _matrix, GVECTORD _vector, GMATRIXD & _outMatrix)
{
	// *BUG FIX* for some reason this was column major & global?
	GMATRIXD Translation = GIdentityMatrixD;
	Translation.data[12] = _vector.x;
	Translation.data[13] = _vector.y;
	Translation.data[14] = _vector.z;
	// order switched
	MultiplyMatrixD(Translation, _matrix, _outMatrix);

	return SUCCESS;
}

GReturn GMatirxCpp::ScalingD(GMATRIXD _matrix, GVECTORD _vector, GMATRIXD & _outMatrix)
{
	GMATRIXD Scale = GIdentityMatrixD;
	Scale.data[0] = _vector.x;
	Scale.data[5] = _vector.y;
	Scale.data[10] = _vector.z;

	MultiplyMatrixD(_matrix, Scale, _outMatrix);

	return SUCCESS;
}

GReturn GMatirxCpp::LerpD(GMATRIXD _matrix1, GMATRIXD _matrix2, double _ratio, GMATRIXD & _outMatrix)
{
	GMATRIXD _m1 = _matrix1;
	GMATRIXD _m2 = _matrix2;
	for (int i = 0; i < 16; i++)
	{
		_outMatrix.data[i] = G_LERP(_m1.data[i], _m2.data[i], _ratio);
	}
	return SUCCESS;
}

GReturn GMatirxCpp::ProjectionLHD(double _fovY, double _aspect, double _zn, double _zf, GMATRIXD & _outMatrix)
{
	if (G_COMPARISON_STANDARD_D(_aspect, 0)) return FAILURE;

	_outMatrix = GZeroMatrixD;
	_outMatrix.row2.data[1] = 1 / tan(_fovY / 2);
	_outMatrix.row1.data[0] = _outMatrix.row2.data[1] / _aspect;
	_outMatrix.row3.data[2] = _zf / (_zf - _zn);
	_outMatrix.row3.data[3] = 1;
	_outMatrix.row4.data[2] = -_zn * _zf / (_zf - _zn);

	return SUCCESS;
}

GReturn GMatirxCpp::LookAtLHD(GVECTORD _eye, GVECTORD _at, GVECTORD _up, GMATRIXD & _outMatrix)
{
	_outMatrix = GIdentityMatrixD;
	GVECTORD temp;
	GVECTORD camDir;	  //zaxis
	GVECTORD camRight;	  //xaxis
	GVECTORD camUp;		  //yaxis
	double magnitudeX;
	double magnitudeZ;

	camDir.x = _at.x - _eye.x;
	camDir.y = _at.y - _eye.y;
	camDir.z = _at.z - _eye.z;

	magnitudeZ = sqrt((camDir.x * camDir.x) + (camDir.y * camDir.y) + (camDir.z * camDir.z));
	if (G_COMPARISON_STANDARD_F(magnitudeZ, 0)) return FAILURE;

	camDir.x /= magnitudeZ;
	camDir.y /= magnitudeZ;
	camDir.z /= magnitudeZ;


	camRight.x = (_up.y * camDir.z) - (_up.z * camDir.y);
	camRight.y = (_up.z * camDir.x) - (_up.x * camDir.z);
	camRight.z = (_up.x * camDir.y) - (_up.y * camDir.x);

	magnitudeX = sqrt((camRight.x * camRight.x) + (camRight.y * camRight.y) + (camRight.z * camRight.z));
	if (G_COMPARISON_STANDARD_F(magnitudeX, 0)) return FAILURE;


	camRight.x /= magnitudeX;
	camRight.y /= magnitudeX;
	camRight.z /= magnitudeX;


	camUp.x = (camDir.y * camRight.z) - (camDir.z * camRight.y);
	camUp.y = (camDir.z * camRight.x) - (camDir.x * camRight.z);
	camUp.z = (camDir.x * camRight.y) - (camDir.y * camRight.x);

	temp.x = camRight.x * _eye.x + camRight.y * _eye.y + camRight.z * _eye.z;
	temp.y = camUp.x * _eye.x + camUp.y * _eye.y + camUp.z * _eye.z;
	temp.z = camDir.x * _eye.x + camDir.y * _eye.y + camDir.z * _eye.z;

	_outMatrix.data[0] = camRight.x;
	_outMatrix.data[4] = camRight.y;
	_outMatrix.data[8] = camRight.z;
	_outMatrix.data[12] = -temp.x;

	_outMatrix.data[1] = camUp.x;
	_outMatrix.data[5] = camUp.y;
	_outMatrix.data[9] = camUp.z;
	_outMatrix.data[13] = -temp.y;

	_outMatrix.data[2] = camDir.x;
	_outMatrix.data[6] = camDir.y;
	_outMatrix.data[10] = camDir.z;
	_outMatrix.data[14] = -temp.z;

	return SUCCESS;
}

GReturn GMatirxCpp::GetCount(unsigned int & _outCount)
{
	//Store reference count.
	_outCount = refCount;

	return GW::SUCCESS;
}

GReturn GMatirxCpp::IncrementCount()
{
	//Check to make sure overflow will not occur.
	if (refCount == G_UINT_MAX)
		return GW::FAILURE;

	//Increment reference count.
	++refCount;

	return GW::SUCCESS;
}

GReturn GMatirxCpp::DecrementCount()
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

GReturn GMatirxCpp::RequestInterface(const GW::GUUIID & _interfaceID, void ** _outputInterface)
{
	if (_outputInterface == nullptr)
		return GW::INVALID_ARGUMENT;

	//If passed in interface is equivalent to current interface (this).
	if (_interfaceID == GW::MATH::GMatrixUUIID)
	{
		//Temporary GFile* to ensure proper functions are called.
		GMatrix* convert = reinterpret_cast<GMatrix*>(this);

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

GATEWARE_EXPORT_EXPLICIT GReturn CreateGMatrix(GMatrix** _outMatrix)
{
	return GW::MATH::CreateGMatrix(_outMatrix);
}

GReturn GW::MATH::CreateGMatrix(GMatrix** _outMatrix)
{
	if (_outMatrix == nullptr)
	{
		return INVALID_ARGUMENT;
	}
	GMatirxCpp* mat = new GMatirxCpp();

	if (mat == nullptr)
	{
		return FAILURE;
	}

	(*_outMatrix) = mat;

	return SUCCESS;
}

