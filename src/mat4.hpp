#ifndef MAT4_HPP
#define MAT4_HPP

#include <cmath>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <algorithm>
#include <utility>

class Mat4 {
    // row major order
    std::vector<float> values;
    const int nR, nC;

    friend std::ostream &operator<<(std::ostream &, const Mat4 &);

public:
    enum INIT {
        IDENTITY, ONES, ZEROS
    };

    Mat4(enum INIT init = IDENTITY) : nR(4), nC(4) {
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

    Mat4(const Mat4 &m) : nR(m.nR), nC(m.nC) {
        values.resize(nR * nC);
        // deep copy the values variable
        for (int i = 0; i < nR; ++i) {
            for (int j = 0; j < nC; ++j) {
                values[i * nC + j] = m.values[i * nC + j];
            }
        }
    }

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

    Mat4 operator~() const {
        Mat4 result;

        for (int i = 0; i < nR; ++i) {
            for (int j = 0; j < nC; ++j) {
                result.values[j * nR + i] = this->values[i * nC + j];
            }
        }

        return result;
    }

    Mat4 operator+(Mat4 const &m) const {
        Mat4 result;

        for (int i = 0; i < nR; ++i) {
            for (int j = 0; j < nC; ++j) {
                result.values[i * nC + j] = this->values[i * nC + j] + m.values[i * nC + j];
            }
        }

        return result;
    }

    Mat4 operator-(Mat4 const &m) const {
        Mat4 result;

        for (int i = 0; i < nR; ++i) {
            for (int j = 0; j < nC; ++j) {
                result.values[i * nC + j] = this->values[i * nC + j] - m.values[i * nC + j];
            }
        }

        return result;
    }

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


    Mat4 operator*(float const &value) const {
        Mat4 result;

        for (int i = 0; i < nR; ++i) {
            for (int j = 0; j < nC; ++j) {
                result.values[i * nC + j] = this->values[i * nC + j] * value;
            }
        }

        return result;
    }


    Mat4 operator%(Mat4 const &m) const {
        Mat4 result;

        for (int i = 0; i < nR; ++i) {
            for (int j = 0; j < nC; ++j) {
                result.values[i * nC + j] = this->values[i * nC + j] * m.values[i * nC + j];
            }
        }

        return result;
    }

    void dumpColumnWise(float *array) {
        for (int i = 0; i < nR; ++i) {
            for (int j = 0; j < nC; ++j) {
                array[i + j * nR] = this->values[i * nC + j];
            }
        }
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
