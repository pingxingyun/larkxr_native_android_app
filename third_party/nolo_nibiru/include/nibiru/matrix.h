// This proprietary software may be used only as
// authorised by a licensing agreement from ARM Limited
// (C) COPYRIGHT 2015 ARM Limited
// ALL RIGHTS RESERVED
// The entire notice above must be reproduced on all authorised
// copies and copies may only be made to the extent permitted
// by a licensing agreement from ARM Limited.
#ifndef MATRIX_H
#define MATRIX_H
#ifndef PI
#define PI 3.141592653f
#endif
#include <math.h>
#include <stdlib.h>
struct vec2 {
	float x;
	float y;

	vec2() :
			x(0.0f), y(0.0f) {
	}
	vec2(float X, float Y) :
			x(X), y(Y) {
	}
	explicit vec2(float S) :
			x(S), y(S) {
	}
	vec2 operator +(const vec2 &rhs) const {
		return vec2(x + rhs.x, y + rhs.y);
	}
	vec2 operator *(const vec2 &rhs) const {
		return vec2(x * rhs.x, y * rhs.y);
	}
	vec2 operator -(const vec2 &rhs) const {
		return vec2(x - rhs.x, y - rhs.y);
	}
	vec2 operator *(const float s) const {
		return vec2(x * s, y * s);
	}
	vec2 operator /(const float s) const {
		return vec2(x / s, y / s);
	}

	vec2 &operator *=(const float s) {
		*this = *this * s;
		return *this;
	}
	vec2 &operator +=(const vec2 &rhs) {
		*this = *this + rhs;
		return *this;
	}
	vec2 &operator *=(const vec2 &rhs) {
		*this = *this * rhs;
		return *this;
	}
	vec2 &operator -=(const vec2 &rhs) {
		*this = *this - rhs;
		return *this;
	}

	float &operator [](unsigned int i) {
		return (&x)[i];
	}
	const float &operator [](unsigned int i) const {
		return (&x)[i];
	}
};

struct vec3 {
	float x;
	float y;
	float z;

//<<<<<<< .mine
	vec3() :
			x(0.0f), y(0.0f), z(0.0f) {
	}
	vec3(float X, float Y, float Z) :
			x(X), y(Y), z(Z) {
	}
	vec3(const vec3& rhs) {
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
	}
	explicit vec3(float S) :
			x(S), y(S), z(S) {
	}
	vec3 operator -() const {
		return vec3(-x, -y, -z);
	}
	vec3 operator +(const vec3 &rhs) const {
		return vec3(x + rhs.x, y + rhs.y, z + rhs.z);
	}
	vec3 operator *(const vec3 &rhs) const {
		return vec3(x * rhs.x, y * rhs.y, z * rhs.z);
	}
	vec3 operator -(const vec3 &rhs) const {
		return vec3(x - rhs.x, y - rhs.y, z - rhs.z);
	}
	vec3 operator *(const float s) const {
		return vec3(x * s, y * s, z * s);
	}
	vec3 operator /(const float s) const {
		return vec3(x / s, y / s, z / s);
	}
//=======
//    vec3() : x(0.0f), y(0.0f), z(0.0f) { }
//    vec3(float X, float Y, float Z) : x(X), y(Y), z(Z) { }
//    explicit vec3(float S) : x(S), y(S), z(S) { }
//    vec3 operator - () const { return vec3(-x, -y, -z); }
//    vec3 operator + (const vec3 &rhs) const { return vec3(x + rhs.x, y + rhs.y, z + rhs.z); }
//    vec3 operator * (const vec3 &rhs) const { return vec3(x * rhs.x, y * rhs.y, z * rhs.z); }
//    vec3 operator - (const vec3 &rhs) const { return vec3(x - rhs.x, y - rhs.y, z - rhs.z); }
//    vec3 operator * (const float s)  const  { return vec3(x * s, y * s, z * s); }
//    vec3 operator / (const float s)  const  { return vec3(x / s, y / s, z / s); }
//>>>>>>> .r17378

	vec3 &operator +=(const vec3 &rhs) {
		*this = *this + rhs;
		return *this;
	}
	vec3 &operator *=(const vec3 &rhs) {
		*this = *this * rhs;
		return *this;
	}
	vec3 &operator -=(const vec3 &rhs) {
		*this = *this - rhs;
		return *this;
	}

//<<<<<<< .mine
	vec3 cross_product(const vec3& rhs) {
		return vec3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z,
				x * rhs.y - y * rhs.x);
	}

	float &operator [](unsigned int i) {
		return (&x)[i];
	}
	const float &operator [](unsigned int i) const {
		return (&x)[i];
	}
//=======
//    float &operator [] (unsigned int i)             { return (&x)[i]; }
//    const float &operator [] (unsigned int i) const { return (&x)[i]; }
//>>>>>>> .r17378

	float length() {
		return sqrt(x * x + y * y + z * z);
	}

	/**
	 * \brief Normalize 3D floating point vector.
	 */
	void normalize(void) {
		float length = sqrt(x * x + y * y + z * z);

		x /= length;
		y /= length;
		z /= length;
	}

	static vec3 cross(const vec3& vector1, const vec3& vector2) {
		/* Floating point vector to be returned. */
		vec3 crossProduct;

		crossProduct.x = (vector1.y * vector2.z) - (vector1.z * vector2.y);
		crossProduct.y = (vector1.z * vector2.x) - (vector1.x * vector2.z);
		crossProduct.z = (vector1.x * vector2.y) - (vector1.y * vector2.x);

		return crossProduct;
	}
};

struct vec4 {
	float x;
	float y;
	float z;
	float w;

	vec4() :
			x(0.0f), y(0.0f), z(0.0f), w(0.0f) {
	}
	vec4(vec3 V, float W) :
			x(V.x), y(V.y), z(V.z), w(W) {
	}
	vec4(float X, float Y, float Z, float W) :
			x(X), y(Y), z(Z), w(W) {
	}
	explicit vec4(float S) :
			x(S), y(S), z(S), w(S) {
	}
	vec4 operator -() const {
		return vec4(-x, -y, -z, -w);
	}
	vec4 operator +(const vec4 &rhs) const {
		return vec4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
	}
	vec4 operator *(const vec4 &rhs) const {
		return vec4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w);
	}
	vec4 operator -(const vec4 &rhs) const {
		return vec4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
	}
	vec4 operator *(const float s) const {
		return vec4(x * s, y * s, z * s, w * s);
	}
	vec4 operator /(const float s) const {
		return vec4(x / s, y / s, z / s, w / s);
	}

	vec4 &operator *=(const float s) {
		*this = *this * s;
		return *this;
	}
	vec4 &operator +=(const vec4 &rhs) {
		*this = *this + rhs;
		return *this;
	}
	vec4 &operator *=(const vec4 &rhs) {
		*this = *this * rhs;
		return *this;
	}
	vec4 &operator -=(const vec4 &rhs) {
		*this = *this - rhs;
		return *this;
	}

	float &operator [](unsigned int i) {
		return (&x)[i];
	}
	const float &operator [](unsigned int i) const {
		return (&x)[i];
	}
    void value_ptr(float *res, int length) {
    	if(length < 4) return;
    	res[0] = x;
    	res[1] = y;
    	res[2] = z;
    	res[3] = w;
    }

	vec3 xyz() const {
		return vec3(x, y, z);
	}
};

static vec3 normalize(const vec3 &v);
static vec3 cross(vec3 lhs, vec3 rhs);
static float length(float x, float y, float z);

struct mat4 {
	vec4 x, y, z, w; // columns

	mat4() {
	}
	explicit mat4(float s) :
			x(0.0f), y(0.0f), z(0.0f), w(0.0f) {
		x.x = s;
		y.y = s;
		z.z = s;
		w.w = s;
	}

	//PASS TEST
	mat4 operator *(const mat4 &rhs) {
		mat4 m;
		for (int lrow = 0; lrow < 4; ++lrow) {
			for (int rcol = 0; rcol < 4; ++rcol) {
				m[rcol][lrow] = 0.0f;
				for (int k = 0; k < 4; ++k) {
					m[rcol][lrow] += (*this)[k][lrow] * rhs[rcol][k];
				}
			}
		}
		return m;
	}

	mat4 operator *(const float s) {
		mat4 m = *this;
		m.x *= s;
		m.y *= s;
		m.z *= s;
		m.w *= s;
		return m;
	}

	vec4 operator *(const vec4 &rhs) {
		return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
	}

	vec4 &operator [](unsigned int i) {
		return (&x)[i];
	}
	const vec4 &operator [](unsigned int i) const {
		return (&x)[i];
	}
	const float *value_ptr() const {
		return &(x[0]);
	}
	float *value_ptr() {
		return &(x[0]);
	}

	/*float *array_ptr() {
		float* value = new float[16];

		for (int lrow = 0; lrow < 4; ++lrow) {
			for (int rcol = 0; rcol < 4; ++rcol) {
				value[lrow * 4 + rcol] = (*this)[lrow][rcol];
			}
		}

		return value;

	}*/

	void setValue(int index, float value) {
		int row = index / 4;
		int col = index % 4;

		(*this)[row][col] = value;
	}

	float getValue(int index) {
		int row = index / 4;
		int col = index % 4;

		return (*this)[row][col];
	}

	//pass test
	void setIdentity(int offset) {
		for (int i = 0; i < 16; i++) {
			setValue(offset + i, 0);
//           sm[smOffset + i] = 0;
		}
		for (int i = 0; i < 16; i += 5) {
			setValue(offset + i, 1.0f);
//           sm[smOffset + i] = 1.0f;
		}
	}

	mat4& post_translate(float tx, float ty, float tz) {
		float value = getValue(12);
		value += (tx * getValue(0)) + (ty * getValue(4)) + (tz * getValue(8));
		setValue(12, value);

		value = getValue(13);
		value += (tx * getValue(1)) + (ty * getValue(5)) + (tz * getValue(9));
		setValue(13, value);

		value = getValue(14);
		value += (tx * getValue(2)) + (ty * getValue(6)) + (tz * getValue(10));
		setValue(14, value);

		value = getValue(15);
		value += (tx * getValue(3)) + (ty * getValue(7)) + (tz * getValue(11));
		setValue(15, value);

//        this[12] += (tx * f_[0]) + (ty * f_[4]) + (tz * f_[8]);
//        f_[13] += (tx * f_[1]) + (ty * f_[5]) + (tz * f_[9]);
//        f_[14] += (tx * f_[2]) + (ty * f_[6]) + (tz * f_[10]);
//        f_[15] += (tx * f_[3]) + (ty * f_[7]) + (tz * f_[11]);
		return *this;
	}

//    void lookAt( const vec3& vec_eye, const vec3& vec_at, const vec3& vec_up )
//    {
//        vec3 vec_forward, vec_up_norm, vec_side;
//
//        vec_forward.x = vec_eye.x - vec_at.x;
//        vec_forward.y = vec_eye.y - vec_at.y;
//        vec_forward.z = vec_eye.z - vec_at.z;
//
//    //    vec_forward.Normalize();
//        normalize(vec_forward);
//        vec_up_norm = vec_up;
//    //    vec_up_norm.Normalize();
//        normalize(vec_up_norm);
//
//        vec_side = cross(vec_up_norm, vec_forward );
//        vec_up_norm = cross(vec_forward, vec_side );
//
//        setValue(0, vec_side.x);
//        setValue(4, vec_side.y);
//        setValue(8, vec_side.z);
//
//        setValue(12, 0);
//
//        setValue(1, vec_up_norm.x);
//        setValue(5, vec_up_norm.y);
//        setValue(9, vec_up_norm.z);
//
//        setValue(13, 0);
//
//        setValue(2, vec_forward.x);
//        setValue(6, vec_forward.y);
//        setValue(10, vec_forward.z);
//
//        setValue(14, 0);
//        setValue(3, 0);
//        setValue(7, 0);
//        setValue(11, 0);
//        setValue(15, 1.0);
//
//    //    result[0] = vec_side.x;
//    //    result[4] = vec_side.y;
//    //    result[8] = vec_side.z;
//    //
//    //    result[12] = 0;
//    //
//    //    result[1] = vec_up_norm.x;
//    //    result[5] = vec_up_norm.y;
//    //    result[9] = vec_up_norm.z;
//    //
//    //    result[13] = 0;
//    //
//    //    result[2] = vec_forward.x;
//    //    result[6] = vec_forward.y;
//    //    result[10] = vec_forward.z;
//    //
//    //    result[14] = 0;
//    //    result[3] = 0;
//    //    result[7] = 0;
//    //    result[11] = 0;
//    //    result[15] = 1.0;
//
//        post_translate( -vec_eye.x, -vec_eye.y, -vec_eye.z );
//
//    }

	//pass test
	void translate(int offset, float x, float y, float z) {
		for (int i = 0; i < 4; i++) {
			int mi = offset + i;
			float value = getValue(12 + mi);
			value += getValue(mi) * x + getValue(4 + mi) * y
					+ getValue(8 + mi) * z;

			setValue(12 + mi, value);
//            m[12 + mi] += m[mi] * x + m[4 + mi] * y + m[8 + mi] * z;
		}
	}

	void setValue(float *value) {
		for (int i = 0; i < 16; i++) {
			setValue(i, value[i]);
		}
	}

	//pass test
	void setRotate(int rmOffset, float a, float x, float y, float z) {
//	setValue(rmOffset + 3, 0);
//	setValue(rmOffset + 7, 0);
//	setValue(rmOffset + 11, 0);
//	setValue(rmOffset + 12, 0);
//	setValue(rmOffset + 13, 0);
//	setValue(rmOffset + 14, 0);
//	setValue(rmOffset + 15, 1);

//    rm[rmOffset + 3] = 0;
//    rm[rmOffset + 7] = 0;
//    rm[rmOffset + 11]= 0;
//    rm[rmOffset + 12]= 0;
//    rm[rmOffset + 13]= 0;
//    rm[rmOffset + 14]= 0;
//    rm[rmOffset + 15]= 1;
		a *= (float) (PI / 180.0f);
		float s = sinf(a);
		float c = cosf(a);
		if (1.0f == x && 0.0f == y && 0.0f == z) {
			setValue(rmOffset + 5, c);
			setValue(rmOffset + 10, c);
			setValue(rmOffset + 6, s);
			setValue(rmOffset + 9, -s);
			setValue(rmOffset + 1, 0);
			setValue(rmOffset + 2, 0);
			setValue(rmOffset + 4, 0);
			setValue(rmOffset + 8, 0);
			setValue(rmOffset + 0, 1);
		} else if (0.0f == x && 1.0f == y && 0.0f == z) {
			setValue(rmOffset + 0, c);
			setValue(rmOffset + 10, c);
			setValue(rmOffset + 8, s);
			setValue(rmOffset + 2, -s);
			setValue(rmOffset + 1, 0);
			setValue(rmOffset + 4, 0);
			setValue(rmOffset + 6, 0);
			setValue(rmOffset + 9, 0);
			setValue(rmOffset + 5, 1);
		} else if (0.0f == x && 0.0f == y && 1.0f == z) {
			setValue(rmOffset + 0, c);
			setValue(rmOffset + 5, c);
			setValue(rmOffset + 1, s);
			setValue(rmOffset + 4, -s);
			setValue(rmOffset + 2, 0);
			setValue(rmOffset + 6, 0);
			setValue(rmOffset + 8, 0);
			setValue(rmOffset + 9, 0);
			setValue(rmOffset + 10, 1);
		} else {
			float len = length(x, y, z);
			if (1.0f != len) {
				float recipLen = 1.0f / len;
				x *= recipLen;
				y *= recipLen;
				z *= recipLen;
			}
			float nc = 1.0f - c;
			float xy = x * y;
			float yz = y * z;
			float zx = z * x;
			float xs = x * s;
			float ys = y * s;
			float zs = z * s;
			setValue(rmOffset + 0, x * x * nc + c);
			setValue(rmOffset + 4, xy * nc - zs);
			setValue(rmOffset + 8, zx * nc + ys);
			setValue(rmOffset + 1, xy * nc + zs);
			setValue(rmOffset + 5, y * y * nc + c);
			setValue(rmOffset + 9, yz * nc - xs);
			setValue(rmOffset + 2, zx * nc - ys);
			setValue(rmOffset + 6, yz * nc + xs);
			setValue(rmOffset + 10, z * z * nc + c);
		}
	}

	bool invertM(mat4* res) {
		// Invert a 4 x 4 Matrix4 using Cramer's Rule

		float* m = value_ptr();
		// transpose Matrix4
		float src0 = m[0];
		float src4 = m[1];
		float src8 = m[2];
		float src12 = m[3];

		float src1 = m[4];
		float src5 = m[5];
		float src9 = m[6];
		float src13 = m[7];

		float src2 = m[8];
		float src6 = m[9];
		float src10 = m[10];
		float src14 = m[11];

		float src3 = m[12];
		float src7 = m[13];
		float src11 = m[14];
		float src15 = m[15];

		// calculate pairs for first 8 elements (cofactors)
		float atmp0 = src10 * src15;
		float atmp1 = src11 * src14;
		float atmp2 = src9 * src15;
		float atmp3 = src11 * src13;
		float atmp4 = src9 * src14;
		float atmp5 = src10 * src13;
		float atmp6 = src8 * src15;
		float atmp7 = src11 * src12;
		float atmp8 = src8 * src14;
		float atmp9 = src10 * src12;
		float atmp10 = src8 * src13;
		float atmp11 = src9 * src12;

		// calculate first 8 elements (cofactors)
		float dst0 = (atmp0 * src5 + atmp3 * src6 + atmp4 * src7)
				- (atmp1 * src5 + atmp2 * src6 + atmp5 * src7);
		float dst1 = (atmp1 * src4 + atmp6 * src6 + atmp9 * src7)
				- (atmp0 * src4 + atmp7 * src6 + atmp8 * src7);
		float dst2 = (atmp2 * src4 + atmp7 * src5 + atmp10 * src7)
				- (atmp3 * src4 + atmp6 * src5 + atmp11 * src7);
		float dst3 = (atmp5 * src4 + atmp8 * src5 + atmp11 * src6)
				- (atmp4 * src4 + atmp9 * src5 + atmp10 * src6);
		float dst4 = (atmp1 * src1 + atmp2 * src2 + atmp5 * src3)
				- (atmp0 * src1 + atmp3 * src2 + atmp4 * src3);
		float dst5 = (atmp0 * src0 + atmp7 * src2 + atmp8 * src3)
				- (atmp1 * src0 + atmp6 * src2 + atmp9 * src3);
		float dst6 = (atmp3 * src0 + atmp6 * src1 + atmp11 * src3)
				- (atmp2 * src0 + atmp7 * src1 + atmp10 * src3);
		float dst7 = (atmp4 * src0 + atmp9 * src1 + atmp10 * src2)
				- (atmp5 * src0 + atmp8 * src1 + atmp11 * src2);

		// calculate pairs for second 8 elements (cofactors)
		float btmp0 = src2 * src7;
		float btmp1 = src3 * src6;
		float btmp2 = src1 * src7;
		float btmp3 = src3 * src5;
		float btmp4 = src1 * src6;
		float btmp5 = src2 * src5;
		float btmp6 = src0 * src7;
		float btmp7 = src3 * src4;
		float btmp8 = src0 * src6;
		float btmp9 = src2 * src4;
		float btmp10 = src0 * src5;
		float btmp11 = src1 * src4;

		// calculate second 8 elements (cofactors)
		float dst8 = (btmp0 * src13 + btmp3 * src14 + btmp4 * src15)
				- (btmp1 * src13 + btmp2 * src14 + btmp5 * src15);
		float dst9 = (btmp1 * src12 + btmp6 * src14 + btmp9 * src15)
				- (btmp0 * src12 + btmp7 * src14 + btmp8 * src15);
		float dst10 = (btmp2 * src12 + btmp7 * src13 + btmp10 * src15)
				- (btmp3 * src12 + btmp6 * src13 + btmp11 * src15);
		float dst11 = (btmp5 * src12 + btmp8 * src13 + btmp11 * src14)
				- (btmp4 * src12 + btmp9 * src13 + btmp10 * src14);
		float dst12 = (btmp2 * src10 + btmp5 * src11 + btmp1 * src9)
				- (btmp4 * src11 + btmp0 * src9 + btmp3 * src10);
		float dst13 = (btmp8 * src11 + btmp0 * src8 + btmp7 * src10)
				- (btmp6 * src10 + btmp9 * src11 + btmp1 * src8);
		float dst14 = (btmp6 * src9 + btmp11 * src11 + btmp3 * src8)
				- (btmp10 * src11 + btmp2 * src8 + btmp7 * src9);
		float dst15 = (btmp10 * src10 + btmp4 * src8 + btmp9 * src9)
				- (btmp8 * src9 + btmp11 * src10 + btmp5 * src8);

		// calculate determinant
		float det = src0 * dst0 + src1 * dst1 + src2 * dst2 + src3 * dst3;

		if (det == 0.0f) {
			return false;
		}

// calculate Matrix4 inverse
		float mInv[16];
		float invdet = 1.0f / det;
		mInv[0] = dst0 * invdet;
		mInv[1] = dst1 * invdet;
		mInv[2] = dst2 * invdet;
		mInv[3] = dst3 * invdet;

		mInv[4] = dst4 * invdet;
		mInv[5] = dst5 * invdet;
		mInv[6] = dst6 * invdet;
		mInv[7] = dst7 * invdet;

		mInv[8] = dst8 * invdet;
		mInv[9] = dst9 * invdet;
		mInv[10] = dst10 * invdet;
		mInv[11] = dst11 * invdet;

		mInv[12] = dst12 * invdet;
		mInv[13] = dst13 * invdet;
		mInv[14] = dst14 * invdet;
		mInv[15] = dst15 * invdet;

		res->setValue(mInv);

		return true;
	}

	//PASS TEST
	void setLookAtM(int rmOffset, float eyeX, float eyeY, float eyeZ,
			float centerX, float centerY, float centerZ, float upX, float upY,
			float upZ) {

		// See the OpenGL GLUT documentation for gluLookAt for a description
		// of the algorithm. We implement it in a straightforward way:

		float fx = centerX - eyeX;
		float fy = centerY - eyeY;
		float fz = centerZ - eyeZ;

		// Normalize f
		float rlf = 1.0f / length(fx, fy, fz);
		fx *= rlf;
		fy *= rlf;
		fz *= rlf;

		// compute s = f x up (x means "cross product")
		float sx = fy * upZ - fz * upY;
		float sy = fz * upX - fx * upZ;
		float sz = fx * upY - fy * upX;

		// and normalize s
		float rls = 1.0f / length(sx, sy, sz);
		sx *= rls;
		sy *= rls;
		sz *= rls;

		// compute u = s x f
		float ux = sy * fz - sz * fy;
		float uy = sz * fx - sx * fz;
		float uz = sx * fy - sy * fx;

		setValue(rmOffset + 0, sx);
		setValue(rmOffset + 1, ux);
		setValue(rmOffset + 2, -fx);
		setValue(rmOffset + 3, 0.0f);

		setValue(rmOffset + 4, sy);
		setValue(rmOffset + 5, uy);
		setValue(rmOffset + 6, -fy);
		setValue(rmOffset + 7, 0.0f);

		setValue(rmOffset + 8, sz);
		setValue(rmOffset + 9, uz);
		setValue(rmOffset + 10, -fz);
		setValue(rmOffset + 11, 0.0f);

		setValue(rmOffset + 12, 0.0f);
		setValue(rmOffset + 13, 0.0f);
		setValue(rmOffset + 14, 0.0f);
		setValue(rmOffset + 15, 1.0f);

		translate(rmOffset, -eyeX, -eyeY, -eyeZ);
	}

	void frustumM(int offset, float left, float right, float bottom, float top,
			float near, float far) {
		if (left == right) {
			//        LOGE("error left == right");
			return;
		}
		if (top == bottom) {
			//    	LOGE("error top == bottom");
			return;
		}
		if (near == far) {
			//    	LOGE("error near == far");
			return;
		}
		if (near <= 0.0f) {
			//    	LOGE("error near <= 0.0f");

			return;
		}
		if (far <= 0.0f) {
			//    	LOGE("error far <= 0.0f");

			return;
		}
		float r_width = 1.0f / (right - left);
		float r_height = 1.0f / (top - bottom);
		float r_depth = 1.0f / (near - far);
		float x = 2.0f * (near * r_width);
		float y = 2.0f * (near * r_height);
		float A = (right + left) * r_width;
		float B = (top + bottom) * r_height;
		float C = (far + near) * r_depth;
		float D = 2.0f * (far * near * r_depth);
		setValue(offset + 0, x);
		setValue(offset + 5, y);
		setValue(offset + 8, A);
		setValue(offset + 9, B);
		setValue(offset + 10, C);
		setValue(offset + 14, D);
		setValue(offset + 11, -1.0f);
		setValue(offset + 1, 0.0f);
		setValue(offset + 2, 0.0f);
		setValue(offset + 3, 0.0f);
		setValue(offset + 4, 0.0f);
		setValue(offset + 6, 0.0f);
		setValue(offset + 7, 0.0f);
		setValue(offset + 12, 0.0f);
		setValue(offset + 13, 0.0f);
		setValue(offset + 15, 0.0f);
	}
};

static vec3 normalize(const vec3 &v) {
	return v / sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

static mat4 transpose(const mat4 &m) {
	vec4 a = m.x;
	vec4 b = m.y;
	vec4 c = m.z;
	vec4 d = m.w;
	mat4 result;
	result.x = vec4(a.x, b.x, c.x, d.x);
	result.y = vec4(a.y, b.y, c.y, d.y);
	result.z = vec4(a.z, b.z, c.z, d.z);
	result.w = vec4(a.w, b.w, c.w, d.w);
	return result;
}

static mat4 perspective(float fovy, float aspect, float z_near, float z_far) {
	mat4 m(1.0f);
	float invtf = 1.0f / tan(fovy * 0.5f);
	m[0].x = invtf / aspect;
	m[1].y = invtf;
	m[2].z = -(z_far + z_near) / (z_far - z_near);
	m[2].w = -1.0f;
	m[3].z = (-2.0f * z_far * z_near) / (z_far - z_near);
	m[3].w = 0.0f;
	return m;
}

static mat4 orthographic(float left, float right, float bottom, float top,
		float z_near, float z_far) {
	mat4 m(1.0f);
	m[0].x = 2.0f / (right - left);
	m[3].x = -(right + left) / (right - left);
	m[1].y = 2.0f / (top - bottom);
	m[3].y = -(top + bottom) / (top - bottom);
	m[2].z = -2.0f / (z_far - z_near);
	m[3].z = -(z_far + z_near) / (z_far - z_near);
	return m;
}

static mat4 rotateX(float rad) {
	float co = cosf(rad);
	float si = sinf(rad);
	mat4 m(1.0f);
	m[1][1] = co;
	m[1][2] = -si;
	m[2][1] = si;
	m[2][2] = co;
	return m;
}

static mat4 rotateY(float rad) {
	float co = cosf(rad);
	float si = sinf(rad);
	mat4 m(1.0f);
	m[0][0] = co;
	m[0][2] = si;
	m[2][0] = -si;
	m[2][2] = co;
	return m;
}

static mat4 rotateZ(float rad) {
	float co = cosf(rad);
	float si = sinf(rad);
	mat4 m(1.0f);
	m[0][0] = co;
	m[1][0] = -si;
	m[0][1] = si;
	m[1][1] = co;
	return m;
}

static mat4 translate(float x, float y, float z) {
	mat4 m(1.0f);
	m[3][0] = x;
	m[3][1] = y;
	m[3][2] = z;
	m[3][3] = 1.0f;
	return m;
}

static mat4 translate(const vec3 &v) {
	mat4 m(1.0f);
	m[3][0] = v.x;
	m[3][1] = v.y;
	m[3][2] = v.z;
	return m;
}

static mat4 scale(float x, float y, float z) {
	mat4 m(1.0f);
	m[0][0] = x;
	m[1][1] = y;
	m[2][2] = z;
	return m;
}

static mat4 scale(float s) {
	return scale(s, s, s);
}

static vec3 cross(vec3 lhs, vec3 rhs) {
	vec3 ret;
	ret.x = lhs.y * rhs.z - lhs.z * rhs.y;
	ret.y = lhs.z * rhs.x - lhs.x * rhs.z;
	ret.z = lhs.x * rhs.y - lhs.y * rhs.x;
	return ret;
}

static float length(float x, float y, float z) {
	return sqrt(x * x + y * y + z * z);
}

static float EPSILON = 0.000000001f;
#define GET_ARRAY_LEN(array,len){len = (sizeof(array) / sizeof(array[0]));}

static bool getRotationMatrix(float R[], float I[], float gravity[],
		float geomagnetic[]) {
	float Ax = gravity[0];
	float Ay = gravity[1];
	float Az = gravity[2];

	float normsqA = (Ax * Ax + Ay * Ay + Az * Az);
	float g = 9.81f;
	float freeFallGravitySquared = 0.01f * g * g;
	if (normsqA < freeFallGravitySquared) {
		// gravity less than 10% of normal value
		return false;
	}

	float Ex = geomagnetic[0];
	float Ey = geomagnetic[1];
	float Ez = geomagnetic[2];

	float Hx = Ey * Az - Ez * Ay;
	float Hy = Ez * Ax - Ex * Az;
	float Hz = Ex * Ay - Ey * Ax;
	float normH = (float) sqrt(Hx * Hx + Hy * Hy + Hz * Hz);

	if (normH < 0.1f) {
		// device is close to free fall (or in space?), or close to
		// magnetic north pole. Typical values are > 100.
		return false;
	}

	float invH = 1.0f / normH;
	Hx *= invH;
	Hy *= invH;
	Hz *= invH;

	float invA = 1.0f / (float) sqrt(Ax * Ax + Ay * Ay + Az * Az);

	Ax *= invA;
	Ay *= invA;
	Az *= invA;

	float Mx = Ay * Hz - Az * Hy;
	float My = Az * Hx - Ax * Hz;
	float Mz = Ax * Hy - Ay * Hx;
	int rLen = 0;
	GET_ARRAY_LEN(R, rLen);
	if (R != NULL) {
		if (rLen == 9) {
			R[0] = Hx;
			R[1] = Hy;
			R[2] = Hz;
			R[3] = Mx;
			R[4] = My;
			R[5] = Mz;
			R[6] = Ax;
			R[7] = Ay;
			R[8] = Az;
		} else if (rLen == 16) {
			R[0] = Hx;
			R[1] = Hy;
			R[2] = Hz;
			R[3] = 0;
			R[4] = Mx;
			R[5] = My;
			R[6] = Mz;
			R[7] = 0;
			R[8] = Ax;
			R[9] = Ay;
			R[10] = Az;
			R[11] = 0;
			R[12] = 0;
			R[13] = 0;
			R[14] = 0;
			R[15] = 1;
		}
	}
	int iLen = 0;
	GET_ARRAY_LEN(I, iLen);
	if (I != NULL) {
		// compute the inclination matrix by projecting the geomagnetic
		// vector onto the Z (gravity) and X (horizontal component
		// of geomagnetic vector) axes.
		float invE = 1.0f / (float) sqrt(Ex * Ex + Ey * Ey + Ez * Ez);
		float c = (Ex * Mx + Ey * My + Ez * Mz) * invE;
		float s = (Ex * Ax + Ey * Ay + Ez * Az) * invE;

		if (iLen == 9) {
			I[0] = 1;
			I[1] = 0;
			I[2] = 0;
			I[3] = 0;
			I[4] = c;
			I[5] = s;
			I[6] = 0;
			I[7] = -s;
			I[8] = c;
		} else if (iLen == 16) {
			I[0] = 1;
			I[1] = 0;
			I[2] = 0;
			I[4] = 0;
			I[5] = c;
			I[6] = s;
			I[8] = 0;
			I[9] = -s;
			I[10] = c;
			I[3] = I[7] = I[11] = I[12] = I[13] = I[14] = 0;
			I[15] = 1;
		}
	}
	return true;
}
//
//static float* getOrientation(float R[], float values[]) {
//	/*
//	 * 4x4 (length=16) case: / R[ 0] R[ 1] R[ 2] 0 \ | R[ 4] R[ 5] R[ 6] 0 |
//	 * | R[ 8] R[ 9] R[10] 0 | \ 0 0 0 1 /
//	 *
//	 * 3x3 (length=9) case: / R[ 0] R[ 1] R[ 2] \ | R[ 3] R[ 4] R[ 5] | \ R[
//	 * 6] R[ 7] R[ 8] /
//	 */
//	int len = 0;
//	GET_ARRAY_LEN(R, len);
//	if (len == 9) {
//		values[0] = (float) atan2(R[1], R[4]);
//		values[1] = (float) asin(-R[7]);
//		values[2] = (float) atan2(-R[6], R[8]);
//	} else {
//		values[0] = (float) atan2(R[1], R[5]);
//		values[1] = (float) asin(-R[9]);
//		values[2] = (float) atan2(-R[8], R[10]);
//	}
//	return values;
//}
//
//static void getRotationMatrixFromVector(float R[], float rotationVector[]) {
//	float q0;
//	float q1 = rotationVector[0];
//	float q2 = rotationVector[1];
//	float q3 = rotationVector[2];
//	int vLen = 0;
//	GET_ARRAY_LEN(rotationVector, vLen);
//
//	if (vLen > 4 || vLen == 4) {
//		q0 = rotationVector[3];
//	} else {
//		q0 = 1 - q1 * q1 - q2 * q2 - q3 * q3;
//		q0 = (q0 > 0) ? (float) sqrt(q0) : 0;
//	}
//
//	float sq_q1 = 2 * q1 * q1;
//	float sq_q2 = 2 * q2 * q2;
//	float sq_q3 = 2 * q3 * q3;
//	float q1_q2 = 2 * q1 * q2;
//	float q3_q0 = 2 * q3 * q0;
//	float q1_q3 = 2 * q1 * q3;
//	float q2_q0 = 2 * q2 * q0;
//	float q2_q3 = 2 * q2 * q3;
//	float q1_q0 = 2 * q1 * q0;
//
//	int rLen = 0;
//	GET_ARRAY_LEN(R, rLen);
//
//	if (rLen == 9) {
//		R[0] = 1 - sq_q2 - sq_q3;
//		R[1] = q1_q2 - q3_q0;
//		R[2] = q1_q3 + q2_q0;
//
//		R[3] = q1_q2 + q3_q0;
//		R[4] = 1 - sq_q1 - sq_q3;
//		R[5] = q2_q3 - q1_q0;
//
//		R[6] = q1_q3 - q2_q0;
//		R[7] = q2_q3 + q1_q0;
//		R[8] = 1 - sq_q1 - sq_q2;
//	} else if (rLen == 16) {
//		R[0] = 1 - sq_q2 - sq_q3;
//		R[1] = q1_q2 - q3_q0;
//		R[2] = q1_q3 + q2_q0;
//		R[3] = 0.0f;
//
//		R[4] = q1_q2 + q3_q0;
//		R[5] = 1 - sq_q1 - sq_q3;
//		R[6] = q2_q3 - q1_q0;
//		R[7] = 0.0f;
//
//		R[8] = q1_q3 - q2_q0;
//		R[9] = q2_q3 + q1_q0;
//		R[10] = 1 - sq_q1 - sq_q2;
//		R[11] = 0.0f;
//
//		R[12] = R[13] = R[14] = 0.0f;
//		R[15] = 1.0f;
//	}
//}
//
//static void getRotationVectorFromGyro(float gyroValues[],
//		float deltaRotationVector[], float timeFactor) {
//	float normValues[3];
//
//	// Calculate the angular speed of the sample
//	float omegaMagnitude = (float) sqrt(
//			gyroValues[0] * gyroValues[0] + gyroValues[1] * gyroValues[1]
//					+ gyroValues[2] * gyroValues[2]);
//
//	// Normalize the rotation vector if it's big enough to get the axis
//	if (omegaMagnitude > EPSILON) {
//		normValues[0] = gyroValues[0] / omegaMagnitude;
//		normValues[1] = gyroValues[1] / omegaMagnitude;
//		normValues[2] = gyroValues[2] / omegaMagnitude;
//	}
//
//	// Integrate around this axis with the angular speed by the timestep
//	// in order to get a delta rotation from this sample over the timestep
//	// We will convert this axis-angle representation of the delta rotation
//	// into a quaternion before turning it into the rotation matrix.
//	float thetaOverTwo = omegaMagnitude * timeFactor;
//	float sinThetaOverTwo = (float) sin(thetaOverTwo);
//	float cosThetaOverTwo = (float) cos(thetaOverTwo);
//	deltaRotationVector[0] = sinThetaOverTwo * normValues[0];
//	deltaRotationVector[1] = sinThetaOverTwo * normValues[1];
//	deltaRotationVector[2] = sinThetaOverTwo * normValues[2];
//	deltaRotationVector[3] = cosThetaOverTwo;
//}
static float* matrixMultiplication(float A[], float B[]) {
	float* result = new float[9];

	result[0] = A[0] * B[0] + A[1] * B[3] + A[2] * B[6];
	result[1] = A[0] * B[1] + A[1] * B[4] + A[2] * B[7];
	result[2] = A[0] * B[2] + A[1] * B[5] + A[2] * B[8];

	result[3] = A[3] * B[0] + A[4] * B[3] + A[5] * B[6];
	result[4] = A[3] * B[1] + A[4] * B[4] + A[5] * B[7];
	result[5] = A[3] * B[2] + A[4] * B[5] + A[5] * B[8];

	result[6] = A[6] * B[0] + A[7] * B[3] + A[8] * B[6];
	result[7] = A[6] * B[1] + A[7] * B[4] + A[8] * B[7];
	result[8] = A[6] * B[2] + A[7] * B[5] + A[8] * B[8];

	return result;
}
static float* getRotationMatrixFromOrientation(float o[]) {
	float xM[9];
	float yM[9];
	float zM[9];

	float sinX = (float) sin(o[1]);
	float cosX = (float) cos(o[1]);
	float sinY = (float) sin(o[2]);
	float cosY = (float) cos(o[2]);
	float sinZ = (float) sin(o[0]);
	float cosZ = (float) cos(o[0]);

	// rotation about x-axis (pitch)
	xM[0] = 1.0f;
	xM[1] = 0.0f;
	xM[2] = 0.0f;
	xM[3] = 0.0f;
	xM[4] = cosX;
	xM[5] = sinX;
	xM[6] = 0.0f;
	xM[7] = -sinX;
	xM[8] = cosX;

	// rotation about y-axis (roll)
	yM[0] = cosY;
	yM[1] = 0.0f;
	yM[2] = sinY;
	yM[3] = 0.0f;
	yM[4] = 1.0f;
	yM[5] = 0.0f;
	yM[6] = -sinY;
	yM[7] = 0.0f;
	yM[8] = cosY;

	// rotation about z-axis (azimuth)
	zM[0] = cosZ;
	zM[1] = sinZ;
	zM[2] = 0.0f;
	zM[3] = -sinZ;
	zM[4] = cosZ;
	zM[5] = 0.0f;
	zM[6] = 0.0f;
	zM[7] = 0.0f;
	zM[8] = 1.0f;

	// rotation order is y, x, z (roll, pitch, azimuth)
	float* resultMatrix = new float[9];
	resultMatrix = matrixMultiplication(xM, yM);
	resultMatrix = matrixMultiplication(zM, resultMatrix);
	return resultMatrix;
}

static float* getOrientation(float R[], float values[]) {
	/*
	 * 4x4 (length=16) case: / R[ 0] R[ 1] R[ 2] 0 \ | R[ 4] R[ 5] R[ 6] 0 |
	 * | R[ 8] R[ 9] R[10] 0 | \ 0 0 0 1 /
	 *
	 * 3x3 (length=9) case: / R[ 0] R[ 1] R[ 2] \ | R[ 3] R[ 4] R[ 5] | \ R[
	 * 6] R[ 7] R[ 8] /
	 */
//	int len = 0;
//	GET_ARRAY_LEN(R, len);
//	if (len == 9) {
	values[0] = (float) atan2(R[1], R[4]);
	values[1] = (float) asin(-R[7]);
	values[2] = (float) atan2(-R[6], R[8]);
//	} else {
//		values[0] = (float) atan2(R[1], R[5]);
//		values[1] = (float) asin(-R[9]);
//		values[2] = (float) atan2(-R[8], R[10]);
//	}
	return values;
}

static void getRotationMatrixFromVector(float R[], float rotationVector[]) {
	float q0;
	float q1 = rotationVector[0];
	float q2 = rotationVector[1];
	float q3 = rotationVector[2];
//	int vLen = 0;
//	GET_ARRAY_LEN(rotationVector, vLen);

//	if (vLen > 4 || vLen == 4) {
	q0 = rotationVector[3];
//	} else {
//		q0 = 1 - q1 * q1 - q2 * q2 - q3 * q3;
//		q0 = (q0 > 0) ? (float) sqrt(q0) : 0;
//	}

	float sq_q1 = 2 * q1 * q1;
	float sq_q2 = 2 * q2 * q2;
	float sq_q3 = 2 * q3 * q3;
	float q1_q2 = 2 * q1 * q2;
	float q3_q0 = 2 * q3 * q0;
	float q1_q3 = 2 * q1 * q3;
	float q2_q0 = 2 * q2 * q0;
	float q2_q3 = 2 * q2 * q3;
	float q1_q0 = 2 * q1 * q0;

//	int rLen = 0;
//	GET_ARRAY_LEN(R, rLen);

//	if (rLen == 9) {
	R[0] = 1 - sq_q2 - sq_q3;
	R[1] = q1_q2 - q3_q0;
	R[2] = q1_q3 + q2_q0;

	R[3] = q1_q2 + q3_q0;
	R[4] = 1 - sq_q1 - sq_q3;
	R[5] = q2_q3 - q1_q0;

	R[6] = q1_q3 - q2_q0;
	R[7] = q2_q3 + q1_q0;
	R[8] = 1 - sq_q1 - sq_q2;
//	} else if (rLen == 16) {
//		R[0] = 1 - sq_q2 - sq_q3;
//		R[1] = q1_q2 - q3_q0;
//		R[2] = q1_q3 + q2_q0;
//		R[3] = 0.0f;
//
//		R[4] = q1_q2 + q3_q0;
//		R[5] = 1 - sq_q1 - sq_q3;
//		R[6] = q2_q3 - q1_q0;
//		R[7] = 0.0f;
//
//		R[8] = q1_q3 - q2_q0;
//		R[9] = q2_q3 + q1_q0;
//		R[10] = 1 - sq_q1 - sq_q2;
//		R[11] = 0.0f;
//
//		R[12] = R[13] = R[14] = 0.0f;
//		R[15] = 1.0f;
//	}
}

static void getRotationMatrixFromVector2(float R[], float rotationVector[]) {
//	float q0 = rotationVector[3];
//	float q1 = rotationVector[0];
//	float q2 = rotationVector[1];
//	float q3 = rotationVector[2];

	float q0 = rotationVector[3];
	float q1 = rotationVector[0];
	float q2 = rotationVector[1];
	float q3 = rotationVector[2];

	//	int vLen = 0;
	//	GET_ARRAY_LEN(rotationVector, vLen);

	//	if (vLen > 4 || vLen == 4) {
	//	} else {
	//		q0 = 1 - q1 * q1 - q2 * q2 - q3 * q3;
	//		q0 = (q0 > 0) ? (float) sqrt(q0) : 0;
	//	}

	float sq_q1 = 2 * q1 * q1;
	float sq_q2 = 2 * q2 * q2;
	float sq_q3 = 2 * q3 * q3;
	float q1_q2 = 2 * q1 * q2;
	float q3_q0 = 2 * q3 * q0;
	float q1_q3 = 2 * q1 * q3;
	float q2_q0 = 2 * q2 * q0;
	float q2_q3 = 2 * q2 * q3;
	float q1_q0 = 2 * q1 * q0;

	//	int rLen = 0;
	//	GET_ARRAY_LEN(R, rLen);

	//	if (rLen == 9) {
	R[0] = 1 - sq_q2 - sq_q3;
	R[1] = q1_q2 - q3_q0;
	R[2] = q1_q3 + q2_q0;
	R[3] = 0;

	R[4] = q1_q2 + q3_q0;
	R[5] = 1 - sq_q1 - sq_q3;
	R[6] = q2_q3 - q1_q0;
	R[7] = 0;

	R[8] = q1_q3 - q2_q0;
	R[9] = q2_q3 + q1_q0;
	R[10] = 1 - sq_q1 - sq_q2;
	R[11] = 0;

	R[12] = 0;
	R[13] = 0;
	R[14] = 0;
	R[15] = 1;

	//	} else if (rLen == 16) {
	//		R[0] = 1 - sq_q2 - sq_q3;
	//		R[1] = q1_q2 - q3_q0;
	//		R[2] = q1_q3 + q2_q0;
	//		R[3] = 0.0f;
	//
	//		R[4] = q1_q2 + q3_q0;
	//		R[5] = 1 - sq_q1 - sq_q3;
	//		R[6] = q2_q3 - q1_q0;
	//		R[7] = 0.0f;
	//
	//		R[8] = q1_q3 - q2_q0;
	//		R[9] = q2_q3 + q1_q0;
	//		R[10] = 1 - sq_q1 - sq_q2;
	//		R[11] = 0.0f;
	//
	//		R[12] = R[13] = R[14] = 0.0f;
	//		R[15] = 1.0f;
	//	}
}

static void getRotationVectorFromGyro(float gyroValues[],
		float deltaRotationVector[], float timeFactor) {
	float normValues[3];

	// Calculate the angular speed of the sample
	float omegaMagnitude = (float) sqrt(
			gyroValues[0] * gyroValues[0] + gyroValues[1] * gyroValues[1]
					+ gyroValues[2] * gyroValues[2]);

	// Normalize the rotation vector if it's big enough to get the axis
	if (omegaMagnitude > EPSILON) {
		normValues[0] = gyroValues[0] / omegaMagnitude;
		normValues[1] = gyroValues[1] / omegaMagnitude;
		normValues[2] = gyroValues[2] / omegaMagnitude;
	}
//	else return;

	// Integrate around this axis with the angular speed by the timestep
	// in order to get a delta rotation from this sample over the timestep
	// We will convert this axis-angle representation of the delta rotation
	// into a quaternion before turning it into the rotation matrix.
	float thetaOverTwo = omegaMagnitude * timeFactor;
	float sinThetaOverTwo = (float) sin(thetaOverTwo);
	float cosThetaOverTwo = (float) cos(thetaOverTwo);
	deltaRotationVector[0] = sinThetaOverTwo * normValues[0];
	deltaRotationVector[1] = sinThetaOverTwo * normValues[1];
	deltaRotationVector[2] = sinThetaOverTwo * normValues[2];
	deltaRotationVector[3] = cosThetaOverTwo;
}

static void multiplyByQuat(float points[], float quat[]) {

	float temp[4];
	temp[0] = points[0];
	temp[1] = points[1];
	temp[2] = points[2];
	temp[3] = points[3];
	points[3] = (temp[3] * quat[3] - temp[0] * quat[0] - temp[1] * quat[1]
			- temp[2] * quat[2]); //w = w1w2 - x1x2 - y1y2 - z1z2
	points[0] = (temp[3] * quat[0] + temp[0] * quat[3] + temp[1] * quat[2]
			- temp[2] * quat[1]); //x = w1x2 + x1w2 + y1z2 - z1y2
	points[1] = (temp[3] * quat[1] + temp[1] * quat[3] + temp[2] * quat[0]
			- temp[0] * quat[2]); //y = w1y2 + y1w2 + z1x2 - x1z2
	points[2] = (temp[3] * quat[2] + temp[2] * quat[3] + temp[0] * quat[1]
			- temp[1] * quat[0]); //z = w1z2 + z1w2 + x1y2 - y1x2
}

static double __toRadians(double angdeg) {
	return angdeg / 180 * 3.14;
}

static double __toDegrees(double angrad) {
	return angrad * 180 / 3.14;
}

static void __get_euler(const float* m, float* euler) {
	//R11 R12 R13
	//R21 R22 R23
	//R31 R32 R33
	//m[ 0]  m[ 4]  m[ 8]  m[12]
	//m[ 1]  m[ 5]  m[ 9]  m[13]
	//m[ 2]  m[ 6]  m[10]  m[14]
	//m[ 3]  m[ 7]  m[11]  m[15]

	float x, y, z;
	if (m[2] == -1.0f) {
		x = (float) atan2(m[4], m[8]);
		y = (float) __toRadians(90);
		z = 0;
	} else if (m[2] == 1.0f) {
		x = (float) atan2(-m[4], -m[8]);
		y = (float) __toRadians(-90);
		z = 0;
	} else {
		y = -(float) asin(m[2]);
		float cosy = (float) cos(y);
		x = (float) atan2(m[6] / cosy, m[10] / cosy);
		z = (float) atan2(m[1] / cosy, m[0] / cosy);
	}

	euler[0] = -(float) __toDegrees(x);
	euler[1] = -(float) __toDegrees(y);
	euler[2] = -(float) __toDegrees(z);
}

#endif
