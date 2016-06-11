from kaleidoscope_db_client import KaleidoscopeDBClient, User


with KaleidoscopeDBClient('/Users/zkozick/Desktop/kaleidoscope_users_1521_saturday.db') as client:

    #
    #
    # to_delete = ['d9767f3b', '34a1ea5e', '7449f75e', 'ec9ba821']
    # for uid in to_delete:
    #     by_uid = client.find_by_uid(uid)
    #     by_uid.uid = ''
    #     video4 = None
    #     print by_uid
    #     client.save(by_uid)
    #     #client.delete(uid)
    #
    all = client.find_all()
    for a in all:
        print a.uid, a.name



    #
    # for user in all:
    #     print user.uid
    #     print user