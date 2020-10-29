#include <wxlaunches.hpp>
#include <eosio/asset.hpp>

using namespace std;
using namespace eosio;

ACTION wxlaunches::websetlaunch(name launch_id, 
                                uint64_t unix_time, 
                                name miner,
                                string device_type,
                                string wxcondition) {

  // Only web app can run this Action
  require_auth("weatherapppp"_n);

  launches_table_t _launches(get_self(), get_self().value);

  // Add the currently stored variables to the launches table
  _launches.emplace(get_self(), [&](auto& launch) {

      launch.launch_id = launch_id;
      launch.unix_time = unix_time; // Comes as function input
      launch.miner = miner;
      launch.device_type = device_type;
      launch.wxcondition = wxcondition;
      launch.level_reached = 1000;
      launch.wx6hrcondition = "N/A";

    });

}

ACTION wxlaunches::setstation(string owner, 
                              float latitude, 
                              float longitude, 
                              float elevation, 
                              float reward_increment) {


  // Only web app can run this Action
  require_auth(get_self());

  station_table_t _station(get_self(), get_self().value);

  // Get first field
  auto station_itr = _station.cbegin();

  // Add the currently stored variables to the launches table
  if ( station_itr == _station.cend()) { // if table is empty

    // Add the station info
    _station.emplace(get_self(), [&](auto& station) {

      station.id = get_self();
      station.owner = owner;
      station.latitude = latitude;
      station.longitude = longitude;
      station.elevation = elevation;
      station.reward_increment = reward_increment;

    });
  } else {

    // Update station info
    _station.modify(station_itr, get_self(), [&](auto& station) {

      station.id = get_self();
      station.owner = owner;
      station.latitude = latitude;
      station.longitude = longitude;
      station.elevation = elevation;
      station.reward_increment = reward_increment;
    });
  }
}

ACTION wxlaunches::addobs(name launch_id,
                  uint64_t unix_time, 
                  float pressure_hpa, 
                  float temperature_c, 
                  float dewPoint_c,
                  float elevation_m) {
  
  // Only self can run this action
  require_auth("noderedtelos"_n);

  // Get our own two tables
  observations_table_t _observations(get_self(), get_self().value);
  launches_table_t _launches(get_self(), get_self().value);
  station_table_t _station(get_self(), get_self().value);

  auto launch_itr = _launches.find(launch_id.value);

  auto station_itr = _station.find(get_self().value);
  //auto station_itr = _station.find("wxstationdat"_n.value);

  uint32_t reward_amt = (uint32_t)(10000 * station_itr->reward_increment); // 10,000 = 1 TLOS
  uint16_t last_level = launch_itr->level_reached;
  name miner = launch_itr->miner;

  uint8_t pressure_hpa_int = (uint8_t)pressure_hpa;

  if( last_level - pressure_hpa_int > 100  ) {

    // Set reward asset. A value of 10000 is equivalent to 1 Telos
    eosio::asset reward = eosio::asset( reward_amt, symbol(symbol_code("TLOS"),4));

    // Use an inline action to send reward to miner
    // TODO: Permission should be made to somewhere else
    action(
        permission_level{ get_self(), "active"_n },
        "eosio.token"_n, "transfer"_n,
        std::make_tuple( get_self(), miner, reward, string("") )
    ).send();
    
    _launches.modify(launch_itr, get_self(), [&](auto& launch) {
      launch.level_reached = last_level - 100;
    });
  }

  // Add the row to the observation set
  _observations.emplace(get_self(), [&](auto& obs) {
    obs.launch_id= launch_id;
    obs.unix_time = unix_time;
    obs.pressure_hpa = pressure_hpa;
    obs.temperature_c = temperature_c;
    obs.dewPoint_c = dewPoint_c;
  });

}

ACTION wxlaunches::websetsixhr(name launch_id,
                               name miner, 
                               string wx6hrcondition) {
  // Only web app can run this Action
  require_auth("weatherapppp"_n);

  launches_table_t _launches(get_self(), get_self().value);  
  auto launch_itr = _launches.find(launch_id.value);

  // Add the currently stored variables to the launches table
  if ( launch_itr->miner == miner )
  {
    _launches.modify(launch_itr, get_self(), [&](auto& launch) {
      launch.wx6hrcondition = wx6hrcondition;
    });
  }

}   

// Dispatch the actions to the blockchain
EOSIO_DISPATCH(wxlaunches, (addobs)(setstation)(websetlaunch)(websetsixhr))
