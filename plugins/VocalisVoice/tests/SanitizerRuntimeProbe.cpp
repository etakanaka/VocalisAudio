/*
 * Vocalis Audio — Vocalis Voice
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
#include <atomic>
#include <thread>
int main()
{
    std::atomic<int> value{0};
    std::thread thread([&] { ++value; });
    thread.join();
    return value == 1 ? 0 : 1;
}
