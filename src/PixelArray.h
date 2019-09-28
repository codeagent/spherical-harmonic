#ifndef SH_PIXELARRAY_H
#define SH_PIXELARRAY_H


#include <inttypes.h>
#include <stdexcept>

namespace sh {

    template<class T>
    class PixelArrayRow {
    protected:
        T *data;
        uint16_t length;
    public:
        PixelArrayRow(T *data, uint16_t length) : data(data), length(length) {}

        uint16_t getLength() const {
            return length;
        }

        T& operator[](int index) {
            if (index < 0 || index >= length) {
                throw std::runtime_error("PixelArrayRow: index out of range " + std::to_string(index));
            }
            return data[index];
        }

        const T operator[](int index) const {
            if (index < 0 || index >= length) {
                throw std::runtime_error("PixelArrayRow: index out of range " + std::to_string(index));
            }
            return data[index];
        }
    };


    template<class T>
    class PixelArray {
    protected:
        uint16_t width;
        uint16_t height;
        T *data;
    public:
        PixelArray(T *data, uint16_t width, uint16_t height) : data(data), width(width), height(height) {}
        PixelArray(const PixelArray<T> &) = delete;
        PixelArray<T> &operator=(PixelArray<T> &) = delete;

        ~PixelArray() {
            delete[] data;
        }

        const T *getData() const {
            return data;
        }

        uint16_t getWidth() const {
            return width;
        }

        uint16_t getHeight() const {
            return height;
        }

        PixelArrayRow<T> operator[](int row) {
            if (row < 0 || row >= height) {
                throw std::runtime_error("PixelArray: index out of range " + std::to_string(row));
            }
            return PixelArrayRow<T>(&data[row * width], width);
        }
    };
}

#endif //SH_PIXELARRAY_H
