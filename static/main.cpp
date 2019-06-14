#include <cstring>
#include <functional>
#include <iostream>
#include <list>
#include <mutex>
#include <ncurses.h>
#include <string>
#include <thread>

#include "../include/bone_marrow.hpp"
#include "../include/brain.hpp"
#include "../include/digestive_system.hpp"
#include "../include/display.hpp"
#include "../include/heart.hpp"
#include "../include/lungs.hpp"
#include "../include/rbc.hpp"
#include "../include/spleen.hpp"

namespace parser {

unsigned parse_args(int argc, char** argv)
{
    unsigned thread_count = 5;

    for (int i = 1; i < argc; ++i) {
        try {
            if (!strcmp(argv[i], "-t") || !strcmp(argv[i], "--threads")) {
                thread_count = std::stoi(argv[i + 1]);
                if (thread_count > 100) {
                    std::cerr << "Number of threads cannot be greater than 100."
                              << std::endl;
                    std::exit(1);
                }
                ++i;
            } else {
                std::cerr << "Unrecognised option \""
                          << argv[i]
                          << "\"."
                          << std::endl;
                std::exit(1);
            }
        } catch (std::invalid_argument& e) {
            std::cerr << "Wrong format of an option's argument."
                      << std::endl;
            std::exit(1);
        } catch (std::logic_error& e) {
            std::cerr << "Missing an argument for an option."
                      << std::endl;
            std::exit(1);
        }
    }
    return thread_count;
}

} // namespace parser

// void test_run(Display& controller)
// {
//     std::list<RBC> rbc_pool;
//     std::mutex mtx;
//     rbc_pool.emplace_back(0, 5, controller);
//     rbc_pool.emplace_back(10, 0, controller);
//     rbc_pool.emplace_back(15, 0, controller);
//     rbc_pool.emplace_back(20, 0, controller);
//     rbc_pool.emplace_back(25, 0, controller);
//     rbc_pool.emplace_back(35, 0, controller);
//     auto& condition = controller.get_kill_switch();
//     while (!condition) {
//         for (auto& rbc : rbc_pool) {
//             std::scoped_lock lg{ rbc.get_rbc_mutex(), mtx };
//             rbc.advance_pos();
//             controller.update_rbc_position(rbc.get_dpositions(), rbc.get_dresources(), rbc.get_dstate());
//         }
//         std::this_thread::sleep_for(std::chrono::milliseconds(20));
//     }
// }

int main(int argc, char** argv)
{
    parser::parse_args(argc, argv);
    Display display_controller{};
    //std::thread test_thread(&Display::main_loop, &display_controller);
    std::list<RBC> rbc_pool;
    std::mutex list_mutex;
    rbc_pool.emplace_back(0, 5, display_controller);
    rbc_pool.emplace_back(10, 0, display_controller);
    rbc_pool.emplace_back(15, 0, display_controller);
    rbc_pool.emplace_back(20, 0, display_controller);
    rbc_pool.emplace_back(25, 0, display_controller);
    rbc_pool.emplace_back(35, 0, display_controller);
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
