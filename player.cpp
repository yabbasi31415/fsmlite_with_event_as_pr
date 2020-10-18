#include "fsm.h"

#include <cassert>
#include <iostream>
#include <string>
#include <memory>

struct CDTitle {
    std::string title;
};

CDTitle cdTitle;

class player: public fsmlite::fsm<player> {
    friend class fsm;  // base class needs access to transition_table

    std::string cd_title;
    bool autoplay = false;

public:
    enum states { Stopped, Open, Empty, Playing, Paused };

    player(state_type init_state = Empty) : fsm(init_state) { }

    void set_autoplay(bool f) { autoplay = f; }

    bool is_autoplay() const { return autoplay; }

    const std::string& get_cd_title() const { return cd_title; }

    // struct cd_detected { std::string title; };
    using cd_detected = std::shared_ptr<CDTitle>;

private:
    // guards
    bool is_autoplay(const cd_detected&) const { return autoplay; }
    bool is_bad_cd(const cd_detected& cd) const { 
        return cd->title.empty(); 
        }

    // actions
    void store_cd_info(const cd_detected& cd);

private:
    using m = player;  // for brevity

    using transition_table = table<
//              Start    Event        Target   Action              Guard (optional)
//  -----------+--------+------------+--------+-------------------+---------------+-
    mem_fn_row< Empty,   cd_detected, Stopped, &m::store_cd_info   /* fallback */  >
//  -----------+--------+------------+--------+-------------------+---------------+-
    >;
};

void player::store_cd_info(const cd_detected& cd)
{
    std::cout << "Detected CD '" << cd->title << "'\n";
    cd_title = cd->title;
}

void test_player()
{
    player p;
    assert(p.current_state() == player::Empty);
    assert(!p.is_autoplay());
    assert(p.get_cd_title().empty());
    
    player::cd_detected new_ev = std::make_shared<CDTitle>(cdTitle);
    new_ev->title = "Louie";

    p.process_event("Louie");
    assert(p.current_state() == player::Stopped);
    assert(p.get_cd_title() == "Louie");
    
}

int main()
{
    test_player();
    return 0;
}
