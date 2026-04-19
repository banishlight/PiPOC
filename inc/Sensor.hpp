#pragma once
#include<optional>


class Sensor {
    public:
        Sensor(bool buf, int size);
        ~Sensor();

        void setValue(float val);
        std::optional<float> getValue() const;

        void setMin(float min);
        float getMin() const;

        void setMax(float max);
        float getMax() const;

    private:
        std::optional<float> _value;
        const bool _buf = false;
        const int _size = 3;

        float _min = 0;
        float _max = 0;
};