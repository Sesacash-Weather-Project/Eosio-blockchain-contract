#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

CONTRACT wxlaunches : public contract {
  public:
    using contract::contract;

    // Should be called by device
    ACTION addobs(name launch_id,
                  uint64_t unix_time, 
                  float pressure_hpa, 
                  float temperature_c, 
                  float dewPoint_c,
                  float elevation_m);

    ACTION setstation(string owner, 
                      float latitude, 
                      float longitude, 
                      float elevation, 
                      float reward_increment);

    ACTION websetlaunch(name launch_id, 
                        uint64_t unix_time, 
                        name miner, 
                        string device_type,
                        string wxcondition);

    ACTION websetsixhr(name launch_id, 
                       name miner,
                       string wx6hrcondition);

  private:

    TABLE observations {
      name launch_id;
      uint64_t unix_time;
      float pressure_hpa;
      float temperature_c;
      float dewPoint_c;
      float elevation_m;

      auto  primary_key() const { return unix_time; }
    };
    typedef multi_index<name("observations"), observations> observations_table_t;

    TABLE launches {
      name launch_id;
      uint64_t unix_time;
      name miner;
      string device_type;
      uint16_t level_reached;
      string wxcondition;
      string wx6hrcondition;

      auto  primary_key() const { return launch_id.value; }
    };
    typedef multi_index<name("launches"), launches> launches_table_t;

    TABLE station {
       name id;
       string owner;
       float latitude;
       float longitude;
       float elevation;
       float reward_increment;

       auto  primary_key() const { return id.value; }
    };

    typedef multi_index<name("station"), station> station_table_t;

};
