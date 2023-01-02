#ifndef MAT4_HPP
#define MAT4_HPP

#include <cmath>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <algorithm>
#include <utility>
#include "vec3.hpp"

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

enum INIT {
    IDENTITY, ONES, ZEROS
};

enum AXIS {
    X, Y, Z
};

class Mat4 {
    // row major order
    std::vector<float> values;
    const int nR, nC;

    friend std::ostream &operator<<(std::ostream &, const Mat4 &);

public:

    // Default is identity transform, can be initialized to ONES, ZEROS if needed
    explicit Mat4(enum INIT init = IDENTITY) : nR(4), nC(4) {
        values.resize(nR * nC);
        switch (init) {
            case IDENTITY:
                setIdentity();
                break;
            case ONES:
                setOnes();
                break;
            case ZEROS:
                setZeros();
                break;
            default:
                setIdentity();
                break;
        }
    }

    // Rotation about x,y,z axes transform
    explicit Mat4(const float angleInDegrees, enum AXIS axis) : nR(4), nC(4) {
        values.resize(nR * nC);
        setIdentity();
        switch (axis) {
            case X:
                values[5] = cos(angleInDegrees / 180 * M_PI);
                values[6] = -sin(angleInDegrees / 180 * M_PI);
                values[9] = sin(angleInDegrees / 180 * M_PI);
                values[10] = cos(angleInDegrees / 180 * M_PI);
                break;
            case Y:
                values[0] = cos(angleInDegrees / 180 * M_PI);
                values[2] = sin(angleInDegrees / 180 * M_PI);
                values[8] = -sin(angleInDegrees / 180 * M_PI);
                values[10] = cos(angleInDegrees / 180 * M_PI);
                break;
            case Z:
                values[0] = cos(angleInDegrees / 180 * M_PI);
                values[1] = -sin(angleInDegrees / 180 * M_PI);
                values[4] = sin(angleInDegrees / 180 * M_PI);
                values[5] = cos(angleInDegrees / 180 * M_PI);
                break;
        }
    }

    // Translation transform
    explicit Mat4(const Vec3 translation) : nR(4), nC(4) {
        values.resize(nR * nC);
        setIdentity();
        values[3] = translation.x;
        values[7] = translation.y;
        values[11] = translation.z;
    }

    // Scale transform
    explicit Mat4(const float scaleX, const float scaleY, const float scaleZ) : nR(4), nC(4) {
        values.resize(nR * nC);
        setIdentity();
        values[0] = scaleX;
        values[5] = scaleY;
        values[10] = scaleZ;
    }

    // Perspective transform
    explicit Mat4(const float near, const float far,
                  const float left, const float right,
                  const float bottom, const float top) : nR(4), nC(4) {
        values.resize(nR * nC);
        setIdentity();
        values[0] = (2 * near) / (right - left);
        values[2] = (right + left) / (right - left);
        values[5] = (2 * near) / (top - bottom);
        values[6] = (top + bottom) / (top - bottom);
        values[10] = -(far + near) / (far - near);
        values[11] = (-2 * far * near) / (far - near);
        values[14] = -1;
        values[15] = 0;
    }

    // Deep copy
    Mat4(const Mat4 &m) : nR(m.nR), nC(m.nC) {
        values.resize(nR * nC);
        for (int i = 0; i < nR; ++i) {
            for (int j = 0; j < nC; ++j) {
                values[i * nC + j] = m.values[i * nC + j];
            }
        }
    }

    // Deep copy
    void operator=(Mat4 const &m) {
        for (int i = 0; i < nR; ++i) {
            for (int j = 0; j < nC; ++j) {
                values[i * nC + j] = m.values[i * nC + j];
            }
        }
    }

    ~Mat4() = default;

    float get(const int &i, const int &j) const {
        return values[i * nC + j];
    }

    std::vector<float> getAll() const {
        return values;
    }

    Vec3 getViewDirection() const {
        return Vec3(values[8], values[9], values[10]) * -1;
    }

    void set(const int &i, const int &j, const float &k) {
        values[i * nC + j] = k;
    }

    Mat4 *setZeros() {
        for (int i = 0; i < nR; ++i) {
            for (int j = 0; j < nC; ++j) {
                values[i * nC + j] = 0;
            }
        }
        return this;
    }

    Mat4 *setOnes() {
        for (int i = 0; i < nR; ++i) {
            for (int j = 0; j < nC; ++j) {
                values[i * nC + j] = 1;
            }
        }
        return this;
    }

    Mat4 *setIdentity() {
        for (int i = 0; i < nR; ++i) {
            for (int j = 0; j < nC; ++j) {
                values[i * nC + j] = (i == j);
            }
        }
        return this;
    }

    Mat4 *setUniform(float low, float high) {
        for (int i = 0; i < nR; ++i) {
            for (int j = 0; j < nC; ++j) {
                float randomNumber = (float) rand() / RAND_MAX;
                randomNumber = low + randomNumber * (high - low);
                values[i * nC + j] = randomNumber;
            }
        }
        return this;
    }

    // Set as viewing matrix
    void setAsViewMatrix(Vec3 const &u, Vec3 const &v, Vec3 const &n, Vec3 const &d) {
        values[0] = u.x;
        values[1] = u.y;
        values[2] = u.z;
        values[3] = d.x;
        values[4] = v.x;
        values[5] = v.y;
        values[6] = v.z;
        values[7] = d.y;
        values[8] = n.x;
        values[9] = n.y;
        values[10] = n.z;
        values[11] = d.z;
        values[12] = 0;
        values[13] = 0;
        values[14] = 0;
        values[15] = 1;
    }

    // Transpose
    Mat4 operator~() const {
        Mat4 result;

        for (int i = 0; i < nR; ++i) {
            for (int j = 0; j < nC; ++j) {
                result.values[j * nR + i] = this->values[i * nC + j];
            }
        }

        return result;
    }

    // Element-wise add
    Mat4 operator+(Mat4 const &m) const {
        Mat4 result;

        for (int i = 0; i < nR; ++i) {
            for (int j = 0; j < nC; ++j) {
                result.values[i * nC + j] = this->values[i * nC + j] + m.values[i * nC + j];
            }
        }

        return result;
    }

    // Element-wise subtract
    Mat4 operator-(Mat4 const &m) const {
        Mat4 result;

        for (int i = 0; i < nR; ++i) {
            for (int j = 0; j < nC; ++j) {
                result.values[i * nC + j] = this->values[i * nC + j] - m.values[i * nC + j];
            }
        }

        return result;
    }

    // Matrix multiply
    Mat4 operator*(Mat4 const &m) const {
        Mat4 result;

        for (int i = 0; i < nR; ++i) {
            for (int j = 0; j < m.nC; ++j) {
                float elementSum = 0;
                for (int k = 0; k < nC; ++k) {
                    elementSum += this->values[i * nC + k] * m.values[k * m.nC + j];
                }
                result.values[i * m.nC + j] = elementSum;
            }
        }
        return result;
    }

    // Element-wise scalar multiply
    Mat4 operator*(float const &value) const {
        Mat4 result;

        for (int i = 0; i < nR; ++i) {
            for (int j = 0; j < nC; ++j) {
                result.values[i * nC + j] = this->values[i * nC + j] * value;
            }
        }

        return result;
    }

    Vec3 transformVector(Vec3 const &v) const {
        Vec3 result(values[0] * v.x + values[1] * v.y + values[2] * v.z + values[3] * 0,
                    values[4] * v.x + values[5] * v.y + values[6] * v.z + values[7] * 0,
                    values[8] * v.x + values[9] * v.y + values[10] * v.z + values[11] * 0);

        return result;
    }

    // Element-wise multiply
    Mat4 operator%(Mat4 const &m) const {
        Mat4 result;

        for (int i = 0; i < nR; ++i) {
            for (int j = 0; j < nC; ++j) {
                result.values[i * nC + j] = this->values[i * nC + j] * m.values[i * nC + j];
            }
        }

        return result;
    }

    void dumpColumnWise(float *array) const {
        for (int i = 0; i < nR; ++i) {
            for (int j = 0; j < nC; ++j) {
                array[i + j * nR] = this->values[i * nC + j];
            }
        }
    }

    Vec3 getTranslationVec3() const {
        return {values[3], values[7], values[11]};
    }
};

std::ostream &operator<<(std::ostream &out, const Mat4 &m) {
    for (int i = 0; i < m.nR; ++i) {
        for (int j = 0; j < m.nC; ++j) {
            out << m.values[i * m.nC + j] << " ";
        }
        out << std::endl;
    }
    return out;
}

#endif
