#include "GBM.cpp"
#include "Inventory.cpp"
#include "MarketQuotes.cpp"
#include "Parameters.cpp"
#include "Sim.cpp"
#include "Draw.cpp"

int main()
{
    Sim sim;

    Draw draw(sim);

    if (!draw.Initialise())
        return 1;

    draw.Run();

    draw.Shutdown();

    return 0;
}