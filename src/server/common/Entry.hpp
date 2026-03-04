#pragma once

class Entry
{
public:
    explicit Entry(std::string val, const uint32_t reads = 0, const uint32_t writes = 0);

    [[maybe_unused]] Entry& read();
    [[maybe_unused]] Entry& write(std::string newVal);
    [[nodiscard]] std::string getVal() const { std::lock_guard<std::mutex> lock(m_mtx); return m_val; }
    [[nodiscard]] uint32_t getReads() const { std::lock_guard<std::mutex> lock(m_mtx); return m_reads; }
    [[nodiscard]] uint32_t getWrites() const { std::lock_guard<std::mutex> lock(m_mtx); return m_writes; }
    [[nodiscard]] std::mutex& getMtx() const { return m_mtx; }

private:
    std::string m_val;
    uint32_t m_reads = 0;
    uint32_t m_writes = 0;
    mutable std::mutex m_mtx;
};