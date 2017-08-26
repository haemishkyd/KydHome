--get the basic data
select state,last_updated from homeassistant.states where entity_id='sensor.front_outside_temperature';
select state,last_updated from homeassistant.states where entity_id='sensor.solar_battery_voltage'; 

--get the "unknown" data
select state,last_updated from homeassistant.states where entity_id='sensor.front_outside_temperature' and state = 'unknown';
select state,last_updated from homeassistant.states where entity_id='sensor.solar_battery_voltage' and state = 'unknown'; 

--delete the "unknown" data
delete from homeassistant.states where  entity_id='sensor.front_outside_temperature' and state = 'unknown';
delete from homeassistant.states where entity_id='sensor.solar_battery_voltage'  and state = 'unknown'; 

--MySQL Front Outside Temperature Last Week
select cast(state as UNSIGNED),last_updated from homeassistant.states where entity_id='sensor.front_outside_temperature'
and last_updated BETWEEN CURDATE() - INTERVAL 7 DAY AND CURDATE() ORDER BY last_updated DESC;

--MySQL Solar Battery Voltage
select cast(state as decimal(5,2)),last_updated from homeassistant.states where entity_id='sensor.solar_battery_voltage'
and last_updated BETWEEN CURDATE() - INTERVAL 7 DAY AND CURDATE() ORDER BY last_updated DESC;
