#pragma once

#include <functional>
#include <vector>

#include "expro/expro.h"

namespace expro_wrapper
{
    template <typename T>
    consteval ExproValueTag GetExproValueTag()
    {
        if constexpr (std::is_same_v<T, bool>)
        {
            return ExproValueTag::Bool;
        }
        else if constexpr (std::is_integral_v<T>)
        {
            return ExproValueTag::Int;
        }
        else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
        {
            return ExproValueTag::Float;
        }
        else
        {
            static_assert(false, "Unsupported type");
        }
    }

    template <typename T>
    T FromExproValue(const ExproValue& aVal)
    {
        if (std::is_same_v<T, bool> && aVal.tag == ExproValueTag::Bool)
        {
            return aVal.value.bool_value;
        }
        else if (std::is_integral_v<T> && aVal.tag == ExproValueTag::Int)
        {
            return aVal.value.int_value;
        }
        else if ((std::is_same_v<T, float> || std::is_same_v<T, double>) && aVal.tag == ExproValueTag::Float)
        {
            return aVal.value.float_value;
        }
        throw std::runtime_error("Type mismatch or unsupported type in FromExproValue");
    }

    template <typename T>
    ExproValue ToExproValue(const T& aVal)
    {
        ExproValue exproVal;
        if constexpr (std::is_same_v<T, bool>)
        {
            exproVal.tag = ExproValueTag::Bool;
            exproVal.value.bool_value = aVal;
        }
        else if constexpr (std::is_integral_v<T>)
        {
            exproVal.tag = ExproValueTag::Int;
            exproVal.value.int_value = static_cast<int64_t>(aVal);
        }
        else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
        {
            exproVal.tag = ExproValueTag::Float;
            exproVal.value.float_value = static_cast<double>(aVal);
        }
        else
        {
            static_assert(false, "Unsupported type");
        }
        return exproVal;
    }

    template <typename UserData, typename Ret, typename... Args>
    struct FncContext
    {
        UserData* userData;
        std::function<Ret(UserData*, Args...)> callback;
    };

    template <typename UserData, typename Ret, typename... Args, size_t... I>
    ExproValue InvokeImpl(FncContext<UserData, Ret, Args...>* aCtx, const ExproValue* aArgs, std::index_sequence<I...>)
    {
        Ret result = aCtx->callback(aCtx->userData, FromExproValue<Args>(aArgs[I])...);
        return ToExproValue(result);
    }

    class SymbolDefinitions
    {
        ExproSymbolDefinitions* m_symbols = nullptr;

    public:
        SymbolDefinitions();
        ~SymbolDefinitions();
        SymbolDefinitions(const SymbolDefinitions&) = delete;
        SymbolDefinitions& operator=(const SymbolDefinitions&) = delete;
        SymbolDefinitions(SymbolDefinitions&&) = delete;
        SymbolDefinitions& operator=(SymbolDefinitions&&) = delete;

        ExproSymbolDefinitions* ExtractRawSymbols()
        {
            auto ret = m_symbols;
            m_symbols = nullptr;
            return ret;
        }

        template <typename UserData, typename Ret, typename... Args>
        void AddFunction(std::string aName, std::function<Ret(UserData*, Args... args)> aFunction, UserData* aUserData = nullptr)
        {
            using LocalContext = FncContext<UserData, Ret, Args...>;
            auto ctx = new LocalContext{.userData = aUserData, .callback = aFunction};  // TMP known memory leak
            auto convertedFnc = [](void* userData, const ExproValue* args, uintptr_t argCount) -> ExproValue {
                return InvokeImpl(static_cast<LocalContext*>(userData), args, std::index_sequence_for<Args...>{});
            };

            std::array<ExproValueTag, sizeof...(Args)> argTypes = {GetExproValueTag<Args>()...};

            if (!expro_symbol_definitions_add_function(m_symbols, aName.c_str(), argTypes.data(), argTypes.size(),
                                                  GetExproValueTag<Ret>(), convertedFnc, ctx))
            {
                throw std::runtime_error("Failed to add function: " + aName);
            }
        }

        template <typename UserData, typename Ret, typename Arg>
        void AddUnaryOperator(std::string aSymbol, Ret (*aFunction)(Arg), void* aUserData = nullptr)
        {
            expro_symbol_definitions_add_unary_operator(m_symbols, aSymbol.c_str(), GetExproValueTag<Arg>(),
                                                        GetExproValueTag<Ret>(), aFunction, aUserData);
        }

        void AddBinaryOperator();
    };

    class ExpressionProcessor
    {
        ExproExpressionProcessor* m_processor = nullptr;

    public:
        ExpressionProcessor(ExproSymbolDefinitions* aSymbolDefinitions);
        ~ExpressionProcessor();

        template <typename T>
        T Evaluate(const std::string& aExpression)
        {
            auto result = expro_processor_process(m_processor, aExpression.c_str());
            if (!result.success)
            {
                std::string errorMessage = result.error_message ? result.error_message : "Unknown error";
                expro_free_result(result);
                throw std::runtime_error("Expression evaluation failed: " + errorMessage);
            }
            auto val = FromExproValue<T>(result.value);
            expro_free_result(result);
            return val;
        }
    };
}