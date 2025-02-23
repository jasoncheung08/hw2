#include "book.h"
#include "util.h"
#include <sstream>

Book::Book(const std::string& name, double price, int qty, const std::string& isbn, const std::string& author) : Product("book", name, price, qty), isbn_(isbn), author_(author) {}

Book::~Book() {}

std::set<std::string> Book::keywords() const {
  std::set<std::string> keys = parseStringToWords(name_);
  std::set<std::string> authorKeys = parseStringToWords(author_);
  keys.insert(authorKeys.begin(), authorKeys.end());
  keys.insert(isbn_);
  return keys;
}

std::string Book::displayString() const {
  std::ostringstream ss;

  ss << name_ << "\n";
  ss << "Author: " << author_ << " ISBN: " << isbn_ << "\n";
  ss << price_ << " " << qty_ << " left.";
  return ss.str();
}

void Book::dump(std::ostream& os) const {
  os << "book\n" << name_ << "\n" << price_ << "\n" << qty_ << "\n" << isbn_ << "\n" << author_ << std::endl;
}