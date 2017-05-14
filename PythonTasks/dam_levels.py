import time
import requests
from sqlalchemy import Column, ForeignKey, Integer, String, Float, Date
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import relationship
from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker
from bs4 import BeautifulSoup

def AddDamShouldWeNeedTo(local_session,dam_name):
	does_it_exist = local_session.query(DamName).filter(DamName.dam_name==dam_name).count()

	if (does_it_exist == 0): 
		# Insert a Person in the person table
		new_dam = DamName(dam_name=dam_name)
		local_session.add(new_dam)
		local_session.commit()

page = requests.get("http://www.umgeni.co.za/media_centre/drd.asp")

soup = BeautifulSoup(page.content, 'html.parser')

table = soup.find_all('table')

row_data = table[3].find_all("tr")
print (row_data[8])


Base = declarative_base()

class DamName(Base):
    __tablename__ = 'dam_names'
    id = Column(Integer, primary_key=True)
    dam_name = Column(String(250), nullable=False)

class DamLevel(Base):
    __tablename__ = 'dam_levels'
    # Here we define columns for the table address.
    # Notice that each column is also a normal Python instance attribute.
    id = Column(Integer, primary_key=True)
    dam_level = Column(Float)
    record_date=Column(Date)
    dam_id = Column(Integer, ForeignKey('dam_names.id'))
    dam = relationship(DamName)

engine = create_engine("postgresql://postgres:empyrean69@localhost:5432/homedata")

#If the table does not exist then we make all the tables
if not engine.dialect.has_table(engine,'dam_names'):
	Base.metadata.create_all(engine)

Base.metadata.bind = engine

DBSession = sessionmaker(bind=engine)
# A DBSession() instance establishes all conversations with the database
# and represents a "staging zone" for all the objects loaded into the
# database session object. Any change made against the objects in the
# session won't be persisted into the database until you call
# session.commit(). If you're not happy about the changes, you can
# revert all of them back to the last commit by calling
# session.rollback()
session = DBSession()

col_data = row_data[8].find_all("td")
print("Dam: "+col_data[1].get_text())
AddDamShouldWeNeedTo(session,col_data[1].get_text())
print("Level: "+col_data[3].get_text())

id_value = session.query(DamName.id).filter(DamName.dam_name==col_data[1].get_text())
new_dam_level = DamLevel(dam_id=id_value,dam_level = float(col_data[3].get_text()[:len(col_data[3].get_text())-1]),record_date = time.strftime('%Y-%m-%d'))
session.add(new_dam_level)
session.commit()			

col_data = row_data[6].find_all("td")
print("Dam: "+col_data[1].get_text())
AddDamShouldWeNeedTo(session,col_data[1].get_text())
print("Level: "+col_data[3].get_text())

id_value = session.query(DamName.id).filter(DamName.dam_name==col_data[1].get_text())
new_dam_level = DamLevel(dam_id=id_value,dam_level = float(col_data[3].get_text()[:len(col_data[3].get_text())-1]),record_date = time.strftime('%Y-%m-%d'))
session.add(new_dam_level)
session.commit()
#*****************************************************************************
# GET THE VAAL DAM STUFF FROM ANOTHER WEBSITE
#*****************************************************************************
page = requests.get("http://www.randwater.co.za/CorporateResponsibility/WWE/Pages/Damlevel.aspx")

soup = BeautifulSoup(page.content, 'html.parser')

table = soup.find_all('table')
row_data = table[6].find_all("tr")
i_table = row_data[1].find_all('table')
i_i_table = i_table[0].find_all('table')
i_row_data = i_i_table[0].find_all("tr")
i_col_data = i_row_data[0].find_all("td")
i_span_data = i_row_data[0].find_all("span")

print ("Dam: "+i_span_data[2].get_text())
AddDamShouldWeNeedTo(session,i_span_data[2].get_text())
level_text = i_span_data[5].get_text()

id_value = session.query(DamName.id).filter(DamName.dam_name==i_span_data[2].get_text())
new_dam_level = DamLevel(dam_id=id_value,dam_level = float(level_text[11:(len(level_text)-1)]),record_date = time.strftime('%Y-%m-%d'))
session.add(new_dam_level)
session.commit()
print ("Level: "+level_text[11:(len(level_text)-1)])





 

