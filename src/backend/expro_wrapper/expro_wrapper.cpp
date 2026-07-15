#include "expro_wrapper/expro_wrapper.h"

#include "expro/expro.h"

namespace expro_wrapper
{
    SymbolDefinitions::SymbolDefinitions()
        : m_symbols(expro_symbol_definitions_new())
    {
        expro_symbol_definitions_add_all_common_operators(m_symbols);
    }

    SymbolDefinitions::~SymbolDefinitions()
    {
        if (m_symbols)
        {
            expro_symbol_definitions_free(m_symbols);
            m_symbols = nullptr;
        }
    }

    ExpressionProcessor::ExpressionProcessor(ExproSymbolDefinitions* aSymbolDefinitions)
        : m_processor(expro_processor_new(aSymbolDefinitions))
    {
    }

    ExpressionProcessor::~ExpressionProcessor()
    {
        if (m_processor)
        {
            expro_processor_free(m_processor);
            m_processor = nullptr;
        }
    }
}
