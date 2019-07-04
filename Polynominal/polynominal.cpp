#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

template<typename T>
void PrintCoeff(ostream& out, int i, const T& coef, bool printed) {
  bool coeffPrinted = false;
  if (coef == 1 && i > 0) {
    out << (printed ? "+" : "");
  } else if (coef == -1 && i > 0) {
    out << "-";
  } else if (coef >= 0 && printed) {
    out << "+" << coef;
    coeffPrinted = true;
  } else {
    out << coef;
    coeffPrinted = true;
  }
  if (i > 0) {
    out << (coeffPrinted ? "*" : "") << "x";
  }
  if (i > 1) {
    out << "^" << i;
  }
}

template<typename T>
class Polynomial {
private:
  vector<T> coeffs_ = {0};

  void Shrink() {
    while (!coeffs_.empty() && coeffs_.back() == 0) {
      coeffs_.pop_back();
    }
  }

  class IndexProxy {
  public:
    IndexProxy(Polynomial& poly, size_t degree)
      : poly_(poly)
      , degree_(degree)
    {
    }

    IndexProxy& operator = (T value) {
      auto& coeffs = poly_.coeffs_;

      if (degree_ >= coeffs.size()) {
        coeffs.resize(degree_ + 1);
      }
      coeffs[degree_] = value;
      poly_.Shrink();
      return *this;
    }

    operator T() const {
      return std::as_const(poly_)[degree_];
    }

  private:
    Polynomial& poly_;
    size_t degree_;
  };

public:
  Polynomial() = default;
  Polynomial(vector<T> coeffs) : coeffs_(move(coeffs)) {
    Shrink();
  }

  template<typename Iterator>
  Polynomial(Iterator first, Iterator last) : coeffs_(first, last) {
    Shrink();
  }

  bool operator ==(const Polynomial& other) const {
    return coeffs_ == other.coeffs_;
  }

  bool operator !=(const Polynomial& other) const {
    return !operator==(other);
  }

  int Degree() const {
    return coeffs_.size() - 1;
  }

  Polynomial& operator +=(const Polynomial& r) {
    if (r.coeffs_.size() > coeffs_.size()) {
      coeffs_.resize(r.coeffs_.size());
    }
    for (size_t i = 0; i != r.coeffs_.size(); ++i) {
      coeffs_[i] += r.coeffs_[i];
    }
    Shrink();
    return *this;
  }

  Polynomial& operator -=(const Polynomial& r) {
    if (r.coeffs_.size() > coeffs_.size()) {
      coeffs_.resize(r.coeffs_.size());
    }
    for (size_t i = 0; i != r.coeffs_.size(); ++i) {
      coeffs_[i] -= r.coeffs_[i];
    }
    Shrink();
    return *this;
  }

  T operator [](size_t degree) const {
    return degree < coeffs_.size() ? coeffs_[degree] : 0;
  }

  IndexProxy operator [](size_t degree) {
    return IndexProxy{*this, degree};
  }

  T operator ()(T x) const {
    T res = 0;
    for (auto it = coeffs_.rbegin(); it != coeffs_.rend(); ++it) {
      res *= x;
      res += *it;
    }
    return res;
  }

  using const_iterator = typename vector<T>::const_iterator;

  const_iterator begin() const {
    return coeffs_.cbegin();
  }

  const_iterator end() const {
    return coeffs_.cend();
  }
};

template <typename T>
ostream& operator <<(ostream& out, const Polynomial<T>& p) {
  bool printed = false;
  for (int i = p.Degree(); i >= 0; --i) {
    if (p[i] != 0) {
      PrintCoeff(out, i, p[i], printed);
      printed = true;
    }
  }
  return out;
}

template <typename T>
Polynomial<T> operator +(Polynomial<T> lhs, const Polynomial<T>& rhs) {
  lhs += rhs;
  return lhs;
}

template <typename T>
Polynomial<T> operator -(Polynomial<T> lhs, const Polynomial<T>& rhs) {
  lhs -= rhs;
  return lhs;
}
