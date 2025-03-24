-- MySQL style scripted SQL
DROP TABLE IF EXISTS test;
CREATE TABLE IF NOT EXISTS test (
  id int NOT NULL AUTO_INCREMENT,
  name varchar(1024) NOT NULL,
  created_at datetime NULL DEFAULT NULL,
  updated_at datetime NULL DEFAULT NULL,
  PRIMARY KEY (id)
);

DROP TABLE IF EXISTS test2;
CREATE TABLE IF NOT EXISTS test2 (
  id int NOT NULL,
  test_id int NOT NULL,
  value varchar(255) NOT NULL
);

DROP TABLE IF EXISTS test_intermediate;
CREATE TABLE IF NOT EXISTS test_intermediate (
  test_id int NOT NULL,
  test_related_id int NOT NULL,
  intermediate_value varchar(1024) DEFAULT NULL,
  created_at datetime NULL DEFAULT NULL,
  updated_at datetime NULL DEFAULT NULL
);

DROP TABLE IF EXISTS test_related;

CREATE TABLE IF NOT EXISTS test_related (
  id int NOT NULL AUTO_INCREMENT,
  value varchar(1024) NOT NULL,
  created_at datetime NULL DEFAULT NULL,
  updated_at datetime NULL DEFAULT NULL,
  PRIMARY KEY (id)
);

DROP SEQUENCE IF EXISTS `author_author_id_seq` ;

DROP TABLE IF EXISTS `author`;

CREATE SEQUENCE `author_author_id_seq` START WITH 1 INCREMENT BY 1;

CREATE TABLE `author` (
  `author_id` int(11) NOT NULL DEFAULT nextval(`author_author_id_seq`) PRIMARY KEY,
  `parent_author_id` int(11) DEFAULT NULL,
  `publisher_id` int(11) DEFAULT NULL,
  `name` varchar(25) COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT 'default_name',
  `updated_at` datetime DEFAULT NULL,
  `created_at` datetime DEFAULT NULL,
  `some_Date` date DEFAULT NULL,
  `some_time` time DEFAULT NULL,
  `some_text` text COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `some_enum` enum('a','b','c') COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `encrypted_password` varchar(50) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `mixedCaseField` varchar(50) COLLATE utf8mb4_unicode_ci DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

DROP TABLE IF EXISTS book;

CREATE TABLE IF NOT EXISTS  book(
	book_id INT NOT NULL PRIMARY KEY AUTO_INCREMENT,
	Author_id INT,
	secondary_author_id INT,
	name VARCHAR(50),
	numeric_test VARCHAR(10) DEFAULT '0',
	special NUMERIC(10,2) DEFAULT 0
);

CREATE TABLE publisher(
	publisher_id INT NOT NULL PRIMARY KEY AUTO_INCREMENT,
	name VARCHAR(25) NOT NULL DEFAULT 'default_name'
) ENGINE=InnoDB;

DROP TABLE IF EXISTS VENUE;

CREATE TABLE IF NOT EXISTS  venue (
	id int NOT NULL AUTO_INCREMENT PRIMARY KEY,
	name varchar(50),
	city varchar(60),
	state char(2),
	address varchar(50),
	phone varchar(10) default NULL,
	UNIQUE(name,address)
);


CREATE TABLE IF NOT EXISTS  event (
	id int NOT NULL auto_increment PRIMARY KEY,
	venue_id int NULL,
	host_id int NOT NULL,
	title varchar(60) NOT NULL,
	description varchar(50),
	type varchar(15) default NULL
);

CREATE TABLE host(
	id INT NOT NULL PRIMARY KEY AUTO_INCREMENT,
	name VARCHAR(25)
);


CREATE TABLE IF NOT EXISTS  host(
	id INT NOT NULL PRIMARY KEY AUTO_INCREMENT,
	name VARCHAR(25)
);

CREATE TABLE IF NOT EXISTS  employee (
	id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	first_name VARCHAR(255) NOT NULL,
	last_name VARCHAR(255) NOT NULL,
	nick_name VARCHAR(255) NOT NULL
);


CREATE TABLE IF NOT EXISTS  position (
	id int NOT NULL AUTO_INCREMENT PRIMARY KEY,
	employee_id int NOT NULL,
	title VARCHAR(255) NOT NULL,
	active SMALLINT NOT NULL
);


CREATE TABLE IF NOT EXISTS  `rm-bldg`(
    `rm-id` INT NOT NULL,
    `rm-name` VARCHAR(10) NOT NULL,
    `space out` VARCHAR(1) NOT NULL
);

CREATE TABLE IF NOT EXISTS  awesome_people(
	id int not null primary key auto_increment,
	author_id int,
	is_awesome int default 1
);

CREATE TABLE amenity (
  `amenity_id` int(11) NOT NULL AUTO_INCREMENT PRIMARY KEY,
  `type` varchar(40) NOT NULL DEFAULT ''
);

CREATE TABLE property(
  `property_id` int(11) NOT NULL AUTO_INCREMENT PRIMARY KEY
);


CREATE TABLE IF NOT EXISTS  amenity (
  `amenity_id` int(11) NOT NULL AUTO_INCREMENT PRIMARY KEY,
  `type` varchar(40) NOT NULL DEFAULT ''
);

CREATE TABLE IF NOT EXISTS  property(
  `property_id` int(11) NOT NULL AUTO_INCREMENT PRIMARY KEY
);

CREATE TABLE IF NOT EXISTS  property_amenity(
  `id` int(11) NOT NULL AUTO_INCREMENT PRIMARY KEY,
  `amenity_id` int(11) NOT NULL DEFAULT '0',
  `property_id` int(11) NOT NULL DEFAULT '0'
);

CREATE TABLE user (
    id INT NOT NULL AUTO_INCREMENT PRIMARY KEY
) ENGINE=InnoDB;

CREATE TABLE newsletter (
    id INT NOT NULL AUTO_INCREMENT PRIMARY KEY
) ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS  user (
    id INT NOT NULL AUTO_INCREMENT PRIMARY KEY
) ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS  newsletter (
    id INT NOT NULL AUTO_INCREMENT PRIMARY KEY
) ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS  user_newsletter (
    id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
    user_id INT NOT NULL,
    newsletter_id INT NOT NULL
) ENGINE=InnoDB;


CREATE TABLE IF NOT EXISTS  valuestore (
  `id` INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
  `key` varchar(20) NOT NULL DEFAULT '',
  `value` varchar(255) NOT NULL DEFAULT ''
) ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS `payment` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `memberid` int(11) NOT NULL,
  `emailid` int(11) NOT NULL,
  `status` varchar(15) CHARACTER SET armscii8 NOT NULL,
  `total_cost` int(11) NOT NULL,
  `trans_fee` int(11) NOT NULL,
  `trans_code` varchar(80) CHARACTER SET ascii COLLATE ascii_bin NOT NULL,
  `created_at` datetime NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `pay_item`
--

CREATE TABLE IF NOT EXISTS `pay_item` (
  `trans_id` int(11) NOT NULL,
  `id` int(11) NOT NULL,
  `detail` varchar(255) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `amount` int(11) NOT NULL,
  `purpose` varchar(15) COLLATE utf8mb4_unicode_ci NOT NULL,
  PRIMARY KEY (`trans_id`,`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

