#include <stdexcept>
#include <utility>

class BadOptionalAccess : public std::exception {
public:
    using exception::exception;

    virtual const char *what() const noexcept override {
        return "Bad optional access";
    }
};

template<typename T>
class Optional {
public:

    Optional() = default;

    Optional(const T &item);

    Optional(T &&item);

    Optional(const Optional &other);

    Optional(Optional &&other) noexcept;

    ~Optional();

    bool HasValue() const;

    T &Value();

    const T &Value() const;

    void Reset();

    Optional &operator=(const T &item) {
        HasValue() ? (void) (*buff_ = item) : (void) (buff_ = new(&data_[0]) T(item), is_initialized_ = true);
        return *this;
    }

    Optional &operator=(T &&item) {
        HasValue() ? (void) (*buff_ = std::forward<T>(item))
                   : (void) (buff_ = new(&data_[0]) T(std::forward<T>(item)), is_initialized_ = true);

        return *this;
    }

    Optional &operator=(const Optional &other) {

        (!other.HasValue() && HasValue()) ? Reset() :
        (other.HasValue() && !HasValue()) ? (void) (buff_ = new(&data_[0]) T(other.Value()), is_initialized_ = true) :
        (other.HasValue() && HasValue()) ? (void) (*buff_ = other.Value(), is_initialized_ = true) : void();

        return *this;
    }

    Optional &operator=(Optional &&other) noexcept {

        (!other.HasValue() && HasValue()) ? Reset() :
        (other.HasValue() && !HasValue()) ? (void) (buff_ = new(&data_[0]) T(
                std::forward<T>(other.Value())), is_initialized_ = true) :
        (other.HasValue() && HasValue()) ? (void) (*buff_ = std::forward<T>(other.Value()), is_initialized_ = true)
                                         : void();

        return *this;
    }

    T &operator*() { return Value(); }

    const T &operator*() const { return Value(); }

    T *operator->() { return &Value(); }

    const T *operator->() const { return &Value(); }

private:
    T *buff_ = nullptr;

    alignas(T) char data_[sizeof(T)]{};
    bool is_initialized_ = false;
};

template<typename T>
void Optional<T>::Reset() {
    is_initialized_ ? (is_initialized_ = false, buff_->~T()) : void();
}

template<typename T>
const T &Optional<T>::Value() const {
    if (!is_initialized_) {
        throw BadOptionalAccess();
    } else {
        return *buff_;
    }
}

template<typename T>
T &Optional<T>::Value() {
    if (!is_initialized_) {
        throw BadOptionalAccess();
    } else {
        return *buff_;
    }
}

template<typename T>
bool Optional<T>::HasValue() const {
    return is_initialized_;
}

template<typename T>
Optional<T>::~Optional() {
    is_initialized_ ? buff_->~T() : void();
}

template<typename T>
Optional<T>::Optional(Optional &&other) noexcept {

    (!other.HasValue() && this->HasValue()) ? Reset() :
    (other.HasValue() && !HasValue()) ? (void) (buff_ = new(&data_[0]) T(
            std::forward<T>(other.Value())), is_initialized_ = true) :
    (other.HasValue() && HasValue()) ? (void) (*buff_ = std::forward<T>(other.Value()), is_initialized_ = true)
                                     : void();
}

template<typename T>
Optional<T>::Optional(const Optional &other) {

    (!other.HasValue() && HasValue()) ? Reset() :
    (other.HasValue() && !this->HasValue()) ? (void) (buff_ = new(&data_[0]) T(
            other.Value()), is_initialized_ = true) :
    (other.HasValue() && HasValue()) ? (void) (*buff_ = other.Value(), is_initialized_ = true) : void();
}


template<typename T>
Optional<T>::Optional(T &&item) {
    buff_ = new(&data_[0]) T(std::forward<T>(item));

    is_initialized_ = true;
}

template<typename T>
Optional<T>::Optional(const T &item) {

    buff_ = new(&data_[0]) T(item);

    is_initialized_ = true;

}