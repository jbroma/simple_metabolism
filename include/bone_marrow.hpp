#pragma once

#include "organ.hpp"
#include "rbc.hpp"

class BoneMarrow : public Organ {

    std::thread _life_thread;

public:
    BoneMarrow(unsigned pos_y, Display& controller, std::list<RBC>& rbc_pool, std::mutex& list_mutex);
    ~BoneMarrow();

    void create_rbc();

    void run();
};