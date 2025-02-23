#include "mydatastore.h"
#include "user.h"
#include "util.h"
#include "book.h"
#include "clothing.h"
#include "movie.h"
#include <iostream>
#include <sstream>
#include <set>
#include <string>
#include <cctype>
#include <vector>
#include <iterator>

using namespace std;

MyDataStore::MyDataStore() {}

MyDataStore::~MyDataStore() {
  // Delete all products
  for (vector<Product*>::iterator it = products_.begin(); it != products_.end(); ++it) {
    delete *it;
  }
  // Delete all users
  for (map<string, User*>::iterator it = users_.begin(); it != users_.end(); ++it) {
    delete it->second;
  }
}

void MyDataStore::addProduct(Product* p) {
  products_.push_back(p);
  indexProductKeywords(p);
}

void MyDataStore::addUser(User* u) {
  // Conv username to lowercase
  string uname = convToLower(u->getName());
  users_[uname] = u;
}

vector<Product*> MyDataStore::search(vector<string>& terms, int type) {
  // type 0 = AND, type 1 = OR
  set<Product*> resultSet;
  bool firstTerm = true;

  for (size_t i = 0; i < terms.size(); ++i) {
    string termLower = convToLower(terms[i]);
    map<string, set<Product*> >::iterator mit = keywordIndex_.find(termLower);
    if (mit != keywordIndex_.end()) {
      if (firstTerm) {
        resultSet = mit->second;
        firstTerm = false;
      }
      else {
        if (type == 0) { // AND
          resultSet = setIntersection(resultSet, mit->second);
        }
        else { // OR
          resultSet = setUnion(resultSet, mit->second);
        }
      }
    }
    else {
      // If 1 term is missing, result is empty.
      if (type == 0) {
        resultSet.clear();
        break;
      }
    }
  }
  vector<Product*> results(resultSet.begin(), resultSet.end());
  return results;
}

void MyDataStore::dump(ostream& ofile) {
  // Dump products
  ofile << "<products>" << endl;
  for (vector<Product*>::iterator it = products_.begin(); it != products_.end(); ++ it) {
    (*it)->dump(ofile);
  }
  ofile << "</products>" << endl;
  // Dump users
  ofile << "<users>" << endl;
  for (map<string, User*>::iterator it = users_.begin(); it != users_.end(); ++it) {
    it->second->dump(ofile);
  }
  ofile << "</users>" << endl;
}

void MyDataStore::indexProductKeywords(Product* p) {
  set<string> keys = p->keywords();
  for (set<string>::iterator it = keys.begin(); it != keys.end(); ++it) {
    string keyword = convToLower(*it);
    keywordIndex_[keyword].insert(p);
  }
}

void MyDataStore::addToCart(const string& username, Product* p){
  string uname = convToLower(username);
  if (users_.find(uname) == users_.end()) {
    cout << "Invalid request" << endl;
    //cout << "DEBUG: Invalid request. Username '" << uname << "' not found." << endl;
    return;
  }
  //cout << "DEBUG: Adding product to " << uname << "'s cart: " << p->getName() << endl;
  userCarts_[uname].push_back(p);
  //cout << "DEBUG: Cart size after adding: " << userCarts_[uname].size() << endl;
}

void MyDataStore::viewCart(const string& username){
  string uname = convToLower(username);
  if (users_.find(uname) == users_.end()) {
    cout << "Invalid username" << endl;
    return;
  }
  map<string, vector<Product*> >::iterator cit = userCarts_.find(uname);
  if (cit == userCarts_.end() || cit->second.empty()) {
    cout << "Cart is empty" << endl;
    return;
  }
  
  int index = 1;
  for (vector<Product*>::iterator it = cit->second.begin(); it != cit->second.end(); ++it) {
    cout << "Item " << index << endl;
    cout << (*it)->displayString() << endl;
    index++;
  }
}

void MyDataStore::checkout(const string& username){
  string uname = convToLower(username);
  map<string, User*>::iterator userIt = users_.find(uname);
  if (userIt == users_.end()) {
    cout << "Invalid username" << endl;
    //cout << "DEBUG: Invalid username in checkout: '" << uname << "'" << endl;
    return;
  }
  User* user = userIt->second;
  map<string, vector<Product*> >::iterator cartIt = userCarts_.find(uname);
  if (cartIt == userCarts_.end() || cartIt->second.empty()) {
    cout << "Cart is empty" << endl;
    //cout << "DEBUG: Cart is empty for " << uname << " at checkout." << endl;
    return;
  }
  vector<Product*>& cart = cartIt->second;
  vector<Product*> remainingCart;

  //cout << "DEBUG: Processing checkout for " << uname << ". Initial cart size: " << cart.size() << endl;

  // Go through products in cart
  for (vector<Product*>::iterator it = cart.begin(); it != cart.end(); ++it) {
    Product* p = *it;
    //cout << "DEBUG: Checking item: " << p->getName() << ", Price: " << p->getPrice() << ", Stock: " << p->getQty() << endl;

    if (p->getQty() > 0 && user->getBalance() >= p->getPrice()) {
      user->deductAmount(p->getPrice());
      p->subtractQty(1);
      //cout << "DEBUG: Purchased " << p->getName() << ". Remaining stock: " << p->getQty() << endl;
    }
    else {
    // product left in cart
      remainingCart.push_back(p);
      //cout << "DEBUG: Could not purchase " << p->getName() << ". Not enough stock or baalnce." << endl;
    }
  }
  cart = remainingCart;
  cout << "Checkout complete for " << username << endl;
  //cout << "DEBUG: Checkout complete. Remaining cart size: " << cart.size() << endl;
}

bool MyDataStore::userExists(const std::string& username) const {
  return users_.find(username) != users_.end();
}