select state,last_updated from states where entity_id='sensor.front_outside_temperature';
select state,last_updated from states where entity_id='sensor.solar_battery_voltage' and last_updated > '2017-05-27' and cast(state as float) < 10.0; 

SELECT table_schema, table_name, column_name
    FROM information_schema.columns where table_schema='public';
    
CREATE TABLE dam_names(
   id   INT              NOT NULL,
   dam_name VARCHAR (20)     NOT NULL,
   PRIMARY KEY (id));
   
insert into dam_names (id,dam_name) values (1,'Spring Grove');

select * from dam_levels where record_date = '2017-04-23' and dam_id=1;

update dam_levels set dam_level=87.2 where record_date='2017-04-24' and dam_id=1;

select * from dam_levels where id =4;

SELECT `AUTO_INCREMENT`
FROM  INFORMATION_SCHEMA.TABLES
WHERE TABLE_NAME   = 'dam_levels';

select * from dam_levels;

delete from states where entity_id='sensor.solar_battery_voltage' and last_updated > '2017-05-27' and cast(state as float) < 10.0; 
