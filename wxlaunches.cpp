#include <wxlaunches.hpp>
#include <eosio/asset.hpp>

using namespace std;
using namespace eosio;

ACTION wxlaunches::websetlaunch(uint64_t unix_time, name miner, string memo, string wxcondition) {
  // Only web app can run this Action
  require_auth("weatherapppp"_n);

  uint64_t nextlaunch;

  launches_table_t _launches(get_self(), get_self().value);  

  if (_launches.cbegin() == _launches.cend())
  {
    nextlaunch = 1;
  } else {
    auto last_itr = _launches.cend(); // Takes the end of the vector
    last_itr--;
    nextlaunch = last_itr->launch_number + 1;
  }

  // Add the currently stored variables to the launches table
  _launches.emplace(get_self(), [&](auto& launch) {
      launch.launch_number = nextlaunch;
      launch.unix_time = unix_time; // Comes as function input
      launch.miner = miner;
      launch.miner_memo = memo;
      launch.wxcondition = wxcondition;
      launch.reward_sent = false;
    });

}

ACTION wxlaunches::iotsetlaunch(float lat, float lon, float pressure, uint32_t reward_amount) {
  // Only web app can run this Action
  require_auth("noderedtelos"_n);

  launches_table_t _launches(get_self(), get_self().value);
  //auto last_itr = _launches.upper_bound(numeric_limits<uint64_t>::max());
  
  // Takes last element in container
  auto last_itr = _launches.cend(); 
  last_itr--;

  // Add the currently stored variables to the launches table
  _launches.modify(last_itr, get_self(), [&](auto& launch) {
      launch.latitude = lat;
      launch.longitude = lon;
      launch.surf_pressure_hpa = pressure;
      launch.reward_amount = reward_amount;
    });

}

ACTION wxlaunches::addobs(uint64_t unix_time, 
                          float pressure_hpa, 
                          float temperature_c, 
                          float relHumidity, 
                          float dewPoint_c,
                          float wind_mph,
                          float wind_degrees) {
  
  // Only self can run this action
  require_auth(get_self());

  // Get our two tables
  observations_table_t _observations(get_self(), get_self().value);
  launches_table_t _launches(get_self(), get_self().value);

  // Takes last element in container
  auto last_itr = _launches.cend();
  last_itr--;

  name miner = last_itr->miner;
  bool if_reward_sent = last_itr->reward_sent;
  uint8_t launch_number = last_itr->launch_number;
  uint32_t reward_amount = last_itr->reward_amount*10000; // 10,000 = 1 TLOS
  string miner_memo = last_itr->miner_memo;

  if (pressure_hpa < 900.0 && !if_reward_sent) {

    // Set reward asset. A value of 10000 is equivalent to 1 Telos
    eosio::asset reward = eosio::asset( reward_amount, symbol(symbol_code("TLOS"),4));

    // Use an inline action to send reward to miner
    // TODO: Permission should be made to somewhere else
    action(
    permission_level{ get_self(), "active"_n },
    "eosio.token"_n, "transfer"_n,
    std::make_tuple( get_self(), miner, reward, miner_memo)
    ).send();
    
    _launches.modify(last_itr, get_self(), [&](auto& launch) {
      launch.reward_sent = true;
    });
  }

  // Add the row to the observation set
  _observations.emplace(get_self(), [&](auto& obs) {
      obs.launch_number = launch_number;
      obs.unix_time = unix_time;
      obs.pressure_hpa = pressure_hpa;
      obs.temperature_c = temperature_c;
      obs.relHumidity = relHumidity;
      obs.dewPoint_c = dewPoint_c;
      obs.wind_mph = wind_mph;
      obs.wind_degrees = wind_degrees;
    });

}

// Dispatch the actions to the blockchain
EOSIO_DISPATCH(wxlaunches, (addobs)(websetlaunch)(iotsetlaunch))
