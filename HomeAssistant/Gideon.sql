select state_id,entity_id,state,last_changed,last_updated from states where DATE(last_updated) = DATE(NOW() - INTERVAL 1 DAY);

SELECT last_updated,
       STATE
FROM states
WHERE entity_id='sensor.front_outside_temperature'
  AND STATE != 'unknown';
  
SELECT last_updated,
       STATE
FROM states
WHERE entity_id='sensor.solar_battery_voltage'
  AND STATE != 'unknown';
  
select * from general_log_data.home_power_use;

insert into general_log_data.home_power_use (rec_date,kwh) values
('2017-10-01',46),
('2017-10-02',47),
('2017-10-03',54),
('2017-10-04',48),
('2017-10-05',55),
('2017-10-06',56);

select state,last_updated from states where entity_id='sensor.front_outside_temperature';
select state,last_updated from states where entity_id='sensor.solar_battery_voltage'; 

SELECT table_schema, table_name, column_name
    FROM information_schema.columns where table_schema='public';
    

SELECT `AUTO_INCREMENT`
FROM  INFORMATION_SCHEMA.TABLES
WHERE TABLE_NAME   = 'dam_levels';

select * from dam_levels;

--remove the unknowns from the database
select state,last_updated from states where entity_id='sensor.front_outside_temperature' and state='unknown';
select state,last_updated from states where entity_id='sensor.solar_battery_voltage' and state='unknown'; 

delete from states where entity_id='sensor.solar_battery_voltage'  and state = 'unknown'; 
delete from states where  entity_id='sensor.front_outside_temperature' and state = 'unknown';