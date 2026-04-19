#pragma once
#include<optional>


class Sensor {
    public:
        Sensor(bool buf = false, int size = 3);
        ~Sensor();

        void setValue(float val);
        std::optional<float> getValue() const;

        void setMin(float min);
        float getMin() const;
        void setMax(float max);
        float getMax() const;
        float getBarPct() const;

    private:
        std::optional<float> _value;
        const bool _buf;
        const int _size;

        float _min = 0;
        float _max = 0;
};