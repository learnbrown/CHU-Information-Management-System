BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS "teachers" (
	"id"	INTEGER,
	"name"	TEXT,
	"course_name"	TEXT,
	"password"	INTEGER,
	"course1"	TEXT,
	"course2"	TEXT
);
CREATE TABLE IF NOT EXISTS "students" (
	"id"	INTEGER,
	"name"	TEXT,
	"class"	INTEGER,
	"password"	INTEGER,
	"course1"	TEXT,
	"course2"	TEXT,
	"score1"	INTEGER,
	"score2"	INTEGER,
	"phone_number"	TEXT,
	"gender"	INTEGER,
	"wish"	TEXT,
	"able_to_revise1"	INTEGER,
	"able_to_revise2"	INTEGER
);
CREATE TABLE IF NOT EXISTS "requests_teacher" (
	"req_id"	TEXT,
	"stu_id"	INTEGER,
	"option"	TEXT,
	"new_score"	INTEGER
);
CREATE TABLE IF NOT EXISTS "requests_student" (
	"req_id"	TEXT,
	"id"	INTEGER,
	"name"	TEXT,
	"gender"	INTEGER,
	"phone_number"	TEXT,
	"wish"	TEXT
);

UPDATE student SET score1 = -1,   score2 = -1,
 phone_number = "-1", gender = -1, wish = "保内", able_to_revise1 = 1, able_to_revise2 = 1;

COMMIT;