#ifndef E8331CF4_7619_42C3_B505_4D5113C3ADCF
#define E8331CF4_7619_42C3_B505_4D5113C3ADCF

#include <algorithm>
#include "Text.h"

class ProgressBar : public Text
{
public:
    ProgressBar(int newLength, float newMaxValue = 1.f, const std::string &newDescription = "")
        : value(newMaxValue), maxValue(newMaxValue), description(newDescription), bar(newLength, '#')
    {
    }

    size_t size() const { return bar.size(); }

    void setValue(float newValue)
    {
        value = newValue;
    }

    void update(float delta) override
    {
        unsigned int i = 0;
        unsigned int count = std::min(size_t(bar.size() * value / maxValue + 0.5f), bar.size());

        for (; i < count; i++)
            bar[i] = '#';

        for (; i < bar.size(); i++)
            bar[i] = '.';

        setText(description + bar);

        Text::update(delta);
    }

private:
    float value, maxValue;
    std::string description, bar;
};

#endif /* E8331CF4_7619_42C3_B505_4D5113C3ADCF */
