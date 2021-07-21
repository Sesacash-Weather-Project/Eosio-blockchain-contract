#include <eosio/eosio.hpp>
#include <eosio/time.hpp>
#include <time.h>

using namespace std;
using namespace eosio;

CONTRACT wxlaunches : public contract {
  public:
    using contract::contract;

    // Should be called by device

    ACTION addobs(name launch_id,
                  uint64_t unix_time_s, 
                  float pressure_hpa, 
                  float temperature_c, 
                  float humidity_percent,
                  float latitude_deg,
                  float longitude_deg,
                  uint16_t gps_elevation_m,
                  uint16_t elevation2_m,
                  uint8_t flags);

    ACTION removeobs( uint64_t unix_time_start,
                      uint64_t unix_time_end);

    ACTION setstation(string owner, 
                      float latitude, 
                      float longitude, 
                      float elevation,
                      float missing,
                      float reward_increment,
                      uint8_t launch_window_hrs,
                      uint8_t launch_freq_hrs);

    ACTION websetlaunch(name launch_id, 
                        uint64_t unix_time,
                        float surf_pressure, 
                        name miner, 
                        string device_type,
                        string wxcondition);

    ACTION setflag(uint64_t bit_number,
                    string description);

    ACTION settimeslots(uint64_t first_start_time);

    ACTION settwelvehr(name launch_id, 
                       name miner,
                       string wx12hrcondition);

  private:

    // Local functions (not actions)
    void sendReward( name miner, float last_level, float pressure);
    void updatetimes( uint64_t first_start_time );
    bool checkIfReleased( float surf_pressure, float pressure_hpa );

    TABLE observations {
      uint64_t unix_time;
      name launch_id;
      float pressure_hpa;
      float temperature_c;
      float humidity_percent;
      float latitude_deg;
      float longitude_deg;
      uint16_t elevation_gps_m;
      uint16_t elevation2_m;
      uint8_t flags;

      auto  primary_key() const { return unix_time; }
    };
    typedef multi_index<name("observations"), observations> observations_table_t;

    TABLE launches {
      name launch_id;
      uint64_t unix_time;
      name miner;
      name assistant;
      float surf_pressure;
      float level_reached;
      string device_type;
      string wxcondition;
      bool if_released;
      float last_known_lat;
      float last_known_lon;
      float last_known_elev;
      string wx12hrcondition;

      auto  primary_key() const { return launch_id.value; }
    };
    typedef multi_index<name("launches"), launches> launches_table_t;

    TABLE station {
       name id;
       string owner;
       float latitude;
       float longitude;
       float elevation;
       float missing;
       float reward_increment;
       uint8_t launch_window_hrs;
       uint8_t launch_freq_hrs;

       auto  primary_key() const { return id.value; }
    };
    typedef multi_index<name("station"), station> station_table_t;

    TABLE flags {
      uint64_t bit_number;
      string description;

      auto primary_key() const { return bit_number; }
    };
    typedef multi_index<name("flags"), flags> flags_table_t;

    TABLE timeslots {
      uint64_t start_time;
      uint64_t end_time;
      string human_window;

      auto primary_key() const { return start_time; }
    };
    typedef multi_index<name("timeslots"), timeslots> timeslots_table_t;

};
