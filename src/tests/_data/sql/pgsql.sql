
DROP TABLE IF EXISTS test;

CREATE TABLE IF NOT EXISTS test (
  id SERIAL PRIMARY KEY,
  name varchar(1024) NOT NULL,
  created_at timestamp NULL DEFAULT NULL,
  updated_at timestamp NULL DEFAULT NULL
);

DROP TABLE IF EXISTS test2;

CREATE TABLE IF NOT EXISTS test2 (
  id SERIAL PRIMARY KEY,
  test_id int NOT NULL,
  value varchar(255) NOT NULL
);

DROP TABLE IF EXISTS test_intermediate;

CREATE TABLE IF NOT EXISTS test_intermediate (
  test_id int NOT NULL,
  test_related_id int NOT NULL,
  intermediate_value varchar(1024) DEFAULT NULL,
  created_at timestamp NULL DEFAULT NULL,
  updated_at timestamp NULL DEFAULT NULL
);

DROP TABLE IF EXISTS test_related;

CREATE TABLE IF NOT EXISTS test_related (
  id SERIAL PRIMARY KEY,
  value varchar(1024) NOT NULL,
  created_at timestamp NULL DEFAULT NULL,
  updated_at timestamp NULL DEFAULT NULL
);

CREATE TABLE author(
	author_id SERIAL PRIMARY KEY,
	parent_author_id INT,
	publisher_id INT,
	name VARCHAR(25) NOT NULL DEFAULT 'default_name',
	updated_at timestamp,
	created_at timestamp,
	"some_Date" date,
	some_time time,
	some_text text,
	encrypted_password varchar(50),
	"mixedCaseField" varchar(50)
);

CREATE TABLE book(
	book_id SERIAL PRIMARY KEY,
	author_id INT,
	secondary_author_id INT,
	name VARCHAR(50),
	numeric_test VARCHAR(10) DEFAULT '0',
	special NUMERIC(10,2) DEFAULT 0.0
);

CREATE TABLE publisher(
	publisher_id SERIAL PRIMARY KEY,
	name VARCHAR(25) NOT NULL DEFAULT 'default_name'
);

CREATE TABLE venue (
	id SERIAL PRIMARY KEY,
	name varchar(50),
	city varchar(60),
	state char(2),
	address varchar(50),
	phone varchar(10) default NULL,
	UNIQUE(name,address)
);

CREATE TABLE event (
	id SERIAL PRIMARY KEY,
	venue_id int NULL,
	host_id int NOT NULL,
	title varchar(60) NOT NULL,
	description varchar(10),
	type varchar(15) default NULL
);

CREATE TABLE host(
	id SERIAL PRIMARY KEY,
	name VARCHAR(25)
);

CREATE TABLE employee (
	id SERIAL PRIMARY KEY,
	first_name VARCHAR(255) NOT NULL,
	last_name VARCHAR(255) NOT NULL,
	nick_name VARCHAR(255) NOT NULL
);

CREATE TABLE position (
	id SERIAL PRIMARY KEY,
	employee_id int NOT NULL,
	title VARCHAR(255) NOT NULL,
	active SMALLINT NOT NULL
);

CREATE TABLE "rm-bldg" (
    "rm-id" SERIAL PRIMARY KEY,
    "rm-name" VARCHAR(10) NOT NULL,
    "space out" VARCHAR(1) NOT NULL
);

CREATE TABLE awesome_people(
	id serial primary key,
	author_id int,
	is_awesome int default 1
);

CREATE TABLE amenity (
	amenity_id serial primary key,
	type varchar(40) NOT NULL
);

CREATE TABLE property(
	property_id serial primary key
);

CREATE TABLE property_amenity (
	id serial primary key,
	amenity_id int not null,
	property_id int not null
);

CREATE TABLE "user" (
	id serial primary key
);

CREATE TABLE newsletter(
	id serial primary key
);

CREATE TABLE user_newsletter (
  id serial primary key,
  user_id int not null,
  newsletter_id int not null
);

CREATE TABLE valuestore (
  id serial primary key,
  key varchar(20) NOT NULL DEFAULT '',
  value varchar(255) NOT NULL DEFAULT ''
);

-- reproduces issue GH-96 for testing
CREATE INDEX user_newsletter_id_and_user_id_idx ON user_newsletter USING btree(id, user_id);

CREATE TABLE IF NOT EXISTS payment (
  id serial primary key,
  memberid int not null,
  emailid int not null,
  status varchar(15) not null,
  total_cost int not null,
  trans_fee int not null,
  trans_code varchar(80) not null,
  created_at timestamp not null
);

-- --------------------------------------------------------

--
-- Table structure for table pay_item
--

CREATE TABLE IF NOT EXISTS pay_item (
  trans_id int not null,
  id int not null,
  detail varchar(255) default null,
  amount int not null,
  purpose varchar(15) not null,
  PRIMARY KEY (trans_id,id)
);