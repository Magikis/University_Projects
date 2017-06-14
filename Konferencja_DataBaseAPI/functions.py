import psycopg2


ok = 0
err = 1

def status( x ):
    if x == ok:
        ansewer = "OK"
    elif type(x) == type( [] ):
        return {"status" : "OK", "data" : x}
    else :
        ansewer = "ERROR"
    return {"status" : ansewer}

def is_user(conn, dict):
    sql = "select * from users where login = %(login)s and password = %(password)s "
    row = read_one_rec(conn, sql, dict)
    return row != None

def is_organizer(conn, dict):
    sql = "select * from users where login = %(login)s and password = %(password)s "
    row = read_one_rec(conn,sql, dict)
    if row == None:
        return False
    return row[2]  == "organizer"


def insert_record(conn, dict, sql):
    cur = conn.cursor()
    try:
        cur.execute(sql, dict)
        cur.close()
        conn.commit()
    except psycopg2.DatabaseError as e:
        # print("Eception: " + str(e))
        cur.close()
        conn.rollback()
        return status(err)
    return status(ok)

def execute_querry(conn, dict, sql, ret_args):
    cur = conn.cursor()
    try:
        cur.execute(sql, dict)
    except psycopg2.DatabaseError as e :
        # print("Eception: " + str(e))
        cur.close()
        conn.rollback()
        return None
    query = cur.fetchall()
    ansewer =   []
    for row in query:
        d = {}
        for i in range(len(ret_args)):
            d.update({ ret_args[i] : str(row[i]) })
        ansewer.append(d)
    return ansewer

def read_one_rec(conn, sql, dict):
    cur  = conn.cursor()
    try:
        cur.execute(sql, dict)
    except psycopg2.DatabaseError as e:
        # print("Eception: " + str(e))
        return None
    row = cur.fetchone()
    cur.close()
    return row

def magic(sql):
    sql = sql.split(";")
    return list( map(lambda x : x + ";" , sql) )

def open_db(dict):
    try:
        conn =  psycopg2.connect("dbname={} user={} password={}".format(
            dict.get("baza"), dict.get("login"), dict.get("password")  )  )
    except psycopg2.DatabaseError as e :
        return None, status(err)
    sql = """select exists ( select * from INFORMATION_SCHEMA.TABLES
    where TABLE_NAME = 'users' ) """
    row = read_one_rec(conn, sql, dict)
    if row != None and row[0] == False :
        with open("model_fizyczny.sql", 'r') as file :
            sql =  file.read().split("--")
            sql.pop()
            file.close()
            for s in sql:
                stat = insert_record(conn, dict, s)
                if stat == status(err):
                    # print("Error on: " + s)
                    return conn,stat
                conn.commit()

    return conn, status(ok)
# open <baza> <login> <password>


def organizer( conn, dict ):
    sql = "insert into users values ( %(newlogin)s , %(newpassword)s , 'organizer' )"
    if dict.get("secret") == "d8578edf8458ce06fbc5bb76a58c5ca4":
        return insert_record(conn, dict, sql)
    else:
        return status(err)

def event(conn, dict):
    if not is_organizer(conn, dict):
        return status(err)
    sql = "insert into event values ( %(eventname)s, %(start_timestamp)s, %(end_timestamp)s);"
    return insert_record(conn, dict, sql)

def user(conn,dict):
    if not is_organizer(conn, dict):
        return status(err)
    sql = " insert into users values ( %(newlogin)s, %(newpassword)s );"
    return insert_record(conn, dict, sql)

# DO POPRAWIENIA
def talk(conn, dict):
    if not is_organizer(conn, dict):
        return status(err)
    sql = "select talk,status from talk where talk = %(talk)s ;"
    row = read_one_rec(conn, sql, dict)
    if  row != None and row[1] == 'rejected':
        return status(err)
    if dict.get("eventname") == "":
        dict.update({"eventname" : None})
    sql = """ insert into talk (talk, start_time, room, title, status)
            VALUES ( %(talk)s, %(start_timestamp)s, %(room)s , %(title)s , 'accepted'  )
            ON CONFLICT (talk) DO update
              set start_time = %(start_timestamp)s,
                  room = %(room)s ,
                  title =  %(title)s ,
                  status =  'accepted';

            insert into propose (login, talk)
            values (  %(speakerlogin)s, %(talk)s )
            on CONFLICT (talk) DO update
              set login = %(speakerlogin)s ;

            insert into talk_in_event (talk, event_name)
            values ( %(talk)s, %(eventname)s )
            on CONFLICT do nothing;

            insert into evaluate
            values (%(login)s, %(talk)s, %(initial_evaluation)s )
            on CONFLICT do nothing; """
    return insert_record(conn, dict, sql)
#KONIEC
# talk <login> <password> <speakerlogin> <talk> <title> <start_timestamp> <room> <initial_evaluation> <eventname>
def register_user_for_event(conn, dict):
    if not is_user(conn, dict):
        return status(err)
    sql = "insert into register values (%(login)s, %(eventname)s);"
    return insert_record(conn, dict, sql)

def attendance(conn, dict):
    if not is_user(conn, dict):
        return status(err)
    sql = "insert into attend values (%(login)s, %(talk)s);"
    return insert_record(conn, dict, sql)

# (*U) evaluation <login> <password> <talk> <rating> // ocena referatu <talk> w skali 0-10 przez uczestnika <login>
def evaluation(conn, dict):
    if not is_user(conn, dict):
        return status(err)
    sql = "insert into evaluate values (%(login)s, %(talk)s, %(rating)s) "
    return insert_record(conn, dict, sql)


def reject(conn, dict):
    if not is_organizer(conn, dict):
        return status(err)
    sql = " update talk set status = 'rejected' where talk = %(talk)s ;"
    return insert_record(conn, dict, sql)

def proposal(conn, dict):
    if not is_user(conn,dict):
        return status(err)
    sql = """ insert into talk values ( %(talk)s , %(start_timestamp)s,
     NULL , %(title)s, 'proposed' );
     insert into propose values (%(login)s, %(talk)s ); """
    return insert_record(conn, dict, sql)

# want_to_be_friend trigger
def friends(conn, dict):
    dict.update({"login" : dict.get("login1")})
    if not is_user(conn, dict):
        return status(err)
    sql = " insert into want_to_be_friend values (%(login)s, %(login2)s );"
    return insert_record(conn, dict, sql)

def user_plan(conn, dict):
    sql = """select pr.login, p.talk, p.start_time, p.title, p.room
     from  event e join register  r using(event_name)
      join talk_in_event  te using(event_name)
      join talk p using(talk)
      join propose pr using(talk)
      where r.login = %(login)s and p.status = 'accepted'
      and p.start_time > now()
      order by p.start_time """
    if dict.get("start_timestamp") != 0:
        sql += " limit %(limit)s"
    ret_args = ["login", "talk", "start_timestamp", "title", "room"]
    return status(execute_querry(conn, dict, sql,ret_args))


def day_plan(conn, dict):
    sql = """ select talk, start_time, title, room
        from talk
        where status = 'accepted'
        and %(timestamp)s::date = start_time::date
        order by room, start_time;"""
    ret_args = [ "talk", "start_timestamp", "title", "room"]
    return status(execute_querry(conn,dict,sql,ret_args))

def best_talks(conn, dict):
    sql = """select t.talk, t.start_time, t.title,
     t.room from talk t  """
    if dict.get("all") != 1:
        sql += """
            join
            ( (select login, talk, grade from
            evaluate e join attend a using(login,talk) )
            union
            (select login, talk, grade from
            evaluate join users using(login)
            where type = 'organizer') ) ag using(talk)    """
    else:
        sql += """join evaluate e using(talk) """
    sql +=  """where status = 'accepted' and start_time between
            %(start_timestamp)s and %(end_timestamp)s
            group by t.talk, t.start_time, t.title, t.room
            order by avg(grade) desc """
    if dict.get("limit") != 0:
        sql += "limit %(limit)s "
    ret_args = ["talk", "start_timestamp", "title", "room"]
    return status(execute_querry(conn, dict, sql, ret_args) )

def most_popular_talks(conn, dict):
    sql = """select t.talk, t.start_time, t.title, t.room from talk t join attend a using(talk)
        where status = 'accepted' and start_time between
        %(start_timestamp)s and %(end_timestamp)s
        group by t.talk, t.start_time, t.title, t.room
        order by count(a.login) desc """
    if dict.get("limit") != 0:
        sql += "limit %(limit)s "
    ret_args = ["talk", "start_timestamp", "title", "room"]
    return status(execute_querry(conn, dict, sql, ret_args) )

def attended_talks(conn, dict):
    if not is_user(conn , dict):
        return status(err)
    sql = """select t.talk, t.start_time, t.title, t.room from talk t join attend a using(talk)
        where status = 'accepted' and login = %(login)s """
    ret_args = ["talk", "start_timestamp", "title", "room"]
    return status(execute_querry(conn, dict, sql, ret_args) )

def abandoned_talks( conn, dict ):
    if not is_organizer(conn , dict):
        return status(err)
    sql = """ select q.talk, t.start_time, t.title, t.room, count(q.login)
    from
    (( select  t.talk , r.login
    from register r join event e using(event_name)
      join talk_in_event te using(event_name)
      join talk t using(talk))
      except
      ( select t.talk, a.login
        from talk t join attend a using(talk)   )) q
    join talk t using(talk)
    group by q.talk, t.start_time, t.title, t.room
    order by count(q.login) desc"""
    if dict.get("limit") != 0:
        sql += " limit %(limit)s"
    ret_args = [ "talk", "start_timestamp", "title", "room", "number" ]
    return status(execute_querry(conn, dict, sql, ret_args))

def recently_added_talks( conn, dict ):
    sql = """ select t.talk, p.login, t.start_time, t.title, t.room
    from talk t join propose p using(talk)
    where t.status = 'accepted'
    order by p.time_of_p desc
     """
    if dict.get("limit") != 0:
        sql += " limit %(limit)s"
    ret_args = ["talk", "speakerlogin", "start_timestamp", "title", "room"]
    return status(execute_querry(conn, dict, sql, ret_args)  )

# (U/O) rejected_talks <login> <password> // jeśli wywołujący ma uprawnienia organizatora zwraca listę wszystkich odrzuconych referatów spontanicznych,
# w przeciwnym przypadku listę odrzuconych referatów wywołującego ją uczestnika
# //  <talk> <speakerlogin> <start_timestamp> <title>

def rejected_talks(conn, dict):
    add_sql = ""
    if not is_organizer(conn, dict):
        if is_user(conn, dict):
            add_sql = " and login = %(login)s"
        else:
            return status(err)
    sql = """select talk, login, start_time, title
            from talk join propose using(talk)
            where status = 'rejected' """ + add_sql
    ret_args = [ "talk", "speakerlogin", "start_timestamp", "title" ]
    return status( execute_querry(conn, dict, sql, ret_args) )

    # (O) proposals <login> <password> // zwraca listę propozycji referatów spontanicznych do zatwierdzenia lub odrzucenia,
    # zatwierdzenie lub odrzucenie referatu polega na wywołaniu przez organizatora funkcji talk lub reject z odpowiednimi parametrami
    # //  <talk> <speakerlogin> <start_timestamp> <title>
def proposals(conn ,dict):
    if not is_organizer(conn, dict):
        return status(err)
    sql = """select talk, login, start_time, title
            from talk join propose using(talk)
            where status = 'proposed' """
    ret_args = [ "talk", "speakerlogin", "start_timestamp", "title" ]
    return status( execute_querry(conn, dict, sql, ret_args) )



# (U) friends_talks <login> <password> <start_timestamp> <end_timestamp> <limit> // lista referatów
# rozpoczynających się w podanym przedziale czasowym wygłaszanych przez znajomych danego uczestnika posortowana wg czasu rozpoczęcia, wypisuje pierwsze <limit> referatów, przy czym 0 oznacza, że należy wypisać wszystkie
# //  <talk> <speakerlogin> <start_timestamp> <title> <room>

def friends_talks(conn, dict):
    if not is_user(conn, dict):
        return status(err)
    sql = """ select t.talk, p.login, t.start_time, t.title, t.room
        from talk t join propose p using (talk)
          join friends f on (p.login = f.has_friend)
          where
          t.status = 'accepted'
          and f.login = %(login)s
           and t.start_time between %(start_timestamp)s
           and %(end_timestamp)s"""
    if dict.get("limit") != 0:
        sql += " limit %(limit)s"
    ret_args = [ "talk", "speakerlogin", "start_timestamp", "title", "room" ]
    return status( execute_querry(conn, dict, sql, ret_args) )

# (U) friends_events <login> <password> <eventname>
# // lista znajomych uczestniczących w danym wydarzeniu
# //  <login> <eventname> <friendlogin>

def friends_events(conn, dict):
    if not is_user(conn, dict) :
        return status(err)
    sql = """select distinct %(login)s , event_name,f.has_friend
            from attend a join talk_in_event te using (talk)
              join friends f on (a.login = f.has_friend )
            where
              f.login = %(login)s
              and event_name = %(eventname)s """
    ret_args = [ "login", "eventname", "friendlogin" ]
    return status( execute_querry(conn, dict, sql, ret_args) )


# (U) recommended_talks <login> <password> <start_timestamp> <end_timestamp> <limit>
# // zwraca referaty rozpoczynające się w podanym przedziale czasowym, które mogą zainteresować danego uczestnika (zaproponuj parametr
#  <score> obliczany na podstawie dostępnych danych – ocen, obecności, znajomości itp.),
# wypisuje pierwsze <limit> referatów wg nalepszego <score>, przy czym 0 oznacza, że należy wypisać wszystkie
# //  <talk> <speakerlogin> <start_timestamp> <title> <room> <score>

def recommended_talks(conn, dict):
    if not is_user(conn, dict):
        return status(err)
    sql = """ select t.talk, p.login, t.start_time, t.title, t.room, score(t.talk, %(login)s )
        from talk t join propose p using (talk)
        where
          t.status = 'accepted'
          and t.start_time between %(start_timestamp)s
          and %(end_timestamp)s
        order by score(t.talk , %(login)s ) desc  """
    if dict.get("limit") != 0:
        sql += " limit %(limit)s"
    ret_args = [ "talk", "speakerlogin", "start_timestamp", "title", "room", "score" ]
    return status( execute_querry(conn, dict, sql, ret_args) )
