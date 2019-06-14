#pragma once

#include "organ.hpp"
#include "rbc.hpp"

class Spleen : public Organ {

    std::thread _life_thread;

public:
    Spleen(unsigned pos_y, Display& controller, std::list<RBC>& rbc_pool, std::mutex& list_mutex);
    ~Spleen();

    void destroy_rbc();

    void run();
};