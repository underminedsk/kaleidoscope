from sqlalchemy import create_engine
from sqlalchemy import Column, Integer, String, Boolean
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker

Base = declarative_base()

class User(Base):
    __tablename__ = 'users'

    id = Column(Integer, primary_key=True)
    uid = Column(String, nullable=False)
    name = Column(String, nullable=True)
    done1 = Column(Boolean, default=False)
    done2 = Column(Boolean, default=False)
    done3 = Column(Boolean, default=False)
    alldone = Column(Boolean, default=False)
    tries1 = Column(Integer, default=0)
    tries2 = Column(Integer, default=0)
    tries3 = Column(Integer, default=0)
    video1 = Column(String)
    video2 = Column(String)
    video3 = Column(String)
    video4 = Column(String)

    def __repr__(self):
        return '<User(id=%s, uid=%s, name=%s, done1=%s, done2=%s, done3=%s, alldone=%s, tries1=%s, tries2=%s, tries3=%s, video1=%s, ' \
               'video2=%s, video3=%s, video4=%s)>' % (self.id, self.uid, self.name, self.done1, self.done2, self.done3, self.alldone, self.tries1,
                                                   self.tries2, self.tries3, self.video1, self.video2, self.video3, self.video4)




class KaleidoscopeDBClient(object):

    def __init__(self, db_file_path, show_sql=False):
        self.db_file_path = db_file_path
        self.show_sql = show_sql

    def __enter__(self):
        engine = create_engine('sqlite:///%s' % self.db_file_path, echo=self.show_sql)
        Base.metadata.create_all(engine)
        Session = sessionmaker(bind=engine)
        self.session = Session()
        return self

    def save(self, user):
        self.session.add(user)
        self.session.commit()

    def find_by_id(self, user_id):
        return self.session.query(User).filter(User.id==user_id).first()

    def find_by_name(self, name):
        return self.session.query(User).filter(User.name==name).all()
		
    def find_by_uid(self, uid):
        return self.session.query(User).filter(User.uid==uid).first()	

    def find_all(self):
        return self.session.query(User).all()

    def __exit__(self, exc_type, exc_val, exc_tb):
        if not exc_val:
            self.session.commit()
        else:
            self.session.rollback()



if __name__ == '__main__':

    #connects to kaleidoscope_users.db database file. in current working dir if one doesnt exist a new file is created
    with KaleidoscopeDBClient('kaleidoscope_users.db') as client:

        #create a new user object.  only uid is required.
        user = User(uid='E1D2F3AB')
        client.save(user)
        print user #note an id has been assigned and default values have been set upon save

        #retrieve that user by id:
        user = client.find_by_uid('E1D2F3AB')
        print user

        #retrieve a user by name (note this function returns a list, not a single row)
        print client.find_by_name('underminedsk')

        #this user doesnt exist yet, so find_by_name returns None
        print client.find_by_name('banjoisland')

        #update a user
        user.done1 = True
        client.save(user)

        print user #fields on user have been updated after save







