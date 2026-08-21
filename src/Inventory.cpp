#pragma once

class Inventory {
private:
  int q; // quantity
  int initial_q;
  float cash = 0.0f;
public:
  Inventory(int initq) : q(initq), initial_q(initq) {}
  void buy(int amount, float price) { 
    q += amount;
    cash -= (amount * price);
  }
  void sell(int amount, float price) {
    q -= amount;
    cash += (amount * price);
  }
  int getQuantity() const { return q; }
  void reset() { q = initial_q; cash = 0.0f; }
  float getCash(){ return cash; }
};
