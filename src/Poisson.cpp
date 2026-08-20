#include <random>
#include <cmath>
#include <iostream>

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

    void setParams(float duration, float intensity)
    {
        float dt = 1.0f / 252.0f;

        lambda = intensity * dt;

        std::cout
            << lambda
            << " BASE LAMBDA\n";
    }

    int sample(float distance, float kappa)
    {
        // A quote crossing the mid is outside
        // the normal exponential fill model.
        if (distance < 0.0f)
            return 0;

        float arrivalRate =
            lambda *
            std::exp(-kappa * distance);

        std::cout << "base: " << lambda
              << " multiplier: " << std::exp(-kappa * distance)
              << " actual lambda: " << arrivalRate
              << "\n";

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