#ifndef VIBRATING_MEMBRANE_H
#define VIBRATING_MEMBRANE_H

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <vector>


class VibratingMembrane : public juce::Component, private juce::Timer
{
public:
    VibratingMembrane()
    {
        resizeGrid(gridSize);
        startTimerHz(60); // simulate at 60 FPS
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        float cellWidth = bounds.getWidth() / static_cast<float>(gridSize);
        float cellHeight = bounds.getHeight() / static_cast<float>(gridSize);

        const int center = gridSize / 2;
        const int radius = gridSize / 2 - 1;

        for (int y = 0; y < gridSize; ++y)
        {
            for (int x = 0; x < gridSize; ++x)
            {
                if (!isInsideCircle(x, y, center, center, radius))
                    continue;

                float value = current[y][x];
                float logValue = std::log10(1.0f + std::abs(value) * 100.0f) / std::log10(101.0f);
                float scaled = 0.5f + 0.5f * std::copysign(logValue, value);
                uint8_t brightness = static_cast<uint8_t>(juce::jlimit(0.0f, 1.0f, 0.5f + 0.5f * scaled) * 255.0f);
                g.setColour(juce::Colour(brightness, brightness, brightness));
                g.fillRect(x * cellWidth, y * cellHeight, cellWidth, cellHeight);
            }
        }
    }

    void mouseDown(const juce::MouseEvent& e) override
    {
        auto bounds = getLocalBounds();
        int x = static_cast<int>((static_cast<float>(e.x) / bounds.getWidth()) * gridSize);
        int y = static_cast<int>((static_cast<float>(e.y) / bounds.getHeight()) * gridSize);

        const int center = gridSize / 2;
        const int radius = gridSize / 2 - 1;

        if (x > 1 && x < gridSize - 1 && y > 1 && y < gridSize - 1 &&
            isInsideCircle(x, y, center, center, radius))
        {
            current[y][x] = 1.0f;
            previous[y][x] = 0.5f; // slight kick for velocity
        }
    }

private:
    void timerCallback() override
    {
        const int timeStepsPerFrame = 10; // simulate 4x faster
        // const float damping = 0.996f;
        const float damping = 0.99f;
        const float c2 = 0.25f;

        for (int step = 0; step < timeStepsPerFrame; ++step)
        {
            const int center = gridSize / 2;
            const int radius = gridSize / 2 - 1;

            for (int y = 1; y < gridSize - 1; ++y)
            {
                for (int x = 1; x < gridSize - 1; ++x)
                {
                    if (!isInsideCircle(x, y, center, center, radius))
                    {
                        next[y][x] = 0.0f;
                        continue;
                    }

                    float laplacian = current[y + 1][x] + current[y - 1][x]
                                    + current[y][x + 1] + current[y][x - 1]
                                    - 4.0f * current[y][x];

                    next[y][x] = damping * (
                        2.0f * current[y][x]
                        - previous[y][x]
                        + c2 * laplacian
                    );
                }
            }

            std::swap(previous, current);
            std::swap(current, next);
        }

        repaint();
    }


    void resizeGrid(int size)
    {
        previous = std::vector<std::vector<float>>(size, std::vector<float>(size, 0.0f));
        current  = std::vector<std::vector<float>>(size, std::vector<float>(size, 0.0f));
        next     = std::vector<std::vector<float>>(size, std::vector<float>(size, 0.0f));
    }

    bool isInsideCircle(int x, int y, int centerX, int centerY, int radius) const
    {
        int dx = x - centerX;
        int dy = y - centerY;
        return dx * dx + dy * dy <= radius * radius;
    }

    const int gridSize = 100;
    std::vector<std::vector<float>> previous, current, next;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VibratingMembrane)
};



#endif //VIBRATING_MEMBRANE_H
