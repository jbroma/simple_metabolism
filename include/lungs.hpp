#pragma once

#include "organ.hpp"
#include "rbc.hpp"

class Lungs : public Organ {

    std::thread _life_thread;

public:
    Lungs(unsigned pos_y, Display& controller, std::list<RBC>& rbc_pool, std::mutex& list_mutex);
    ~Lungs();

    void respirate();

    void run();
};