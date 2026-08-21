#pragma once

#include <random>
#include <cmath>

class Poisson {
private:
    float lambda;
    std::mt19937 generator;

public:

    Poisson()
        : lambda(0.0f),
          generator(std::random_device{}())
    {
    }

    void setParams(float duration, float intensity, int steps)
    {
        float dt = duration / static_cast<float>(steps);

        lambda = intensity * dt;
    }

    int sample(float distance, float kappa)
    {
        // A quote at or through the mid is aggressively priced, so it
        // fills at the full base rate rather than not at all.
        if (distance < 0.0f)
            distance = 0.0f;

        float arrivalRate =
            lambda *
            std::exp(-kappa * distance);

        if (arrivalRate <= 0.0f)
            return 0;

        std::poisson_distribution<int> distribution(
            arrivalRate
        );

        return distribution(generator);
    }

    float getLambda()
    {
        return lambda;
    }
};
