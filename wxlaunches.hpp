#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

CONTRACT wxlaunches : public contract {
  public:
    using contract::contract;

    // Should be called by device
    ACTION addobs(uint64_t unix_time, 
                  float pressure_hpa, 
                  float temperature_c, 
                  float relHumidity, 
                  float dewPoint_c,
                  float wind_mph, // Calculated off chain
                  float wind_degrees); // Calculated off chain

    ACTION websetlaunch(uint64_t unix_time, name miner, string memo, string wxcondition);

    ACTION iotsetlaunch(float lat, float lon, float pressure, uint32_t reward_amount);

    //auto get_last_launch_iterator();
    //void set_reward_sent(bool ifSent);

  private:

    TABLE observations {
      uint8_t launch_number;
      uint64_t unix_time;
      float pressure_hpa;
      float temperature_c;
      float relHumidity;
      float dewPoint_c;
      float wind_mph;
      float wind_degrees;

      auto  primary_key() const { return unix_time; }
    };
    typedef multi_index<name("observations"), observations> observations_table_t;

    TABLE launches {
      uint64_t launch_number;
      uint64_t unix_time;
      name miner;
      string miner_memo;
      float latitude;
      float longitude;
      float surf_pressure_hpa;
      string wxcondition;
      uint32_t reward_amount;
      bool reward_sent;

      auto  primary_key() const { return launch_number; }
    };
    typedef multi_index<name("launches"), launches> launches_table_t;

};
