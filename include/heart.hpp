#pragma once

#include "organ.hpp"
#include "rbc.hpp"

class Heart : public Organ {

    std::thread _life_thread;

public:
    Heart(unsigned pos_y, Display& controller, std::list<RBC>& rbc_pool, std::mutex& list_mutex);
    ~Heart();

    void pump();
    void set_pulmonary_speed(RBC& rbc);
    void set_systemic_speed(RBC& rbc);

    void run();
};