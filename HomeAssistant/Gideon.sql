select state,last_updated from states where entity_id='sensor.front_outside_temperature';
select state,last_updated from states where entity_id='sensor.solar_battery_voltage'; 

SELECT table_schema, table_name, column_name
    FROM information_schema.columns where table_schema='public';
    
CREATE TABLE dam_names(
   id   INT              NOT NULL,
   dam_name VARCHAR (20)     NOT NULL,
   PRIMARY KEY (id));
   
insert into dam_names (id,dam_name) values (1,'Spring Grove');

select * from dam_levels;
