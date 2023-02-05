#include "software/logger/coloured_cout_sink.h"

#include "software/logger/custom_logging_levels.h"

ColouredCoutSink::ColouredCoutSink(bool print_detailed) : print_detailed(print_detailed)
{
}

std::string ColouredCoutSink::colourToString(const FG_Colour colour)
{
    switch (colour)
    {
        case (FG_Colour::YELLOW):
            return "33";
        case (FG_Colour::RED):
            return "31";
        case (FG_Colour::GREEN):
            return "32";
        case (FG_Colour::WHITE):
            return "37";
        default:
            return "";
    }
}

FG_Colour ColouredCoutSink::getColour(const LEVELS level)
{
    if (level.value == WARNING.value)
    {
        return FG_Colour::YELLOW;
    }
    if (level.value == G3LOG_DEBUG.value)
    {
        return FG_Colour::GREEN;
    }
    if (g3::internal::wasFatal(level))
    {
        return FG_Colour::RED;
    }

    return FG_Colour::WHITE;
}

void ColouredCoutSink::resetColour()
{
    std::ostringstream oss;
    oss << "\033[" << colourToString(FG_Colour::WHITE) << "m"
        << " "
        << "\033[m";
    std::cout << oss.str();
}

void ColouredCoutSink::displayColouredLog(g3::LogMessageMover log_entry)
{
    auto level  = log_entry.get()._level;
    auto colour = colourToString(getColour(level));

    if (level.value == VISUALIZE.value || level.value == CSV.value)
    {
        // Don't log anything that calls LOG(VISUALIZE) and LOG(CSV)
        return;
    }

    bool past_time = log_entry.get()._timestamp - LOG_INTERVAL_TIMESTAMP > last_msg_timestamp;
    std::cout << "Past time: " << past_time << "\n";
    std::cout << "Repeat: " << (log_entry.get().message() == last_msg) << "\n";
    if (log_entry.get().message() == last_msg && past_time) {
        // repeated message outside timestamp, increase repeats and don't log
        num_repeats++;
        return;
    }

    // log and save info
    std::ostringstream oss;
    if (print_detailed)
    {
        oss << "\033[" << colour << "m" << " (" << num_repeats << ") " << log_entry.get().toString() << "\033[m";
    }
    else
    {
        oss << "\033[" << colour << "m" << log_entry.get().message() << " (" << num_repeats << ")" << "\n\033[m";
    }
    std::cout << oss.str() << std::flush;
    resetColour();

    last_msg = log_entry.get().message();
    last_msg_timestamp = log_entry.get()._timestamp;
    num_repeats = 1;
}
