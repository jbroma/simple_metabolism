#include <functional>
#include <iostream>
#include <list>
#include <mutex>
#include <ncurses.h>
#include <thread>

#include "../include/bone_marrow.hpp"
#include "../include/brain.hpp"
#include "../include/digestive_system.hpp"
#include "../include/display.hpp"
#include "../include/heart.hpp"
#include "../include/lungs.hpp"
#include "../include/rbc.hpp"
#include "../include/spleen.hpp"

int main(int argc, char** argv)
{
    Display display_controller{};
    std::list<RBC> rbc_pool;
    std::mutex list_mutex;
    rbc_pool.emplace_back(0, 5, display_controller, RBC_State::INIT);
    rbc_pool.emplace_back(10, 0, display_controller, RBC_State::INIT);
    rbc_pool.emplace_back(15, 0, display_controller, RBC_State::INIT);
    rbc_pool.emplace_back(20, 0, display_controller, RBC_State::INIT);
    rbc_pool.emplace_back(25, 0, display_controller, RBC_State::INIT);
    rbc_pool.emplace_back(35, 0, display_controller, RBC_State::INIT);
    Lungs lungs{ 0, display_controller, rbc_pool, list_mutex };
    Heart heart{ 13, display_controller, rbc_pool, list_mutex };
    DigestiveSystem dgs{ 29, display_controller, rbc_pool, list_mutex };
    Spleen spleen{ 37, display_controller, rbc_pool, list_mutex };
    BoneMarrow bone_marrow{ 45, display_controller, rbc_pool, list_mutex };
    Brain brain{ 21, display_controller, rbc_pool, list_mutex, lungs, heart, dgs, spleen, bone_marrow };
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    display_controller.get_start_cv().notify_all();
    display_controller.main_loop();

    return 0;
}
