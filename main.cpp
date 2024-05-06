#include <iostream>
#include <string>

#include "openai.hpp"

using namespace std;

void welcome_and_check_pin();
bool deposit(int &balance, int amount);
bool withdraw(int &balance, int amount);

int infer_menu_choice();

int main() {
  int balance{500};

  // 1. Ask for PIN and check it.
  welcome_and_check_pin();

  // GPT interface demo.
  int choice = infer_menu_choice();
  if (choice == 1) {
    int amount;
    cout << "Enter an amount to deposit (cents)" << endl;
    cin >> amount;
    cin.ignore(1000, '\n');
    deposit(balance, amount);
    cout << "After depositing " << amount << " cents the new balance is "
         << balance << endl;
  } else if (choice == 2) {
    int amount;
    cout << "Enter an amount to withdraw (cents)" << endl;
    cin >> amount;
    cin.ignore(1000, '\n');
    withdraw(balance, amount);
    cout << "After withdrawing " << amount << " cents the new balance is "
         << balance << endl;
  }
  return 0;
}

// - Say something to the user "Welcome the ATM. Please enter your pin."
// - function to check pin
// - if pin inputed == users pin the output message all good
// else -output pin is wrong please re-enter
// put this into a loop so everytime the pin is wrong they can re-enter it
// once pin is good loop breaks
void welcome_and_check_pin() {
  // Will use the api key provided by `OPENAI_API_KEY` environment variable
  openai::start();
  // THis is silly. We would not put a call to AI here, but it is a good
  // example.
  auto chat_completion = openai::chat().create(R"(
    {
        "model": "gpt-4-turbo",
        "messages":[{"role":"system", "content":"Pretend to be an ATM interface that is asking the user for their PIN. Tell them they must enter their PIN to continue. Be whimsical and funny."}],
        "max_tokens": 50,
        "temperature": 0.5
    }
    )"_json);
  // Print the text of the response
  cout << chat_completion["choices"][0]["message"]["content"] << endl;
  constexpr int CORRECT_PIN = 1234;
  int pin{0};
  while (pin != CORRECT_PIN) {
    cin >> pin;
    if (cin.fail()) {
      cin.clear();
      cin.ignore(1000, '\n');
      cerr << "Invalid input. Please enter a number." << endl;
      continue;
    }
    if (pin == CORRECT_PIN) {
      cout << "PIN is correct. Welcome!" << endl;
      break;
    } else {
      cout << "PIN is incorrect. Please re-enter." << endl;
    }
  }
  cin.ignore(1000, '\n');
}
// function to ask user what they want to do ex: check balance, deposit, or
// withdraw and get input
//
// funcion to deposit
// Parameters:
// - balance: is the current balance in cents.
// - amount: is the amount to withdraw in cents.
bool deposit(int &balance, int amount) {
  if (amount <= 0) {
    cerr << "Must enter a positive amount to deposit" << endl;
    return false;
  }
  // ??? Where to we ask how much they want to deposit.
  // ??? Where to display new balance?
  balance += amount;
  return true;
}
// Function to withdraw.
// Parameters:
//  - balance: is the current balance.
//  - amount is the amount to withdraw.
bool withdraw(int &balance, int amount) {
  if (amount <= 0) {
    cerr << "Must enter a positive amount to withdraw." << endl;
    return false;
  }
  if (amount > balance) {
    cerr << "Insufficient Funds." << endl;
    return false;
  }
  balance -= amount;
  return true;
}
// function to check balance
// parameters:
//  - balance: is the current balance.
void check_balance(int balance) {
  cout << "Your balance is: " << balance << " cents." << endl;
}

int infer_menu_choice() {
  // Will use the api key provided by `OPENAI_API_KEY` environment variable
  openai::start();
  string input;
  cout << "What do you want to do? " << endl;
  getline(cin, input);
  auto chat_completion = openai::chat().create(
      {{"model", "gpt-4-turbo"},
       {"messages",
        nlohmann::json::array(
            {{{"role", "system"},
              {"content",
               "Infer what the user wants to do: deposit, withdraw, or none. "
               "Return only a single lowercase word under all circumstances."}},
             {{"role", "user"}, {"content", input}}})},
       {"max_tokens", 50},
       {"temperature", 0.5}});
  // Print the text of the response
  string choice = chat_completion["choices"][0]["message"]["content"];
  cout << "You chose to: " << choice << endl;
  if (choice == "deposit") {
    return 1;
  } else if (choice == "withdraw") {
    return 2;
  } else {
    return 0;
  }
}
