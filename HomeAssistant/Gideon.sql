select state,last_updated from states where entity_id='sensor.front_outside_temperature';
select state,last_updated from states where entity_id='sensor.solar_battery_voltage'; 

SELECT table_schema, table_name, column_name
    FROM information_schema.columns where table_schema='public';
    
CREATE TABLE dam_names(
   id   INT              NOT NULL,
   dam_name VARCHAR (20)     NOT NULL,
   PRIMARY KEY (id));
   
insert into dam_names (id,dam_name) values (1,'Spring Grove');

select * from dam_names;
select * from dam_levels;
select * from dam_levels where record_date = '2017-04-23' and dam_id=1;

update dam_levels set dam_level=87.2 where record_date='2017-04-24' and dam_id=1;ame

select * from dam_levels where id =4;

SELECT `AUTO_INCREMENT`
FROM  INFORMATION_SCHEMA.TABLES
WHERE TABLE_NAME   = 'dam_levels';

select * from dam_levels;

--remove the unknowns from the database
select state,last_updated from states where entity_id='sensor.front_outside_temperature' and state='unknown';
select state,last_updated from states where entity_id='sensor.solar_battery_voltage' and state='unknown'; 

delete from states where entity_id='sensor.solar_battery_voltage'  and state = 'unknown'; 
delete from states where  entity_id='sensor.front_outside_temperature' and state = 'unknown';

SELECT *, pg_size_pretty(total_bytes) AS total
    , pg_size_pretty(index_bytes) AS INDEX
    , pg_size_pretty(toast_bytes) AS toast
    , pg_size_pretty(table_bytes) AS TABLE
  FROM (
  SELECT *, total_bytes-index_bytes-COALESCE(toast_bytes,0) AS table_bytes FROM (
      SELECT c.oid,nspname AS table_schema, relname AS TABLE_NAME
              , c.reltuples AS row_estimate
              , pg_total_relation_size(c.oid) AS total_bytes
              , pg_indexes_size(c.oid) AS index_bytes
              , pg_total_relation_size(reltoastrelid) AS toast_bytes
          FROM pg_class c
          LEFT JOIN pg_namespace n ON n.oid = c.relnamespace
          WHERE relkind = 'r'
  ) a
) a;

SELECT "public"."dam_levels"."record_date" AS "record_date", "public"."dam_levels"."dam_level" AS "dam_level", "public"."dam_levels"."dam_id" AS "dam_id",  "public"."dam_names"."dam_name" AS "dam_name"
FROM "public"."dam_levels","public"."dam_names" WHERE "public"."dam_levels"."dam_id" = '2' AND "public"."dam_levels"."dam_id" = "public"."dam_names"."id"
LIMIT 2000;