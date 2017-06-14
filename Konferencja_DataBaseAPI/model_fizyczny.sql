
create type talk_status as enum ('proposed', 'rejected', 'accepted');
--
create type user_type as enum ('organizer', 'normal');
--
create table event (
  event_name text primary key not null ,
  start_time timestamp not null  ,
  end_time timestamp not null );
--
create table talk (
  talk text  primary key not null ,
  start_time timestamp not null ,
  room integer,
  title text not null  ,
  status talk_status not null  );
--
create table users (
  login text primary key not null ,
  password text not null  ,
  type user_type not null default 'normal' );
--
create table register (
  login text not null  references users on delete cascade ,
  event_name   text  not null references event on delete cascade,
  unique (login,event_name) );
--
create table talk_in_event(
  talk text not null  references talk on delete cascade,
  event_name text references event on delete cascade,
  unique (talk)
  );
--
create table attend (
  login text not null  references users on delete cascade,
  talk text not null  references talk on delete cascade,
  unique (login, talk)
);
--
create table propose(
  login text not null references  users on delete cascade,
  talk text not null references   talk on delete cascade,
  time_of_p timestamp not null default   now(),
  unique (talk)
);
--
create table evaluate(
  login text not null  references users on delete cascade,
  talk text not null  references talk on delete cascade,
  grade integer not null  check (grade between 0 and 10),
  unique (login, talk)
);
--
create table want_to_be_friend(
  login text not null  references users on delete cascade,
  want_friend text not null   references users on delete cascade,
  unique (login, want_friend)
);
--
create table friends(
  login text  not null references users on delete cascade,
  has_friend text not null  references users on delete cascade,
  unique (login, has_friend) );
--

create or replace function fun_friend_trigger()
  returns trigger as
  $X$
  BEGIN
    if exists (select * from friends where login = new.want_friend and has_friend = new.login)
    then return null;
    end if;
    if  exists (select * from want_to_be_friend where login = new.want_friend and want_friend = new.login)
    then
      insert into friends values (new.want_friend, new.login), (new.login, new.want_friend);
      delete from want_to_be_friend where login = new.want_friend and want_friend = new.login;
      return null;
    end if;
    return new;
  END
  $X$
  language PLpgSQL;
--
create or replace function fun_attendance_trigger()
  returns trigger as
  $X$
  BEGIN
    if exists (select * from talk where talk = new.talk and status = 'accepted')
    then return new;
    end if;
    return null;
  END
  $X$
  language PLpgSQL;
--
  create or replace function fun_talkinevent_trigger()
  returns trigger as
  $X$
  declare
  talks_s timestamp;
  event_s timestamp;
  event_e timestamp;
  BEGIN
  if new.event_name is null then return new; else
  select start_time into talks_s from talk where talk = new.talk;
  select start_time, end_time into event_s, event_e from event where event_name = new.event_name;
  if (talks_s between event_s and event_e)
  then return new;
  else
  raise  'talk not in event time' ;
  end if;
  end if;

  END
  $X$
  language PLpgSQL;
--
create or replace function score( talk text , login text )
  returns real as
  $X$
  declare
    friends_in_talk integer;
    all_in_talk integer;
    avg_grade real;
  BEGIN
    select count(f.has_friend) into friends_in_talk
    from  friends f join register r on (r.login = f.has_friend)
    join talk_in_event te using(event_name)
    where
      te.talk = $1
      and f.login = $2;

    select count(t.login) into all_in_talk from register t join talk_in_event te using(event_name)
    where t.login not like $2;

    select avg(e.grade) into avg_grade from evaluate e where e.talk = $1;

    IF (avg_grade is NULL ) THEN  select 0 into avg_grade; END IF;
    IF (all_in_talk = 0) THEN return avg_grade; END IF;
    return (friends_in_talk ::real / all_in_talk :: real) :: real * 5.0 + avg_grade
     ;
  END
  $X$
  language PLpgSQL;
--

drop  trigger if exists friend_trigger on want_to_be_friend;
--
create trigger friend_trigger before insert on want_to_be_friend for each row
  execute procedure fun_friend_trigger();
--

drop  trigger if exists attendance_trigger on attend;
--
create trigger attendance_trigger before insert on attend for each row
  execute procedure fun_attendance_trigger();
--

drop trigger if exists talkinevent_trigger on talk_in_event;
--
create trigger talkinevent_trigger before insert on talk_in_event for each row
    execute procedure fun_talkinevent_trigger();
--
