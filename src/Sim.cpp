#include <iostream>

#include "Parameters.cpp"
#include "Inventory.cpp"
#include "Stoikov.cpp"
#include "Draw.cpp"

class Sim {
private:
    Parameters parameters;
    Inventory inventory;
    Stoikov stoikov;
    Draw draw;

public:
    Sim()
        : parameters(),
          inventory(0),
          stoikov(
              parameters.getVol(),
              parameters.getRiskAversion(),
              parameters.getLiquidity(),
              parameters.getIntensity(),
              parameters.getDuration(),
              inventory
          ),
          draw(parameters)
    {
    }

    Parameters& getParameters()
    {
        return parameters;
    }

    Inventory& getInventory()
    {
        return inventory;
    }

    Stoikov& getStoikov()
    {
        return stoikov;
    }

    void startSim()
    {
        std::cout << "Starting simulation\n";

        parameters.createMarketPath();
        std::cout << parameters.getIntensity();

        for (int i = 0; i < parameters.getPath().size(); i++)
        {
            step(i);
        }

        std::cout << "Simulation complete\n";
    }

    void step(int i)
    {
        stoikov.quotes(
            i,
            parameters.getPath()[i]
        );
    }

    void run()
    {
        if (!draw.Initialise())
            return;

        while (draw.IsOpen())
        {
            // -----------------------------
            // Start ImGui frame
            // -----------------------------

            draw.BeginFrame();

            // -----------------------------
            // Draw all UI
            // -----------------------------

            draw.init();

            // -----------------------------
            // Handle UI events
            // -----------------------------

            if (draw.StartPressed())
            {
                startSim();
            }

            // -----------------------------
            // Finish frame
            // -----------------------------

            draw.EndFrame();
        }

        draw.Shutdown();
    }
};