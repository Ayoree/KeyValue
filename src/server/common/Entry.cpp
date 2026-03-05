#include "Entry.hpp"

Entry::Entry(std::string val, const uint32_t reads /*= 0*/, const uint32_t writes /*= 0*/)
:
    m_val(std::move(val)), m_reads(reads), m_writes(writes)
{}

Entry &Entry::read()
{
    std::shared_lock lk(m_mtx);
    ++m_reads;
    return *this;
}

Entry &Entry::write(std::string newVal)
{
    std::lock_guard lk(m_mtx);
    m_val = std::move(newVal);
    ++m_writes;
    return *this;
}
