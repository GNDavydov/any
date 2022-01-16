// Copyright 2022 GNDavydov

#ifndef INCLUDE_ANY_HPP_
#define INCLUDE_ANY_HPP_


#include <memory>
#include <utility>
#include <typeinfo>

namespace my_any {
    class any {
        template<typename ValueType>
        friend const ValueType *any_cast(const any *) noexcept;

    private:
        struct storage_base {
            virtual ~storage_base() = default;

            [[nodiscard]] virtual const std::type_info &type() const noexcept = 0;

            [[nodiscard]] virtual std::unique_ptr<storage_base> clone() const = 0;
        };

        template<typename ValueType>
        struct storage final : public storage_base {
            template<typename... Args>
            explicit storage(Args &&... args)
                    : value(std::forward<Args>(args)...) {}

            [[nodiscard]] const std::type_info &type() const noexcept override {
                return typeid(ValueType);
            }

            [[nodiscard]] std::unique_ptr<storage_base> clone() const override {
                return std::make_unique<storage<ValueType>>(value);
            }

            ValueType value;
        };

        std::unique_ptr<storage_base> instance_ = nullptr;

    public:
        // constructors
        constexpr any() noexcept = default;

        any(const any &copy) {
            if (copy.instance_) instance_ = copy.instance_->clone();
        }

        any(any &&move) noexcept
                : instance_(std::move(move.instance_)) {}

        template<typename ValueType, typename = std::enable_if_t<
                !std::is_same_v<std::decay_t<ValueType>, any>>>
        explicit any(ValueType &&value) {
            emplace<std::decay_t<ValueType>>(std::forward<ValueType>(value));
        }

        // assignment operators
        any &operator=(const any &copy) {
            any(copy).swap(*this);
            return *this;
        }

        any &operator=(any &&move) noexcept {
            any(std::move(move)).swap(*this);
            return *this;
        }

        template<typename ValueType, typename = std::enable_if_t<
                !std::is_same_v<std::decay_t<ValueType>, any>>>
        any &operator=(ValueType &&rhs) {
            any(std::forward<ValueType>(rhs)).swap(*this);
            return *this;
        }

        // modifiers
        template<typename ValueType, typename... Args>
        void emplace(Args &&... args) {
            instance_ = std::make_unique<storage<std::decay_t<ValueType>>>(std::forward<Args>(args)...);
        }

        void reset() noexcept {
            instance_.reset();
        }

        void swap(any &other) noexcept {
            std::swap(instance_, other.instance_);
        }

        // observers
        [[nodiscard]] bool has_value() const noexcept {
            return instance_.operator bool();
        }

        [[nodiscard]] const std::type_info &type() const noexcept {
            return instance_ ? instance_->type() : typeid(void);
        }
    };

    class bad_any_cast : public std::exception {
    public:
        [[nodiscard]] const char *what() const noexcept final {
            return "bad any cast";
        }
    };

    // C++20
    template<typename T>
    using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

    // any_cast
    template<typename ValueType>
    ValueType any_cast(const any &anything) {
        using value_type_cvref = remove_cvref_t<ValueType>;
        if (auto *value = any_cast<value_type_cvref>(&anything)) {
            return static_cast<ValueType>(*value);
        } else {
            throw bad_any_cast();
        }
    }


    template<typename ValueType>
    const ValueType *any_cast(const any *anything) noexcept {
        if (!anything) return nullptr;
        auto *storage = dynamic_cast<any::storage<ValueType> *>(anything->instance_.get());
        if (!storage) return nullptr;
        return &storage->value;
    }

    // make_any
    template<typename ValueType, typename... Args>
    any make_any(Args &&... args) {
        return any(std::in_place_type<ValueType>, std::forward<Args>(args)...);
    }

    template<typename ValueType, typename List, typename... Args>
    any make_any(std::initializer_list<List> list, Args &&... args) {
        return any(std::in_place_type<ValueType>, list, std::forward<Args>(args)...);
    }

}

#endif //INCLUDE_ANY_HPP_
