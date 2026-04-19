#include <Sensor.hpp>
#include <optional>
#include <algorithm>

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

float Sensor::getBarPct() const {
    if (!_value.has_value()) return 0.0f;
    return std::clamp((_value.value() - _min) / (_max - _min), 0.0f, 1.0f);
}