#include <Sensor.hpp>
#include <optional>

Sensor::Sensor(bool buf, int size) : _buf(buf), _size(size) {

}

Sensor::~Sensor() {

}

void Sensor::setValue(float val) {
    if (_buf) {
        if (_value.has_value())
            _value = ((*_value * (_size - 1)) + val) / _size;
        else
            _value = val; // seed with first reading
    } else {
        _value = val;
    }
}

std::optional<float> Sensor::getValue() const {
    return _value;
}

void Sensor::setMin(float min) {
    _min = min;
}

float Sensor::getMin() const {
    return _min;
}

void Sensor::setMax(float max) {
    _max = max;
}

float Sensor::getMax() const {
    return _max;
}