#pragma once

class Inventory {
private:
  int q; // quantity
  int initial_q;
  float profit = 0;
public:
  Inventory(int initq) : q(initq), initial_q(initq) {}
  void buy(int amount, float price) { q += amount; }
  void sell(int amount, float price) { q -= amount; }
  int getQuantity() const { return q; }
  void reset() { q = initial_q; profit = 0.0f; }
};
