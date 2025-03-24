-- INTEGER works for autoinc == ROWID alias, but INT does not!
CREATE TABLE test (
  id INTEGER PRIMARY KEY,
  name varchar(1024) NOT NULL,
  created_at timestamp DEFAULT NULL,
  updated_at timestamp DEFAULT NULL
);

CREATE TABLE test2 (
  id INTEGER PRIMARY KEY,
  test_id int NOT NULL,
  value varchar(255) NOT NULL
);

CREATE TABLE test_intermediate (
  test_id int NOT NULL,
  test_related_id int NOT NULL,
  intermediate_value varchar(1024) DEFAULT NULL,
  created_at timestamp DEFAULT NULL,
  updated_at timestamp DEFAULT NULL,
  PRIMARY KEY(test_id, test_related_id)
);

CREATE TABLE IF NOT EXISTS test_related (
  id INTEGER PRIMARY KEY,
  value varchar(1024) NOT NULL,
  created_at timestamp DEFAULT NULL,
  updated_at timestamp DEFAULT NULL
);


CREATE TABLE author (
	author_id INTEGER  PRIMARY KEY,
	parent_author_id INT,
	publisher_id INT,
	name VARCHAR(25) NOT NULL DEFAULT default_name, 
	updated_at timestamp,
	created_at timestamp,
	some_Date date,
	some_time time,
	some_text text,
	encrypted_password varchar(50),
	mixedCaseField varchar(50)
);

CREATE TABLE book (
	book_id INTEGER PRIMARY KEY,
	Author_id INT,
	secondary_author_id INT,
	name VARCHAR(50),
	numeric_test VARCHAR(10) DEFAULT '0',
	special NUMERIC(10,2) DEFAULT 0
);

CREATE TABLE publisher (
	publisher_id INTEGER PRIMARY KEY,
	name VARCHAR(25) NOT NULL DEFAULT default_name
);

CREATE TABLE `venue` (
  `id` INTEGER PRIMARY KEY,
  `name` varchar(50),
  `city` varchar(60),
  `state` char(2),
  `address` varchar(50),
  `phone` varchar(10) default NULL,
  UNIQUE(`name`,`address`)
);

CREATE TABLE event (
  id INTEGER PRIMARY KEY,
  venue_id int NULL,
  host_id int NOT NULL,
  title varchar(60) NOT NULL,
  description varchar(10),
  type varchar(15) default NULL
);

CREATE TABLE `host` (
	id INTEGER PRIMARY KEY,
	name VARCHAR(25)
);

CREATE TABLE employee (
	id INTEGER  PRIMARY KEY,
	first_name VARCHAR( 255 ) NOT NULL ,
	last_name VARCHAR( 255 ) NOT NULL ,
	nick_name VARCHAR( 255 ) NOT NULL
);

CREATE TABLE `position` (
  id INTEGER PRIMARY KEY,
  employee_id int NOT NULL,
  title VARCHAR(255) NOT NULL,
  active SMALLINT NOT NULL
);

CREATE TABLE `rm-bldg`(
    `rm-id` INTEGER NOT NULL,
    `rm-name` VARCHAR(10) NOT NULL,
    `space out` VARCHAR(1) NOT NULL
);

CREATE TABLE awesome_people(
	id INTEGER primary key,
	author_id int,
	is_awesome int default 1
);

CREATE TABLE `amenity` (
  `amenity_id` INTEGER PRIMARY KEY,
  `type` varchar(40) DEFAULT NULL
);

CREATE TABLE property(
  `property_id` INTEGER PRIMARY KEY
);

CREATE TABLE property_amenities(
  `id` INTEGER PRIMARY KEY,
  `amenity_id` INT NOT NULL,
  `property_id` INT NOT NULL
);

CREATE TABLE `user` (
    id INTEGER PRIMARY KEY
);

CREATE TABLE newsletter (
    id INTEGER PRIMARY KEY
);

CREATE TABLE user_newsletter (
    id INTEGER PRIMARY KEY,
    user_id INT NOT NULL,
    newsletter_id INT NOT NULL
);

CREATE TABLE valuestore (
  `id` INTEGER PRIMARY KEY,
  `key` varchar(20) NOT NULL DEFAULT '',
  `value` varchar(255) NOT NULL DEFAULT ''
);

CREATE TABLE IF NOT EXISTS `payment` (
  `id` INTEGER PRIMARY KEY,
  `memberid` INTEGER NOT NULL,
  `emailid` INTEGER NOT NULL,
  `status` varchar(15) NOT NULL,
  `total_cost` INTEGER NOT NULL,
  `trans_fee` INTEGER NOT NULL,
  `trans_code` varchar(80) NOT NULL,
  `created_at` datetime NOT NULL
);

-- --------------------------------------------------------

--
-- Table structure for table `pay_item`
--

CREATE TABLE IF NOT EXISTS `pay_item` (
  `trans_id` INTEGER NOT NULL,
  `id` INTEGER NOT NULL,
  `detail` varchar(255) DEFAULT NULL,
  `amount` INTEGER NOT NULL,
  `purpose` varchar(15) NOT NULL,
  PRIMARY KEY (`trans_id`,`id`)
);