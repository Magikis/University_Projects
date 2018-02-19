#! /usr/bin/env python3
import json
# import psycopg2
import functions as f


aviable_fucntions = {
    "open": f.open_db,
    "organizer": f.organizer,
    "user": f.user,
    "event": f.event,
    "talk": f.talk,
    "register_user_for_event": f.register_user_for_event,
    "attendance": f.attendance,
    "evaluation": f.evaluation,
    "reject": f.reject,
    "proposal": f.proposal,
    "friends": f.friends,
    "user_plan": f.user_plan,
    "day_plan": f.day_plan,
    "best_talks": f.best_talks,
    "most_popular_talks": f.most_popular_talks,
    "attended_talks": f.attended_talks,
    "abandoned_talks": f.abandoned_talks,
    "recently_added_talks": f.recently_added_talks,
    "rejected_talks": f.rejected_talks,
    "proposals": f.proposals,
    "friends_talks": f.friends_talks,
    "friends_events": f.friends_events,
    "recommended_talks": f.recommended_talks
}


def test(conn):
    print(f.is_user(conn, {"login": 'a', "password": 'c'}))
    return


def main():
    conn = None
    it = 1
    try:
        while True:
            # print(it)
            it += 1
            file_line = input()
            json_line = json.loads(file_line)
            keys = list(json_line.keys())
            if len(keys) != 1:
                print("Unable to parse")
                continue
            function_name = keys[0]
            args = json_line[function_name]
            fun_to_run = aviable_fucntions.get(function_name)
            if fun_to_run is None:
                print("No name of {} fucntion", function_name)
                continue
            elif function_name == "open":
                conn, output = f.open_db(args)
            else:
                output = aviable_fucntions[function_name](conn, args)
            print(json.dumps(output, indent=1, sort_keys=True))
    except EOFError:
        pass
    # conn.close()
    return 0


if __name__ == "__main__":
    main()
